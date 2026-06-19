#ifndef VER_2_BATTERY_H
#define VER_2_BATTERY_H

#include "stm32f1xx_hal.h"

/**
 * @brief 初始化电池测量模块（启用 VREFINT、启动 DMA 循环采集）
 * @note 必须确保 ADC1 已在 CubeMX 中正确配置为：
 *       - Scan Mode: Enabled
 *       - Continuous Mode: Enabled
 *       - Number of Conversions: 2 (Rank1=外部通道, Rank2=VREFINT)
 *       - 并已添加 DMA Circular 模式
 */
void ADC_Battery_Init(void);

/**
 * @brief 供 DMA 中断调用的处理函数（由 stm32f1xx_it.c 调用）
 */
void Battery_IRQHandler(ADC_HandleTypeDef* hadc);

/**
 * @brief 电池电量扫描函数
 */
void ADC_Battery_Scan(void);


#endif //VER_2_BATTERY_H