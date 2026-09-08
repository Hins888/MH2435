#ifndef __BSP_TRNG_H
#define __BSP_TRNG_H

#include "mh2435.h"
#include "./usart/bsp_debug_usart.h"

#define TIMES   (0x03)

ErrorStatus RNG_GetRandom(void);
ErrorStatus RNG_ReadDepth(void);
ErrorStatus RNG_ReadOverflow(void);
ErrorStatus RNG_GetComplementaryRandomNumber(void);
ErrorStatus RNG_Interrupt(void);

#endif	/* __BSP_TRNG_H */

