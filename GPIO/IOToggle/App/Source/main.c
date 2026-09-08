#include "main.h"

GPIO_InitTypeDef GPIO_InitStructure;

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
	IOSetup(IO_LED1,IO_DEFAULT_OUTPUT_CONFIG);
	IOSetup(IO_LED2,IO_DEFAULT_OUTPUT_CONFIG);


    while (1)
    {
		IOH(IO_LED1);
		IOL(IO_LED2);
		SystemDelay(500);
		IOH(IO_LED2);
		IOL(IO_LED1);
		SystemDelay(500);

    }
}

#ifdef USE_FULL_ASSERT

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
    while (1) {}
}
#endif
