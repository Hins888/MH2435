#ifndef __BSP_SPI_DMA_H__
#define __BSP_SPI_DMA_H__

#include "mh2435.h"
#include "./usart/bsp_debug_usart.h"

#define DEBUG_DATA_SIZE   (0x100)

/*SPI接口定义-开头****************************/
#define DEBUG_SPI                           SPI1
#define DEBUG_SPI_CLK                       PeripheralSPI1

#define DEBUG_SPI_SCK_PIN                   GPIO_Pin_3                  
#define DEBUG_SPI_SCK_GPIO_PORT             GPIOB                       
#define DEBUG_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define DEBUG_SPI_SCK_PINSOURCE             GPIO_PinSource3
#define DEBUG_SPI_SCK_AF                    GPIO_AF_SPI1

#define DEBUG_SPI_MISO_PIN                  GPIO_Pin_4                
#define DEBUG_SPI_MISO_GPIO_PORT            GPIOB                   
#define DEBUG_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define DEBUG_SPI_MISO_PINSOURCE            GPIO_PinSource4
#define DEBUG_SPI_MISO_AF                   GPIO_AF_SPI1

#define DEBUG_SPI_MOSI_PIN                  GPIO_Pin_5                
#define DEBUG_SPI_MOSI_GPIO_PORT            GPIOB                     
#define DEBUG_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define DEBUG_SPI_MOSI_PINSOURCE            GPIO_PinSource5
#define DEBUG_SPI_MOSI_AF                   GPIO_AF_SPI1

#define DEBUG_SPI_NSS_PIN                   GPIO_Pin_4               
#define DEBUG_SPI_NSS_GPIO_PORT             GPIOA                     
#define DEBUG_SPI_NSS_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define DEBUG_SPI_NSS_PINSOURCE            	GPIO_PinSource4
#define DEBUG_SPI_NSS_AF                   	GPIO_AF_SPI1



/*SPI DMA 定义****************************/
#define DEBUG_SPITX_DR_BASE               DEBUG_SPI->DR		
#define DEBUG_SPITX_DMA_CLK               PeripheralDMA2
#define DEBUG_SPITX_DMA_CHANNEL           DMA_Channel_3
#define DEBUG_SPITX_DMA_STREAM            DMA2_Stream3
#define DEBUG_SPITX_DMA_TCIF              DMA_FLAG_TCIF3

#define DEBUG_SPIRX_DR_BASE               DEBUG_SPI->DR
#define DEBUG_SPIRX_DMA_CLK               PeripheralDMA2
#define DEBUG_SPIRX_DMA_CHANNEL           DMA_Channel_3
#define DEBUG_SPIRX_DMA_STREAM            DMA2_Stream0
#define DEBUG_SPIRX_DMA_TCIF              DMA_FLAG_TCIF0



extern uint8_t TransmitBuf[DEBUG_DATA_SIZE];
extern uint8_t ReceiveBuf[DEBUG_DATA_SIZE];

void SPI_DMA_Config(void);
void DEBUG_SPI_Init(uint8_t SampleMode);

#endif
