#ifndef __BSP_RGB_LCD_H__
#define __BSP_RGB_LCD_H__

#include <stdio.h>
#include "mh2435.h"	
	
typedef enum {
    DisplayColorRGB565 = 0x02,
    DisplayColorRGB888 = 0x03,	//When configuring RGB666, use RGB888
} DisplayColorEnum;


struct DisplayStruct_s {
	uint32_t Pointer;
	
    uint16_t HorizontalSyncActive : 8;
    uint16_t HorizontalBackPorch;
    uint16_t HorizontalActive;
    uint16_t HorizontalFrontPorch;

    uint16_t VerticalSyncActive : 8;
    uint16_t VerticalBackPorch;
    uint16_t VerticalActive;
    uint16_t VerticalFrontPorch;

	
    IOEnum ResetIO : 8;
    DisplayColorEnum Color : 8;
	
    uint32_t PixelClock : 20;
	
	bool IsHsyncValidLow : 1;
	bool IsVsyncValidLow : 1;
	bool IsDataEnableValidLow : 1;
	bool IsPclkFallingSampling : 1;
};

typedef struct DisplayStruct_s      DisplayStruct;

void BL_Unit_Config(uint8_t unit);
void LTDC_Config(DisplayStruct* display);
bool DisplayStartupSequence(DisplayStruct* display, bool isStart, bool isNegativeReset, uint8_t tPower, uint8_t tReset, uint8_t tReady);

#endif  /* __BSP_RGB_LCD_H__ */

