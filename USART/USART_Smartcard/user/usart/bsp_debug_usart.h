#ifndef __BSP_DEBUG_USART_H
#define	__BSP_DEBUG_USART_H

#include "mh2435.h"
#include <stdio.h>



//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_PERIPH                         PeripheralUSART1
#define DEBUG_USART_PERIPH_INIT                  	RCC_APB2PeriphClockCmd

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
/************************************************************/

#define DEBUG_USART_IRQHandler					USART1_IRQHandler
#define	DEBUG_USART_IRQn						USART1_IRQn


//串口波特率
#define DEBUG_USART_BAUDRATE                    115200



void Debug_USART_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch );
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);
uint8_t Usart_ReadByte(USART_TypeDef * pUSARTx);

#endif /* __BSP_DEBUG_USART_H */
