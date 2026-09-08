#include "bsp_usart/bsp_usart.h"

enum LOG_LEVEL log_level = LOG_LEVEL_INFO;

//Retarget Printf
int fputc(int ch, FILE *f)
{
    USART_SendData(DEBUG_USART, (uint8_t)ch);

    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) != SET);

    return ch;
}

void BSP_USART_Configuration(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
		
	PeripheralEnable(DEBUG_USART_RX_GPIO_PERIPH, true);
	PeripheralEnable(DEBUG_USART_TX_GPIO_PERIPH, true);
	PeripheralEnable(DEBUG_USART_PERIPH, true);

	GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);
	GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
			
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_RxFIFO = USART_RxFIFO_Enable;
	USART_Init(DEBUG_USART, &USART_InitStructure); 
	USART_Cmd(DEBUG_USART, ENABLE);
}

/**
 * @brief   Transmits single data through the USARTx peripheral.
 * @param 	pUSARTx: where x can be 1, 2, 3, 4, 6 or 8 to select the USART or
 *         UART peripheral.
 * @param  	ch: the data to transmit.
 * @retval None
 */
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	USART_SendData(pUSARTx,ch);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/**
 * @brief   Transmits specifies the length of the data through the USARTx peripheral.
 * @param 	pUSARTx: where x can be 1, 2, 3, 4, 6 or 8 to select the USART or
 *         UART peripheral.
 * @param  	str: the data to transmit.
 * @param  	strlen:length of data to transmit.
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
 * @param 	pUSARTx: where x can be 1, 2, 3, 4, 6 or 8 to select the USART or
 *         UART peripheral.
 * @param  	str: the string to transmit.
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

