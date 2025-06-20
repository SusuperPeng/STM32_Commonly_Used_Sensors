#ifndef _USART_H_
#define _USART_H_

#include "stm32f10x.h"
 
#define USART1_RX_ENABLE  0
#define USART1_RX_SIZE  256
#define USART1_TX_SIZE  256

#if  USART1_RX_ENABLE
extern char USART1_RX_FLAG;//接受完成标志位
extern unsigned int USART1_RX_LENG ;//接收字节数
extern char USART1_RXBUFF[USART1_RX_SIZE];
#endif



void Usart1_Init(u32 bound);
void u1_printf(char *fmt,...);

#endif
