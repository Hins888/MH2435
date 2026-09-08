/**
  ******************************************************************************
  * @file    main.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   串口接收FIFO测试,
  *			 对端一次发送数据量不大于FIFO深度(8)时不会出现数据丢失
  ******************************************************************************
  */
#include "./usart/bsp_debug_usart.h"
int main(void)
{
	uint8_t recv_data;
	/*初始化USART 配置模式为 115200 8-N-1 & 使能FIFO*/
    Debug_USART_Config();
	
	printf("UART FIFO Demo V1.0.\n");

	while(1)
	{
		//等待数据输入
		recv_data = Usart_ReadByte(DEBUG_USART);	
		
		Usart_SendString(DEBUG_USART,(uint8_t *)"Usart recv data:");
		Usart_SendByte(DEBUG_USART,recv_data);
		//只会发送第一个字节数据（换行符）
		Usart_SendStr_length(DEBUG_USART,(uint8_t *)"\ntest",1);
	}
}
