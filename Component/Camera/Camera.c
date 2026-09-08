#include "Camera.h"

bool CameraClockInit(CameraStruct* camera) {
    if (camera->ClockIO == IONone) {
        if (camera->ClockSetup) {
            camera->ClockSetup();
        }
        return true;
    }

    // Enable timer clock
    PeripheralEnable(camera->Timer, true);
    PeripheralReset(camera->Timer);

    uint8_t timerAF    = 0;
    uint8_t timerIndex = camera->Timer - PeripheralTIMStart;
    timerIndex++;
    if (timerIndex <= 2)
        timerAF = 1;
    else if (timerIndex <= 5)
        timerAF = 2;
    else if (timerIndex <= 11)
        timerAF = 3;
    else if (timerIndex <= 14)
        timerAF = 9;

    // Configure XCLK IO as alternate function
    IOConfigStruct ioConfig = MakeIOConfig(IOModeAlternate, timerAF, IOPullNone, IOSpeedHigh, IODriveLow);
    IOSetup(camera->ClockIO, ioConfig);

    TIM_TypeDef* tim = (TIM_TypeDef*)PeripheralMap[camera->Timer].Base;

    uint32_t timerRootClock = ClockGet(PeripheralMap[camera->Timer].Node) / 1000;
    uint16_t timerDivisor   = timerRootClock / camera->FrequencyKHz;

    if (timerDivisor == 0)
        return false;

    // reduce timer divisor clock error
    uint32_t timerError = timerRootClock / timerDivisor - camera->FrequencyKHz;
    if (timerError && (camera->FrequencyKHz - timerRootClock / (timerDivisor + 1) < timerError))
        timerDivisor += 1;

    // Configure timer to generate square wave
    TIM_TimeBaseInitTypeDef timerConfig = {0};
    timerConfig.TIM_Period              = 1;
    timerConfig.TIM_Prescaler           = timerDivisor - 1;
    timerConfig.TIM_ClockDivision       = 0;
    timerConfig.TIM_CounterMode         = TIM_CounterMode_Up;
    TIM_TimeBaseInit(tim, &timerConfig);

    TIM_OCInitTypeDef timerOC = {0};
    timerOC.TIM_OCMode        = TIM_OCMode_PWM1;
    timerOC.TIM_OutputState   = TIM_OutputState_Enable;
    timerOC.TIM_Pulse         = 1;
    timerOC.TIM_OCPolarity    = TIM_OCPolarity_High;
    switch (camera->TimerChannel) {
        case 1:
            TIM_OC1Init(tim, &timerOC);
            break;
        case 2:
            TIM_OC2Init(tim, &timerOC);
            break;
        case 3:
            TIM_OC3Init(tim, &timerOC);
            break;
        case 4:
            TIM_OC4Init(tim, &timerOC);
            break;
        default:
            break;
    }

    // for advance timer
    TIM_CtrlPWMOutputs(tim, ENABLE);

    TIM_Cmd(tim, ENABLE);

    return true;
}

#define CameraPower(isOn, isNegativePower)    IOSet(camera->PowerIO, isOn ^ isNegativePower)
#define CameraReset(isReset, isNegativeReset) IOSet(camera->ResetIO, isReset ^ isNegativeReset)

bool CameraStartupSequence(CameraStruct* camera, bool isStart, bool isNegativePower, bool isNegativeReset, uint8_t tPower, uint8_t tReset, uint8_t tReady) {
    // Setup PWDN control io
    if (camera->PowerIO != IONone) {
        IOSetup(camera->PowerIO, IO_DEFAULT_OUTPUT_CONFIG);
    }

    // Setup RST control io
    if (camera->ResetIO != IONone) {
        IOSetup(camera->ResetIO, IO_DEFAULT_OUTPUT_CONFIG);
    }

    // power off and reset at first
    CameraPower(false, isNegativePower);
    CameraReset(true, isNegativeReset);
    SystemDelay(tPower);

    if (isStart) {
        // power on
        CameraPower(true, isNegativePower);
        SystemDelay(tReset);
        // release reset
        CameraReset(false, isNegativeReset);
        SystemDelay(tReady);
    }

    return true;
}

static CameraStruct* csiCamera = NULL;

static bool CSIStart(CameraStruct* camera, bool isStart) {
    CSIStruct* csi = camera->CSIHandle;
    csi->Start(csi, isStart);

    if (camera->ExecuteCommand)
        camera->ExecuteCommand(camera, isStart ? CameraCommandResume : CameraCommandSuspend, NULL);
    return true;
}

