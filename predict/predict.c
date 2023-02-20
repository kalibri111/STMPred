//
// Created by Иван Лескин on 19/07/2022.
//

#include "predict.h"
#include "math.h"


/**
 * Computes l2 norm squared distance between SVMDataUnits
 * @param first
 * @param second
 * @param el_size size of element of SVMDataUnit_type
 * @return
 */
double l2distsq(SVMDataUnit_type* first, SVMDataUnit_type* second, int32_t el_size) {
    float32_t result = 0;
    uint32_t fields_n = sizeof(*first) / el_size;

    float32_t fsq;
    float32_t ssq;

    arm_power_f32((float32_t *) first, fields_n, &fsq);  // sum of squares
    arm_power_f32((float32_t *) second, fields_n, &ssq);
    result = sqrtf(fabsf(fsq - ssq));
    return result;
}


double rbf_kernel_(SVMDataUnit_type* x1, SVMDataUnit_type* x2, double gamma) {
    return exp(-gamma * l2distsq(x1, x2, sizeof(double)));
}

void rbf_kernel(SVMDataUnit_type* input, SVMDataUnit_type* trained, size_t trained_size, double gamma) {
    for (int i = 0; i < trained_size; ++i) {
        kernel_matrix[i] = rbf_kernel_(input, trained + i, gamma);
    }
}

void transform_(double* v, const double* std, const double* mean) {
    *v -= * mean;
    *v /= * std;
}

/**
 * Perform standardization by centering and scaling inplace.
 * @param x
 */
void transform(SVMDataUnit_type* x) {
    transform_(&(x->z_min), &std_trained_z_min, &mean_trained_z_min);
    transform_(&(x->z_mean), &std_trained_z_mean, &mean_trained_z_mean);
    transform_(&(x->y_mean), &std_trained_y_mean, &mean_trained_y_mean);
    transform_(&(x->z_energy), &std_trained_z_energy, &mean_trained_z_energy);
    transform_(&(x->y_energy), &std_trained_y_energy, &mean_trained_y_energy);
    transform_(&(x->x_energy), &std_trained_x_energy, &mean_trained_x_energy);
    transform_(&(x->sma), &std_trained_sma, &mean_trained_sma);
    transform_(&(x->x_mean_pos1), &std_trained_x_mean_pos1, &mean_trained_x_mean_pos1);
    transform_(&(x->y_min_pos1), &std_trained_y_min_pos1, &mean_trained_y_min_pos1);
    transform_(&(x->x_min_pos1), &std_trained_x_min_pos1, &mean_trained_x_min_pos1);

}

void vector_squared(float32_t* src, float32_t* result) {
    arm_dot_prod_f32(
            src,
            src,
            BATCH_SIZE,
            result
    );
}

void vector_add(float32_t* srcA, float32_t* srcB, float32_t* result) {
    arm_add_f32(
            srcA,
            srcB,
            result,
            BATCH_SIZE
    );
}

void vector_sqrt(const float32_t* src, float32_t* result) {
    for (int i = 0; i < BATCH_SIZE; ++i) {
        arm_sqrt_f32(src[i], &(result[i]));
    }
}

void statistics(SVMDataUnit_type* to_fill, double* x, double* y, double* z) {
    uint32_t min_idx;  // not used
    float32_t x_squared[BATCH_SIZE];
    float32_t y_squared[BATCH_SIZE];
    float32_t z_squared[BATCH_SIZE];

    arm_mean_f32(
            (float32_t *) y,
            BATCH_SIZE,
            (float32_t *) &(to_fill->y_mean)
            );  // y_mean

    arm_mean_f32(
            (float32_t *) z,
            BATCH_SIZE,
            (float32_t *) &(to_fill->z_mean)
            );  // z_mean

    arm_min_f32(
            (float32_t *) z,
            BATCH_SIZE,
            (float32_t *) &(to_fill->z_min),
            &min_idx
            );  // z_min
    // z_energy (mean of squared)
    arm_power_f32(
            (float32_t *) z,
            BATCH_SIZE,
            (float32_t *) &(to_fill->z_energy)
            );
    to_fill->z_energy /= BATCH_SIZE;
    // y energy
    arm_power_f32(
            (float32_t *) y,
            BATCH_SIZE,
            (float32_t *) &(to_fill->y_energy)
    );
    to_fill->y_energy /= BATCH_SIZE;
    // x energy
    arm_power_f32(
            (float32_t *) x,
            BATCH_SIZE,
            (float32_t *) &(to_fill->x_energy)
    );
    to_fill->x_energy /= BATCH_SIZE;
    // sma
    vector_squared((float32_t *) x, x_squared);
    vector_squared((float32_t *) y, y_squared);
    vector_squared((float32_t *) z, z_squared);
    vector_add(x_squared, y_squared, (float32_t *) &(to_fill->sma));
    vector_add((float32_t *) &(to_fill->sma), z_squared, (float32_t *) &(to_fill->sma));
    vector_sqrt((float32_t *) &(to_fill->sma), (float32_t *) &(to_fill->sma));
    to_fill->x_min_pos1 = x_min_pos1;
    to_fill->y_min_pos1 = y_min_pos1;
    to_fill->x_mean_pos1 = x_mean_pos1;
}

double decision_function_(SVMDataUnit_type* measure, void* kernel_fn(SVMDataUnit_type*, SVMDataUnit_type*, size_t, double)) {
    float32_t decision = 1;
    kernel_fn(measure, x_trained, TRAIN_SIZE, GAMMA);

    // kernel * y * lambdas scalar
    for (int i = 0; i < TRAIN_SIZE; ++i) {
        kernel_matrix[i] *= y_trained[i];
        kernel_matrix[i] *= lambda_trained[i];
    }

    // sum -> one elements
    arm_mean_f32(kernel_matrix, TRAIN_SIZE, &decision);
    decision *= TRAIN_SIZE;
    decision += b_trained;

    return decision;
}

int8_t predict(double* x, double* y, double* z, void* kernel_fn) {
    // packing data
    SVMDataUnit_type measure;
    statistics(&measure, x, y, z);

    transform(&measure);
    int8_t p = (int8_t) decision_function_(&measure, kernel_fn);

    p = p > 0;
    p += 1;
    p /= 2;

    return p;
}

void calibrate(float32_t* x, float32_t* y) {
    uint32_t idx;  // not used
    arm_mean_f32(x, BATCH_SIZE, (float32_t *) &x_mean_pos1);
    arm_min_f32(x, BATCH_SIZE, (float32_t *) &x_min_pos1, &idx);
    arm_min_f32(y, BATCH_SIZE, (float32_t *) &y_min_pos1, &idx);
}