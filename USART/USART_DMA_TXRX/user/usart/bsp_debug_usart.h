#ifndef __BSP_DEBUG_USART_H
#define	__BSP_DEBUG_USART_H

#include "mh2435.h"
#include <stdio.h>



//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_PERIPH                      PeripheralUSART1

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



//串口波特率
#define DEBUG_USART_BAUDRATE                    115200

#define DEBUG_DATA_SIZE   (0x10)


/*USART DMA 定义****************************/
#define DEBUG_USART_TX_DR_BASE               DEBUG_USART->DR		
#define DEBUG_USART_TX_DMA_CLK               PeripheralDMA2	
#define DEBUG_USART_TX_CLK_INIT              PeripheralEnable
#define DEBUG_USART_TX_DMA_CHANNEL           DMA_Channel_4
#define DEBUG_USART_TX_DMA_STREAM            DMA2_Stream7


#define DEBUG_USART_RX_DR_BASE               DEBUG_USART->DR
#define DEBUG_USART_RX_DMA_CLK               PeripheralDMA2	
#define DEBUG_USART_RX_CLK_INIT              PeripheralEnable
#define DEBUG_USART_RX_DMA_CHANNEL           DMA_Channel_4
#define DEBUG_USART_RX_DMA_STREAM            DMA2_Stream2


#define USART_TX_DMA_IRQHandler					DMA2_Stream7_IRQHandler
#define	USART_TX_DMA_IRQn						DMA2_Stream7_IRQn



#define USART_RX_DMA_IRQHandler					DMA2_Stream2_IRQHandler
#define	USART_RX_DMA_IRQn						DMA2_Stream2_IRQn


extern uint8_t TransmitBuf[DEBUG_DATA_SIZE];
extern uint8_t ReceiveBuf[DEBUG_DATA_SIZE];

void Debug_USART_Config(void);
void Debug_USART_DMA_Config(void);

#endif /* __BSP_DEBUG_USART_H */
