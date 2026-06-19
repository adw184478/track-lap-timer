//
// Created by Administrator on 2026/5/8.
//

#include "timer.h"

#include "stm32f1xx_hal.h"  // 根据实际 STM32 系列修改（如 f4xx, h7xx 等）

// 静态变量：保存 timer_start() 记录的起始时间
static uint32_t s_tick_start = 0;

/**
 * @brief  记录当前 HAL 系统滴答时间作为计时起点
 */
void timer_start(void)
{
    s_tick_start = HAL_GetTick();
}

/**
 * @brief  计算并返回从 timer_start() 到当前的时间差值
 * @retval 时间差值（单位：毫秒，ms）
 */
uint32_t timer_getval(void)
{
    uint32_t current_tick = HAL_GetTick();
    return (current_tick - s_tick_start);
}