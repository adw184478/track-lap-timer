#include "soft_i2c.h"
#include "delay.h"

/* -------------------- GPIO 初始化（开漏 + 上拉） -------------------- */
/**
 * @brief 配置 SCL 和 SDA 为开漏输出，并打开端口时钟
 */
void I2C_GPIO_Init(void)
{
    //CubeMX配置
}

/* -------------------- I2C 总线操作 -------------------- */
void I2C_Init(void)
{
    //延时初始化
    DWT_Delay_Init();

    I2C_SCL_HIGH();
    I2C_SDA_HIGH();
    delay_us(50);
    delay_us(50);
}

void I2C_Start(void)
{
    I2C_SDA_HIGH();
    I2C_SCL_HIGH();
    delay_us(50);
    I2C_SDA_LOW();
    delay_us(50);
    I2C_SCL_LOW();
    delay_us(50);
}

void I2C_Stop(void)
{
    I2C_SDA_LOW();
    I2C_SCL_HIGH();
    delay_us(50);
    I2C_SDA_HIGH();
    delay_us(50);
}

uint8_t I2C_WriteByte(uint8_t data)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        if (data & 0x80)
            I2C_SDA_HIGH();
        else
            I2C_SDA_LOW();

        delay_us(50);
        I2C_SCL_HIGH();
        delay_us(50);
        I2C_SCL_LOW();
        delay_us(50);

        data <<= 1;
    }

    // 读取应答
    I2C_SDA_HIGH();
    delay_us(50);
    I2C_SCL_HIGH();
    delay_us(50);
    uint8_t ack = I2C_SDA_READ();
    I2C_SCL_LOW();
    delay_us(50);

    return ack;
}

uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t i, data = 0;

    I2C_SDA_HIGH();   // 释放SDA
    delay_us(50);

    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        I2C_SCL_HIGH();
        delay_us(50);
        if (I2C_SDA_READ())
            data |= 0x01;
        I2C_SCL_LOW();
        delay_us(50);
    }

    // 发送应答/非应答
    if (ack)
        I2C_SDA_LOW();   // ACK
    else
        I2C_SDA_HIGH();  // NACK

    delay_us(50);
    I2C_SCL_HIGH();
    delay_us(50);
    I2C_SCL_LOW();
    delay_us(50);
    I2C_SDA_HIGH();      // 释放SDA

    return data;
}