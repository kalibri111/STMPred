//
// Created by Иван Лескин on 14/02/2023.
//

#ifndef STM32WBBLUETOOTH_RETARGET_PRINTF_H
#define STM32WBBLUETOOTH_RETARGET_PRINTF_H

#include "stm32wbxx_hal.h"
#include <sys/stat.h>
#include "app_conf.h"

void RetargetInit(UART_HandleTypeDef *huart);
int _isatty(int fd);
int _write(int fd, char* ptr, int len);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char* ptr, int len);
int _fstat(int fd, struct stat* st);

#endif//STM32WBBLUETOOTH_RETARGET_PRINTF_H
