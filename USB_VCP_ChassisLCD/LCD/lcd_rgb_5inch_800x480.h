#ifndef __DIP_5INCH_H__
#define __DIP_5INCH_H__
#include <stdio.h>
#include "Display.h"

#if (USE_800_480_565 == USE_LCD_TYPE || USE_800_480_888 == USE_LCD_TYPE )

#if USE_800_480_888 == USE_LCD_TYPE 
#define DPI_DISPLAY_COLOR		DisplayColorRGB888
#define DPI_USE_SDRAM			1
#define CONFIG_DPI_IO_LIST                                                          \
    ((IOEnum[]) {                                                                   \
        PG7, PI10, PI9,  PF10,                         /* Sync Pins: CK HS VS DE */ \
        PG6, PH12, PH11, PH10, PH9,  PH8,  PH6,  PG13, /* R Pins: R7-R0          */ \
        PI2, PI1,  PI0,  PH15, PH14, PH13, PB0,  PB1,  /* G Pins: R7-R0          */ \
        PI7, PI6,  PI5,  PI4,  PG11, PG10, PG12, PG14, /* B Pins: R7-R0          */ \
    })
	
#else
#define DPI_DISPLAY_COLOR		DisplayColorRGB565
#define DPI_USE_SDRAM			1
#define CONFIG_DPI_IO_LIST                                                          \
    ((IOEnum[]) {                                                                   \
        PG7, PI10, PI9,  PF10,                         /* Sync Pins: CK HS VS DE */ \
        PG6, PH12, PH11, PH10, PH9,  IONone,  IONone,  IONone, /* R Pins: R7-R0          */ \
        PI2, PI1,  PI0,  PH15, PH14, PH13, IONone,  IONone,  /* G Pins: R7-R0          */ \
        PI7, PI6,  PI5,  PI4,  PG11, IONone, IONone, IONone, /* B Pins: R7-R0          */ \
    })
	
#endif
	
#ifndef CONFIG_DPI_IO_COUNT
#define CONFIG_DPI_IO_COUNT (sizeof(CONFIG_DPI_IO_LIST) / sizeof(IOEnum))
#endif

#define DPI_RESET_IO     IONone
		
#define DPI_BACKLIGHT_IO PA4

#define DPI_PIXEL_CLOCK 25000

#define DPI_HSA  20
#define DPI_HBP  80
#define DPI_HACT 800
#define DPI_HFP  100

#define DPI_VSA  6
#define DPI_VBP  6
#define DPI_VACT 480
#define DPI_VFP  8


#ifndef DPI_RESET_IO
#define DPI_RESET_IO IONone
#endif
	
#define MY_DISP_HOR_RES 	DPI_HACT
#define MY_DISP_VER_RES 	DPI_VACT

	
extern DisplayStruct DPI_display;
	
bool DPIPanelConstractor(DisplayStruct* display,uint32_t Pointer);
	
#endif

#endif  /* __DIP_5INCH_H__ */

