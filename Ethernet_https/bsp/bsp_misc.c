#include "bsp_api.h"

uint32_t buffer[4] = {0x00};
#define TRNG_DATA   (TRNG->RNG_PN)

int bsp_gen_random(unsigned char *random, unsigned int len)
{
    unsigned int i, count;
    //uint32_t * p_rand = (uint32_t *)random;
    //uint32_t times = 0x00;

    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_TRNG, ENABLE);
    PeripheralEnable(PeripheralTRNG, true);
    TRNG_Start(TRNG0);

    i = 0;
    while(i < len)
    {
        TRNG_Start(TRNG0);
#if 0
        times = 0;
        while(TRNG->RNG_CSR & TRNG_RNG_CSR_S128_TRNG0_Mask)
        {
            bsp_delayms(10);
            times ++;
            if(times > 10)
            {
                printf("bsp random %d %d\n", i, len);
                //goto _exit;
            }
        }
#endif
        buffer[0] = TRNG_DATA;
        buffer[1] = TRNG_DATA;
        buffer[2] = TRNG_DATA;
        buffer[3] = TRNG_DATA;
        count = (len - i) > 16 ? 16 : (len - i);
        memcpy(random + i, buffer, count);
        i += count;
        if(i >= len) goto _exit;
    }
    //printf("bsp random %d %d\n", i, len);
    
_exit:
    TRNG_Stop(TRNG0);
    if(i < len ) return -1;
    else return 0;
}

int bsp_get_random(void)
{
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_TRNG, ENABLE);
    TRNG_Start(TRNG0);
    
    TRNG_Get(buffer,TRNG0);
    
    TRNG_Stop(TRNG0);
    
    return   buffer[0];
}
