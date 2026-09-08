#include "DisplayDPI.h"

static DisplayStruct* dpiDisplay;

#if defined(__CC_ARM)
__ASM static void XFCSetClock(uint32_t xfcParam) {
    mov32 r1, #__cpp(QSPI_BASE + 0x14);
    str   r0, [r1];

    ; // clang-format off
0   ldr   r2, [r1];
    cmp   r2, r0;
    bne   %b0;
    ; // clang-format on

    bx lr;
}
#else
__attribute__((noinline)) static void XFCSetClock(uint32_t xfcParam) {
    __ASM volatile( //
        "str  %[rP], [%[rQ]]\n"

        "0:\n"
        "    ldr  r2, [%[rQ]]\n"
        "    cmp  r2, %[rP]\n"
        "    bne  0b\n" //
        : :             //
        [rQ] "r"(QSPI_BASE + 0x14),
        [rP] "r"(xfcParam) //
    );
}
#endif

static void DPIClockDivide(uint8_t divisor) {
    ClockNodeEnum systemClock  = ClockGetSelection(ClockNodeSYSS);
    ClockNodeEnum clockSource  = ClockGetSelection(ClockNodeSRC);
    uint32_t      xfcParam     = QSPI->DEVICE_PARA;
    uint32_t      xfcSlowParam = QSPI->DEVICE_PARA & ~QSPI_DEVICE_PARA_TIMMING;

    volatile bool isCacheHit = false;
CacheHit:
    // Step 1: Select clock source as system clock
    if (isCacheHit) {
        ClockSelect(ClockNodeSYSS, clockSource);
        XFCSetClock(xfcSlowParam);
    }

    // Step 2: Config PLL clock
    if (isCacheHit) {
        SystemCoreClockUpdate();

        // Main PLL
        ClockEnable(ClockNodePLL1G, false);
        ClockDivide(ClockNodePLL1R, divisor);
        ClockEnable(ClockNodePLL1G, true);
    }

    // Step 3: Select system PLL clock as system clock
    if (isCacheHit) {
        ClockSelect(ClockNodeSYSS, systemClock);
        XFCSetClock(xfcParam);
    }

    // Step 0: Hit Cache
    if (!isCacheHit) {
        ClockSelect(ClockNodeSYSS, ClockGetSelection(ClockNodeSYSS));
        XFCSetClock(xfcParam);
        isCacheHit = true;
        goto CacheHit;
    }

    SystemCoreClockUpdate();
}

bool DisplayDPIClockInit(DisplayStruct* display) {
    //  - Display Pixel Clock
    ClockNodeEnum pixelClockNode = ClockNodePLL1R;
    if ((ClockGetSelection(ClockNodeDPC) != pixelClockNode) || (ClockGet(ClockNodeDPC) != display->PixelClock * 1000)) {
        uint32_t pll1Clock         = ClockGet(ClockNodePLL1);
        uint32_t pixelClockDivisor = pll1Clock / display->PixelClock / 1000;

        // reduce divisor clock error
        uint32_t clockError = pll1Clock / pixelClockDivisor - (display->PixelClock * 1000);
        if (clockError && ((display->PixelClock * 1000) - pll1Clock / (pixelClockDivisor + 1) < clockError))
            pixelClockDivisor += 1;

        DPIClockDivide(pixelClockDivisor);
    }
    ClockSelect(ClockNodeDPC, pixelClockNode);
    return true;
}

