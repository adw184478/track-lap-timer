#include "key_driver.h"


#include "timer.h"

// 宏定义：执行按键事件回调函数（带空指针检查，自动传递用户数据）
#define EVENT_CB(ev)   do { if (handle->cb[ev]) handle->cb[ev](handle, handle->user_data); } while(0)

// Button handle list head
static Button* head_handle = NULL;

// Forward declarations
static void button_handler(Button* handle);
static inline uint8_t button_read_level(Button* handle);

/**
  * @brief  初始化按键结构体
  * @param  handle: 按键结构体指针
  * @param  pin_level: 读取按键对应GPIO电平的函数
  * @param  active_level: 按键按下时的有效电平
  * @param  button_id: 按键编号
  * @retval 无
  */
void button_init(Button* handle, uint8_t(*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id)
{
	if (!handle || !pin_level) return;  // 参数合法性检查

	memset(handle, 0, sizeof(Button));
	handle->event = (uint8_t)BTN_NONE_PRESS;            // 初始无按键事件
	handle->hal_button_level = pin_level;               // 注册读取GPIO电平的函数
	handle->button_level = !active_level;               // 初始电平设置为与有效电平相反
	handle->active_level = active_level;                // 保存按键有效电平
	handle->button_id = button_id;                     // 保存按键编号
	handle->state = BTN_STATE_IDLE;                     // 设置初始状态为空闲
	// user_data 已通过 memset 清零
}

/**
  * @brief  绑定按键事件的回调函数
  * @param  handle: 按键结构体指针
  * @param  event: 要绑定的触发事件类型
  * @param  cb: 事件触发后要执行的回调函数
  * @param  user_data: 传递给回调函数的用户数据指针（每个按键独立保存）
  * @retval 无
  */
void button_attach(Button* handle, ButtonEvent event, BtnCallback cb, void* user_data)
{
	// 参数合法性检查：按键句柄为空 或 事件编号超出范围，则直接退出
	if (!handle || event >= BTN_EVENT_COUNT) return;

	// 将回调函数保存到按键结构体的对应事件位置
	handle->cb[event] = cb;

	// 将用户数据保存到按键结构体
	handle->user_data = user_data;
}

/**
  * @brief  Detach the button event callback function
  * @param  handle: the button handle struct
  * @param  event: trigger event type
  * @retval None
  */
void button_detach(Button* handle, ButtonEvent event)
{
	if (!handle || event >= BTN_EVENT_COUNT) return;  // parameter validation
	handle->cb[event] = NULL;
}

/**
  * @brief  Get the button event that happened
  * @param  handle: the button handle struct
  * @retval button event
  */
ButtonEvent button_get_event(Button* handle)
{
	if (!handle) return BTN_NONE_PRESS;
	return (ButtonEvent)(handle->event);
}

/**
  * @brief  Get the repeat count of button presses
  * @param  handle: the button handle struct
  * @retval repeat count
  */
uint8_t button_get_repeat_count(Button* handle)
{
	if (!handle) return 0;
	return handle->repeat;
}

/**
  * @brief  Reset button state to idle
  * @param  handle: the button handle struct
  * @retval None
  */
void button_reset(Button* handle)
{
	if (!handle) return;
	handle->state = BTN_STATE_IDLE;
	handle->ticks = 0;
	handle->repeat = 0;
	handle->event = (uint8_t)BTN_NONE_PRESS;
	handle->debounce_cnt = 0;
}

/**
  * @brief  Check if button is currently pressed
  * @param  handle: the button handle struct
  * @retval 1: pressed, 0: not pressed, -1: error
  */
int button_is_pressed(Button* handle)
{
	if (!handle) return -1;
	return (handle->button_level == handle->active_level) ? 1 : 0;
}

/**
  * @brief  Read button level with inline optimization
  * @param  handle: the button handle struct
  * @retval button level
  */
static inline uint8_t button_read_level(Button* handle)
{
	return handle->hal_button_level(handle->button_id);
}

/**
  * @brief  按键驱动核心函数，状态机实现
  * @param  handle: 按键结构体指针
  * @retval 无
  */
static void button_handler(Button* handle)
{
	uint8_t read_gpio_level = button_read_level(handle);

	// 如果不是空闲状态，就进行计时（防止溢出）
	if (handle->state > BTN_STATE_IDLE) {
		if (handle->ticks < UINT16_MAX) {
			handle->ticks++;
		}
	}

	/* 按键消抖处理 */
	// 如果读到的电平与上一次不同，开始消抖计数
	if (read_gpio_level != handle->button_level) {
		// 连续多次读到相同新电平，才认为电平真正变化
		if (++(handle->debounce_cnt) >= DEBOUNCE_TICKS) {
			handle->button_level = read_gpio_level;
			handle->debounce_cnt = 0;
		}
	} else {
		// 电平没有变化，消抖计数器清零
		handle->debounce_cnt = 0;
	}

	/* 状态机主逻辑 */
	switch (handle->state) {
	case BTN_STATE_IDLE:
		// 空闲状态：检测是否按下
		if (handle->button_level == handle->active_level) {
			// 检测到按键按下
			handle->event = (uint8_t)BTN_PRESS_DOWN;

			EVENT_CB(BTN_PRESS_DOWN);
			handle->ticks = 0;
			handle->repeat = 1;
			handle->state = BTN_STATE_PRESS;
		} else {
			handle->event = (uint8_t)BTN_NONE_PRESS;
		}
		break;

	case BTN_STATE_PRESS:
		// 按下状态：判断松开 或 长按
		if (handle->button_level != handle->active_level) {
			// 按键松开
			handle->event = (uint8_t)BTN_PRESS_UP;
			EVENT_CB(BTN_PRESS_UP);
			handle->ticks = 0;
			handle->state = BTN_STATE_RELEASE;
		} else if (handle->ticks > LONG_TICKS) {
			// 长按判定
			handle->event = (uint8_t)BTN_LONG_PRESS_START;
			EVENT_CB(BTN_LONG_PRESS_START);
			handle->state = BTN_STATE_LONG_HOLD;
		}
		break;

	case BTN_STATE_RELEASE:
		// 松开状态：等待判断单击/双击/重复按
		if (handle->button_level == handle->active_level) {
			// 再次按下
			handle->event = (uint8_t)BTN_PRESS_DOWN;
			EVENT_CB(BTN_PRESS_DOWN);
			if (handle->repeat < PRESS_REPEAT_MAX_NUM) {
				handle->repeat++;
			}
			handle->event = (uint8_t)BTN_PRESS_REPEAT;
			EVENT_CB(BTN_PRESS_REPEAT);
			handle->ticks = 0;
			handle->state = BTN_STATE_REPEAT;
		} else if (handle->ticks > SHORT_TICKS) {
			// 超时，判断点击类型
			if (handle->repeat == 1) {
				handle->event = (uint8_t)BTN_SINGLE_CLICK;
				EVENT_CB(BTN_SINGLE_CLICK);
			} else if (handle->repeat == 2) {
				handle->event = (uint8_t)BTN_DOUBLE_CLICK;
				EVENT_CB(BTN_DOUBLE_CLICK);
			}
			handle->state = BTN_STATE_IDLE;
		}
		break;

	case BTN_STATE_REPEAT:
		// 重复按下状态
		if (handle->button_level != handle->active_level) {
			// 松开
			handle->event = (uint8_t)BTN_PRESS_UP;
			EVENT_CB(BTN_PRESS_UP);
			if (handle->ticks < SHORT_TICKS) {
				handle->ticks = 0;
				handle->state = BTN_STATE_RELEASE;
			} else {
				handle->state = BTN_STATE_IDLE;
			}
		} else if (handle->ticks > SHORT_TICKS) {
			// 按住太久，当作普通按下
			handle->ticks = 0;
			handle->repeat = 0;
			handle->state = BTN_STATE_PRESS;
		}
		break;

	case BTN_STATE_LONG_HOLD:
		// 长按保持状态
		if (handle->button_level == handle->active_level) {
			// 持续按住
			handle->event = (uint8_t)BTN_LONG_PRESS_HOLD;
			EVENT_CB(BTN_LONG_PRESS_HOLD);
		} else {
			// 长按后松开
			handle->event = (uint8_t)BTN_PRESS_UP;
			EVENT_CB(BTN_PRESS_UP);
			handle->state = BTN_STATE_IDLE;
		}
		break;

	default:
		// 错误状态，重置为空闲
		handle->state = BTN_STATE_IDLE;
		break;
	}
}

/**
  * @brief  启动按键，将按键句柄添加到工作链表中
  * @param  handle: 目标按键结构体指针
  * @retval 0: 成功  -1: 已存在  -2: 参数无效
  */
int button_start(Button* handle)
{
	if (!handle) return -2;  // invalid parameter

	MULTIBUTTON_LOCK();
	Button* target = head_handle;
	while (target) {
		if (target == handle) {
			MULTIBUTTON_UNLOCK();
			return -1;  // already exist
		}
		target = target->next;
	}

	handle->next = head_handle;
	head_handle = handle;
	MULTIBUTTON_UNLOCK();
	return 0;
}

/**
  * @brief  Stop the button work, remove the handle from work list
  * @param  handle: target handle struct
  * @retval None
  */
void button_stop(Button* handle)
{
	if (!handle) return;  // parameter validation

	MULTIBUTTON_LOCK();
	Button** curr;
	for (curr = &head_handle; *curr; ) {
		Button* entry = *curr;
		if (entry == handle) {
			*curr = entry->next;
			entry->next = NULL;  // clear next pointer
			MULTIBUTTON_UNLOCK();
			return;
		} else {
			curr = &entry->next;
		}
	}
	MULTIBUTTON_UNLOCK();
}

/**
  * @brief  后台计时函数，需要定时器每隔 5ms 调用一次
  *         回调函数在锁之外执行，因此可以安全地
  *         在回调中调用 button_start()/button_stop() 而不会死锁
  * @param  无
  * @retval 无
  */
void button_ticks(void)
{
	Button* target;
	Button* next;
	MULTIBUTTON_LOCK();
	target = head_handle;
	MULTIBUTTON_UNLOCK();
	while (target) {
		MULTIBUTTON_LOCK();
		next = target->next;
		MULTIBUTTON_UNLOCK();
		button_handler(target);
		target = next;
	}
}