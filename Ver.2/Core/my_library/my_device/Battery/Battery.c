#include "Battery.h"

#include "adc.h"
#include "cmsis_os2.h"
#include "device_state.h"
#include "stm32f1xx_hal_adc_ex.h"
#include "tim.h"


/* ---------------------- 私有参数 ---------------------- */

// 分压电阻系数 (33K+10K)/10K = 4.3
#define DIVIDER_RATIO       4.3

// 平均滤波采样次数
#define AVG_SAMPLE_COUNT    20

// DMA 缓冲区大小 (2个半字：外部电压 + VREFINT)
#define DMA_BUFFER_SIZE     2

//最小电压
#define MIN_VOLTAGE         9000
//最大电压
#define MAX_VOLTAGE         12600

/* ---------------------- 私有变量 ---------------------- */

// DMA 缓冲区（由 DMA 自动填充）
static uint16_t adc_dma_buffer[DMA_BUFFER_SIZE];

// 累加器和计数
static uint32_t ext_sum = 0;
static uint32_t vref_sum = 0;
static uint8_t  sample_cnt = 0;

// 最终计算结果（带平均滤波）
static uint8_t battery_percentage = 0;

/* ---------------------- 函数实现 ---------------------- */

void ADC_Battery_Init(void) {
    // 1.启动ADC1的校准（必须在ADC启动前执行）
    HAL_ADCEx_Calibration_Start(&hadc1);

    // 2. 启动 ADC + DMA 循环采集
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buffer, DMA_BUFFER_SIZE);

}

void ADC_Battery_Scan(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buffer, DMA_BUFFER_SIZE);
}

void Battery_IRQHandler(ADC_HandleTypeDef* hadc) {
    if (hadc == &hadc1) {
        // 读取 DMA 缓冲区中的最新数据
        uint16_t vref_raw = adc_dma_buffer[0]; //参考电压
        uint16_t ext_raw = adc_dma_buffer[1];  //电池分压

        static uint8_t last_battery_percentage = 0;

        // 累加
        ext_sum += ext_raw;
        vref_sum += vref_raw;
        sample_cnt++;

        // 累积足够次数后计算平均值
        if (sample_cnt >= AVG_SAMPLE_COUNT) {
            uint16_t ext_avg = ext_sum / AVG_SAMPLE_COUNT;
            uint16_t vref_avg = vref_sum / AVG_SAMPLE_COUNT;

            //基准电压值
            vref_avg = 1.2 * 1000 * 4095 / vref_avg;

            //电池电压值（mv）
            ext_avg = (float)ext_avg / 4095.0f * vref_avg * DIVIDER_RATIO;

            // 限制范围并映射到 0~100
            if (ext_avg <= MIN_VOLTAGE) {
                battery_percentage = 0;
            } else if (ext_avg >= MAX_VOLTAGE) {
                battery_percentage = 100;
            } else {
                battery_percentage = (float)(ext_avg - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE) * 100;
            }



            // 只有百分比发生变化时才更新，消除临界跳变
            if (last_battery_percentage != battery_percentage) {
                last_battery_percentage = battery_percentage;

                // 仅在电量变化时发送队列，减少无效消息
                Device_Queue_t device = {
                    .device = BATTERY,
                    .data.battery = battery_percentage
                };

                osMessageQueuePut(data_into_QueueHandle, &device, 0, 0);
            }

            // 重置累加器，准备下一轮
            ext_sum = 0;
            vref_sum = 0;
            sample_cnt = 0;
        }
    }
}
