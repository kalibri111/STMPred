//
// Created by Иван Лескин on 19/10/2022.
//

#ifndef STMPRED_CUSTOM_TASK_H
#define STMPRED_CUSTOM_TASK_H
#include "custom_stm.h"
#include "mpu5060.h"
#include "stm32_seq.h"
#include "app_conf.h"

void NotifyAxCharMpi(void);
void UpdateVCountCharMpi(void);
#endif //STMPRED_CUSTOM_TASK_H
