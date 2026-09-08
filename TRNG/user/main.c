#include "./usart/bsp_debug_usart.h"
#include "./trng/bsp_trng.h"

#define MODE_INTERRUPT


extern uint32_t buffer[4];

uint8_t Interrupt_F = 0;
int main(void)
{
    ErrorStatus result = ERROR;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
    Debug_USART_Config();
	printf("TRNG Demo V1.0.\n");
	
#ifdef MODE_INTERRUPT
	
	result = RNG_Interrupt();	//中断获取随机数
	while(!Interrupt_F);
	printf("RNG_Interrupt is %s\n", (result == SUCCESS) ? "ok": "fail...");
#else
	result = RNG_GetRandom();	//轮询获取随机数
	printf("RNG_GetRandom is %s\n", (result == SUCCESS) ? "ok": "fail...");
#endif

    while(1);
}


//中断回调函数获取随机数
#ifdef MODE_INTERRUPT
void HASH_RNG_IRQHandler(void)
{
    static uint32_t times = 0x00;
  
	if (times == TIMES)
	{
		TRNG_ClearITPendingBit(TRNG_IT_RNG0_S128);
		TRNG_ITConfig(DISABLE);
		Interrupt_F = 1;
		return;
	}

	if(ERROR == TRNG_Get(buffer,TRNG0))
	{
		printf("TRNG is error \n");
	}
	printf("\nrandom 1:0x%08x\n", buffer[0]);
	printf("random 2:0x%08x\n", buffer[1]);
	printf("random 3:0x%08x\n", buffer[2]);
	printf("random 4:0x%08x\n", buffer[3]);

	TRNG_Start(TRNG0);

	times++;
}

#endif


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

