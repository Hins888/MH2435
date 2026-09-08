/**
  ******************************************************************************
  * @file    sleep.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   sleep 相关操作文件
  ******************************************************************************

  */ 
#include "sleep.h"
#include "./usart/bsp_debug_usart.h"
#include "./gpio/bsp_gpio.h"
#include "./rtc/bsp_rtc.h"


void Print_Menu(void)
{
	printf("===============CPU SLEEP MENU================\n");
	printf("1 RTC WakeUp CPU Sleep Test\n");
	printf("2 GPIO(PB3) WakeUp CPU Sleep Test\n");
	printf("b Back\n");
	printf("p Print Menu\n");
	printf("===================MENU END===================\n");		
}

uint8_t Get_Cmd(void)
{
	uint8_t tmp = 0;

	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
		tmp =  (uint8_t)USART_ReceiveData(USART1);
	if (tmp)
	{
		printf("cmd is %c\n", tmp);
	}
	return tmp;
}

void GPIO_WakeUpTest(void)
{
    char *text = "SleepMode_CPUSleep";

	GPIO_Config();	//GPIO Configuration(PB3)
	
	//sleep时关闭部分时钟
	RCC_AHB1PeriphClockLPModeCmd(RCC_AHB1Periph_GPIOA,DISABLE);
//	PeripheralEnable(PeripheralUSART1, false);		//关闭调试串口时钟后可能打印乱码，不允许功能
	
	printf("Please pull down PB3 \r\n");
	printf("%s In\r\n", text);
	
	__WFI();
	
	printf("%s Out \r\n",text);
	
	//关闭GPIO外部中断
	GPIO_EXTI_Config(DISABLE);
}

void RTC_WakeUpTest(void)
{
    char *text = "SleepMode_CPUSleep";
	
	RTC_Config();	//RTC Configuration
	//sleep时关闭部分时钟
	RCC_AHB1PeriphClockLPModeCmd(RCC_AHB1Periph_GPIOA,DISABLE);
//	PeripheralEnable(PeripheralUSART1, false);		//关闭调试串口时钟后可能打印乱码，不允许功能
	printf("RTC will trigger the interrput after 5s\n");
	printf("%s In\r\n", text);
	
	__WFI();
	printf("%s Out \r\n",text);
	
	//关闭中断
	RTC_ITConfig(DISABLE);
	RTC_ClearITPendingBit();
}

void Test_CPUSleep(void)
{ 
	volatile uint8_t cmd = 0;

	Print_Menu();
	while (1)
	{
		cmd = Get_Cmd();		
		switch (cmd)
		{
		case '1':
			RTC_WakeUpTest();
			Print_Menu();		
			break;
		case '2':
			GPIO_WakeUpTest();
			Print_Menu();
			break;
		
		default:
			break;	
		}
	}
}
