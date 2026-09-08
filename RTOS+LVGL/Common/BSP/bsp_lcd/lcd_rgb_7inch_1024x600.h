#ifndef __DIP_7INCH_H__
#define __DIP_7INCH_H__

#include <stdio.h>
#include "mh2435.h"
#include "lvgl_config.h"

#if (DPI_LCD == LCD_EVB_7INCH)
#define DPI_RESET_IO     PA8
#define DPI_HSA          20
#define DPI_HBP          160
#define DPI_HFP          180
#define DPI_HACT         1024

#define DPI_VSA          20
#define DPI_VBP          20
#define DPI_VFP          20
#define DPI_VACT         600

#define DPI_PIXEL_CLOCK	  25000	//kHz

#ifndef DPI_RESET_IO
#define DPI_RESET_IO IONone
#endif
	
#define MY_DISP_HOR_RES 	DPI_HACT
#define MY_DISP_VER_RES 	DPI_VACT
	
extern DisplayStruct DPI_display;
	
bool DPIPanelConstractor(DisplayStruct* display,uint32_t Pointer);
	
#endif

#endif  /* __DIP_7INCH_H__ */

