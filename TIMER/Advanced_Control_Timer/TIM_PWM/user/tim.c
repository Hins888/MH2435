#include "tim.h"
#include "stdio.h"


void TIM_MspConfiguration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	PeripheralEnable(PeripheralGPIOA, true);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
}


void TIM_PWMConfiguration(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	PeripheralEnable(PeripheralTIM1, true);
	
	TIM_MspConfiguration();

	TIM_BaseInitStruct.TIM_Period = 10000;
	TIM_BaseInitStruct.TIM_Prescaler = 18000;
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_BaseInitStruct);	
	
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_Pulse = TIM_BaseInitStruct.TIM_Period/2;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OC1Init(TIM1, &TIM_OCInitStruct);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

