#ifndef VER_2_DELAY_H
#define VER_2_DELAY_H

#include <stdint.h>

/**
 * @brief 初始化 DWT 用于微秒延时（需在 main 中调用一次）
 */
void DWT_Delay_Init(void);

/**
 * @brief 微秒级阻塞延时
 * @param us 微秒数
 */
void delay_us(uint32_t us);

/**
 * @brief 毫秒级阻塞延时
 * @param ms 毫秒数
 */
void delay_ms(uint32_t ms);

/**
 * @brief 秒级阻塞延时
 * @param s 秒数
 */
void delay_s(uint32_t s);

#endif //VER_2_DELAY_H