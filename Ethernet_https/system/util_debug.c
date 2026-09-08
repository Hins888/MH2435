#include "bsp_api.h"  
#include "api.h"  
#include <stdarg.h>

#define DEBUG_USART         USART1

static uint8_t  log_printf = 1;
//static char  sbuffer[1024];

void Debug_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
        
    PeripheralEnable(PeripheralUSART1, true);
    PeripheralEnable(PeripheralGPIOA, true);
 
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
            
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(DEBUG_USART, &USART_InitStructure); 
    USART_Cmd(DEBUG_USART, ENABLE);
}

uint8_t GetCmd(void)
{
    uint8_t tmp = 0;

    if (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE))
    {
        tmp = USART_ReceiveData(DEBUG_USART);
    }
    return tmp;
}

int fputc(int ch, FILE *f)
{
    USART_SendData(DEBUG_USART, (uint8_t) ch);
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);        

    return (ch);
}

int fgetc(FILE *f)
{
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(DEBUG_USART);
}


void log_debug(int level, const char *func, int line,  const char *format, ... )
{
    //va_list      varg;
    //unsigned int len;

    if(!log_printf) return ;
    //memset(sbuffer, 0, sizeof(sbuffer));
    //snprintf(sbuffer, sizeof(sbuffer), "%s:%d", func, line);
    //va_start(varg, format);
    //vsnprintf(sbuffer, sizeof(sbuffer), format, varg);
    
    //len = strlen((char*)sbuffer);
    //printf("%s:%d-%s", func, line, sbuffer);
    
    //va_end(varg);
    
    printf("%s:%d %s", func, line, format);
}


void printHex(char* title, const void * buffer, uint32_t size)
{
    uint32_t i;
    if(title) printf("%s:", title);
    for (i = 0; i < size; i++)
    {
        if (i % 16 == 0) {
            //if (i % 32 == 0)
            printf("\r\n");
//            else
//              logdbg(" ");
        }
        printf("%02X ", ((unsigned char *)buffer)[i]);
    }
    printf("\r\n");
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {}
}
#endif




































