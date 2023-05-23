//
// Created by Иван Лескин on 13/11/2022.
//

#ifndef STMPRED_SVMDATAUNIT_H
#define STMPRED_SVMDATAUNIT_H
/**
 * For vector-based data
 */
typedef struct {
    float32_t z_mean;
    float32_t y_mean;

    float32_t z_energy;
    float32_t y_energy;
    float32_t x_energy;

    float32_t sma;

    float32_t z_min;

    float32_t x_mean_pos1;
    float32_t y_min_pos1;
    float32_t x_min_pos1;

} SVMDataUnit_type;
#endif //STMPRED_SVMDATAUNIT_H
