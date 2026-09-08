#include "tim.h"
#include "stdio.h"

void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
	
	PeripheralEnable(PeripheralTIM1, true);

	TIM_BaseInitStruct.TIM_Period = 10000;
	TIM_BaseInitStruct.TIM_Prescaler = 18000;
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseInitStruct.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1, &TIM_BaseInitStruct);	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_Cmd(TIM1, ENABLE);
}

