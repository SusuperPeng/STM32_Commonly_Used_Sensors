#include "stm32f10x.h"
#include "ADC.h"


// 配置 PA6为模拟输入
void ADC_GPIO_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}



// 配置 ADC 工作模式
void ADC_WorkMode_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;               // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                    // 关闭扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;               // 连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;           // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                          // 转换通道数 = 1
   
     ADC_Init(ADC1, &ADC_InitStructure);

    // 使能 ADC1 时钟
    //只有F1系列需要这样设计
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADC 时钟 = PCLK2/6（最大14MHz）
    
    // 配置通道顺序、采样时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_55Cycles5); // PA6

    ADC_Cmd(ADC1, ENABLE); // 使能 ADC1   
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

//读取单通道ADC值
int Read_ADC_Values(void)
{
    int ADC_Values;
    // 软件触发 ADC转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // 等待EOC完成
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    ADC_Values = ADC_GetConversionValue(ADC1);
    
    // 转换完成，软件关闭 ADC转换
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    
    return ADC_Values;
}
