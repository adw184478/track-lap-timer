#include "device_state.h"

#include <stdio.h>
#include <string.h>

#include "Battery.h"
#include "Buzzer.h"
#include "main.h"
#include "cmsis_os2.h"
#include "timer.h"
#include "TM1650.h"
/*===========================================================================
 * 全局变量定义
 *===========================================================================*/
static Device_Data_t g_device;   /* 唯一全局实例 */

/*===========================================================================
 * 接口函数实现
 *===========================================================================*/

/**
 * @brief 初始化设备数据
 */
void DeviceData_Init(TimingMode_t timing_mode, uint8_t target_count )
{
    g_device.timing_mode = timing_mode;
    g_device.state = STATE_READY;
    g_device.display_mode = DISP_BATTERY;   /* 上电默认显示电量 */
    g_device.target_count = target_count;
    g_device.current_count = 0;
    g_device.total_time_ms = 0;
    g_device.battery_percent = 100;         /* 假设满电，后续由 ADC 任务更新 */
}

/**
 * @brief 按键事件处理（状态机核心逻辑）
 *
 * 所有按键行为在此函数中定义，根据当前设备状态做出响应。
 * 此函数不涉及 FreeRTOS 队列，仅接收事件枚举值。
 */
void DeviceData_HandleKeyEvent(KeyEvent_t event)
{
    switch (event) {
        case KEY_MODE:
            /* 切换显示模式 */
            g_device.display_mode = (g_device.display_mode == DISP_BATTERY) ? DISP_TIME : DISP_BATTERY;

            break;

        case KEY_READY:
            /* 短按准备键：根据当前状态决定是否进入 READY */
            if (g_device.state == STATE_TIMING) {
                g_device.state = STATE_READY;
                /* 此处不重置 total_time 或 lap_count，保持历史记录 */
            }
            /* 在 STATE_READY 或 STATE_FINISH 时按下无效 */
            break;

        case KEY_RESET:
            if (g_device.timing_mode == TIMING_MANUAL)
            {
                switch (g_device.state) {
                    case STATE_READY:
                        timer_start();
                        g_device.state = STATE_TIMING;          // 进入计时状态
                        break;

                    case STATE_TIMING:
                            // 已跑完目标圈数
                            g_device.total_time_ms = timer_getval_ms();
                            g_device.state = STATE_FINISH;      // 进入结束状态
                        break;

                    case STATE_FINISH:
                        DeviceData_Reset();
                        break;

                    default:
                        break;
                }
            }
            else
            {
                /* 短按复位键：清除所有记录 */
                DeviceData_Reset();
                break;
            }

        default:
            break;
    }

}

/**
 * @brief 复位设备记录（不影响电量显示）
 */
void DeviceData_Reset(void)
{
    g_device.state = STATE_READY;
    g_device.total_time_ms = 0;
    g_device.current_count = 0;
}

/**
 * @brief 红外传感器触发处理
 */
void DeviceData_HandleE18Event(void)
{
    //判断是否是显示时间
    if (g_device.display_mode != DISP_TIME || g_device.timing_mode != TIMING_AUTO)
        return;

    if (g_device.state == STATE_READY || g_device.state == STATE_TIMING)
    {
        //蜂鸣器响
        Device_Queue_t device = {
            .device = E18_D80NK,
            .data.buzzer = 1
        };
        osMessageQueuePut(data_output_QueueHandle, &device, 0, 0);
    }

    switch (g_device.state) {
        case STATE_READY:
            timer_start();
            g_device.state = STATE_TIMING;          // 进入计时状态
            break;

        case STATE_TIMING:
            g_device.current_count++;               // 完成一圈
            if (g_device.current_count >= g_device.target_count) {
                // 已跑完目标圈数
                g_device.total_time_ms = timer_getval_ms();
                g_device.state = STATE_FINISH;      // 进入结束状态
            }
            break;

        default:
            break;
    }
}



/**
 * @brief 刷新TM1650数码管显示
 * @note  显示规则：
 *        1. DISP_BATTERY 模式：显示电池电量百分比 (0~100)
 *        2. DISP_TIME    模式：显示计时时间，格式为 秒.百分秒 (XX.XX)
 *           - 空闲/准备状态：显示 00.00
 *           - 计时中：实时显示当前计时值
 *           - 计时结束：显示最终总时间
 */
static void Send_Display_data()
{
    //延时一秒去或者电量不等于100
    static uint8_t display_first = 1;
    if (display_first)
    {
        timer_start();
        display_first = 0;
    }
    if (timer_getval_s() <= 1 && g_device.battery_percent == 100) return;

    Device_Queue_t device = {.device = LED_DISPLAY};

    if (g_device.display_mode == DISP_BATTERY)
    {
        device.data.display.num = g_device.battery_percent;
        device.data.display.dot_pos = 0;

    }
    else /* DISP_TIME 计时模式 */
    {
        uint32_t time_ms = 0;

        /* 根据设备状态获取对应时间值 */
        switch (g_device.state)
        {
            case STATE_READY:
                time_ms = 0;
                break;

            case STATE_TIMING:
                time_ms = timer_getval_ms(); /* 读取实时计时毫秒数 */
                break;

            case STATE_FINISH:
                time_ms = g_device.total_time_ms; /* 显示最终总时间 */
                break;

            default:
                time_ms = 0;
                break;
        }

        /* 毫秒转 XX.X 格式（秒.百分秒），适配4位数码管 */
        uint8_t sec  = time_ms / 1000;               // 整数秒
        uint8_t cs   = (time_ms % 1000) / 100;        // 百分秒
        uint16_t display_num = sec * 10 + cs;

        /* 显示时间，小数点在左数第3位（秒与百分秒之间） */
        device.data.display.num = display_num;
        device.data.display.dot_pos = 3;

    }

    osMessageQueuePut(data_output_QueueHandle, &device, 0, 0);
}

//数据向内流转任务函数
void data_into_device_Task(void *argument)
{
    Device_Queue_t device_data;
    for(;;)
    {
        osMessageQueueGet(data_into_QueueHandle,&device_data,NULL,osWaitForever);

        switch (device_data.device)
        {
            case KEY: DeviceData_HandleKeyEvent(device_data.data.key_id);
                break;
            case E18_D80NK: DeviceData_HandleE18Event();
                break;
            case BATTERY:  g_device.battery_percent = device_data.data.battery;
                break;
            case TIMER: ADC_Battery_Scan();
                        Send_Display_data();
                break;
            default:
                break;
        }
    }
}

void data_output_device_Task(void *argument)
{
    Device_Queue_t device_data;
    for(;;)
    {
        osMessageQueueGet(data_output_QueueHandle,&device_data,0,osWaitForever);

        switch (device_data.device)
        {
            case BUZZER:
                Buzzer_Beep(50);
                break;
            case LED_DISPLAY:
                TM1650_DisplayNum(device_data.data.display.num,      // 数码管显示刷新函数
                                  device_data.data.display.dot_pos);
                break;
            default: ;
        }
    }

}



