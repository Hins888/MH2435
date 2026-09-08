/**
  ******************************************************************************
  * @file    bsp_spi_dma.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   初始化SPI ,配置使用DMA模式收发数据
  ******************************************************************************
  */
  
#include "./spi/bsp_spi_dma.h"

uint8_t TransmitBuf[DEBUG_DATA_SIZE] = {0x00};
uint8_t ReceiveBuf[DEBUG_DATA_SIZE] = {0x00};


void DEBUG_SPI_Init(uint8_t SampleMode)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能 DEBUG_SPI 及GPIO 时钟 */
	/*!< SPI_DEBUG_SPI_CS_GPIO, SPI_DEBUG_SPI_MOSI_GPIO, 
	   SPI_DEBUG_SPI_MISO_GPIO,SPI_DEBUG_SPI_SCK_GPIO 时钟使能 */
	RCC_AHB1PeriphClockCmd (DEBUG_SPI_SCK_GPIO_CLK | DEBUG_SPI_MISO_GPIO_CLK|DEBUG_SPI_MOSI_GPIO_CLK|DEBUG_SPI_NSS_GPIO_CLK, ENABLE);

	/*!< SPI_DEBUG_SPI 时钟使能 */
	PeripheralEnable(DEBUG_SPI_CLK, ENABLE);


	//设置引脚复用
	GPIO_PinAFConfig(DEBUG_SPI_NSS_GPIO_PORT,DEBUG_SPI_NSS_PINSOURCE,DEBUG_SPI_NSS_AF); 
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


	SPI_Cmd(DEBUG_SPI, DISABLE);

	/* DEBUG_SPI 模式配置 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SampleMode & 0x02;
	SPI_InitStructure.SPI_CPHA = SampleMode & 0x01;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(DEBUG_SPI, &SPI_InitStructure);
	
	//SPI FIFO
	SPI_TxFIFOCmd(DEBUG_SPI,ENABLE);
	SPI_RxFIFOCmd(DEBUG_SPI,ENABLE);
	
	SPI_TxFIFOThresholdConfig(DEBUG_SPI,SPI_TXFIFO_THRESHOLD_1);
	SPI_RxFIFOThresholdConfig(DEBUG_SPI,SPI_RXFIFO_THRESHOLD_1);
}
/**
  * @brief  SPI TX/RX DMA 配置
  * @param  无
  * @retval 无
  */
void SPI_DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /*开启DMA时钟*/
  PeripheralEnable(DEBUG_SPITX_DMA_CLK, ENABLE);
  PeripheralEnable(DEBUG_SPIRX_DMA_CLK, ENABLE);
  /* 复位初始化DMA数据流 */
  DMA_DeInit(DEBUG_SPITX_DMA_STREAM);
	DMA_DeInit(DEBUG_SPIRX_DMA_STREAM);
  /* 确保DMA数据流复位完成 */
  while (DMA_GetCmdStatus(DEBUG_SPITX_DMA_STREAM) != DISABLE);
  while (DMA_GetCmdStatus(DEBUG_SPIRX_DMA_STREAM) != DISABLE); 

  //TX
  DMA_InitStructure.DMA_Channel = DEBUG_SPITX_DMA_CHANNEL;  
  /*设置DMA源：SPI数据寄存器地址*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&DEBUG_SPITX_DR_BASE;	 
  /*内存地址(要传输的变量的指针)*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)TransmitBuf;
  /*方向：从内存到外设*/		
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;	
  /*传输大小DMA_BufferSize=DEBUG_DATA_SIZE*/	
  DMA_InitStructure.DMA_BufferSize = DEBUG_DATA_SIZE;
  /*外设地址不增*/	    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*内存地址自增*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*外设数据单位*/	
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*内存数据单位 8bit*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMA模式：非循环模式*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	 
  /*优先级：中*/	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
  /*禁用FIFO*/
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*存储器突发传输 1个节拍*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*外设突发传输 1个节拍*/
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
  /*配置DMA的数据流7*/		   
  DMA_Init(DEBUG_SPITX_DMA_STREAM, &DMA_InitStructure);
  
   //RX
  DMA_InitStructure.DMA_Channel = DEBUG_SPIRX_DMA_CHANNEL;  
  /*设置DMA源：SPI数据寄存器地址*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&DEBUG_SPIRX_DR_BASE;	 
  /*内存地址(要传输的变量的指针)*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ReceiveBuf;
  /*方向：从外设到内存*/		
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
  /*传输大小DMA_BufferSize=DEBUG_DATA_SIZE*/	
  DMA_InitStructure.DMA_BufferSize = DEBUG_DATA_SIZE;
  /*外设地址不增*/	    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*内存地址自增*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*外设数据单位*/	
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*内存数据单位 8bit*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMA模式：非循环模式*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	 
  /*优先级：中*/	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
  /*禁用FIFO*/
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*存储器突发传输 1个节拍*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*外设突发传输 1个节拍*/
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
  /*配置DMA的数据流7*/		   
  DMA_Init(DEBUG_SPIRX_DMA_STREAM, &DMA_InitStructure);
  
}

