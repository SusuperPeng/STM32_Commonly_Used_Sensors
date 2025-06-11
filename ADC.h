#ifndef __ADC_MULTI_H
#define __ADC_MULTI_H


void ADC_GPIO_Init(void);
void ADC_WorkMode_Init(void);
void ADC_ReInit(void);

uint16_t Read_ADC_Channel(uint8_t channel);

extern uint16_t adc_value[2];  // 存放两个通道的ADC值


#endif 