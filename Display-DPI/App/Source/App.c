#include "DisplayBSP.h"
#include "TouchBSP.h"
#include "SDRAMBSP.h"

#ifndef CONFIG_DISPLAY_BUFFER_SIZE
#if USE_COMPONENT_SDRAM_BSP
#define CONFIG_DISPLAY_BUFFER_SIZE (800*480*4)
#else
#define CONFIG_DISPLAY_BUFFER_SIZE (480*480*4)
#endif
#endif


// Lvgl Display Buffer
#if CONFIG_DISPLAY_BUFFER_SIZE > 0
#if USE_COMPONENT_SDRAM_BSP
static uint32_t* DisplayBuffer = (uint32_t*)0x60000000;
#else
static uint32_t DisplayBuffer[CONFIG_DISPLAY_BUFFER_SIZE >> 2];
#endif
#endif


int main() {
#if USE_COMPONENT_SDRAM_BSP	
    SDRAMSetup();
#endif
	
    // select and setup an available display interface
    DisplayStruct* display = DisplaySelectSetup(DisplayBuffer, CONFIG_DISPLAY_BUFFER_SIZE);

    DisplayLayerStruct* layer = NULL;

    if (display) {
        DisplayTestPattern(display);
        display->Start(display, true);
        layer = display->Layer;
    }

    // Touch Setup
    TouchStruct* touch = NULL;

    if (display && TouchSetup(display->HorizontalActive, display->VerticalActive, false, false, false)) {
        touch = &TouchPort;
    }

    while (1) {
        if (!touch)
            continue;

        // Draw touch point
        bool isContinueRead = false;
        do {
            uint16_t x, y;

            bool isTouched;
            isContinueRead = touch->GetCoordinate(touch, &isTouched, &x, &y);

            if (!isTouched)
                continue;

            if ((x + 3) >= layer->Width || (y + 3) >= layer->Height)
                continue;

            for (uint16_t xo = 0; xo < 3; xo++)
                for (uint16_t yo = 0; yo < 3; yo++) {
                    layer->Buffer[0].RGB565[x + xo + (y + yo) * layer->Width].R = 0x1F;
                }
        }
        while (isContinueRead);

        SystemDelay(2);
    }
}
