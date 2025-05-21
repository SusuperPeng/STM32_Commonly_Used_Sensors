#include "DHT11.h"
#include "stm32f10x.h"
#include "delay.h"  


// DHT11连接在 PA1，以下宏用于配置其输入输出模式
#define DHT11_PORT GPIOA
#define DHT11_PIN  GPIO_Pin_1

#define DHT11_IN()  { DHT11_PORT->CRL &= 0xFFFFFF0F; DHT11_PORT->CRL |= 0x00000080; } // PA1输入模式（浮空）
#define DHT11_OUT() { DHT11_PORT->CRL &= 0xFFFFFF0F; DHT11_PORT->CRL |= 0x00000030; } // PA1推挽输出

#define DHT11_DQ_H  GPIO_SetBits(DHT11_PORT, DHT11_PIN)
#define DHT11_DQ_L  GPIO_ResetBits(DHT11_PORT, DHT11_PIN)
#define DHT11_READ  GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)

void DHT11_GPIO_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}



/****************************************************************************
函数名称：void DHT11_Start(void)
函数参数：无
函数返回值：无
描述：MCU启动DHT11+MCU等待DHT11的数据输入
****************************************************************************/
void DHT11_Start(void)
{
    DHT11_OUT();		//设置为推挽输出模式
    DHT11_DQ_L;			//设置输出模式为0
    Delay_ms(20);       // 拉低至少18ms
    DHT11_DQ_H;			//设置输出模式为1，确保0输出的时间维持在18~30ms之间
    Delay_us(30);       // 拉高 20~40us
    DHT11_IN();         // 切换为输入模式，等待响应
}
/****************************************************************************
函数名称：uint8_t DHT11_Check(void)
函数参数：无
函数返回值：1：失败     0：成功
描述：
	第一个 while：目的是等待DHT11对MCU响应的低电平
	1.在启动信号发出后，若高电平持续时长>=100us，即DHT11未对MCU响应的低电平，DHT11自检失败；返回1
	2.在启动信号发出后，若高电平持续时长<=100us，即DHT11对MCU响应的低电平
	  

	第二个 while：目的是等待DHT11对MCU响应的高电平
	1.在DHT11对MCU响应的低电平后，若低电平持续时长>=100us，即DHT11自检失败；返回1
	2.在DHT11对MCU响应的低电平后，若低电平持续时长<=100us，即DHT11对MCU响应的高电平，自检成功；返回0
****************************************************************************/
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    while(DHT11_READ     &&     retry < 100)  // 等待 DHT11 拉低响应
    {
        retry++;
        Delay_us(1);
    }
    if(retry >= 100) return 1;

    retry = 0;
    while(!DHT11_READ     &&     retry < 100)  // 等待 DHT11 拉高
    {
        retry++;
        Delay_us(1);
    }
    return (retry >= 100) ? 1 : 0;
}
/****************************************************************************
函数名称：uint8_t DHT11_Read_Bit(void)，从DHT11的数据线上读取1位数据
函数参数：无；
函数返回值：0：该位数据为0    1：该位数据为1    0XFF:超时，返回错误标志（非法值）
描述：
	第一个 while：目的等待50us的低电平作为数据传输的初始信号
	1.在DHT11自检测成功后，若高电平持续时长>=100us，
	  即为超时，DHT11未对MCU响应的低电平，DHT11未准备发送一个 bit，返回0XFF非法值
	2.在DHT11自检测成功后，若高电平持续时长<=100us，
	  即DHT11未对MCU响应的低电平，（执行下一个while）来确保输入的数据是1还是0

	第二个 while：目的确定高电平信号的持续时间来判断输入的数据是1还是0
	1.等待接收高电平信号，若低电平持续时长>=100us,即为超时,该数据有问题，返回0XFF非法值
	2.等待接收高电平信号，若低电平持续时长<=100us，则接收到到高电平信号，
	  在40us后再次测量此电平信号来确定改位的数据是0还是1，返回0或1
****************************************************************************/
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;

    // 1. 等待 DHT11 拉低（开始发送位）
    while(DHT11_READ)
    {
        if (++retry > 100)
            return 0xFF;  // 低电平等待超时
        Delay_us(1);
    }

    // 2. 等待 DHT11 拉高（开始计时）
    retry = 0;
    while(!DHT11_READ)
    {
        if (++retry > 100)
            return 0xFF;  // 高电平等待超时
        Delay_us(1);
    }

    // 3. 延迟 40us 后读取电平判断是 0 还是 1
    Delay_us(40);

    return DHT11_READ ? 1 : 0;
}
/****************************************************************************
函数名称：uint8_t DHT11_Read_Byte(void)
函数参数：0XFF数据采集失败
函数返回值：无
描述：采集一个字节的数据
****************************************************************************/
uint8_t DHT11_Read_Byte(void)
{	
	uint8_t i, data = 0,bit;
    for(i = 0; i < 8; i++)
    {
		bit = DHT11_Read_Bit();       // 读取一位
        if(bit == 0xFF) return 0xFF;  // 超时错误
        data <<= 1;				      // 左移准备放下一个 bit
        data |= bit;			      // 累加当前 bit 到低位
    }
    return data;
}
/****************************************************************************
函数名称：uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi)
函数参数：无
函数返回值：1：自检测失败    2：数据检测失败    3：校验失败    0：成功
描述：
****************************************************************************/
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    DHT11_Start();              // 发送开始信号
    if(DHT11_Check()==0)           // 检查应答
	{
		for(i = 0; i < 5; i++)
		{
			buf[i] = DHT11_Read_Byte();
			if(buf[i] == 0xFF)
				return 2;          // 读取超时
		}

		// 校验和：前4个字节之和 == 第5个字节
		if(buf[0] + buf[1] + buf[2] + buf[3] != buf[4])
			return 3;              // 校验失败

		*humi = buf[0];            // 湿度整数部分
		*temp = buf[2];            // 温度整数部分
		}  
	else return 1;              //自检失败
    return 0;                  // 成功
}
///****************************************************************************
//函数名称：uint8_t Read_Temperature(void)
//函数参数：无
//函数返回值：温度数据
//描述：读取温度数据
//****************************************************************************/
//uint8_t temperature = 0;
//uint8_t humidity = 0;

//uint8_t Read_Temperature(void)
//{	
//	uint8_t res;
//	
//	res=DHT11_Read_Data(&temperature, &humidity);
//	
//    return temperature;             // 失败返回特殊错误值
//}
///****************************************************************************
//函数名称：uint8_t Read_Humidity(void)
//函数参数：无
//函数返回值：湿度数据
//描述：读取湿度数据
//****************************************************************************/
//uint8_t Read_Humidity(void)
//{
//	uint8_t res;
//	
//	res=DHT11_Read_Data(&temperature, &humidity);
//	
//    return humidity;             // 失败返回特殊错误值
//}