bool CameraCSIConstractor(CameraStruct* camera, I2CPortStruct* i2cPort, CSIStruct* csi, uint32_t* frameBuffer) {
    memset(camera, 0, sizeof(CameraStruct));
    camera->Port        = CameraPortCSI;
    camera->I2CPort     = i2cPort;
    camera->FrameBuffer = frameBuffer;
    camera->CSIHandle   = csi;

    camera->PowerIO = IONone;
    camera->ResetIO = IONone;
    camera->ClockIO = IONone;

    camera->Start = CSIStart;

    camera->PixelClock = 48000;

    csiCamera = camera;

#if (FEATURE_CSI_BACKEND == CSI_BACKEND_IPI)
    CSIConstractor(csi, CSI_BASE, IPI_BASE);
#else
    CSIConstractor(csi, CSI_BASE);
    dvpCamera = camera;
#endif

    csi->Lanes     = CSILanes1;
    csi->DataType  = CSIDataTypeRGB565;
    csi->LaneClock = 192000;
    return true;
}

static void CSIClockInit(CameraStruct* camera) {
    //  - Reference Clock
    if (ClockGet(ClockNodeREF) != 48000000) {
        ClockMultiply(ClockNodePLL3, ClockRatio(768.0 / 12.0));
        ClockDivide(ClockNodePLL3R, ClockRatio(768.0 / 48.0));
        ClockSelect(ClockNodeREF, ClockNodePLL3R);
        ClockEnable(ClockNodePLL3G, true);
    }
    ClockSelect(ClockNodeREF, ClockNodePLL3R); // Select PLL3R as Reference 48MHz Clock

    //  - CSI Pixel Clock: default to use ClockNodePLL2Q
    ClockEnable(ClockNodePLL2G, true);
    uint32_t pll2Clock         = ClockGet(ClockNodePLL2);
    uint32_t pixelClockDivisor = pll2Clock / camera->PixelClock / 1000;
    if (SystemCoreClock == 48000000U) {
        pixelClockDivisor = pll2Clock / 24000000;
    }

    ClockEnable(ClockNodePLL2G, false);
    ClockDivide(ClockNodePLL2Q, pixelClockDivisor);
    ClockEnable(ClockNodePLL2G, true);

    ClockSelect(ClockNodeCSI, ClockNodePLL2Q);

    PeripheralEnable(PeripheralPCFG, true);
    PCFG->HSI_CAL |= BIT23;
}

