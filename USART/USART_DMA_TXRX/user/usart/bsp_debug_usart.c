/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   重定向c库printf函数到usart端口
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"

uint8_t TransmitBuf[DEBUG_DATA_SIZE] = {0x00};
uint8_t ReceiveBuf[DEBUG_DATA_SIZE] = {0x00};


/**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 配置中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = USART_TX_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART_RX_DMA_IRQn;
	NVIC_Init(&NVIC_InitStructure);
  
}


 /**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
		
	PeripheralEnable(DEBUG_USART_RX_GPIO_PERIPH, true);
	PeripheralEnable(DEBUG_USART_TX_GPIO_PERIPH, true);

    /* 使能 UART 时钟 */
    PeripheralEnable(DEBUG_USART_PERIPH, true);

	/* 连接 PXx 到 USARTx_Tx*/
	GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);

	/*  连接 PXx 到 USARTx__Rx*/
	GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF);

	/* 配置Tx引脚为复用功能  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	/* 配置Rx引脚为复用功能 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
			
	/* 配置串DEBUG_USART 模式 */
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(DEBUG_USART, &USART_InitStructure); 
	USART_Cmd(DEBUG_USART, ENABLE);
}

/**
  * @brief  USART TX/RX DMA 配置
  * @param  无
  * @retval 无
  */
void Debug_USART_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	/*开启DMA时钟*/
	DEBUG_USART_TX_CLK_INIT(DEBUG_USART_TX_DMA_CLK, true);
	DEBUG_USART_RX_CLK_INIT(DEBUG_USART_RX_DMA_CLK, true);
	/* 复位初始化DMA数据流 */
	DMA_DeInit(DEBUG_USART_TX_DMA_STREAM);
	DMA_DeInit(DEBUG_USART_RX_DMA_STREAM);
	/* 确保DMA数据流复位完成 */
	while (DMA_GetCmdStatus(DEBUG_USART_TX_DMA_STREAM) != DISABLE);
	while (DMA_GetCmdStatus(DEBUG_USART_RX_DMA_STREAM) != DISABLE); 

	//TX
	DMA_InitStructure.DMA_Channel = DEBUG_USART_TX_DMA_CHANNEL;  
	/*设置DMA源：SPI数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&DEBUG_USART_TX_DR_BASE;	 
	/*内存地址(要传输的变量的指针)*/
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ReceiveBuf;
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
	DMA_Init(DEBUG_USART_TX_DMA_STREAM, &DMA_InitStructure);

	//RX
	DMA_InitStructure.DMA_Channel = DEBUG_USART_RX_DMA_CHANNEL;  
	/*设置DMA源：SPI数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&DEBUG_USART_RX_DR_BASE;	 
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
	DMA_Init(DEBUG_USART_RX_DMA_STREAM, &DMA_InitStructure);


	//中断初始化
	DMA_ITConfig(DEBUG_USART_TX_DMA_STREAM, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DEBUG_USART_RX_DMA_STREAM, DMA_IT_TC, ENABLE);
	NVIC_Configuration();

}


int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口DEBUG_USART */
	USART_SendData(DEBUG_USART, (uint8_t) ch);
	
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);		

	return (ch);
}


/*********************************************END OF FILE**********************/
