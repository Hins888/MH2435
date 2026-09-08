#ifndef _RUN_LED_H__
#define _RUN_LED_H__

#include "mh2435.h"

#include "rtos-freertos.h"
#include "lvgl_config.h"

extern TaskHandle_t RunLedHandle;

void RunLed_Create(void);

#endif
