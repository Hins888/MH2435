#include "Display.h"


#define DisplayReset(isReset, isNegativeReset) IOSet(display->ResetIO, isReset ^ isNegativeReset)

DisplayStruct DPI_display;

bool DisplayStartupSequence(DisplayStruct* display, bool isStart, bool isNegativeReset, uint8_t tPower, uint8_t tReset, uint8_t tReady) {
    if (display->ResetIO == IONone)
        return false;

    IOSetup(display->ResetIO, IO_DEFAULT_OUTPUT_CONFIG);

    if (isStart) {
        DisplayReset(false, isNegativeReset);
        SystemDelay(tPower);
        DisplayReset(true, isNegativeReset);
        SystemDelay(tReset);
        DisplayReset(false, isNegativeReset);
        SystemDelay(tReady);
    }
    else {
        DisplayReset(true, isNegativeReset);
        SystemDelay(tReset);
    }

    return true;
}



