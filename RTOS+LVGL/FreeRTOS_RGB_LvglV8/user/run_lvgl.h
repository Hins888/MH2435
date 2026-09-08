#ifndef _RUN_LVGL_H__
#define _RUN_LVGL_H__

#include "mh2435.h"

#include "rtos-freertos.h"
#include "lvgl_config.h"


extern TaskHandle_t RunLvglHandle;

void RunLvgl_Create(void);



#endif
