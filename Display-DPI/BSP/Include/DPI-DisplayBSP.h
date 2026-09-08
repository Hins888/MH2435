#ifndef __DISPLAY_DPI_BSP_H__
#define __DISPLAY_DPI_BSP_H__

#include "Display.h"

// Select Board Configs of DPI

// DPI Board Configs
#define DPI_BACKLIGHT_IO PC8

#define CONFIG_DPI_IO_LIST                                                          \
    ((IOEnum[]) {                                                                   \
        PG7, PI10, PI9,  PF10,                         /* Sync Pins: CK HS VS DE */ \
        PG6, PH12, PH11, PH10, PH9,  PH8,  PH6,  PG13, /* R Pins: R7-R0          */ \
        PI2, PI1,  PI0,  PH15, PH14, PH13, PB0,  PB1,  /* G Pins: R7-R0          */ \
        PI7, PI6,  PI5,  PI4,  PI3,  PG10, PG12, PG14, /* B Pins: R7-R0          */ \
    })

#ifndef DPI_RESET_IO
#define DPI_RESET_IO IONone
#endif

#if defined(DPI_BACKLIGHT_IO) && !defined(DPI_BACKLIGHT_ON)
#define DPI_BACKLIGHT_ON(isOn) IOSet(DPI_BACKLIGHT_IO, isOn)
#endif

#ifndef CONFIG_DPI_IO_LIST
/*!
    @brief DPI IO List

    @param CK: #0 { PE14, PG7,  PI14     }
    @param HS: #1 { PC6,  PI10, PI12     }
    @param VS: #2 { PA4,  PI9,  PI13     }
    @param DE: #3 { PE13, PF10, PI10(13) }

    @param R7: #4  { PE15, PG6           }
    @param R6: #5  { PA8,  PH12,  PB1(9) }
    @param R5: #6  { PA12, PC0,   PH11   }
    @param R4: #7  { PA5,  PA11,  PH10   }
    @param R3: #8  { PH9,  PB0(9)        }
    @param R2: #9  { PA1,  PC10,  PH8    }
    @param R1: #10 { PA2,  PH3,   PH6    }
    @param R0: #11 { PG13, PH2,   PI15   }

    @param G7: #12 { PB5,  PD3,  PG8,    PI2 }
    @param G6: #13 { PC7,  PI1,  PI11(9)     }
    @param G5: #14 { PB11, PI0,  PH4(9)      }
    @param G4: #15 { PB10, PH4,  PH15        }
    @param G3: #16 { PE11, PH14, PG10(9)     }
    @param G2: #17 { PA6,  PH13, PI15(9)     }
    @param G1: #18 { PB0,  PE6               }
    @param G0: #19 { PB1,  PE5               }

    @param B7: #20 { PB9,  PI7                   }
    @param B6: #21 { PB8,  PI6                   }
    @param B5: #22 { PA3,  PI5                   }
    @param B4: #23 { PE13, PI4,  PA5(9), PG12(9) }
    @param B3: #24 { PD10, PG11, PI3             }
    @param B2: #25 { PD6,  PG10, PA3(9)          }
    @param B1: #26 { PG12                        }
    @param B0: #27 { PE4,  PG14                  }
*/
#endif

#ifndef CONFIG_DPI_IO_COUNT
#define CONFIG_DPI_IO_COUNT (sizeof(CONFIG_DPI_IO_LIST) / sizeof(IOEnum))
#endif

// Panel Presets
#define DISPLAY_DPI_PRESET_800x480  0
#define DISPLAY_DPI_PRESET_480x800  1
#define DISPLAY_DPI_PRESET_1024x600 2

#ifndef CONFIG_DISPLAY_DPI_PRESET
#define CONFIG_DISPLAY_DPI_PRESET DISPLAY_DPI_PRESET_800x480
#endif

/*!
    TargetPixelClock:    |      30 |      30 |     42 | MHz
    TargetPixelPeriod:   |   33.33 |   33.33 |  23.81 | ns
    TargetFPS:           |      60 |      60 |     56 | Frame
    FrameClockCount:     |  500000 | 5000000 | 750000 |
    TargetHorizonCount:  |    1000 |     558 |   1200 |
    TargetVerticalCount: |     500 |     896 |    625 |
    ---------------------+---------+---------+--------+
    HSA                  |      20 |       8 |      6 |
    HBP                  |      80 |      10 |    160 |
    HACT                 |     800 |     480 |   1024 |
    HFP                  |     100 |      60 |     10 |
    ---------------------+---------+---------+--------+
    VSA                  |       6 |       2 |      1 |
    VBP                  |       6 |      18 |     22 |
    VACT                 |     480 |     800 |    600 |
    VFP                  |       8 |      76 |      2 |
*/
#if CONFIG_DISPLAY_DPI_PRESET == DISPLAY_DPI_PRESET_800x480
#define DPI_PIXEL_CLOCK 30000

#define DPI_HSA  20
#define DPI_HBP  80
#define DPI_HACT 800
#define DPI_HFP  100

#define DPI_VSA  6
#define DPI_VBP  6
#define DPI_VACT 480
#define DPI_VFP  8
#elif CONFIG_DISPLAY_DPI_PRESET == DISPLAY_DPI_PRESET_480x800
#define DPI_PIXEL_CLOCK 30000

#define DPI_HSA  8
#define DPI_HBP  10
#define DPI_HACT 480
#define DPI_HFP  60

#define DPI_VSA  2
#define DPI_VBP  18
#define DPI_VACT 800
#define DPI_VFP  76
#elif CONFIG_DISPLAY_DPI_PRESET == DISPLAY_DPI_PRESET_1024x600
#define DPI_PIXEL_CLOCK 42000

#define DPI_HSA  6
#define DPI_HBP  160
#define DPI_HACT 1024
#define DPI_HFP  10

#define DPI_VSA  1
#define DPI_VBP  22
#define DPI_VACT 600
#define DPI_VFP  2
#endif

extern DisplayStruct DPIDisplay;

extern bool DPIDisplayConfig(DisplayStruct* display);

extern bool DPIDisplaySetup(uint32_t* buffer, uint32_t size);

#endif // __DPI_PANELS_H__
