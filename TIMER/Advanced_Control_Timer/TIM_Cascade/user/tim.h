#ifndef __TIM_H
#define __TIM_H

#include "mh2435.h"
#include "mh2435_tim.h"

#define MASTER_TIM		TIM1
#define SLAVE_TIM		TIM8

void TIM_CascadeConfiguration(void);

#endif



