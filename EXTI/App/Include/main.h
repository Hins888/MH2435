// Copyright (c) 2011-2023 Megahunt Technologies Inc.
// SPDX-License-Identifier: Apache-2.0

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "mh2435.h"

#define LED1_PIN       GPIO_Pin_6
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_CLK  RCC_AHB1Periph_GPIOB

#define LED2_PIN       GPIO_Pin_7
#define LED2_GPIO_PORT GPIOB
#define LED2_GPIO_CLK  RCC_AHB1Periph_GPIOB

#define LED1_Toggle() LED1_GPIO_PORT->ODR ^= LED1_PIN
#define LED2_Toggle() LED2_GPIO_PORT->ODR ^= LED2_PIN

#endif /* __MAIN_H */
