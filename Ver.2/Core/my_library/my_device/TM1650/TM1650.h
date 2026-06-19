#ifndef __TM1650_H
#define __TM1650_H

#include "stm32f1xx_hal.h"

/* -------------------- 枚举定义 -------------------- */

/**
 * @brief 显示开关状态
 */
typedef enum {
    TM1650_DISPLAY_OFF = 0,  // 关闭显示
    TM1650_DISPLAY_ON  = 1   // 开启显示
} TM1650_DisplayState_t;

/**
 * @brief 亮度等级 0~7 共8级
 */
typedef enum {
    TM1650_BRIGHTNESS_0 = 0,  // 第1级（最暗）
    TM1650_BRIGHTNESS_1,
    TM1650_BRIGHTNESS_2,
    TM1650_BRIGHTNESS_3,
    TM1650_BRIGHTNESS_4,
    TM1650_BRIGHTNESS_5,
    TM1650_BRIGHTNESS_6,
    TM1650_BRIGHTNESS_7       // 第8级（最亮）
} TM1650_Brightness_t;

/**
 * @brief 数码管位号（4位数码管 1~4）
 */
typedef enum {
    TM1650_DIG_1 = 1,
    TM1650_DIG_2 = 2,
    TM1650_DIG_3 = 3,
    TM1650_DIG_4 = 4
} TM1650_Digit_t;

/**
 * @brief 可显示字符/熄灭状态（对应段码表索引）
 */
typedef enum {
    TM1650_NUM_0 = 0,
    TM1650_NUM_1,
    TM1650_NUM_2,
    TM1650_NUM_3,
    TM1650_NUM_4,
    TM1650_NUM_5,
    TM1650_NUM_6,
    TM1650_NUM_7,
    TM1650_NUM_8,
    TM1650_NUM_9,
    TM1650_NUM_OFF  // 熄灭
} TM1650_Number_t;

/* -------------------- 函数声明 -------------------- */

void TM1650_Init(TM1650_Brightness_t brightness);
void TM1650_SetDisplay(TM1650_DisplayState_t state);
void TM1650_DisplayNum(uint16_t number, uint8_t dp_pos);
void TM1650_Clear(void);

#endif // __TM1650_H