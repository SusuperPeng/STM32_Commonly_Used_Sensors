#include "usart.h"
#include "stdio.h"      //包含需要的头文件
#include "stdarg.h"		//包含需要的头文件 
#include "string.h"     //包含需要的头文件

#if USART1_RX_ENABLE	
char USART1_RX_FLAG = 0;//接受完成标志位
unsigned int USART1_RX_LENG = 0;//接收字节数
char USART1_RXBUFF[USART1_RX_SIZE] = {0};
#endif

/****************************************************************************
函数名称：void Usart1_Init(u32 bound)
函数参数：u32 bound  波特率
函数返回值：无
描述：串口初始化  PA9:TX   PA10 RX
****************************************************************************/
void Usart1_Init(u32 bound)
{
    // 1. 定义结构体
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
#if USART1_RX_ENABLE
    NVIC_InitTypeDef NVIC_InitStruct;
#endif

    // 2. 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    // GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);   // USART1时钟

    // 3. 配置 PA9 (TX) -> 复用推挽输出
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 4. 配置 PA10 (RX) -> 浮空输入
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 5. USART 配置
    USART_InitStruct.USART_BaudRate            = bound;                     // 波特率
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;      // 8位数据
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;         // 1位停止位
    USART_InitStruct.USART_Parity              = USART_Parity_No;          // 无奇偶校验
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
#if USART1_RX_ENABLE
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;           // 发送 + 接收
#else
    USART_InitStruct.USART_Mode = USART_Mode_Tx;                           // 仅发送
#endif
    USART_Init(USART1, &USART_InitStruct);         // 初始化串口
    USART_Cmd(USART1, ENABLE);                     // 使能串口

#if USART1_RX_ENABLE
    // 6. 配置中断优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 中断优先级组设置

    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 7. 开启串口接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能接收中断
#endif
}
/****************************************************************************
函数名称：void usart1_printf(char *fmt,...)
函数参数：可变传参
函数返回值：无
描述：格式化输出字符串参数
****************************************************************************/
__align(8) char USART1_TXBUFF[USART1_TX_SIZE];		// 发送缓冲区，8字节对齐可提高DMA效率（如将来使用）

void u1_printf(char *fmt,...)
{
	//定义两个变量：
	//length：保存最终要发送字符串的长度。
	//i：用于循环发送每一个字符。
	unsigned int length = 0,i = 0;
	
	va_list ap;  //构建传参列表
	va_start(ap,fmt);//使用参数fmt初始化ap列表、
	vsprintf(USART1_TXBUFF,fmt,ap);
	va_end(ap);//清理（必要）
	
	length = strlen((const char *)USART1_TXBUFF);
	while((USART1->SR & 0x40)== 0);//发送完成标志位 0 未完成
	for(i = 0;i < length;i++)
	{
		USART1->DR = USART1_TXBUFF[i];
		while((USART1->SR & 0x40)== 0);
	}
	
}
/***************************************************
函数名称：USART1_IRQHandler
描述：串口1接收中断处理
***************************************************/
void USART1_IRQHandler(void)
{
#if USART1_RX_ENABLE
    if ( (USART1_RX_FLAG == 0) && (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) )
    {
        char rx = USART_ReceiveData(USART1);  // 读取接收数据

        // 接收缓冲区存储
        if (USART1_RX_LENG < USART1_RX_SIZE)
        {
            USART1_RXBUFF[USART1_RX_LENG++] = rx;

            // 可选：在这里判断结束符（比如 '\n'）作为接收完成标志
            if (rx == 0x0A) // 可根据协议换成 '\r' 或 0x0A/0x0D           
                USART1_RX_FLAG = 1;  // 设置接收完成标志           
        }
        else       
            USART1_RX_LENG = 0;  // 溢出保护，清零重新接收
        
		
		//采用空闲中断读取作为接收完成标志，适用于手动的单发数据
		//不能采用USART_ClearITPendingBit(USART3, USART_IT_IDLE); // ? 无效！不能清除 IDLE 中断
//		if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET)
//		{
//		a=USART3->SR;  		//USART_IT_IDLE 空闲中断标志不会清除,程序会一直反复进入中断
//		a=USART3->DR;		//标志位清零
//		USART1_RX_FLAG = 1;
//		}
		
		
        // 清除中断标志
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
#endif
}

/*  	
	if(USART1_RX_FLAG == 1)
	{		
		u1_printf("接收到的数据是：%s".USART1_RXBUFF);
		
		if(USART3_Rcv_Buff[2] == 'A')//前
		{
			//功能函数	
		}
		

		USART1_RX_FLAG=0;
		memset(USART3_Rcv_Buff,0,256);
		USART1_RX_LENG=0;
	}

*/



