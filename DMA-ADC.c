#include "DMA-ADC.h"

/* DMA传输缓冲区，存储3个通道采样值 */
volatile uint16_t ADC_ConvertedValues[ADC_CHANNEL_NUM] = {0};

void ADC_MultiChannel_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* 1. 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* 2. 配置GPIO为模拟输入模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. DMA配置 */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;            // ADC数据寄存器地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValues;     // 存储数据的缓冲区地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                         // 外设到内存
    DMA_InitStructure.DMA_BufferSize = ADC_CHANNEL_NUM;                        // 3个通道数据
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;           // 外设地址不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                    // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 16位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;        // 16位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                            // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                               // 禁止内存到内存模式
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* 4. ADC配置 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;               // 扫描模式使能
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;         // 连续转换使能
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;     // 右对齐
    ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_NUM;       // 3通道
    ADC_Init(ADC1, &ADC_InitStructure);

    /* 配置转换顺序和采样时间 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);

    /* 使能ADC DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* 使能ADC */
    ADC_Cmd(ADC1, ENABLE);

    /* 复位校准 */
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    /* 开始校准 */
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    /* 启动转换 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
