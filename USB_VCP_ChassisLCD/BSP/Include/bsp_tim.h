#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include "mh2435.h"

#define BASIC_TIM           		TIM6
#define BASIC_TIM_CLK       		RCC_APB1Periph_TIM6

#define BASIC_TIM_IRQn				TIM6_DAC_IRQn
#define BASIC_TIM_IRQHandler    	TIM6_DAC_IRQHandler

void TIMx_Configuration(void);


#endif


