#include <stdio.h>
#include "sleep.h"

#include "./usart/bsp_debug_usart.h"
#include "./gpio/bsp_gpio.h"

int main(int argc, char *argv[])
{
	RCC_ClocksTypeDef RCC_ClocksStatus;
   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	Debug_USART_Config();
	
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	
    printf("\n");
	printf("SYSCLK_Frequency: %d \r\n",RCC_ClocksStatus.SYSCLK_Frequency);
	printf("HCLK_Frequency:   %d \r\n",RCC_ClocksStatus.HCLK_Frequency);
	printf("PCLK1_Frequency:  %d \r\n",RCC_ClocksStatus.PCLK1_Frequency);
	printf("PCLK2_Frequency:  %d \r\n",RCC_ClocksStatus.PCLK2_Frequency);
	printf("Powerdown Sleep Test V1.0 \r\n");

	Test_PowerdownSleep();
	
	while(1);
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
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    while (1)
    {
    }
}
#endif

