//
// Created by Administrator on 2026/5/8.
//

#ifndef VER_1_TIMER_H
#define VER_1_TIMER_H

#include <stdint.h>

/**
 * @brief  记录当前时间作为计时起点
 * @note   多次调用会重置计时起点
 */
void timer_start(void);

/**
 * @brief  获取从 timer_start() 到当前的时间差值
 * @retval 时间差值（单位：毫秒，ms）
 * @note   利用 32 位无符号减法自动处理溢出，最大支持 49.7 天间隔
 */
uint32_t timer_getval(void);

#define timer_getval_ms()          timer_getval()            // 获取当前计时时间  单位ms
#define timer_getval_s()           (timer_getval() / 1000)   // 获取当前计时时间  单位s

#endif //VER_1_TIMER_H