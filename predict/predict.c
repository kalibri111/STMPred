//
// Created by Иван Лескин on 19/07/2022.
//

#include "predict.h"
#include "retarget_printf.h"
#include "app_common.h"
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
    return exp(gamma * l2distsq(x1, x2, sizeof(double)));
}

void rbf_kernel(SVMDataUnit_type* input, SVMDataUnit_type* trained, size_t trained_size, double gamma) {
    for (int i = 0; i < trained_size; ++i) {
        kernel_matrix[i] = rbf_kernel_(input, trained + i, gamma);
    }
}

void transform_(float32_t * v, const float32_t * std, const float32_t * mean) {
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
    transform_(&(x->sma), &std_trained_sma, &mean_trained_sma);
    transform_(&(x->x_mean_pos1), &std_trained_x_mean_pos1, &mean_trained_x_mean_pos1);
    transform_(&(x->y_min_pos1), &std_trained_y_min_pos1, &mean_trained_y_min_pos1);

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

void statistics(SVMDataUnit_type* to_fill, float32_t * x, float32_t * y, float32_t * z) {
    uint32_t min_idx;  // not used
    float32_t x_squared[BATCH_SIZE];
    float32_t y_squared[BATCH_SIZE];
    float32_t z_squared[BATCH_SIZE];

//    double buf[3] = {1, 2, 3};
//    float32_t res = 0;
//
//    arm_mean_f32(
//            &buf,
//            3,
//            &res
//    );  // y_mean
//
//    printf("experiment: %f\n\r", res);

    arm_mean_f32(
             y,
            BATCH_SIZE,
            &(to_fill->y_mean)
            );  // y_mean

    double sumy = 0;
    for (int i = 0; i < BATCH_SIZE; ++i) {
        sumy += y[i];
    }


    arm_mean_f32(
            z,
            BATCH_SIZE,
            (float32_t *) &(to_fill->z_mean)
            );  // z_mean

    arm_min_f32(
            z,
            BATCH_SIZE,
            (float32_t *) &(to_fill->z_min),
            &min_idx
            );  // z_min

    // z_energy (mean of squared)
    arm_power_f32(
            z,
            BATCH_SIZE,
            (float32_t *) &(to_fill->z_energy)
            );
    to_fill->z_energy /= BATCH_SIZE;
    // y energy
    arm_power_f32(
            y,
            BATCH_SIZE,
            (float32_t *) &(to_fill->y_energy)
    );
    to_fill->y_energy /= BATCH_SIZE;

    // x energy
    arm_power_f32(
            x,
            BATCH_SIZE,
            (float32_t *) &(to_fill->x_energy)
    );
    to_fill->x_energy /= BATCH_SIZE;

#ifndef SIGNAL_LOG
    printf("y_mean: %f\n\r", to_fill->y_mean);
    printf("z_mean: %f\n\r", to_fill->z_mean);
    printf("z_min: %f\n\r", to_fill->z_min);
    printf("z_energy: %f\n\r", to_fill->z_energy);
    printf("y_energy: %f\n\r", to_fill->y_energy);
    printf("x_energy: %f\n\r", to_fill->x_energy);
#endif
    // -sma-
    float32_t sma_buff[BATCH_SIZE];
    vector_squared(x, x_squared);
    vector_squared( y, y_squared);
    vector_squared(z, z_squared);
    vector_add(x_squared, y_squared, &sma_buff);
    vector_add(&sma_buff, z_squared, &sma_buff);
    vector_sqrt(&sma_buff, &sma_buff);
    arm_mean_f32(&sma_buff, BATCH_SIZE, &(to_fill->sma));
    // ----

    to_fill->x_min_pos1 = x_min_pos1;
    to_fill->y_min_pos1 = y_min_pos1;
    to_fill->x_mean_pos1 = x_mean_pos1;
}

float32_t decision_function_(SVMDataUnit_type* measure/*, void* kernel_fn(SVMDataUnit_type*, SVMDataUnit_type*, size_t, double)*/) {
    float32_t decision = 1;
//    kernel_fn(measure, x_trained, TRAIN_SIZE, GAMMA);
//    rbf_kernel(measure, x_trained, TRAIN_SIZE, GAMMA);

    // kernel * y * lambdas scalar
    for (int i = 0; i < TRAIN_SIZE; ++i) {
        kernel_matrix[i] *= weight_trained[i];
    }

    // sum -> one elements
    arm_mean_f32(kernel_matrix, TRAIN_SIZE, &decision);
    decision *= TRAIN_SIZE;

    // TODO: remove this little helping
    if (measure->z_mean > -0.8) {
        return 1;
    } else {
        return 0;
    }

    return decision;
}

float32_t predict(float32_t * x, float32_t * y, float32_t * z/*, void* kernel_fn*/) {
    // packing data
    SVMDataUnit_type measure;
    statistics(&measure, x, y, z);

//    transform(&measure);
    float32_t p = decision_function_(&measure/*, kernel_fn*/);

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