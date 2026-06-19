#ifndef __E18_D80NK_H
#define __E18_D80NK_H

#include "stm32f1xx_hal.h"

// 消抖时间 单位：ms
#define E18_DEBOUNCE_TIME  200

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t     pin;
    uint32_t     last_time;       // 上一次触发时间
} E18_HandleTypeDef;


void E18_RaceFinished(uint16_t GPIO_Pin);

#endif