#include "DPI-DisplayBSP.h"

DisplayStruct DPIDisplay;

static bool DPIPanelConstractor(DisplayStruct* display) {
    DisplayStartupSequence(display, true, true, 5, 20, 120);

    display->PixelClock = DPI_PIXEL_CLOCK;

    display->HorizontalSyncActive = DPI_HSA;
    display->HorizontalBackPorch  = DPI_HBP;
    display->HorizontalActive     = DPI_HACT;
    display->HorizontalFrontPorch = DPI_HFP;

    display->VerticalSyncActive = DPI_VSA;
    display->VerticalBackPorch  = DPI_VBP;
    display->VerticalActive     = DPI_VACT;
    display->VerticalFrontPorch = DPI_VFP;

    display->Color = DisplayColorRGB565;

    return true;
}

__WEAK bool DPIDisplayConfig(DisplayStruct* display) {
    return true;
}

static IOEnum dpiIOList[] = CONFIG_DPI_IO_LIST;

bool DPIDisplaySetup(uint32_t* buffer, uint32_t size) {

#ifdef DPI_BACKLIGHT_IO
    IOSetup(DPI_BACKLIGHT_IO, IO_DEFAULT_OUTPUT_CONFIG);
    DPI_BACKLIGHT_ON(true);
#endif

    DisplayStruct* display = &DPIDisplay;
    DisplayDPIConstractor(display);
    display->ResetIO         = DPI_RESET_IO;
    display->TearingEffectIO = IONone;

    display->IOCount = CONFIG_DPI_IO_COUNT;
    display->IOList  = dpiIOList;

    // Panel constractor
    if (!DPIPanelConstractor(display))
        return false;

    if (!DPIDisplayConfig(display))
        return false;

    DisplayLayerConstractor(display, buffer, size);

    return display->Init(display);
}
