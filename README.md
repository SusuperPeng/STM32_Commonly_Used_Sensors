//OUT→PA1

//主函数做一个示例

#include "DHT11.h"

int mian(){

uint8_t temperature = 0;
uint8_t humidity = 0;
uint8_t res = 0;

DHT11_GPIO_Init();

while(1){

res=DHT11_Read_Data(temperature, humidity);    //res返回标志位

//直接对temperature和humidity两个参数采集就行，温湿度数据已经存放在两个参数里

    }
  
}





 
