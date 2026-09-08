#include <stdio.h>
#include "mh2435.h"
#include "mh2435_sysctrl.h"
#include "debug.h"

void r_printf(uint32_t b, char *s)
{
    if (0 != b)
    {
        printf("Pass ");printf("%s", s);
    }
    else
    {
        printf("Fail ");printf("%s", s);
        while(1);
    }
}


void ouputRes(char *pcFmt, void *pvbuff, uint32_t u32Len)
{
    int32_t i;
    uint8_t *pu8Buff = pvbuff;
    
    printf("%s", pcFmt);
    for (i = 0; i < u32Len; i++)
    {
        printf("%02X", pu8Buff[i]);
    }
    printf("\n");
}


void ouputArrayHex(char *pcFmt, void *pvbuff, uint32_t u32Len)
{
    int32_t i;
    uint8_t *pu8Buff = pvbuff;
    
    printf("%s", pcFmt);
    for (i = 0; i < u32Len; i++)
    {
        printf("0x%02X, ", pu8Buff[i]);
    }
    printf("\n");
}



