
#ifndef VER_1_KEY_DRIVER_H
#define VER_1_KEY_DRIVER_H

#include <stdint.h>
#include <string.h>



// 配置常量 - 可根据需求修改
#define TICKS_INTERVAL          5    // 单位ms - 定时器中断间隔时间
#define DEBOUNCE_TICKS          3    // 最大值7 (0 ~ 7) - 按键消抖滤波次数
#define SHORT_TICKS             (300 / TICKS_INTERVAL)   // 短按判定阈值
#define LONG_TICKS              (1000 / TICKS_INTERVAL)  // 长按判定阈值
#define PRESS_REPEAT_MAX_NUM    15   // 重复计数器最大值

// 编译时检查：消抖计数器为3位变量，最大值为7
#if DEBOUNCE_TICKS > 7
  #error "DEBOUNCE_TICKS exceeds 3-bit field maximum (7)"
#endif

// 结构体前置声明
typedef struct _Button Button;

// 按键回调函数类型定义
typedef void (*BtnCallback)(Button* handle, void* user_data);

// 按键事件类型枚举
typedef enum {
	BTN_PRESS_DOWN = 0,     // 按键按下
	BTN_PRESS_UP,           // 按键松开
	BTN_PRESS_REPEAT,       // 检测到重复按下
	BTN_SINGLE_CLICK,       // 单击完成
	BTN_DOUBLE_CLICK,       // 双击完成
	BTN_LONG_PRESS_START,   // 长按开始
	BTN_LONG_PRESS_HOLD,    // 长按保持触发
	BTN_EVENT_COUNT,        // 事件总数量
	BTN_NONE_PRESS          // 无按键事件
} ButtonEvent;

// 按键状态机状态枚举
typedef enum {
	BTN_STATE_IDLE = 0,     // 空闲状态
	BTN_STATE_PRESS,        // 按下状态
	BTN_STATE_RELEASE,      // 松开状态，等待超时判断
	BTN_STATE_REPEAT,       // 重复按下状态
	BTN_STATE_LONG_HOLD     // 长按保持状态
} ButtonState;

// 按键结构体
struct _Button {
	uint16_t ticks;                     // 计时计数器
	uint8_t  repeat : 4;                // 重复计数 (0-15)
	uint8_t  event : 4;                 // 当前事件 (0-15)
	uint8_t  state : 3;                 // 状态机状态 (0-7)
	uint8_t  debounce_cnt : 3;          // 消抖计数器 (0-7)
	uint8_t  active_level : 1;          // 按键有效电平 (0或1)
	uint8_t  button_level : 1;          // 当前按键电平
	uint8_t  button_id;                 // 按键编号
	uint8_t  (*hal_button_level)(uint8_t button_id);  // 读取GPIO电平的底层函数
	BtnCallback cb[BTN_EVENT_COUNT];    // 回调函数数组
	void*    user_data;                 // 传递给回调函数的用户数据指针
	Button* next;                       // 链表中的下一个按键节点
};

// 可选功能：RTOS环境下的线程安全支持
// 在包含此头文件前，定义 MULTIBUTTON_THREAD_SAFE 并实现 MULTIBUTTON_LOCK()/MULTIBUTTON_UNLOCK()
// 宏定义，即可开启链表操作的线程安全功能
//
// 注意：回调函数在锁外部执行，因此使用普通(非递归)互斥锁即可保证安全
// 回调函数中可以自由调用 button_stop()/button_start()，不会产生死锁
//
// 使用示例：
//   #define MULTIBUTTON_THREAD_SAFE
//   #define MULTIBUTTON_LOCK()   osMutexAcquire(btn_mutex, osWaitForever)
//   #define MULTIBUTTON_UNLOCK() osMutexRelease(btn_mutex)
//   #include "multi_button.h"
#ifdef MULTIBUTTON_THREAD_SAFE
  #if !defined(MULTIBUTTON_LOCK) || !defined(MULTIBUTTON_UNLOCK)
    #error "Define MULTIBUTTON_LOCK() and MULTIBUTTON_UNLOCK() when using MULTIBUTTON_THREAD_SAFE"
  #endif
#else
  #define MULTIBUTTON_LOCK()
  #define MULTIBUTTON_UNLOCK()
#endif

#ifdef __cplusplus
extern "C" {
#endif

// 对外公开API函数
void button_init(Button* handle, uint8_t(*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id);
void button_attach(Button* handle, ButtonEvent event, BtnCallback cb, void* user_data);
void button_detach(Button* handle, ButtonEvent event);
ButtonEvent button_get_event(Button* handle);
int  button_start(Button* handle);
void button_stop(Button* handle);
void button_ticks(void);

// 工具函数
uint8_t button_get_repeat_count(Button* handle);
void button_reset(Button* handle);
int button_is_pressed(Button* handle);

#ifdef __cplusplus
}
#endif

/*
### 2. 定义按键实例
```c
static Button btn1;
```

### 3. 实现 GPIO 读取函数
```c
uint8_t read_button_gpio(uint8_t button_id)
{
	switch (button_id) {
		case 1:
			return HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
		default:
			return 0;
	}
}
```

### 4. 初始化按键
```c
// 初始化按键 (active_level: 0=低电平有效, 1=高电平有效)
button_init(&btn1, read_button_gpio, 0, 1);
```

### 5. 注册事件回调
```c
void btn1_single_click_handler(Button* btn, void* user_data)
{
	printf("Button 1: Single Click\n");
}

button_attach(&btn1, BTN_SINGLE_CLICK, btn1_single_click_handler, NULL);
```

### 6. 启动按键处理
```c
button_start(&btn1);
```

### 7. 定时调用处理函数
```c
// 在 5ms 定时器中断中调用
void timer_5ms_interrupt_handler(void)
{
	button_ticks();
}*/



#endif //VER_1_KEY_DRIVER_H