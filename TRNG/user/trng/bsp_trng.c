#include "bsp_trng.h"

#define TRNGx TRNG0
uint32_t buffer[4] = {0x00};
ITStatus status = RESET;

static void RNG_Configuration(void)
{
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_TRNG, ENABLE);
	TRNG_Start(TRNG0);
}

ErrorStatus RNG_GetRandom(void)
{
    RNG_Configuration();

    uint32_t times = 0x00;
    while (1)
    {
        if (times == TIMES) break;

        if(SUCCESS == TRNG_Get(buffer,TRNG0))
		{
			printf("\r\nrandom 1:0x%08x\n", buffer[0]);
			printf("random 2:0x%08x\n", buffer[1]);
			printf("random 3:0x%08x\n", buffer[2]);
			printf("random 4:0x%08x\n", buffer[3]);
		
			TRNG_Start(TRNG0);
			times++;
		}
        
    }

    return SUCCESS;
}

ErrorStatus RNG_Interrupt(void)
{
    ErrorStatus result = SUCCESS;

	
	RNG_Configuration();

    TRNG_ITConfig(ENABLE);
	NVIC_ClearPendingIRQ(HASH_RNG_IRQn);
	
    NVIC_InitTypeDef nvic = {HASH_RNG_IRQn, 1, 1, ENABLE};
    NVIC_Init(&nvic);

    return result;
}
ErrorStatus RNG_ErrorTest(void)
{
    ErrorStatus result = SUCCESS;

	if(TRNG_GetITStatus(TRNG_IT_TOT_D_FAIL) || TRNG_GetITStatus(TRNG_IT_TOT_FAIL) || TRNG_GetITStatus(TRNG_IT_CHI_FAIL))
		status = RESET;
    return result;
}


