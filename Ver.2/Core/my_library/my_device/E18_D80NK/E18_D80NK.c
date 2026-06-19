#include "E18_D80NK.h"
#include "cmsis_os2.h"
#include "device_state.h"
#include "main.h"

// 全局实例
E18_HandleTypeDef e18 =
{
    .port =        GPIOB,
    .pin  =   GPIO_PIN_6,
    .last_time =        0
};



void E18_RaceFinished(uint16_t GPIO_Pin)
{
    // 判断是否是 PB6 中断
    if (GPIO_Pin == e18.pin)
    {
        uint32_t now_tick = HAL_GetTick();

        // 时间戳消抖：间隔不足 20ms 直接忽略
        if((now_tick - e18.last_time) < E18_DEBOUNCE_TIME)
        {
            return;
        }
        e18.last_time    = now_tick; // 更新上一次有效时间戳

        Device_Queue_t device = {
            .device = E18_D80NK,
            .data.e18_touch = 1
        };
        // 【正确发送队列】
        osMessageQueuePut(data_into_QueueHandle, &device, 0, 0);

    }
}