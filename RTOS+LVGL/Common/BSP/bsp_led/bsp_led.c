#include "bsp_led/bsp_led.h"


void BSP_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	IOInit(GPIO_RED,&GPIO_InitStructure);
	IOL(GPIO_RED);
	
	IOInit(GPIO_GREE,&GPIO_InitStructure);
	IOL(GPIO_GREE);
}
