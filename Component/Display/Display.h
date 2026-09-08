
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "mh2435.h"

/* --------  Default Use/Config Definition  --------------------------------- */

#ifndef CONFIG_DISPLAY_MAX_LAYER
#define CONFIG_DISPLAY_MAX_LAYER 2
#endif

#define __ARGC64__(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, \
                   a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, \
                   a58, a59, a60, a61, a62, a63, a64, N, ...)                                                                                                  \
    N

#define __ARGC__(...)                                                                                                                                        \
    __ARGC64__(0, ##__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, \
               33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/* --------  Exported types  ------------------------------------------------ */

typedef enum {
    DisplayPortDPI,
    DisplayPortDSI,
    DisplayPortDBI,
} DisplayPortEnum;

typedef enum {
    DisplayColorRGB565 = 2,
    DisplayColorRGB888 = 3,
} DisplayColorEnum;

typedef enum {
    DisplayRefreshContinuous,
    DisplayRefreshAdaptive,
} DisplayRefreshEnum;

typedef struct {
    uint8_t B;
    uint8_t G;
    uint8_t R;
} RGB888PixelStruct;

typedef struct __PACKED {
    uint16_t B : 5;
    uint16_t G : 6;
    uint16_t R : 5;
} RGB565PixelStruct;

typedef enum {
    DisplayStateDrawing    = 0,
    DisplayStateReady      = 1,
    DisplayStateRefreshing = 2,
} DisplayStateEnum;

typedef struct DisplayStruct_s      DisplayStruct;
typedef struct DisplayLayerStruct_s DisplayLayerStruct;
typedef void (*ChangedCallbackType)(DisplayLayerStruct* layer);

struct DisplayLayerStruct_s {
    uint16_t XOffset;
    uint16_t YOffset;
    uint16_t Width;
    uint16_t Height;
    struct {
        union {
            uint32_t* Pointer;

            RGB565PixelStruct* RGB565;
            RGB888PixelStruct* RGB888;
        };
        uint32_t         Size;
        DisplayStateEnum State;
    } Buffer[2];
    LTDC_Layer_TypeDef* Register;
    DisplayColorEnum    Color;
    volatile void*      CurrentBuffer;
    volatile void*      NextBuffer;
    bool (*ChangeBuffer)(DisplayLayerStruct* layer, void* buffer, ChangedCallbackType changeCallback);
    void (*ChangedCallback)(DisplayLayerStruct* layer);
};

#include "DisplayDPI.h"

struct DisplayStruct_s {
    uint32_t ID;

    uint16_t HorizontalSyncActive : 8;
    uint16_t HorizontalBackPorch;
    uint16_t HorizontalActive;
    uint16_t HorizontalFrontPorch;

    uint16_t HorizontalDummy;

    uint16_t VerticalSyncActive : 8;
    uint16_t VerticalBackPorch;
    uint16_t VerticalActive;
    uint16_t VerticalFrontPorch;

    uint32_t PixelClock : 20;

    DisplayColorEnum Color : 8;

    DisplayPortEnum Port : 4;

    DisplayRefreshEnum RefreshMode : 1;

    // Port IO Config
    IOEnum ResetIO : 8;
    IOEnum TearingEffectIO : 8;
    bool   IsTearingEffectValidLow : 1;

    union {
        // IOList for DPI
        struct {
            IOEnum* IOList;
            uint8_t IOCount;
        };
        // DPI Config
        struct {
            uint64_t : 40;

            bool IsHsyncValidLow : 1;
            bool IsVsyncValidLow : 1;
            bool IsDataEnableValidLow : 1;
            bool IsPclkFallingSampling : 1;
        };
    };

    // Layer Config
    DisplayLayerStruct Layer[CONFIG_DISPLAY_MAX_LAYER];

    bool (*Init)(DisplayStruct* display);
    bool (*Start)(DisplayStruct* display, bool isStart);
    bool (*Refresh)(DisplayStruct* display, void* buffer, uint16_t width, uint16_t height, uint16_t offsetX, uint16_t offsetY,
                    ChangedCallbackType changeCallback);
    bool (*SyncCallback)(DisplayStruct* display, bool isBackPorch);
};

extern bool DisplayStartupSequence(DisplayStruct* display, bool isStart, bool isNegativeReset, uint8_t tPower, uint8_t tReset, uint8_t tReady);

extern bool DisplayLayerConstractor(DisplayStruct* display, uint32_t* buffer, uint32_t size);

extern void DisplayTestPattern(DisplayStruct* display);

#endif // __DISPLAY_H__
