#include "soft_i2c.h"
#include "tm1650.h"

/* -------------------- 内部常量与静态变量 -------------------- */
#define TM1650_CMD_DISP_CFG  0x48

// 4位数码管显存地址：下标0~3对应左数第1~4位
static const uint8_t g_dig_addr[4] = {0x68, 0x6A, 0x6C, 0x6E};

// 共阴段码表，索引与 TM1650_Number_t 一一对应
static const uint8_t g_seg_table[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F,
    0x00    // 熄灭
};

static TM1650_Brightness_t g_current_brightness;

/* -------------------- 内部底层接口 -------------------- */
static void TM1650_WriteSeg(uint8_t dig_idx, uint8_t raw_seg)
{
    if (dig_idx >= 4) return;

    I2C_Start();
    I2C_WriteByte(g_dig_addr[dig_idx]);
    I2C_WriteByte(raw_seg);
    I2C_Stop();
}

/* -------------------- 对外接口函数 -------------------- */
void TM1650_Init(TM1650_Brightness_t brightness)
{
    I2C_Init();

    if (brightness > TM1650_BRIGHTNESS_7) {
        brightness = TM1650_BRIGHTNESS_7;
    }
    g_current_brightness = brightness;

    TM1650_SetDisplay(TM1650_DISPLAY_ON);
    TM1650_Clear();
}

void TM1650_SetDisplay(TM1650_DisplayState_t state)
{
    uint8_t reg_brightness;

    reg_brightness = (g_current_brightness + 1) & 0x07;

    // 命令字节构造：
    // bit7: 保留(0)
    // bit6~bit4: 亮度值
    // bit3: 0=8段显示模式
    // bit2~bit1: 保留(0)
    // bit0: 显示开关(1=开, 0=关)
    uint8_t cmd = (reg_brightness << 4) | (state == TM1650_DISPLAY_ON ? 0x01 : 0x00);
    // 显式清除bit3、bit2、bit1，确保8段模式、保留位为0
    cmd &= ~( (1<<3) | (1<<2) | (1<<1) );

    I2C_Start();
    I2C_WriteByte(TM1650_CMD_DISP_CFG);
    I2C_WriteByte(cmd);
    I2C_Stop();

}

/**
 * @brief  显示4位数字，可指定小数点位置，自动消隐前导零
 * @param  number: 0~9999 的整数
 * @param  dp_pos: 小数点位置 0~3，0为无小数点
 */
void TM1650_DisplayNum(uint16_t number, uint8_t dp_pos)
{
    uint8_t dig[4];
    uint8_t seg;
    uint8_t i;
    uint8_t leading_zero = 1; // 前导零标记

    // 参数合法性修正
    if (number > 9999) number = 9999;
    if (dp_pos > 3) dp_pos = 0;

    // 拆分4位数字：dig[0]=千位(左1)，dig[3]=个位(右4)
    dig[0] = number / 1000;
    dig[1] = (number / 100) % 10;
    dig[2] = (number / 10) % 10;
    dig[3] = number % 10;

    // 逐位显示
    for (i = 0; i < 4; i++)
    {
        // 前导零消隐：遇到非零 或 小数点前最后一位 时停止消隐
        if (leading_zero && dig[i] == 0 && i != dp_pos - 1)
        {
            TM1650_WriteSeg(i, g_seg_table[TM1650_NUM_OFF]);
        }
        else
        {
            leading_zero = 0;
            seg = g_seg_table[dig[i]];

            // 当前位是小数点所在位，点亮小数点
            if (dp_pos != 0 && i == dp_pos - 1)
            {
                seg |= 0x80;
            }

            TM1650_WriteSeg(i, seg);
        }
    }
}

void TM1650_Clear(void)
{
    uint8_t i;
    for (i = 0; i < 4; i++)
    {
        TM1650_WriteSeg(i, g_seg_table[TM1650_NUM_OFF]);
    }
}