#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include <stdio.h>
#include "mh2435.h"

#define DEBUG_USART                             USART1
#define DEBUG_USART_PERIPH                         PeripheralUSART1

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_PERIPH              PeripheralGPIOA
#define DEBUG_USART_RX_PIN                      GPIO_Pin_10
#define DEBUG_USART_RX_AF                       GPIO_AF_USART1
#define DEBUG_USART_RX_SOURCE                   GPIO_PinSource10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_PERIPH              PeripheralGPIOA
#define DEBUG_USART_TX_PIN                      GPIO_Pin_9
#define DEBUG_USART_TX_AF                       GPIO_AF_USART1
#define DEBUG_USART_TX_SOURCE                   GPIO_PinSource9

enum LOG_LEVEL
{
    LOG_LEVEL_OFF = -3,
    LOG_LEVEL_ERROR = -2,
    LOG_LEVEL_WARN = -1,
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_ALL = 2,
};

extern enum LOG_LEVEL log_level;

#define LOG_DEBUG(format,...)	\
do {							\
	if (log_level >= LOG_LEVEL_DEBUG) {	\
		printf("[%20s] [%6d] [DEBUG]  "format"\n", __FILE__, __LINE__, ##__VA_ARGS__);	\
	}	\
} while(0)
 
#define LOG_INFO(format,...)	\
do {							\
	if (log_level >= LOG_LEVEL_INFO) {	\
		printf("[%20s] [%6d] [INFO ]  "format"\n", __FILE__, __LINE__, ##__VA_ARGS__);	\
	}	\
} while(0)
 
#define LOG_WARN(format,...)	\
do {							\
	if (log_level >= LOG_LEVEL_WARN) {	\
		printf("[%20s] [%6d] [WARN ]  "format"\n", __FILE__, __LINE__, ##__VA_ARGS__);	\
	}	\
} while(0)
 
#define LOG_ERROR(format,...)	\
do {							\
	if (log_level >= LOG_LEVEL_ERROR) {	\
		printf("[%20s] [%6d] [ERROR]  "format"\n", __FILE__, __LINE__, ##__VA_ARGS__);	\
	}	\
} while(0)



void BSP_USART_Configuration(uint32_t BaudRate);

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch );
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen ); 
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);


#endif  /* __BSP_USART_H__ */

