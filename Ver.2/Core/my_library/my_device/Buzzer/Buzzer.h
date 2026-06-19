
#ifndef _BUZZER_H
#define _BUZZER_H

#include "stm32f1xx_hal.h"

// 蜂鸣器引脚定义
#define BUZZER_GPIO_PORT  GPIOA
#define BUZZER_GPIO_PIN   GPIO_PIN_15

// 电平控制宏定义
#define BUZZER_ON()    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_SET)
#define BUZZER_OFF()   HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_RESET)


void Buzzer_Beep(uint32_t ms);

#endif