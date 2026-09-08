#include "mh2435_it.h"
#include "./usart/bsp_debug_usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler (void)
{
}


void USART_TX_DMA_IRQHandler(void)
{
	if(DMA_GetITStatus(DEBUG_USART_TX_DMA_STREAM,DMA_IT_TCIF))
	{
        DMA_ClearITPendingBit(DEBUG_USART_TX_DMA_STREAM, DMA_IT_TCIF);
	}
	
    NVIC_ClearPendingIRQ(USART_TX_DMA_IRQn);
}

void USART_RX_DMA_IRQHandler(void)
{
    if(DMA_GetITStatus(DEBUG_USART_RX_DMA_STREAM,DMA_IT_TCIF))
	{
        DMA_ClearITPendingBit(DEBUG_USART_RX_DMA_STREAM, DMA_IT_TCIF);
	}
	memcpy(TransmitBuf,ReceiveBuf,DEBUG_DATA_SIZE);
	//重启接收,使能发送
	DMA_Cmd(DEBUG_USART_RX_DMA_STREAM, ENABLE);
	DMA_Cmd(DEBUG_USART_TX_DMA_STREAM, ENABLE);
    NVIC_ClearPendingIRQ(USART_RX_DMA_IRQn);
}
