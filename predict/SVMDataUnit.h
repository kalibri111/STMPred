//
// Created by Иван Лескин on 13/11/2022.
//

#ifndef STMPRED_SVMDATAUNIT_H
#define STMPRED_SVMDATAUNIT_H
/**
 * For vector-based data
 */
typedef struct {
    double z_mean;
    double y_mean;

    double z_energy;
    double y_energy;
    double x_energy;

    double sma;

    double z_min;

    double x_mean_pos1;
    double y_min_pos1;
    double x_min_pos1;

} SVMDataUnit_type;
#endif //STMPRED_SVMDATAUNIT_H
