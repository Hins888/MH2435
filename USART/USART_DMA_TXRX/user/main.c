/**
  ******************************************************************************
  * @file    main.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   串口DMA接发测试
  ******************************************************************************
  */
#include "./usart/bsp_debug_usart.h"
int main(void)
{
	uint8_t recv_data;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	/*初始化USART 配置模式为 115200 8-N-1*/
    Debug_USART_Config();
	
	printf("UART Demo V1.0.\n");
	
	//DMA初始化
	Debug_USART_DMA_Config();
	
	//使能USART DMA收发功能
	USART_DMACmd(DEBUG_USART,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);
	
	//使能DMA发送流
	DMA_Cmd(DEBUG_USART_RX_DMA_STREAM, ENABLE);  
	
	while(1)
	{
		//等待DMA接收到DEBUG_DATA_SIZE字节数据，然后将接收到的数据通过DMA发送
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