bool DisplayDPIInit(DisplayStruct* display) {
    DisplayDPIClockInit(display);

    PeripheralEnable(PeripheralLTDC, true);
    PeripheralReset(PeripheralLTDC);

    dpiDisplay = display;

    LTDC_InitTypeDef dpiConfig;

    // DPI IO Setup
    if (display->Port == DisplayPortDPI) {
        dpiConfig.LTDC_HSPolarity = display->IsHsyncValidLow ? LTDC_HSPolarity_AL : LTDC_HSPolarity_AH;
        dpiConfig.LTDC_VSPolarity = display->IsVsyncValidLow ? LTDC_VSPolarity_AL : LTDC_VSPolarity_AH;
        dpiConfig.LTDC_DEPolarity = display->IsDataEnableValidLow ? LTDC_DEPolarity_AL : LTDC_DEPolarity_AH;
        dpiConfig.LTDC_PCPolarity = display->IsPclkFallingSampling ? LTDC_PCPolarity_IIPC : LTDC_PCPolarity_IPC;

        if (display->IOCount) {
            IOConfigStruct ioConfig = MakeIOConfig(IOModeAlternate, GPIO_AF_LTDC, IOPullNone, IOSpeedMedium, IODriveMedium);

            IOEnum* dpiIO = display->IOList;
            for (int i = 0; i < display->IOCount; i++) {
                if (*dpiIO == IONone) {
                    dpiIO++;
                    continue;
                }
                else if ((i == 3 && *dpiIO == PI10)) {
                    ioConfig.Alternate = 13;
                }
                else if (                                             //
                    (i == 5 && *dpiIO == PB1) ||                      //
                    (i == 8 && *dpiIO == PB0) ||                      //
                    (i == 13 && *dpiIO == PI11) ||                    //
                    (i == 14 && *dpiIO == PH4) ||                     //
                    (i == 16 && *dpiIO == PG10) ||                    //
                    (i == 17 && *dpiIO == PI15) ||                    //
                    (i == 23 && (*dpiIO == PA5 || *dpiIO == PG12)) || //
                    (i == 25 && *dpiIO == PA3)                        //
                ) {
                    ioConfig.Alternate = 9;
                }
                IOSetup(*(dpiIO++), ioConfig);
            }
        }
    }
    else {
        dpiConfig.LTDC_HSPolarity = LTDC_HSPolarity_AH;
        dpiConfig.LTDC_VSPolarity = LTDC_VSPolarity_AH;
        dpiConfig.LTDC_DEPolarity = LTDC_DEPolarity_AH;
        dpiConfig.LTDC_PCPolarity = LTDC_PCPolarity_IPC;
    }

    /* Timing configuration */
    //  - Horizontal synchronization width = HSA - 1
    //  - Accumulated horizontal back porch = HSA + HBP - 1
    //  - Accumulated active width = HSA + HBP + HACT - 1
    //  - Total width = HSA + HBP + Active Width + HFP - 1
    dpiConfig.LTDC_HorizontalSync     = display->HorizontalSyncActive - 1;
    dpiConfig.LTDC_AccumulatedHBP     = display->HorizontalSyncActive + display->HorizontalBackPorch - 1;
    dpiConfig.LTDC_AccumulatedActiveW = dpiConfig.LTDC_AccumulatedHBP + display->HorizontalActive + display->HorizontalDummy;
    dpiConfig.LTDC_TotalWidth         = dpiConfig.LTDC_AccumulatedActiveW + display->HorizontalFrontPorch;

    //  - Vertical synchronization height = VSA - 1
    //  - Accumulated vertical back porch = VSA + VBP - 1
    //  - Accumulated active height = VSA + VBP + Active Heigh - 1
    //  - Total height = VSA + VBP + Active Heigh + VFP - 1
    dpiConfig.LTDC_VerticalSync       = display->VerticalSyncActive - 1;
    dpiConfig.LTDC_AccumulatedVBP     = display->VerticalSyncActive + display->VerticalBackPorch - 1;
    dpiConfig.LTDC_AccumulatedActiveH = dpiConfig.LTDC_AccumulatedVBP + display->VerticalActive;
    dpiConfig.LTDC_TotalHeigh         = dpiConfig.LTDC_AccumulatedActiveH + display->VerticalFrontPorch;

    // fix clock error by +/- HorizontalFrontPorch
    uint32_t pixelClock = ClockGet(ClockNodeDPC) / 1000;
    if (pixelClock != display->PixelClock) {
        int clockError = pixelClock - display->PixelClock;

        int total  = dpiConfig.LTDC_TotalWidth + 1;
        int offset = clockError * total / (int)display->PixelClock;
        if (offset < 0) {
            int min = -display->HorizontalFrontPorch / 2;
            offset  = min > offset ? min : offset;
        }
        dpiConfig.LTDC_TotalWidth = total + offset - 1;
    }

    /* Configure R,G,B component values for LCD background color */
    dpiConfig.LTDC_BackgroundRedValue   = 0xFF;
    dpiConfig.LTDC_BackgroundGreenValue = 0;
    dpiConfig.LTDC_BackgroundBlueValue  = 0;

    /* Initialize LTDC */
    LTDC_Init(&dpiConfig);

    DisplayLayerStruct* layer = display->Layer;

    LTDC_Layer_InitTypeDef layerConfig;
    /* Layer1 Configuration ------------------------------------------------------*/
    /* Windowing configuration */
    //  - Horizontal start = horizontal synchronization + Horizontal back porch
    //  - Vertical start   = vertical synchronization + vertical back porch
    //  - Horizontal stop = Horizontal start + window width - 1
    //  - Vertical stop   = Vertical start + window height - 1
    layerConfig.LTDC_HorizontalStart = display->HorizontalSyncActive + display->HorizontalBackPorch + layer->XOffset + display->HorizontalDummy;
    layerConfig.LTDC_HorizontalStop  = layerConfig.LTDC_HorizontalStart - 1 + layer->Width;
    layerConfig.LTDC_VerticalStart   = display->VerticalSyncActive + display->VerticalBackPorch + layer->YOffset;
    layerConfig.LTDC_VerticalStop    = layerConfig.LTDC_VerticalStart - 1 + layer->Height;

    /* Pixel Format configuration*/
    layerConfig.LTDC_PixelFormat = layer->Color == DisplayColorRGB565 ? LTDC_Pixelformat_RGB565 : LTDC_Pixelformat_RGB888;

    /* Alpha constant (255 totally opaque) */
    layerConfig.LTDC_ConstantAlpha = 255;

    /* Default Color configuration (configure A,R,G,B component values) */
    layerConfig.LTDC_DefaultColorBlue  = 0xFF;
    layerConfig.LTDC_DefaultColorGreen = 0;
    layerConfig.LTDC_DefaultColorRed   = 0;
    layerConfig.LTDC_ConstantAlpha     = 255;
    layerConfig.LTDC_DefaultColorAlpha = 0;
    /* Configure blending factors */
    layerConfig.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
    layerConfig.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

    /* Start Address configuration : frame buffer is located at FLASH memory */
    layerConfig.LTDC_CFBStartAdress = (uint32_t)layer->Buffer[0].Pointer;

    /* the length of one line of pixels in bytes + 3 then :
     Line Lenth = Active high width x number of bytes per pixel + 3
     Active high width         = 480
     number of bytes per pixel = 3    (pixel_format : RGB888) */
    layerConfig.LTDC_CFBLineLength = layer->Width * layer->Color;

    /*  the pitch is the increment from the start of one line of pixels to the
      start of the next line in bytes, then :
      Pitch = Active high width x number of bytes per pixel     */
    layerConfig.LTDC_CFBPitch = layer->Width * layer->Color;

    /* Configure the number of lines */
    layerConfig.LTDC_CFBLineNumber = layer->Height;

    /* Initializes the Layer */
    LTDC_LayerInit(LTDC_Layer1, &layerConfig);

    /* Enable Layer 1 */
    LTDC_LayerCmd(LTDC_Layer1, ENABLE);

    /* Reload configuration of Layer 1 */
    LTDC_ReloadConfig(LTDC_IMReload);

#if FEATURE_LTDC_SUPPORT_BURST
    LTDC->AHBCFG = 0x37;
#endif

    // Interrupt Config
    LTDC_LIPConfig(layerConfig.LTDC_VerticalStop + 1);
    LTDC_ITConfig(LTDC_IT_LI, ENABLE);
    NVIC_SetPriority(LTDC_IRQn, 3);
    NVIC_EnableIRQ(LTDC_IRQn);

    return true;
}

