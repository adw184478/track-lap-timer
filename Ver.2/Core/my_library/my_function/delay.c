#include "delay.h"
#include "stm32f1xx_hal.h" // 根据你的芯片型号替换头文件

/* -------------------- 微秒延时（基于 DWT） -------------------- */
/**
 * @brief 初始化 DWT 用于微秒延时（需在 main 中调用一次）
 *        使用前必须使能 DWT 计数器，例如：DWT_Delay_Init();
 */
void DWT_Delay_Init(void)
{
    // 0 = 未初始化，1 = 已初始化
    static uint8_t is_initialized = 0;

    // 已经初始化过，直接返回，不重复操作寄存器
    if (is_initialized)
    {
        return;
    }

    //给通讯设备初始化延时
    HAL_Delay(100);

    // 执行完整初始化流程
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // 标记为已初始化
    is_initialized = 1;
}

/**
 * @brief 微秒级延时（基于系统时钟）
 * @param us 微秒数
 * @note  系统时钟频率通过 SystemCoreClock 获取，需包含头文件 stm32f1xx_hal.h
 */
void delay_us(uint32_t us)
{
    if (SystemCoreClock == 0) return; // 如果时钟没配置好，直接返回，防止死锁
    uint32_t startTick = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - startTick) < ticks);
}

/**
 * @brief 毫秒级阻塞延时（基于 DWT，分段防止溢出）
 * @param ms 毫秒数
 */
void delay_ms(uint32_t ms)
{
    // 循环每次延时 1ms，避免长延时导致 DWT 计数器溢出
    while (ms--)
    {
        delay_us(1000);
    }
}

/**
 * @brief 秒级阻塞延时
 * @param s 秒数
 */
void delay_s(uint32_t s)
{
    // 循环每次延时 1000ms，兼容任意长延时
    while (s--)
    {
        delay_ms(1000);
    }
}