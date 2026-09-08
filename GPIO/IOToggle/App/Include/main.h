// Copyright (c) 2011-2023 Megahunt Technologies Inc.
// SPDX-License-Identifier: Apache-2.0

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "mh2435.h"

/* Exported define -----------------------------------------------------------*/
#ifdef MH2435_QFN88
#define IO_LED1 	PA2
#define IO_LED2 	PA3
#else
#define IO_LED1 	PB6
#define IO_LED2 	PB7
#endif
#endif /* __MAIN_H */
