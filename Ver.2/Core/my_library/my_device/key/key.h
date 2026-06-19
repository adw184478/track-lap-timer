#ifndef MY_KU_APP_KEY_H
#define MY_KU_APP_KEY_H

#include "key_driver.h"

void buttons_init();
void set_key_flag(uint8_t flag);
uint8_t get_key_flag();
uint8_t get_button_gpio(uint8_t button_id);


/*
 5ms的中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        set_key_flag(1);
    }
}
*/

/*
    buttons_init();       // 初始化MultiButton
    while (1)
    {
        // 5ms 到了再执行按键状态机
        if(get_key_flag())
        {
            set_key_flag(0);
            button_ticks();
        }
    }
 */


#endif //MY_KU_APP_KEY_H