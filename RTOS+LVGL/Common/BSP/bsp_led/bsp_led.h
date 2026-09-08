#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include <stdio.h>
#include "mh2435.h"

#if (CONFIG_BOARD == 10)
#define GPIO_GREE	PE13
#define GPIO_RED	PE14
#else
#define GPIO_GREE	PC14
#define GPIO_RED	PC15
#endif




void BSP_GPIO_Config(void);

#endif  /* __BSP_LED_H__ */

