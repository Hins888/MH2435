#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_Systick.h"
#include "./iwdg/bsp_iwdg.h"
int main(void)
{
	uint8_t cnt = 6;
    Debug_USART_Config();
	
	printf("\n IWDG Demo V1.0.\r\n");
	
	/* 检查是否为独立看门狗复位 */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* 独立看门狗复位 */
		printf("Reset signal is IWDG \r\n");

		/* 清除标志 */
		RCC_ClearFlag();
	
	}
	else
	{
		printf("Reset signal is not IWDG \r\n");
	}
  
	SysTick_Init();
	
	// IWDG 约1s 超时溢出
	IWDG_Config(IWDG_Prescaler_64 ,625); 

	while(1)
	{
		if(cnt)
		{
			cnt--;
			Delay_10us(50000);//50ms
			printf("IWDG_Feed. \r\n");
			// 时间内准时喂狗的话，不喂狗后就会产生复位
			IWDG_Feed();		
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
