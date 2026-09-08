#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include "mh2435.h"
#include "mh_rsa.h"
#include "mh_rand.h"
#include "mh_bignum.h"
#include "mh_bignum_tool.h"
#include "mh_misc.h"
#include "mh_crypt_version.h"
#include "mh_sm7.h"
#include "mh_sm2.h"
#include "mh_ecc.h"
#define  CRYPT_INT                       0

#define MH_MAX_RSA_MODULUS_WORDS        ((MH_MAX_RSA_MODULUS_BYTES + 3) / 4)
#define MH_MAX_RSA_PRIME_WORDS          ((MH_MAX_RSA_PRIME_BYTES + 3) / 4)

void USART_Configuration(unsigned int BaudRate);

extern void RSA_Test(void);
extern void AES_Test(void);
extern void DES_Test(void);
extern void TDES_Test(void);
extern void SM4_Test(void);
extern void SM3_Test(void);
extern void SHA_Test(void);
extern void SM1_Test(void);
extern void SM7_Test(void); 
extern void SM2_Test();
extern void ECC_Test();      
extern void RSA_Test();

extern void SM9_SignVeriTest();
extern void SM9_EncDecTest();
extern void SM9_EncapDecapTest();


uint8_t  RngBuff[0x60] = {0};

int main(int args)
{
    int i = 0;
    uint32_t ver = 1;
    uint32_t check = 0;
    uint32_t tBuf[64] = {0};

    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_TRNG | RCC_AHB2Periph_ACRYPT | RCC_AHB2Periph_SCRYPT, ENABLE);
    
    USART_Configuration(115200);
    TRNG->RNG_ANA &= ~(0xF<<12);
    MHRAND_Init((uint32_t*)&RngBuff, sizeof(RngBuff));               //硬件产生随机数

    ver = mh_crypt_version();
    printf(" Test Demo V1.1, secure lib version is V%02x.%02x.%02x.%02x\n", ver >> 24, (ver >> 16)&0xFF, (ver>>8)&0xFF, ver & 0xFF);
    printf(" Crypt Test V1.0 start......\r\n");

    while(1)
    { 
        AES_Test();
        DES_Test();
        TDES_Test();
        SHA_Test();
        SM1_Test();
        SM3_Test();
        SM4_Test();      
        SM7_Test();
        SM2_Test();
        ECC_Test();      
        RSA_Test();
        SM9_SignVeriTest();
        SM9_EncDecTest();
        SM9_EncapDecapTest();

        printf("\r\n");
        printf(" Crypt Test V1.0 finish.......\r\n");
    }

    return 0;
}

void USART_Configuration(unsigned int BaudRate)
{
    PeripheralEnable(PeripheralGPIOA, true);
    PeripheralEnable(PeripheralUSART1, true);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

    USART_InitTypeDef usart;
    usart.USART_BaudRate = BaudRate;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &usart);

    USART_Cmd(USART1, ENABLE);
}

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);

    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);

    return ch;
}


void printf_monobit(void)
{
    uint32_t i,j;
    uint32_t t;
    int32_t sum;
    for(i = 0; i <= 0xff; i++)
    {
        sum = 0;
        for(j = 0; j < 8; j++)
        {
            t = (i >> j) & 0x01;
            sum += 2*(int)t-1;
        }
        printf("%3d,",sum);
        if(15 == i % 16)
            printf("\n");
    }
    
}

void printf_byte_bit(void)
{
    uint32_t i,j;
    uint32_t t;
    int32_t sum;
    for(i = 0; i <= 0xff; i++)
    {
        sum = 0;
        for(j = 0; j < 8; j++)
        {
            t = (i >> j) & 0x01;
            sum +=t;
        }
        printf("%3d,",sum);
        if(15 == i % 16)
            printf("\n");
    }
    
}


void printf_byte_bit_runs(void)
{
    uint32_t i,j;
    int32_t sum;
    for(i = 0; i <= 0xff; i++)
    {
        sum = 0;
        for(j = 0; j < 7; j++)
        {
            if((i>>j & 0x03) == 0x01 || ((i>>j & 0x03) == 0x02))
            sum ++;
        }
        printf("%3d,",sum);
        if(15 == i % 16)
            printf("\n");
    }
}


void printf_long_runs_max(void)
{
    uint32_t i,j;
    int32_t sum;
    int32_t max;
    for(i = 0; i <= 0xff; i++)
    {
        sum = 0;max = 0;
        for(j = 0; j < 8; j++)
        {
            if(i>>j & 0x01)
            {
                sum ++;
                if( sum > max)
                    max = sum;
            }
            else
                sum = 0;
        }
        printf("%3d,",max);
        if(15 == i % 16)
            printf("\n");
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
    {
    }
}
#endif

