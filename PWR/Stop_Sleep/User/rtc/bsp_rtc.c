/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   RTC唤醒初始化文件，时钟固定为外部32K
  ******************************************************************************

  */ 
#include "./rtc/bsp_rtc.h"
#include "./usart/bsp_debug_usart.h"

void NVIC_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;

	EXTI_Init(&EXTI_InitStructure);
}

void RTC_Config(void)
{
	PeripheralEnable(PeripheralBPU, true);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能LSE
	PWR_BackupAccessCmd(ENABLE);
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* rtc stop counting and counter cleared to 0 */
    RTC_ResetCounter();
    RTC_SetRefRegister(0);
    printf("Get \"RTC\" value:%d\n", RTC_GetRefRegister());
	
	RTC_SetAlarm(5);
	NVIC_Configuration();
	RTC_ITConfig(ENABLE);
	RTC_ClearITPendingBit();
}
