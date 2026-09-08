#include "SDRAMBSP.h"

static void SDRAMIOSetup(IOEnum sdramIOList[], int ioCount) {
    IOConfigStruct ioConfig = MakeIOConfig(IOModeAlternate, GPIO_AF_SDRAM, IOPullNone, IOSpeedHigh, IODriveHigh);
    for (int i = 0; i < ioCount; i++) {
        IOSetup(sdramIOList[i], ioConfig);
    }
}

void SDRAMSetup(void) {
    // IO Config
    SDRAMIOSetup(CONFIG_SDRAM_IO_LIST, sizeof(CONFIG_SDRAM_IO_LIST) / sizeof(IOEnum));

    // Timming Config
    __IO SDRAMTimmingUnion sdramTimming = CONFIG_SDRAM_DEFAULT_TIMMING;
    // - Clock Divide
    ClockDivide(ClockNodeSDRAM, sdramTimming.Divisor);
    // - Sample Delay
    SDRAM_SampleDelayConfig(sdramTimming.Delay);

    PeripheralEnable(PeripheralSDRAM, true);
    PeripheralReset(PeripheralSDRAM);

    SDRAM_InitTypeDef sdramConfig = {
        .MemoryDataWidth     = SDRAM_MemoryDataWidth_16bit,
        .BankAddrBitsNumber  = SDRAM_BankAddrBitsNumber_2bit,
        .ColumnBitsNumber    = SDRAM_ColumnBitsNumber_8bit,
        .RowBitsNumber       = SDRAM_RowBitsNumber_12bit,
        .PrechargeAlgorihm   = SDRAM_PrechargeAlgorihm_Delayed,
        .FullRefreshBeforeSR = SDRAM_RefreshAllRowBeforeEnterSR,
        .FullRefreshAfterSR  = SDRAM_RefreshAllRowAfterEnterSR,
        .ClkEdgeSel          = SDRAM_ClkEdgeSelectRising,
        .ReadPipe            = 2,
    };
    SDRAM_Init(&sdramConfig);

    // - Controller Timming
    SDRAM->STMG0R = sdramTimming.TimmingReg[0];
    SDRAM->STMG1R = sdramTimming.TimmingReg[1];
    SDRAM->SREFR  = sdramTimming.TimmingReg[2];

    SDRAM_SendCommand(SDRAM_CommandEnterInitialize);
    while (SDRAM_GetCommandStatus(SDRAM_CommandStatus_Initialize) == SET);
}
