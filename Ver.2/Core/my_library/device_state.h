#ifndef VER_2_DEVICE_STATE_H
#define VER_2_DEVICE_STATE_H

#include <stdint.h>

/*===========================================================================
 * 枚举定义
 *===========================================================================*/

/**
 * @brief 计时触发模式
 */
typedef enum {
    TIMING_AUTO = 0,                 /* 自动计时：红外传感器触发启停 */
    TIMING_MANUAL,                   /* 手动计时：按键触发启停      */
} TimingMode_t;

/**
 * @brief 设备运行状态
 */
typedef enum {
    STATE_READY,                    /* 准备状态    */
    STATE_TIMING,                   /* 计时状态    */
    STATE_FINISH                    /* 结束状态    */
} SystemState_t;

/**
 * @brief 数码管显示模式
 */
typedef enum {
    DISP_BATTERY = 0,               /* 显示电量    */
    DISP_TIME,                      /* 显示时间    */
} DisplayMode_t;

/**
 * @brief 按键事件类型（队列传递用）
 */
typedef enum {
    KEY_MODE = 0,                   /* 模式键      */
    KEY_READY,                      /* 准备键      */
    KEY_RESET                       /* 复位键      */
} KeyEvent_t;

/**
 * @brief 设备事件类型（队列传递用）
 */
typedef enum {
    KEY = 0,                         /* 按键       */
    E18_D80NK,                       /* 红外传感器  */
    BUZZER,                          /* 蜂鸣器     */
    BATTERY,                         /* 电池       */
    LED_DISPLAY,                     /* 数码管     */
    TIMER                            /* 定时器     */
} Device_t;

/**
 * @brief 统一队列消息结构体
 */
typedef struct {
    Device_t                device;  /* 设备                  */
    union {
        KeyEvent_t          key_id;  /* 按键事件：按键编号       */
        uint8_t          e18_touch;  /* 红外传感器              */
        uint8_t             buzzer;  /* 蜂鸣器事件              */
        uint8_t            battery;  /* 电量事件：百分比         */
        struct {
            uint16_t           num;  /* 要显示的4位数字          */
            uint8_t        dot_pos;  /* 小数点位置：0=无小数点    */
        } display;
        uint8_t           time;      /*定时器                  */
    } data;
} Device_Queue_t;


/*===========================================================================
 * 设备核心数据结构
 *===========================================================================*/

/**
 * @brief 设备全局数据
 */
typedef struct {
    /* 运行状态 */
    TimingMode_t    timing_mode;            /* 计时模式            */
    SystemState_t   state;                  /* 当前系统状态         */
    DisplayMode_t   display_mode;           /* 当前数码管显示模式    */

    /* 圈速记录 */
    uint8_t         target_count;           /* 目标圈数             */
    uint8_t         current_count;          /* 当前圈数             */
    uint32_t        total_time_ms;          /* 总计时长（毫秒）       */

    /* 电量信息 */
    uint8_t         battery_percent;        /*电池电量百分比 (0~100) */
} Device_Data_t;



/*===========================================================================
 * 接口函数
 *===========================================================================*/

/**
 * @brief 初始化设备数据结构（上电时调用一次）
 */
void DeviceData_Init(TimingMode_t timing_mode, uint8_t target_count );


/**
 * @brief 按键事件统一处理函数（由状态机任务调用，内部根据状态和事件修改 g_device）
 */
void DeviceData_HandleKeyEvent(KeyEvent_t event);

/**
 * @brief 复位所有记录（总时间、圈数）
 */
void DeviceData_Reset(void);

/**
 * @brief 红外传感器触发处理
 */
void DeviceData_HandleE18Event(void);

#endif //VER_2_DEVICE_STATE_H