bool DisplayDPIStart(DisplayStruct* display, bool isStart) {
    LTDC_Cmd(isStart);
    return true;
}

static bool DisplayDPIRefresh(DisplayStruct* display, void* buffer, uint16_t width, uint16_t height, uint16_t offsetX, uint16_t offsetY,
                              void (*ChangedCallback)(DisplayLayerStruct* layer)) {
    DisplayLayerStruct* layer = display->Layer;
    if (display->RefreshMode == DisplayRefreshContinuous)
        return layer->ChangeBuffer(layer, buffer, ChangedCallback);

    layer->NextBuffer = buffer;
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

void LTDC_IRQHandler(void) {
    if (LTDC_GetFlagStatus(LTDC_IT_FU)) {
        LTDC_ClearFlag(LTDC_IT_FU);
    }

    if (!LTDC_GetFlagStatus(LTDC_IT_LI))
        return;

    LTDC_ClearFlag(LTDC_IT_LI);

    if (!dpiDisplay)
        return;

    // Display Port Sync Callback
    if (dpiDisplay->SyncCallback)
        dpiDisplay->SyncCallback(dpiDisplay, false);

    // Display Layer Update
    DisplayLayerStruct* layer = dpiDisplay->Layer;
    if (layer->NextBuffer && dpiDisplay->RefreshMode == DisplayRefreshContinuous) {
        layer->Register->CFBAR = (uint32_t)layer->NextBuffer;
        LTDC->SRCR             = LTDC_SRCR_VBR;

        layer->CurrentBuffer = layer->NextBuffer;
        layer->NextBuffer    = NULL;
        if (layer->ChangedCallback) {
            layer->ChangedCallback(layer);
            layer->ChangedCallback = NULL;
        }
    }

    if (LTDC->DSI_WIER) {
        if (LTDC->DSI_WISR & BIT0) {
            LTDC->DSI_WIFCR = BIT0;
            LTDC->DSI_WIFCR = 0;
        }
    }

    if (LTDC_GetFlagStatus(LTDC_IT_RR)) {
        LTDC_ClearFlag(LTDC_IT_RR);
    }
}

bool DisplayDPIConstractor(DisplayStruct* display) {
    memset(display, 0, sizeof(DisplayStruct));

    display->Port    = DisplayPortDPI;
    display->ResetIO = IONone;
    display->Color   = DisplayColorRGB565;

    display->Init    = DisplayDPIInit;
    display->Start   = DisplayDPIStart;
    display->Refresh = DisplayDPIRefresh;

    display->IsHsyncValidLow       = false;
    display->IsVsyncValidLow       = false;
    display->IsDataEnableValidLow  = false;
    display->IsPclkFallingSampling = true;

    display->Layer[0].Register = (LTDC_Layer_TypeDef*)LTDC_Layer1_BASE;
    display->Layer[1].Register = (LTDC_Layer_TypeDef*)LTDC_Layer2_BASE;

    return true;
}
