//
// Created by Иван Лескин on 20/10/2022.
//
#include "custom_task.h"
#define MPU_DOUBLE sizeof(double)
I2C_HandleTypeDef hi2c;
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
    MPU6050_Read_All(&hi2c, &MPU_Data);
    // copy data
    memcpy(NotifyCharData               , &(MPU_Data.Ax), MPU_DOUBLE);
    memcpy(NotifyCharData + MPU_DOUBLE  , &(MPU_Data.Ay), MPU_DOUBLE);
    memcpy(NotifyCharData + 2*MPU_DOUBLE, &(MPU_Data.Az), MPU_DOUBLE);

    Custom_STM_App_Update_Char(CUSTOM_STM_DBG_A_CHR, (uint8_t *) NotifyCharData);

    UTIL_SEQ_SetTask(1 << CFG_TASK_READ_ALL_MPU_VALUES, CFG_SCH_PRIO_1);
}

void UpdateVCountCharMpi(void) {

}