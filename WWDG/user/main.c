#include "./usart/bsp_debug_usart.h"
#include "./wwdg/bsp_wwdg.h"

int main(void)
{
	uint8_t wwdg_tr, wwdg_wr;
	uint8_t cnt = 6;
    Debug_USART_Config();
	
	RCC_ClocksTypeDef clocks;
	
	RCC_GetClocksFreq(&clocks);
	
	printf("IWDG Demo V1.0.\r\n");
	printf("\nSYSCLK_Frequency: %d \r\n",clocks.SYSCLK_Frequency);
	printf("HCLK_Frequency:   %d \r\n",clocks.HCLK_Frequency);
	printf("PCLK1_Frequency:  %d \r\n",clocks.PCLK1_Frequency);
	printf("PCLK2_Frequency:  %d \r\n",clocks.PCLK2_Frequency);

	
	/* 检查是否为独立看门狗复位 */
	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
	{
		/* 独立看门狗复位 */
		printf("Reset signal is WWDG \r\n");

		/* 清除标志 */
		RCC_ClearFlag();
	}
	else
	{
		printf("Reset signal is not WWDG \r\n");
	}

	// 初始化WWDG：配置计数器初始值，配置上窗口值，启动WWDG，使能提前唤醒中断
	WWDG_Config(0x7F,0x50,WWDG_Prescaler_8);	

	// 窗口值我们在初始化的时候设置成0X5F，这个值不会改变
	wwdg_wr = WWDG->CFR & 0X7F;
	
	while(1)
	{
		wwdg_tr = WWDG->CR & 0X7F;
		
		if( wwdg_tr < wwdg_wr )
		{
			if(cnt)	//连续喂狗cnt次之后停止喂狗，系统复位
			{
				cnt--;
				// 喂狗，重新设置计数器的值为最大0X7F
				WWDG_Feed();
				printf("IWDG_Feed. \r\n");
			}
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
