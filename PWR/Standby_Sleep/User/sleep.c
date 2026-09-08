#include "sleep.h"
#include "./usart/bsp_debug_usart.h"
#include "./gpio/bsp_gpio.h"

void Print_Menu(void)
{
	printf("===============STANDBY SLEEP MENU================\n");
	printf("1 GPIO(PA0) WakeUp Standby Sleep Test\n");
	printf("2 GPIO(PE13) WakeUp Standby Sleep Test\n");	
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

void GPIO_A0_WakeUpTest(void)
{
    char *text = "SleepMode_StandbySleep";

	printf("%s In\r\n", text);
	printf("Please pull up PA0 \r\n");
	//清除所有唤醒源使能
	PWR->CSR &= ~(0x3FF << 4);
	
	//清除WU状态位
	PWR_ClearFlag (PWR_FLAG_WU);
	
	//使能唤醒信息
    PWR_WakeUpPinCmd(ENABLE);

	//进入待机模式
	PWR_EnterSTANDBYMode();

	printf("%s Out\r\n", text);//不会执行，唤醒后复位重启
}

void GPIO_E13_WakeUpTest(void)
{
    char *text = "SleepMode_StandbySleep";
	
	GPIO_Config();

	printf("%s In \r\n", text);
	printf("Please pull down PE13 \r\n");
	//清除所有唤醒源使能
	PWR->CSR &= ~(0x3FF << 4);
	
	//清除WU状态位
	PWR_ClearFlag (PWR_FLAG_WU);
	
	//使能唤醒信息
	PWR->CSR |= PWR_CSR_EWUP_PE13;
	
	//进入待机模式
	PWR_EnterSTANDBYMode();

	printf("%s Out\r\n", text);//不会执行，唤醒后复位重启
}

void Test_StandbySleep(void)
{ 
	volatile uint8_t cmd = 0;

	Print_Menu();
	while (1)
	{
		cmd = Get_Cmd();		
		switch (cmd)
		{
		case '1':
			GPIO_A0_WakeUpTest();
			Print_Menu();
			break;
		
		case '2':
			GPIO_E13_WakeUpTest();
			Print_Menu();
			break;

		case 'b':
			return;
		
		case 'p':
			Print_Menu();
			break;			
		default:
			break;	
		}
	}
}
