/* Includes ------------------------------------------------------------------*/
#include "mh2435_it.h"
#include "bsp_debug_usart.h"
#include "bsp_keyboard.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


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
	printf("hardfault!!!\n");

  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
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
  {}
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
  {}
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
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetFlagStatus(EXTI_Line6) != RESET)
	{
		do
		{
			GPIO_ResetBits(col_ports[0], col_pins[0]);
			GPIO_SetBits(col_ports[1], col_pins[1]);
			if (GPIO_ReadInputDataBit(row_ports[0], row_pins[0]) == 0) 
			{
				SystemDelayUs(20000);
				if (GPIO_ReadInputDataBit(row_ports[0], row_pins[0]) == 0)
				{
					key_code = KEY_S1;	
				}

				printf("key_code = %d\n", key_code);
				break;
			}
			GPIO_SetBits(col_ports[0], col_pins[0]);
			GPIO_ResetBits(col_ports[1], col_pins[1]);	
			if (GPIO_ReadInputDataBit(row_ports[0], row_pins[0]) == 0) 
			{
				SystemDelayUs(20000);
				if (GPIO_ReadInputDataBit(row_ports[0], row_pins[0]) == 0)
				{
					key_code = KEY_S2;	
				}

				printf("key_code = %d\n", key_code);
				break;
			}	
		}while(0);

		EXTI_ClearITPendingBit(EXTI_Line6);
		GPIO_ResetBits(col_ports[0],col_pins[0]);
		GPIO_ResetBits(col_ports[1],col_pins[1]);	
	}
	if (EXTI_GetFlagStatus(EXTI_Line7) != RESET)
	{
		do
		{
			GPIO_ResetBits(col_ports[0], col_pins[0]);
			GPIO_SetBits(col_ports[1], col_pins[1]);
			if (GPIO_ReadInputDataBit(row_ports[1], row_pins[1]) == 0) 
			{
				SystemDelayUs(20000);
				if (GPIO_ReadInputDataBit(row_ports[1], row_pins[1]) == 0)
				{
					key_code = KEY_S4;	
				}
				
				printf("key_code = %d\n", key_code);
				break;
			}
			
			GPIO_SetBits(col_ports[0], col_pins[0]);
			GPIO_ResetBits(col_ports[1], col_pins[1]);	
			if (GPIO_ReadInputDataBit(row_ports[1], row_pins[1]) == 0) 
			{
				SystemDelayUs(20000);
				if (GPIO_ReadInputDataBit(row_ports[1], row_pins[1]) == 0)
				{
					key_code = KEY_S3;	
				}

				printf("key_code = %d\n", key_code);
				break;
			}	
		}while(0);
		
		EXTI_ClearITPendingBit(EXTI_Line7);
		GPIO_ResetBits(col_ports[0],col_pins[0]);
		GPIO_ResetBits(col_ports[1],col_pins[1]);			
	}	
}
