#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"


void ADC_GPIO_Init(void);
void ADC_WorkMode_Init(void);
void ADC_ReInit(void);
int Read_ADC_Values(void);

#endif
