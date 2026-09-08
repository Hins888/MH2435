#include <stdio.h>
#include "./usart/bsp_debug_usart.h"
#include "./pvd/bsp_pvd.h" 
#include "./systick/bsp_SysTick.h"

int main(int argc, char *argv[])
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	Debug_USART_Config();
	
	SysTick_Init();
	
    printf("\n");
    printf("PWR_PVD Demo V1.0.\n");

	PVD_Config();
	
	while(1)
	{
		Delay_ms(500);
		printf("PVDO FLAG is %d \r\n",(PWR->CSR & BIT2)?1:0);
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
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    while (1)
    {
    }
}
#endif