static bool CSISetup(CameraStruct* camera) {
    CSIClockInit(camera);

    PeripheralEnable(PeripheralCSI, true);
    PeripheralReset(PeripheralCSI);

    CSIStruct*          csi    = camera->CSIHandle;
    CameraWindowStruct* window = &camera->Window;
    csi->HorizontalPixelCount  = window->X;
    csi->VerticalActivePeriod  = window->Y;

    uint8_t pixelBitCount        = 0;
    uint8_t pixelCountInSixCycle = 1;

    switch (camera->Color) {
        case CameraColorRaw8RGGB:
        case CameraColorRaw8BGGR:
        case CameraColorRaw8GBRG:
        case CameraColorRaw8GRGG:
            csi->DataType        = CSIDataTypeRAW8;
            csi->BayerPattern    = camera->Color & BITS(1, 0);
            pixelBitCount        = 8;
            pixelCountInSixCycle = 18;
            break;
        case CameraColorRaw10RGGB:
        case CameraColorRaw10BGGR:
        case CameraColorRaw10GBRG:
        case CameraColorRaw10GRGG:
            csi->DataType        = CSIDataTypeRAW10;
            csi->BayerPattern    = camera->Color & BITS(1, 0);
            pixelBitCount        = 10;
            pixelCountInSixCycle = 18;
            break;

        case CameraColorRGB444:
        case CameraColorRGB555:
        case CameraColorRGB565:
        case CameraColorRGB666:
        case CameraColorRGB888:
            csi->DataType        = (CSIDataTypeEnum)((camera->Color & BITS(1, 0)) + (camera->Color & BIT5 ? 3 : 0) + 0x20);
            pixelBitCount        = 16;
            pixelCountInSixCycle = 6;
            break;

        case CameraColorYUV422:
            csi->DataType        = CSIDataTypeYUV422;
            pixelBitCount        = 16;
            pixelCountInSixCycle = 6;
            break;
        case CameraColorYUV420:
            csi->DataType        = CSIDataTypeYUV420;
            pixelBitCount        = 12;
            pixelCountInSixCycle = 12;
            break;
        case CameraColorYUV444: // Not supported
        case CameraColorYUV400: // Gray pixel can not be input color
        default:
            return false;
    }

    if (csi->IsIPIColor16Bits)
        pixelCountInSixCycle = pixelCountInSixCycle / 3;

    csi->HorizontalSyncActivePeriod = 32;

    uint32_t pixelClock     = ClockGet(ClockNodeCSI) / 1000 * pixelCountInSixCycle / 6;
    uint32_t lanePixelClock = csi->LaneClock / ((csi->Lanes + 1) * pixelBitCount);
    if (pixelClock > lanePixelClock) {
        csi->HorizontalSyncDelayPeriod = csi->HorizontalPixelCount * 6 / pixelCountInSixCycle * (pixelClock - lanePixelClock) / lanePixelClock;
    }

#if (FEATURE_CSI_BACKEND == CSI_BACKEND_IPI)
#define COLOR_TYPE_RAW 0
#define COLOR_TYPE_YUV 1
#define COLOR_TYPE_RGB 2

    PeripheralEnable(PeripheralIPI, true);

    // output color config
    uint8_t oColorByteCount = ((camera->MemoryColor & BITS(5, 4)) >> 4) + 1;
    uint8_t oColorType      = (camera->MemoryColor & BITS(3, 2)) >> 2;

    switch (oColorType) {
        case COLOR_TYPE_RAW:
            csi->MemoryColor = BIT6 | oColorByteCount - 1;
            break;
        case COLOR_TYPE_YUV:
            switch (camera->MemoryColor) {
                case CameraColorYUV400: // YUV420 Plane 2 without P1
                    csi->MemoryColor = IPIMemoryColorYUV420;
                    break;
                case CameraColorYUV420: // TODO: YUV420 Plane 2
                    csi->MemoryColor = IPIMemoryColorYUV420S;
                    break;
                case CameraColorYUV422: // YUV422
                    csi->MemoryColor = IPIMemoryColorYUV422L;
                    break;
                case CameraColorYUV444:
                    csi->MemoryColor = IPIMemoryColorYUV444L;
                    break;
                default:
                    return false;
            }
            break;
        case COLOR_TYPE_RGB:
            switch (camera->MemoryColor) {
                case CameraColorRGB888:
                    csi->MemoryColor = IPIMemoryColorRGB888;
                    break;
                case CameraColorRGB565:
                    csi->MemoryColor = IPIMemoryColorRGB565;
                    break;
                default: // Other colors not supported now
                    return false;
            }
            break;
        default:
            return false;
    }

    uint16_t width  = window->X;
    uint16_t height = window->Y;

    // Crop Window
    CameraWindowStruct* cropWindow = &camera->CropWindow;
    if (cropWindow->X > 0 && cropWindow->Y > 0 && (cropWindow->X + cropWindow->XOffset) <= width && (cropWindow->Y + cropWindow->YOffset) <= height) {
        csi->CropXOffset   = cropWindow->XOffset;
        csi->CropYOffset   = cropWindow->YOffset;
        csi->CropX         = cropWindow->X;
        csi->CropY         = cropWindow->Y;
        csi->IsCropEnabled = true;

        width  = cropWindow->X / (1 << csi->HorizontalSampling);
        height = cropWindow->Y / (1 << csi->HorizontalSampling);
    }

    uint32_t frameBuffer = (uint32_t)camera->FrameBuffer;
    // Memory Window
    CameraWindowStruct* memoryWindow = &camera->MemoryWindow;
    if (memoryWindow->X > 0 && memoryWindow->Y > 0 && (memoryWindow->X >= width + memoryWindow->XOffset) &&
        (memoryWindow->Y >= height + memoryWindow->YOffset)) {
        width = memoryWindow->X;
        frameBuffer += (width * memoryWindow->YOffset + memoryWindow->XOffset) * oColorByteCount;
    }
    csi->MemoryBuffer = (void*)frameBuffer;
    csi->MemoryPitch  = width * oColorByteCount;

    NVIC_SetPriority(IPI_IRQn, 1);
    NVIC_ClearPendingIRQ(IPI_IRQn);
    NVIC_EnableIRQ(IPI_IRQn);
#else
    if (!DVPSetup(camera))
        return false;
#endif

    return csi->Init(csi);
}

#if (FEATURE_CSI_BACKEND == CSI_BACKEND_IPI)
void IPI_IRQHandler(void) {
    if (csiCamera == NULL)
        return;

    IPIRegisterStruct* ipi = csiCamera->CSIHandle->IPIReg;
    if (ipi->STS & (BIT4 | BIT3)) {
        // Clear interrupt flags
        ipi->STS = BIT4 | BIT3;
        if (csiCamera->IsSnapshot)
            csiCamera->Start(csiCamera, false);

        if (csiCamera->SyncCallback)
            csiCamera->SyncCallback(csiCamera, false);
    }
}
#endif


bool CameraInit(CameraStruct* camera) {
    if (camera->Port == CameraPortCSI)
        CSISetup(camera);

    return true;
}
