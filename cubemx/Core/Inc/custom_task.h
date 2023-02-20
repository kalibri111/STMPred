//
// Created by Иван Лескин on 19/10/2022.
//

#ifndef STMPRED_CUSTOM_TASK_H
#define STMPRED_CUSTOM_TASK_H
#include "custom_stm.h"
#include "mpu5060.h"
#include "stm32_seq.h"
#include "app_conf.h"
#include "predict.h"
#include "retarget_printf.h"
#include "app_common.h"

#define LD2_Pin GPIO_PIN_0
#define LD2_GPIO_Port GPIOB

void NotifyAxCharMpi(void);
void UpdateVCountCharMpi(void);
#endif //STMPRED_CUSTOM_TASK_H
