#include "Display.h"

void DisplayTestPattern(DisplayStruct* display) {
    DisplayLayerStruct* layer = &display->Layer[0];
    if (display->Color == DisplayColorRGB888) {
        RGB888PixelStruct* p24 = layer->Buffer->RGB888;

        for (int y = 0; y < layer->Height; y++) {
            for (int x = 0; x < layer->Width; x++) {
                p24->R = 0;
                p24->G = (y * 16 / layer->Height) * 16;
                p24->B = (x * 8 / layer->Width) * 32;
                p24++;
            }
        }
    }
    else {
        RGB565PixelStruct* p16 = layer->Buffer->RGB565;
        for (int y = 0; y < layer->Height; y++) {
            for (int x = 0; x < layer->Width; x++) {
                p16->R = 0;
                p16->G = (y * 16 / layer->Height) * 4;
                p16->B = (x * 8 / layer->Width) * 4;
                p16++;
            }
        }

        // Draw test frame
        for (int y = 0; y < layer->Height; y++) {
            p16    = &layer->Buffer->RGB565[y * layer->Width + layer->Width - 2];
            p16->R = 0;
            p16->G = 63;
            p16->B = 0;

            p16++;
            p16->R = 31;
            p16->G = 0;
            p16->B = 0;

            *(p16 + 1 - layer->Width) = *p16;
        }

        p16 = layer->Buffer->RGB565;
        for (int x = 0; x < layer->Width; x++) {
            p16->R = 31;
            p16->G = 0;
            p16->B = 0;

            *(p16 + (uint32_t)(layer->Height - 1) * layer->Width) = *p16;
            p16++;
        }
    }
}

#define DisplayReset(isReset, isNegativeReset) IOSet(display->ResetIO, isReset ^ isNegativeReset)
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

bool DisplayLayerChangeBuffer(DisplayLayerStruct* layer, void* buffer, void (*ChangedCallback)(DisplayLayerStruct* layer)) {
    if (layer->NextBuffer)
        return false;

    if (buffer)
        layer->NextBuffer = buffer;
    else
        layer->NextBuffer = layer->CurrentBuffer == layer->Buffer[0].Pointer ? layer->Buffer[1].Pointer : layer->Buffer[0].Pointer;

    if (ChangedCallback) {
        layer->ChangedCallback = ChangedCallback;
    }
    else {
        while (layer->NextBuffer) {
            __NOP();
        }
    }
    return true;
}

bool DisplayLayerConstractor(DisplayStruct* display, uint32_t* buffer, uint32_t size) {
    // default layer constractor
    DisplayLayerStruct* layer = display->Layer;

    uint32_t targetSize = 0;

    uint32_t* pointer = buffer;

    for (int i = 0; i < CONFIG_DISPLAY_MAX_LAYER; i++) {
        layer->ChangeBuffer = DisplayLayerChangeBuffer;

        layer->Color = display->Color;
        // Buffer 0
        targetSize = display->HorizontalActive * display->VerticalActive * display->Color;
        targetSize = ((targetSize + 3) >> 2) << 2; // 4-Byte align
        if (targetSize <= size) {
            layer->Width  = display->HorizontalActive;
            layer->Height = display->VerticalActive;
        }
        else if (i > 0) {
            break;
        }
        else {
            // Only buffer 0 is valid
            layer->Width   = display->HorizontalActive;
            layer->Height  = size / (display->HorizontalActive * display->Color);
            layer->YOffset = (display->VerticalActive - layer->Height) / 2;
            targetSize     = layer->Width * layer->Height * display->Color;
        }

        layer->CurrentBuffer     = pointer;
        layer->Buffer[0].Pointer = pointer;
        layer->Buffer[0].Size    = targetSize;

        size -= targetSize;
        pointer += targetSize >> 2;

        // Buffer 1
        targetSize = layer->Width * layer->Height * display->Color;
        targetSize = ((targetSize + 3) >> 2) << 2; // 4-Byte align
        if (targetSize <= size) {
            layer->Buffer[1].Pointer = pointer;
            layer->Buffer[1].Size    = targetSize;
        }
        else {
            break;
        }

        size -= targetSize;
        pointer += targetSize / 4;

        layer++;
    }

    return true;
}
