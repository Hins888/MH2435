#include "bsp_rgb_lcd.h"



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

static bool DisplayDPIClockInit(DisplayStruct* display) {
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



static void LCD_AF_GPIOConfig(DisplayStruct* display)
{
   GPIO_InitTypeDef GPIO_InitStruct;
    
    // RGB565/RGB888
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
	
	if(display->BacklightIO)
    {
		IOSetup(display->BacklightIO, IO_DEFAULT_OUTPUT_CONFIG);
		IOH(display->BacklightIO);
	}

}

/**********************
 *   STATIC FUNCTIONS
 **********************/
void LTDC_Config(DisplayStruct* display) {
	
    LTDC_InitTypeDef       LTDC_InitStruct;
    LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

    NVIC_InitTypeDef NVIC_InitStruct;
    uint16_t hsw, vsw, hbp, vbp, hfp, vfp;
    uint32_t pwidth, pheight;

	LCD_AF_GPIOConfig(display);
	
    ClockSelect(ClockNodeDPC, ClockNodePLL1R); // Select PLL1R as DSI pixel clock
    PeripheralEnable(PeripheralLTDC, true);
	
	
	DisplayDPIClockInit(display);

	LTDC_InitStruct.LTDC_HSPolarity = display->IsHsyncValidLow ? LTDC_HSPolarity_AL : LTDC_HSPolarity_AH;
	LTDC_InitStruct.LTDC_VSPolarity = display->IsVsyncValidLow ? LTDC_VSPolarity_AL : LTDC_VSPolarity_AH;
	LTDC_InitStruct.LTDC_DEPolarity = display->IsDataEnableValidLow ? LTDC_DEPolarity_AL : LTDC_DEPolarity_AH;
	LTDC_InitStruct.LTDC_PCPolarity = display->IsPclkFallingSampling ? LTDC_PCPolarity_IIPC : LTDC_PCPolarity_IPC;

    pwidth = display->HorizontalActive;
    pheight = display->VerticalActive;
	
    hbp     = display->HorizontalBackPorch;
    hfp     = display->HorizontalFrontPorch;
    hsw     = display->HorizontalSyncActive;
    vbp     = display->VerticalBackPorch ;
    vfp     = display->VerticalFrontPorch;
    vsw     = display->VerticalSyncActive;
   
    /* Horizontal synchronization width = Hsync - 1 */     
    LTDC_InitStruct.LTDC_HorizontalSync = hsw - 1;
    /* Vertical synchronization height = Vsync - 1 */
    LTDC_InitStruct.LTDC_VerticalSync = vsw - 1;
    /* Accumulated horizontal back porch = Hsync + HBP - 1 */
    LTDC_InitStruct.LTDC_AccumulatedHBP = hsw + hbp - 1; 
    /* Accumulated vertical back porch = Vsync + VBP - 1 */
    LTDC_InitStruct.LTDC_AccumulatedVBP = vsw + vbp - 1;  
    /* Accumulated active width = Hsync + HBP + Active Width - 1 */  
    LTDC_InitStruct.LTDC_AccumulatedActiveW = hsw + hbp + pwidth - 1;
    /* Accumulated active height = Vsync + VBP + Active Heigh - 1 */
    LTDC_InitStruct.LTDC_AccumulatedActiveH = vsw + vbp + pheight - 1;
    /* Total width = Hsync + HBP + Active Width + HFP - 1 */
    LTDC_InitStruct.LTDC_TotalWidth = hsw + hbp + pwidth + hfp - 1; 
    /* Total height = Vsync + VBP + Active Heigh + VFP - 1 */
    LTDC_InitStruct.LTDC_TotalHeigh = vsw + vbp + pheight + vfp - 1;
    
    /* Configure R,G,B component values for LCD background color */                   
    LTDC_InitStruct.LTDC_BackgroundRedValue = 0xFF;            
    LTDC_InitStruct.LTDC_BackgroundGreenValue = 0xFF;          
    LTDC_InitStruct.LTDC_BackgroundBlueValue = 0xFF;

    /* Initialize LTDC */
    LTDC_Init(&LTDC_InitStruct);
    
    /* Layer1 Configuration ------------------------------------------------------*/

    /* Windowing configuration */ 
    /* In this case all the active display area is used to display a picture then :
     Horizontal start = horizontal synchronization + Horizontal back porch = 43 
     Vertical start   = vertical synchronization + vertical back porch     = 12
     Horizontal stop = Horizontal start + window width -1 = 43 + 480 -1 
     Vertical stop   = Vertical start + window height -1  = 12 + 272 -1      */ 
    LTDC_Layer_InitStruct.LTDC_HorizontalStart = hsw + hbp;
    LTDC_Layer_InitStruct.LTDC_HorizontalStop = (pwidth + LTDC_Layer_InitStruct.LTDC_HorizontalStart - 1); 
    LTDC_Layer_InitStruct.LTDC_VerticalStart = vsw + vbp;
    LTDC_Layer_InitStruct.LTDC_VerticalStop = (pheight + LTDC_Layer_InitStruct.LTDC_VerticalStart - 1);

    /* Pixel Format configuration*/           
    LTDC_Layer_InitStruct.LTDC_PixelFormat = display->Color == DisplayColorRGB565 ? LTDC_Pixelformat_RGB565 : LTDC_Pixelformat_RGB888;;
    
    /* Alpha constant (255 totally opaque) */
    LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 

    /* Default Color configuration (configure A,R,G,B component values) */          
    LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0XFF;
    LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0XFF;
    LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0XFF;
    LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
    
    /* Configure blending factors */       
    LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
    LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

    LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)display->Pointer;

    LTDC_Layer_InitStruct.LTDC_CFBLineLength = pwidth * display->Color;
    LTDC_Layer_InitStruct.LTDC_CFBLineNumber = pheight;
    LTDC_Layer_InitStruct.LTDC_CFBPitch = pwidth *display->Color;
    
    LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);
    
    LTDC_LIPConfig(LTDC_Layer_InitStruct.LTDC_VerticalStop + 1);
    
    LTDC->AHBCFG |= 0X07;
	
    LTDC_ITConfig(LTDC_IT_LI, ENABLE);
    NVIC_SetPriority(LTDC_IRQn, 3);
    NVIC_EnableIRQ(LTDC_IRQn);
	
 
    LTDC_LayerCmd(LTDC_Layer1, ENABLE);
    LTDC_ReloadConfig(LTDC_IMReload);
    LTDC_Cmd(ENABLE);
}


void disp_init(uint32_t Displayaddrr)
{
	DisplayStruct* displayPort = NULL;
	displayPort = &DPI_display;
	DPIPanelConstractor(displayPort,Displayaddrr);
    LTDC_Config(displayPort);
	
}

extern void custom_LTDC_IRQHandler(void);

void LTDC_IRQHandler(void) {
	custom_LTDC_IRQHandler();
}


