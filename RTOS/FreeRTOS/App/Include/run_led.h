#ifndef _RUN_LED_H
#define _RUN_LED_H

#include "mh2435.h"

#include "RetargetIO.h"
#include "rtos-freertos.h"

extern TaskHandle_t RunLedHandle;

void RunLed_Create(void);

#endif
