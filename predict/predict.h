//
// Created by Иван Лескин on 19/07/2022.
//

#ifndef STMPRED_PREDICT_H
#define STMPRED_PREDICT_H

#define ARM_MATH_CM4

#include "arm_math.h"
#include "app_conf.h"
#include "mpu5060.h"
#include "trained.h"
#define SVM_METRICS_CNT 10

/**
 * For coordinate-wise data
 */
static double z_mean   [BATCH_SIZE];
static double y_mean   [BATCH_SIZE];
static double z_energy [BATCH_SIZE];
static double y_energy [BATCH_SIZE];
static double x_energy [BATCH_SIZE];
static double sma      [BATCH_SIZE];
static double z_min    [BATCH_SIZE];

/**
 * Calibrating data in sit position
 */
static double x_mean_pos1;
static double y_min_pos1;
static double x_min_pos1;

/**
 * Buffer for kernel
 */
static float32_t kernel_matrix[TRAIN_SIZE];

/**
 * Realizes SVM predict with specific kernel function
 * sum(kernel_fn(x, x_trained) * y_trained * lambda_trained, axis=1) + b_trained
 * @param x incoming data from mpu with shape (batch_size * 3)
 * @param kernel_fn pointer to kernel function
 * @return
 */
int8_t predict(double* x, double* y, double* z, void* kernel_fn);

/**
 * Rbf kernel function exp(-gamma * || x - y ||**2)
 * @param x1 first set of metrics
 * @param x2 second set of metrics
 * @param gamma
 * @return Value of kernel for two samples
 */
double rbf_kernel_(SVMDataUnit_type* x1, SVMDataUnit_type* x2, double gamma);

/**
 * Computes rbf for each between test and train feature spaces. Result is (x1_size, x2_size) stored in kernel_matrix
 * @param input test data
 * @param trained train data
 * @param trained_size number of samples
 * @param gamma hyperparameter
 */
void rbf_kernel(SVMDataUnit_type* input, SVMDataUnit_type* trained, size_t trained_size, double gamma);

/**
 * Evaluate pos1-values
 * @param x
 * @param y
 */
void calibrate(float32_t* x, float32_t* y);
#endif //STMPRED_PREDICT_H
