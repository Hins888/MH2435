#include <stdio.h>
#include <string.h>
#include "tim.h"
#include "usart.h"

int main(void)
{
	USART_Configuration();
	
	TIM_OnePulseConfiguration();
	
	printf("Advanced timer one pulse demo v1.0\n");
	printf("PA8 out one pulse.\n");

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
  while (1)
  {}
}
#endif
