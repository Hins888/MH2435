#include <stdio.h>
#include "sleep.h"

#include "./usart/bsp_debug_usart.h"
#include "./gpio/bsp_gpio.h"

void GPIO_ConfigInit(void)
{
    RCC_AHB1PeriphClockCmd(0x1FF, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    GPIO_Init(GPIOF, &GPIO_InitStruct);
    GPIO_Init(GPIOG, &GPIO_InitStruct);
    GPIO_Init(GPIOH, &GPIO_InitStruct);
    GPIO_Init(GPIOI, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All & ~(GPIO_Pin_9 | GPIO_Pin_10);
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int main(int argc, char *argv[])
{
	RCC_ClocksTypeDef RCC_ClocksStatus;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    
    GPIO_ConfigInit();
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    
	Debug_USART_Config();
	
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	
    printf("\n");
    printf("StopSleep Demo V1.0.\n");
	printf("SYSCLK_Frequency: %d \r\n",RCC_ClocksStatus.SYSCLK_Frequency);
	printf("HCLK_Frequency:   %d \r\n",RCC_ClocksStatus.HCLK_Frequency);
	printf("PCLK1_Frequency:  %d \r\n",RCC_ClocksStatus.PCLK1_Frequency);
	printf("PCLK2_Frequency:  %d \r\n",RCC_ClocksStatus.PCLK2_Frequency);

	Test_STOPSleep();
	
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

