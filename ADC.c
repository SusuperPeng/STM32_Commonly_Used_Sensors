#include "stm32f10x.h"

uint16_t adc_value[2]; // 用于存储两个通道的采样值

void ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // PA0 (IN0), PA1 (IN1)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_WorkMode_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    // 打开 ADC1 和 AFIO 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);

    // 设置 ADC 时钟为 PCLK2/6 = 12MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    // 配置 ADC 模式
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;            // 扫描多个通道
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;     // 关闭连续模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;                 // 配置两路通道
    ADC_Init(ADC1, &ADC_InitStructure);


    // 启动 ADC1
    ADC_Cmd(ADC1, ENABLE);
}

// 复位ADC+设置ADC工作模式+校准ADC 
void ADC_ReInit(void)
{
    //复位 ADC1 状态
    ADC_DeInit(ADC1);
    //配置 ADC 的GPIO引脚
    ADC_GPIO_Init();
    //配置 ADC工作模式
    ADC_WorkMode_Init();

   
    //复位 ADC 校准寄存器
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    //启动校准 ADC
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}


uint16_t Read_ADC_Channel(uint8_t channel)
{
    // 配置指定 ADC 通道
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_55Cycles5);

    // 启动转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // 等待转换完成
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    // 返回转换结果
    return ADC_GetConversionValue(ADC1);
}
