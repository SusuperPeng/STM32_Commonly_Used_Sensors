#ifndef __ADC_MULTI_H
#define __ADC_MULTI_H


void ADC1_Init(void);
// 全局变量由 main 文件定义，在这里声明（确保加 volatile）
extern volatile uint16_t ADC_Value;

#endif 