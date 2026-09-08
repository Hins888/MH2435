#include "tim.h"
#include "stdio.h"

void TIM_CascadeConfiguration(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
	
	PeripheralEnable(PeripheralTIM1, true);
	PeripheralEnable(PeripheralTIM8, true);

	TIM_BaseInitStruct.TIM_Period = 10000;
	TIM_BaseInitStruct.TIM_Prescaler = 18000;
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseInitStruct.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(MASTER_TIM, &TIM_BaseInitStruct);	
	TIM_ClearFlag(MASTER_TIM, TIM_FLAG_Update);
	TIM_TimeBaseInit(SLAVE_TIM, &TIM_BaseInitStruct);	
	TIM_ClearFlag(SLAVE_TIM, TIM_FLAG_Update);
	
	TIM_SelectOutputTrigger(MASTER_TIM, TIM_TRGOSource_Update);
	TIM_SelectMasterSlaveMode(MASTER_TIM, TIM_MasterSlaveMode_Enable);
	
	TIM_SelectSlaveMode(SLAVE_TIM, TIM_SlaveMode_Trigger);
	TIM_SelectInputTrigger(SLAVE_TIM, TIM_TS_ITR0);
}