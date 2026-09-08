#include "lcd_rgb_4_3inch_800x480.h"

#if (DPI_LCD == LCD_EVB_4_3INCH)

DisplayStruct DPI_display; 
 
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

	display->PixelClock	= DPI_PIXEL_CLOCK;		//kHz

    display->Color = DisplayColorRGB565;
	
	display->Pointer = Pointer;

	display->ResetIO = DPI_RESET_IO;
	
    DisplayStartupSequence(display, true, true, 5, 20, 120);
	
    return true;
}

#endif
