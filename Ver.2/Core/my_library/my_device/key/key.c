//
// Created by Administrator on 2026/4/30.
//

#include "key.h"

#include <stdio.h>


#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "device_state.h"

#define MAX_BUTTONS 3

// 按键引脚定义，改成你自己的
#define KEY1_PORT   GPIOA
#define KEY1_PIN    GPIO_PIN_8

#define KEY2_PORT   GPIOA
#define KEY2_PIN    GPIO_PIN_9

#define KEY3_PORT   GPIOA
#define KEY3_PIN    GPIO_PIN_2



// 通用事件打印
void bin_press_up(Button* btn, void* user_data)
{
    // printf("[BTN%d] %s (repeat: %d, pressed: %s)\r\n",
    //            btn->button_id, event_name,btn->event,
    //            button_get_repeat_count(btn),
    //            button_is_pressed(btn) ? "Yes" : "No");

    // 赋值按键ID
    uint8_t key_id;

    switch (btn->button_id) {
        case 1: key_id = KEY_MODE;  printf("3\r\n");break;
        case 2: key_id = KEY_READY; printf("3\r\n");break;
        case 3: key_id = KEY_RESET; printf("3\r\n");break;
        default: return; // 无效按键，直接返回
    }

    Device_Queue_t device = {
        .device = KEY,
        .data.key_id = key_id
    };
    // 【正确发送队列】
    osMessageQueuePut(data_into_QueueHandle, &device, 0, 0);


}

// Hardware abstraction layer function
uint8_t read_button_gpio(uint8_t button_id)
{
    switch(button_id)
    {
        case 1: return HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN);
        case 2: return HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN);
        case 3: return HAL_GPIO_ReadPin(KEY3_PORT, KEY3_PIN);
        default: return 6;
    }
}

// 按键对象
static Button buttons[MAX_BUTTONS];
static uint8_t button_tick_flag = 0; // 5ms 标志

void set_key_flag(uint8_t flag) {
    button_tick_flag = flag;
}

uint8_t get_key_flag() {
    return button_tick_flag;
}



// Event handlers
//static void on_single_click(Button* btn, void* user_data) { (void)user_data; generic_event_handler(btn, "Single Click"); }
//static void on_long_press_hold(Button* btn, void* user_data) { (void)user_data; generic_event_handler(btn, "Long Press Start"); }


// Initialize a single button with all event handlers
void init_button(int index, uint8_t button_id)
{
    button_init(&buttons[index], read_button_gpio, 0, button_id);

    //button_attach(&buttons[index], BTN_PRESS_DOWN, on_single_click, NULL);
    button_attach(&buttons[index], BTN_PRESS_UP, bin_press_up, NULL);
    //button_attach(&buttons[index], BTN_SINGLE_CLICK, on_single_click, NULL);
    button_start(&buttons[index]);

}

void buttons_init(){
    for (uint8_t i = 0; i < MAX_BUTTONS; i++) {
        init_button(i, i+1);

    }

}

void Start_KeyTask(void *argument)
{
    /* USER CODE BEGIN Start_KeyTask */
    /* Infinite loop */
    for(;;)
    {
        button_ticks();
        osDelay(10);
    }
    /* USER CODE END Start_KeyTask */
}