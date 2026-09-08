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

/**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 配置中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
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

	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;
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

	NVIC_Configuration();
	USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);
}


/**
 * @brief   Transmits single data through the USARTx peripheral.
 * @param 	pUSARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or
 *         UART peripheral.
 *			ch: the data to transmit.
 * @retval None
 */
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	/* 发送一个字节数据到USART1 */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/**
 * @brief   Transmits specifies the length of the data through the USARTx peripheral.
 * @param 	pUSARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or
 *         UART peripheral.
 *			str: the data to transmit.
*			strlen:length of data to transmit.
 * @retval None
 */
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen )
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(k < strlen);
}

/**
 * @brief   Transmits string data through the USARTx peripheral.
 * @param 	pUSARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or
 *         UART peripheral.
 *			str: the string to transmit.
 * @retval None
 */
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str)
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');
}
/**
 * @brief  Read a data by the USARTx peripheral.
 * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or
 *         UART peripheral.
 * @retval The received data.
 */
uint8_t Usart_ReadByte(USART_TypeDef * pUSARTx)
{
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_RXNE) == RESET);
	return USART_ReceiveData(pUSARTx);
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
