/**
  ******************************************************************************
  * @file    bsp_spi_dma.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   初始化SPI ,配置使用轮询模式收发数据
  ******************************************************************************
  */
  
#include "./spi/bsp_spi.h"

uint8_t TransmitBuf[DEBUG_DATA_SIZE] = {0x00};
uint8_t ReceiveBuf[DEBUG_DATA_SIZE] = {0x00};


void DEBUG_SPI_Init(uint8_t SampleMode)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能 DEBUG_SPI 及GPIO 时钟 */
	/*!< SPI_DEBUG_SPI_CS_GPIO, SPI_DEBUG_SPI_MOSI_GPIO, 
	   SPI_DEBUG_SPI_MISO_GPIO,SPI_DEBUG_SPI_SCK_GPIO 时钟使能 */
	RCC_AHB1PeriphClockCmd (DEBUG_SPI_SCK_GPIO_CLK | DEBUG_SPI_MISO_GPIO_CLK|DEBUG_SPI_MOSI_GPIO_CLK|DEBUG_CS_GPIO_CLK, ENABLE);

	/*!< SPI_DEBUG_SPI 时钟使能 */
	PeripheralEnable(DEBUG_SPI_CLK, ENABLE);

	//设置引脚复用
	GPIO_PinAFConfig(DEBUG_SPI_SCK_GPIO_PORT,DEBUG_SPI_SCK_PINSOURCE,DEBUG_SPI_SCK_AF); 
	GPIO_PinAFConfig(DEBUG_SPI_MISO_GPIO_PORT,DEBUG_SPI_MISO_PINSOURCE,DEBUG_SPI_MISO_AF); 
	GPIO_PinAFConfig(DEBUG_SPI_MOSI_GPIO_PORT,DEBUG_SPI_MOSI_PINSOURCE,DEBUG_SPI_MOSI_AF); 

	/*!< 配置 SPI_DEBUG_SPI 引脚: SCK */
	GPIO_InitStructure.GPIO_Pin = DEBUG_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  

	GPIO_Init(DEBUG_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	/*!< 配置 SPI_DEBUG_SPI 引脚: MISO */
	GPIO_InitStructure.GPIO_Pin = DEBUG_SPI_MISO_PIN;
	GPIO_Init(DEBUG_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	/*!< 配置 SPI_DEBUG_SPI 引脚: MOSI */
	GPIO_InitStructure.GPIO_Pin = DEBUG_SPI_MOSI_PIN;
	GPIO_Init(DEBUG_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< 配置 SPI_DEBUG_SPI 引脚: CS */
	GPIO_InitStructure.GPIO_Pin = DEBUG_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(DEBUG_CS_GPIO_PORT, &GPIO_InitStructure);

	/* 停止信号 DEBUG: CS引脚高电平*/
	SPI_DEBUG_CS_HIGH();

	/* DEBUG_SPI 模式配置 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SampleMode & 0x02;
	SPI_InitStructure.SPI_CPHA = SampleMode & 0x01;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(DEBUG_SPI, &SPI_InitStructure);
	
//	//SPI FIFO
//	SPI_TxFIFOCmd(DEBUG_SPI,ENABLE);
//	SPI_RxFIFOCmd(DEBUG_SPI,ENABLE);

//	SPI_TxFIFOThresholdConfig(DEBUG_SPI,SPI_TXFIFO_THRESHOLD_1);
//	SPI_RxFIFOThresholdConfig(DEBUG_SPI,SPI_RXFIFO_THRESHOLD_1);


	/* 使能 DEBUG_SPI  */
	SPI_Cmd(DEBUG_SPI, ENABLE);

}

