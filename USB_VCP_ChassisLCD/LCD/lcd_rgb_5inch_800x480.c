#include "lcd_rgb_5inch_800x480.h"

#if (USE_800_480_565 == USE_LCD_TYPE || USE_800_480_888 == USE_LCD_TYPE)


static IOEnum dpiIOList[] = CONFIG_DPI_IO_LIST;
bool DPIPanelConstractor(DisplayStruct* display,uint32_t Pointer) {
    display->HorizontalSyncActive = DPI_HSA;
    display->HorizontalBackPorch  = DPI_HBP;
    display->HorizontalActive     = DPI_HACT;
    display->HorizontalFrontPorch = DPI_HFP;

    display->VerticalSyncActive = DPI_VSA;
    display->VerticalBackPorch  = DPI_VBP;
    display->VerticalActive     = DPI_VACT;
    display->VerticalFrontPorch = DPI_VFP;

	display->IsHsyncValidLow       = false;
    display->IsVsyncValidLow       = false;
    display->IsDataEnableValidLow  = false;
    display->IsPclkFallingSampling = true;

    display->Color = DPI_DISPLAY_COLOR;
	display->Pointer = Pointer;

#ifdef DPI_PIXEL_CLOCK
	display->PixelClock	= DPI_PIXEL_CLOCK;		//kHz
#else
	display->PixelClock	= 25000;		//kHz
#endif

#ifdef DPI_DISPLAY_ROTATION
	display->rotation = DPI_DISPLAY_ROTATION;
#else
	display->rotation = DISPLAT_ROTATION_0;
#endif

	display->ResetIO = DPI_RESET_IO;
	display->BacklightIO = DPI_BACKLIGHT_IO;
	
	display->IOCount = CONFIG_DPI_IO_COUNT;
    display->IOList  = dpiIOList;
	
    DisplayStartupSequence(display, true, true, 5, 20, 120);
	
    return true;
}

#endif
