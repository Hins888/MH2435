#include "bsp_lcd/bsp_rgb_lcd.h"
 

#define BL_GPIO_PORT 	GPIOC
#define BL_GPIO_CLK 	PeripheralGPIOC
#define BL_GPIO_PIN 	GPIO_Pin_8
#define BL_GPIO_SOURCE  GPIO_PinSource8
#define BL_GPIO_AF		GPIO_AF_TIM3
#define BL_TIMX			TIM3
#define BL_TIMX_CLK		PeripheralTIM3

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

static void BL_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIO clock enable */
	PeripheralEnable(BL_GPIO_CLK, true);

	/* Connect TIMx pins to AF */  
	GPIO_PinAFConfig(BL_GPIO_PORT, BL_GPIO_SOURCE, BL_GPIO_AF); 
	
	GPIO_InitStructure.GPIO_Pin = BL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(BL_GPIO_PORT, &GPIO_InitStructure); 
}


static void BL_Init_Frequency(u16 Psc, u16 Arr) 
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	BL_GPIO_Configuration();
	
	PeripheralEnable(BL_TIMX_CLK, true);

	if (Arr <= 0 || Psc <= 0)
		return;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = Arr-1;
	TIM_TimeBaseStructure.TIM_Prescaler = Psc-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(BL_TIMX, &TIM_TimeBaseStructure);

	/* PWM3 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC3Init(BL_TIMX, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(BL_TIMX, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(BL_TIMX, ENABLE);
	
//	TIM_CtrlPWMOutputs(BL_TIMX, ENABLE);    //TIM1 | TIM8

	/* TIM3 enable counter */
	TIM_Cmd(BL_TIMX, ENABLE);
}

/*
	背光占空比设置
    unit： 高电平占控比，支持0~100
*/
void BL_Unit_Config(uint8_t unit)
{
	uint32_t arr = 0;
	arr = (BL_TIMX->ARR + 1) / 100 * (unit);
	TIM_SetCompare3(BL_TIMX, arr);
}


static void LCD_AF_GPIOConfig(DisplayStruct* display)
{
   GPIO_InitTypeDef GPIO_InitStruct;
    
    // RGB565/RGB888
    PeripheralEnable(PeripheralGPIOA, true);
    PeripheralEnable(PeripheralGPIOB, true);
    PeripheralEnable(PeripheralGPIOE, true);
    PeripheralEnable(PeripheralGPIOF, true);	
    PeripheralEnable(PeripheralGPIOG, true);
    PeripheralEnable(PeripheralGPIOH, true);
    PeripheralEnable(PeripheralGPIOI, true);
    PeripheralEnable(PeripheralPCFG, true);

    GPIO_PinAFConfig(GPIOI, GPIO_PinSource0, GPIO_AF_LTDC); // G5	
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource1, GPIO_AF_LTDC); // G6	
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource2, GPIO_AF_LTDC); // G7
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource3, GPIO_AF_LTDC); // B3	
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource4, GPIO_AF_LTDC); // B4
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource5, GPIO_AF_LTDC); // B5
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource6, GPIO_AF_LTDC); // B6	
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource7, GPIO_AF_LTDC); // B7	
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource9, GPIO_AF_LTDC); // VS
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource10, GPIO_AF_LTDC);// HS
	
    /* GPIOI configuration */	
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |
								 GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOI, &GPIO_InitStruct); 

    /* GPIOH configuration */
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource9, GPIO_AF_LTDC);  // R3
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource10, GPIO_AF_LTDC); // R4	
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource11, GPIO_AF_LTDC); // R5
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource12, GPIO_AF_LTDC); // R6
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource13, GPIO_AF_LTDC); // G2
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource14, GPIO_AF_LTDC); // G3
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource15, GPIO_AF_LTDC); // G4
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                               GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
							   
	if(DisplayColorRGB888 == display->Color)
	{
		GPIO_PinAFConfig(GPIOH, GPIO_PinSource6, GPIO_AF_LTDC);  // R1
		GPIO_PinAFConfig(GPIOH, GPIO_PinSource8, GPIO_AF_LTDC);  // R2
		
		GPIO_InitStruct.GPIO_Pin |= GPIO_Pin_6 | GPIO_Pin_8;
	}
    GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* GPIOG configuration */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_LTDC);  // R7
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource7, GPIO_AF_LTDC);  // CLK	 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

	if(DisplayColorRGB888 == display->Color)
	{
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_LTDC); // B2
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_LTDC); // B1
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_LTDC); // R0	
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_LTDC); // B0
		GPIO_InitStruct.GPIO_Pin |= GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	}
	
    GPIO_Init(GPIOG, &GPIO_InitStruct);
	
    /* GPIOF configuration */
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_LTDC);  // DE

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	if(DisplayColorRGB888 == display->Color)
	{
		/* GPIOB configuration */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_LTDC);  // G1
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_LTDC);  // G0
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_Init(GPIOB, &GPIO_InitStruct);	
	}
	
	/* BL */
	BL_Init_Frequency(1500,1000); //PCLK * 2/Psc/Arr = 0.1KHz
    BL_Unit_Config(0);
	
//    IOSetup(PC8, IO_DEFAULT_OUTPUT_CONFIG);
//    IOH(PC8);		
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
    
    NVIC_InitStruct.NVIC_IRQChannel = LTDC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    
    NVIC_Init(&NVIC_InitStruct);
    
    LTDC_LIPConfig(LTDC_Layer_InitStruct.LTDC_VerticalStop + 1);
    LTDC_ITConfig(LTDC_IT_LI, ENABLE);
    LTDC_ITConfig(LTDC_IT_FU, ENABLE);
    LTDC_ITConfig(LTDC_IT_TERR, ENABLE);
    
    LTDC->AHBCFG |= 0X07;
    
    LTDC_LayerCmd(LTDC_Layer1, ENABLE);
    LTDC_ReloadConfig(LTDC_IMReload);
    LTDC_Cmd(ENABLE);
}
