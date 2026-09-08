/**
  ******************************************************************************
  * @file    main.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   串口中断接收测试
  ******************************************************************************
  */
#include "./usart/bsp_debug_usart.h"


uint8_t Rxflag=0;
uint8_t ucTemp;
int main(void)
{
	uint8_t ucaRxBuf[256];
	uint16_t usRxCount=0; 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	/*初始化USART 配置模式为 115200 8-N-1*/
    Debug_USART_Config();
	
	printf("UART Demo V1.0.\n");

	while(1)
	{	
		/* 
			接收DEBUG_USART口的数据，分析并处理 
			可以将此段代码封装为一个函数，在主程序其它流程调用
		*/
		if(Rxflag)
		{
			if (usRxCount < sizeof(ucaRxBuf))
			{
				ucaRxBuf[usRxCount++] = ucTemp;
			}
			else
			{
				usRxCount = 0;
			}
			
			/* 简单的通信协议，遇到回车换行符认为1个命令帧，可自行加其它判断实现自定义命令 */
			/* 遇到换行字符，认为接收到一个命令 */
			if (ucTemp == 0x0A)	/* 换行字符 */
			{		
				/*检测到有回车字符就把数据返回给上位机*/
				Usart_SendString(DEBUG_USART,"recv data: ");
				Usart_SendStr_length( DEBUG_USART, ucaRxBuf, usRxCount );
				usRxCount = 0;
			}
			Rxflag=0;
		}
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