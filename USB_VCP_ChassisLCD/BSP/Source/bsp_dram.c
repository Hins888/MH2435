#include "bsp_sdram.h"


void SDRAM_PinConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);
	
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_SDRAM);     //DQM[0]
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_SDRAM);	 //DQM[1]	
	
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOE, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);

	
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource8, GPIO_AF_SDRAM);	 //CLK	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOG, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);


    GPIO_PinAFConfig(GPIOH, GPIO_PinSource2, GPIO_AF_SDRAM);	 //CKE	
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource3, GPIO_AF_SDRAM);	 //CSN[0]	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOH, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOH, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);


    GPIO_PinAFConfig(GPIOG, GPIO_PinSource4, GPIO_AF_SDRAM);	 //BA0	
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource5, GPIO_AF_SDRAM);	 //BA1	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; 
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOG, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);


    GPIO_PinAFConfig(GPIOF, GPIO_PinSource11, GPIO_AF_SDRAM);	 //RAS_N	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOF, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);


    GPIO_PinAFConfig(GPIOG, GPIO_PinSource15, GPIO_AF_SDRAM);	 //CAS_N	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOG, &GPIO_InitStructure); 

    GPIO_DriveStrengthConfig(GPIOG, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);


    GPIO_PinAFConfig(GPIOH, GPIO_PinSource5, GPIO_AF_SDRAM);	 //WE_N	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOH, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOH, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);


    GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_SDRAM);	 //A0	
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_SDRAM);	 //A1
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF_SDRAM);	 //A2	
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource13, GPIO_AF_SDRAM);	 //A3	
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource14, GPIO_AF_SDRAM);	 //A4	
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource15, GPIO_AF_SDRAM);	 //A5
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_SDRAM);	 //A6	
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF_SDRAM);	 //A7
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF_SDRAM);	 //A8
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_SDRAM);	 //A9	
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_SDRAM);	 //A10	
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_SDRAM);	 //A11	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOF, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOI, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOI, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOG, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);


    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_SDRAM);	 //D0	
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_SDRAM);	 //D1
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_SDRAM);	 //D2	
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_SDRAM);	 //D3	
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_SDRAM);	 //D4	
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_SDRAM);	 //D5
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_SDRAM);	 //D6	
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_SDRAM);	 //D7
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource8, GPIO_AF_SDRAM);	 //D8
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SDRAM);	 //D9	
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource3, GPIO_AF_SDRAM);	 //D10	
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource11, GPIO_AF_SDRAM);	 //D11	
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource12, GPIO_AF_SDRAM);	 //D12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_SDRAM);	 //D13	
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_SDRAM);	 //D14	
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_SDRAM);	 //D15

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOD, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOE, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_Init(GPIOI, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOI, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High);
 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOC, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High); 
 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_DriveStrengthConfig(GPIOG, GPIO_InitStructure.GPIO_Pin, GPIO_DriveStrength_High); 	 
}

void SDRAM_Config(void)
{
    SDRAM_InitTypeDef   SDRAM_InitStructure;
    SDRAM_TimingTypeDef Timing;

	SDRAM_CLKDivConfig(SDRAM_CLKDIV_Div2);
    SDRAM_SampleDelayConfig(0x02);	
//    SDRAM_SampleDelayConfig(0x01);    //150MHz
	
    SDRAM_DeInit();

    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_SDRAM, ENABLE);
	
	SDRAM_PinConfig();

    SDRAM_InitStructure.MemoryDataWidth     = SDRAM_MemoryDataWidth_16bit;
    SDRAM_InitStructure.BankAddrBitsNumber  = SDRAM_BankAddrBitsNumber_2bit;
    SDRAM_InitStructure.ColumnBitsNumber    = SDRAM_ColumnBitsNumber_8bit;
    SDRAM_InitStructure.RowBitsNumber       = SDRAM_RowBitsNumber_12bit;
    SDRAM_InitStructure.PrechargeAlgorihm   = SDRAM_PrechargeAlgorihm_Delayed;
    SDRAM_InitStructure.FullRefreshBeforeSR = SDRAM_RefreshAllRowBeforeEnterSR;
    SDRAM_InitStructure.FullRefreshAfterSR  = SDRAM_RefreshAllRowAfterEnterSR;
    SDRAM_InitStructure.ClkEdgeSel          = SDRAM_ClkEdgeSelectRising;
    SDRAM_InitStructure.ReadPipe            = 2;
    SDRAM_Init(&SDRAM_InitStructure);

    // SDRAMCLK = 120MHz, 8.33ns
    Timing.CASLatency        = SDRAM_CASLatency_3CLK;
    Timing.RasMinDelay       = 4;     // t_ras 40ns
    Timing.RCDDelay          = 1;     // t_rcd 15ns
    Timing.RPDelay           = 1;     // t_rp 15ns
    Timing.WriteRecoveryTime = 1;     // t_wr 10ns
    Timing.RCARTime          = 6;     // t_rc 55ns
    Timing.XSRDelay          = 6;     // t_xsr 57ns
    Timing.RCTime            = 6;     // t_rc 55ns
    Timing.InitDelay         = 24009; // 200us
    Timing.InitRefNumber     = 7;

    Timing.RefCycle = 1822;

//    // SDRAMCLK = 150MHz, 6.67ns
//    Timing.CASLatency        = SDRAM_CASLatency_3CLK;
//    Timing.RasMinDelay       = 7;     // t_ras 40ns
//    Timing.RCDDelay          = 2;     // t_rcd 15ns
//    Timing.RPDelay           = 2;     // t_rp 15ns
//    Timing.WriteRecoveryTime = 1;     // t_wr 10ns
//    Timing.RCARTime          = 8;     // t_rc 55ns
//    Timing.XSRDelay          = 8;     // t_xsr 57ns
//    Timing.RCTime            = 8;     // t_rc 55ns
//    Timing.InitDelay         = 29985; // 200us
//    Timing.InitRefNumber     = 7;

//    Timing.RefCycle = 2292;

    SDRAM_TimingConfig(&Timing);

    SDRAM_SendCommand(SDRAM_CommandEnterInitialize);
    while (SDRAM_GetCommandStatus(SDRAM_CommandStatus_Initialize) == SET);
}
