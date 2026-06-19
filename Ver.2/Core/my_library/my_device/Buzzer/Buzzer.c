
#include "Buzzer.h"
#include "cmsis_os2.h"

void Buzzer_Beep(uint32_t ms)
{
    BUZZER_ON();
    osDelay(ms);  // 使用宏定义
    BUZZER_OFF();
}
