#ifndef   _DHT11_H_
#define   _DHT11_H_

#include "sys.h"
#include "stm32f10x.h"


void DHT11_Start(void);
uint8_t DHT11_Check(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi);

//uint8_t Read_Temperature(void);
//uint8_t Read_Humidity(void);

#endif