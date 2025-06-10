#ifndef __ADC_MULTI_H
#define __ADC_MULTI_H

#include "stm32f10x.h"

#define ADC_CHANNEL_NUM 3

extern volatile uint16_t ADC_ConvertedValues[ADC_CHANNEL_NUM];

/**
 * @brief  ADC多通道初始化函数，配置3个通道，开启DMA循环采样
 */
void ADC_MultiChannel_Init(void);

#endif /* __ADC_MULTI_H */
