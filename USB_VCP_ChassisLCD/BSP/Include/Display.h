#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "mh2435.h"	

#define  DISPLAT_ROTATION_0 	 	0x00
#define  DISPLAT_ROTATION_90 		0x01 
#define  DISPLAT_ROTATION_180		0x02
#define  DISPLAT_ROTATION_270		0x03



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

	
	uint8_t rotation;
    IOEnum ResetIO : 8;
    IOEnum BacklightIO : 8;
    DisplayColorEnum Color : 8;
	
    uint32_t PixelClock : 20;
	
	// IOList for DPI
	IOEnum* IOList;
    uint8_t IOCount;
	
	bool IsHsyncValidLow : 1;
	bool IsVsyncValidLow : 1;
	bool IsDataEnableValidLow : 1;
	bool IsPclkFallingSampling : 1;
};

typedef struct DisplayStruct_s      DisplayStruct;

bool DisplayStartupSequence(DisplayStruct* display, bool isStart, bool isNegativeReset, uint8_t tPower, uint8_t tReset, uint8_t tReady);

#endif