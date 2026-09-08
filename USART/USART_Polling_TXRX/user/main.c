/**
  ******************************************************************************
  * @file    main.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   串口轮询接发测试
  ******************************************************************************
  */
#include "./usart/bsp_debug_usart.h"
int main(void)
{
	uint8_t recv_data;
	/*初始化USART 配置模式为 115200 8-N-1*/
    Debug_USART_Config();
	
	printf("UART Demo V1.0.\n");

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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif
