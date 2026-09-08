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
	printf("===============STOP SLEEP MENU================\n");
	printf("1 RTC WakeUp Stop Sleep Test\n");
	printf("2 GPIO(PA3) WakeUp Stop Sleep Test\n");
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

void Stop_Prepare(void)
{
    PeripheralEnable(PeripheralTRNG, true);
    
    PeripheralEnable(PeripheralPCFG, true);
    PCFG->USBHS_CR1 &= ~BIT16;
    PCFG->USBHS_CFG &= ~BIT7;
    *(uint32_t *)0x50000038 &= ~BIT(0);

    TRNG->RNG_ANA |= 0xF << 12;
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_TRNG | RCC_AHB2Periph_ACRYPT | RCC_AHB2Periph_SCRYPT, DISABLE);
    RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_TRNG | RCC_AHB2Periph_ACRYPT | RCC_AHB2Periph_SCRYPT, ENABLE);
    
    PWR->CR |= BIT(13) | BIT(15);
    PWR->CR |= BIT(28) | BIT(30) | BIT(31);
    
    PWR->CR = (PWR->CR & ~(0xFFF << 16)) | (0x1 << 16) | (0x1 << 18) | (0x1 << 20) | (0x1 << 22) | (0x1 << 24) | (0x1 << 26);
}

static void STOPSleep_Recover(void)
{
	RCC_ClocksTypeDef RCC_ClocksStatus;
    GPIO_InitTypeDef GPIO_InitStructure;
	
    /* After wake-up from STOP reconfigure the system clock */
	RCC_HSECmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != SET);
	RCC_PLLCmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_PLL1RDY) != SET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLL);
    while (RCC_GetSYSCLKSource() != ClockNodePLL1P);
    
    PeripheralEnable(PeripheralPCFG, true);
    PCFG->USBHS_CR1 |= BIT16;
    PCFG->USBHS_CFG |= BIT7;
    *(uint32_t *)0x50000038 |= BIT(0);
    
    PWR->CR &= ~(BIT(30) | BIT(31));
    while(!(PWR->CSR & BIT31));
    PWR->CR &= ~(BIT(13) | BIT(15));
    
    RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_TRNG | RCC_AHB2Periph_ACRYPT | RCC_AHB2Periph_SCRYPT, DISABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_TRNG | RCC_AHB2Periph_ACRYPT | RCC_AHB2Periph_SCRYPT, ENABLE);
    
    TRNG->RNG_ANA &= ~(0xF << 12);
    
    /*USARTx_Tx and Rx*/
	GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);
	GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
    
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	printf("SYSCLK_Frequency: %d \r\n",RCC_ClocksStatus.SYSCLK_Frequency);
	printf("HCLK_Frequency:   %d \r\n",RCC_ClocksStatus.HCLK_Frequency);
	printf("PCLK1_Frequency:  %d \r\n",RCC_ClocksStatus.PCLK1_Frequency);
	printf("PCLK2_Frequency:  %d \r\n",RCC_ClocksStatus.PCLK2_Frequency);
}

void GPIO_WakeUpTest(void)
{
    char *text = "SleepMode_StopSleep";

	GPIO_Config();	//GPIO Configuration(PC0)
	Stop_Prepare();
	//清除所有唤醒源使能
	PWR->CSR &= ~(0x1F << 9);
	//清除WU状态位
	PWR_ClearFlag (PWR_FLAG_WU);
	
	printf("Please pull up PA3 \r\n");
	printf("%s In\r\n", text);
	printf("CSR %#x \r\n",PWR->CSR);

	//进入停止模式
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	
	//关闭GPIO外部中断
	GPIO_EXTI_Config(DISABLE);
	
	//唤醒后重配时钟
	STOPSleep_Recover();
	
	printf("%s Out %#x \r\n", text,PWR->CSR);
}

void RTC_WakeUpTest(void)
{
    char *text = "SleepMode_StopSleep";
	
	RTC_Config();	//RTC Configuration
	Stop_Prepare();
	
	//清除所有唤醒源使能
	PWR->CSR &= ~(0x1F << 9);
	//清除WU状态位
	PWR_ClearFlag (PWR_FLAG_WU);
	printf("RTC will trigger the interrput after 5s\n");
	printf("%s In\r\n", text);
	printf("CSR %#x \r\n",PWR->CSR);
	//进入停止模式
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	
	//关闭中断
	RTC_ITConfig(DISABLE);
	RTC_ClearITPendingBit();
	
	//唤醒后重配时钟
	STOPSleep_Recover();
	
	printf("%s Out %#x \r\n", text,PWR->CSR);
}

void Test_STOPSleep(void)
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
            Print_Menu();
			break;	
        case 0: break;
		}
	}
}




