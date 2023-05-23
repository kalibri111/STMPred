//
// Created by Иван Лескин on 20/10/2022.
//
#include "custom_task.h"
#define TRANSFER_DATA_SIZE sizeof(float)
I2C_HandleTypeDef hi2c1;
uint8_t charCounter = 0;

float32_t MPU_x[BATCH_SIZE];
float32_t MPU_y[BATCH_SIZE];
float32_t MPU_z[BATCH_SIZE];


MPU6050_t MPU_Data = {
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1
};

static uint8_t UpdateCharData[247];
static uint8_t NotifyCharData[247] = {0};

void NotifyAxCharMpi(void) {
    MPU6050_Read_All(&hi2c1, &MPU_Data);
    // copy data
    float32_t x = (float32_t)(MPU_Data.Ax);
    float32_t y = (float32_t)(MPU_Data.Ay);
    float32_t z = (float32_t)(MPU_Data.Az);

#ifdef SIGNAL_LOG
#ifdef JSON_LOG
    printf("{\"x\": %f, \"y\": %f, \"z\": %f},\n\r", x, y, z);
#endif
#ifndef JSON_LOG
    printf("x: %f, y: %f, z: %f\n\r", x, y, z);
#endif
#endif
    MPU_x[charCounter] = x;
    MPU_y[charCounter] = y;
    MPU_z[charCounter] = z;

    memcpy(NotifyCharData                 , &(x), TRANSFER_DATA_SIZE);
    memcpy(NotifyCharData + TRANSFER_DATA_SIZE    , &(y), TRANSFER_DATA_SIZE);
    memcpy(NotifyCharData + 2 * TRANSFER_DATA_SIZE, &(z), TRANSFER_DATA_SIZE);

    Custom_STM_App_Update_Char(CUSTOM_STM_DBG_A_CHR, (uint8_t *) NotifyCharData);
}

void UpdateVCountCharMpi(void) {
    if (charCounter < BATCH_SIZE) {
        NotifyAxCharMpi();
        charCounter += 1;
    } else {
        charCounter = 0;
        float32_t p_res = predict(MPU_x, MPU_y, MPU_z/*, rbf_kernel*/);
#ifndef SIGNAL_LOG
        printf("PREDICT RESULT %f\n\r", p_res);
#endif
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
}