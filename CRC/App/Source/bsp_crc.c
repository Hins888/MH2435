#include "bsp_crc.h"

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

uint32_t arr[]   = {0x20220902, 0x20220903, 0x20220904, 0x20220905, 0x20220906, 0x20220907, 0x20220908, 0x20220909};
uint32_t arr16[] = {0x0927, 0x0928, 0x0929, 0x0930};

static void CRC_GpioConfigForSpeed(void) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin   = GPIO_Pin_0;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &gpio);

    printf("PB0 is used to test CRC caculation speed...\n");
}

static void CRC_Init(CRCType_TypeDef CRC_Type) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

    if (CRC_Type == CRC_Type_16) {
        CRC->CSR = 0x48;
        CRC->INI = 0xFFFF;
        CRC->XOR = 0xFFFF;
    }
    else {
        CRC->CSR = 0xCA;
        CRC->INI = 0xFFFFFFFF;
        CRC->XOR = 0xFFFFFFFF;
    }

    CRC_ResetDR();
}

static void CRC_CalculateBytesNum(uint32_t BytesNum) {
    CRC->CSR &= ~CRC_CSR_BytesNum_4;
    CRC->CSR |= BytesNum;
}

void CRC_ResultXOR(FunctionalState NewState) {
    if (NewState != DISABLE) {
        CRC->CSR |= (0x01UL << 5);
    }
    else {
        CRC->CSR &= ~(0x01UL << 5);
    }
}

static void CRC_ResultReversion(FunctionalState NewState) {
    if (NewState != DISABLE) {
        CRC->CSR |= (0x01UL << 4);
    }
    else {
        CRC->CSR &= ~(0x01UL << 4);
    }
}

static void CRC_InputBytesReversion(FunctionalState NewState) {
    if (NewState != DISABLE) {
        CRC->CSR &= ~(0x01UL << 3);
    }
    else {
        CRC->CSR |= (0x01UL << 3);
    }
}

static void CRC_InputBitsReversion(FunctionalState NewState) {
    if (NewState != DISABLE) {
        CRC->CSR |= (0x01UL << 2);
    }
    else {
        CRC->CSR &= ~(0x01UL << 2);
    }
}

static void CRC_PolyChangeCRC16(FunctionalState NewState) {
    if (NewState != DISABLE) {
        CRC->CSR |= (0x01UL << 0);
    }
    else {
        CRC->CSR &= ~(0x01UL << 0);
    }
}

ErrorStatus CRC_CalculationFunction(void) {
    ErrorStatus result    = SUCCESS;
    uint32_t    crcresult = 0x00;
    CRC_GpioConfigForSpeed();

    CRC_Init(CRC_Type_32);
    crcresult = CRC_CalcBlockCRC(arr, ARR_SIZE(arr));

    if (crcresult != 0xE0FF48B1) {
        result = ERROR;
    }

    return result;
}

ErrorStatus CRC_BytesNumFunction(CRCType_TypeDef CRC_Type) {
    ErrorStatus result = SUCCESS;

    uint32_t result32[] = {0xE0FF48B1, 0x1702F4C5, 0xC270C5A7, 0x6782FB04};
    uint32_t byte32[]   = {CRC_CSR_BytesNum_4, CRC_CSR_BytesNum_3, CRC_CSR_BytesNum_2, CRC_CSR_BytesNum_1};

    uint32_t result16[] = {0x03CB, 0x18A8};
    uint32_t byte16[]   = {CRC_CSR_BytesNum_2, CRC_CSR_BytesNum_1};

    uint32_t size = (CRC_Type == CRC_Type_32) ? ARR_SIZE(result32) : ARR_SIZE(result16);
    uint32_t i    = 0;
    uint32_t Crc_result;

    while (i < size) {
        CRC_Init(CRC_Type);

        if (CRC_Type == CRC_Type_32) {
            CRC_CalculateBytesNum(byte32[i]);
            Crc_result = CRC_CalcBlockCRC(arr, ARR_SIZE(arr));
            if (Crc_result != result32[i]) {
                result = ERROR;
                break;
            }
        }
        else {
            CRC_CalculateBytesNum(byte16[i]);
            if (CRC_CalcBlockCRC(arr16, ARR_SIZE(arr16)) != result16[i]) {
                result = ERROR;
                break;
            }
        }

        i++;
    }

    return result;
}

ErrorStatus CRC_XORFunction(CRCType_TypeDef CRC_Type) {
    ErrorStatus result = SUCCESS;

    CRC_Init(CRC_Type);
    CRC_ResultXOR(ENABLE);

    if (CRC_Type == CRC_Type_32) {
        CRC->XOR = 0x20220906;
        CRC_ResetDR();

        if (CRC_CalcBlockCRC(arr, ARR_SIZE(arr)) != 0xC0DD41B7) {
            result = ERROR;
        }
    }
    else {
        CRC->XOR = 0x0928;
        CRC_ResetDR();

        if (CRC_CalcBlockCRC(arr16, ARR_SIZE(arr16)) != 0x0AE3) {
            result = ERROR;
        }
    }

    return result;
}

ErrorStatus CRC_ResultReversionFunction(CRCType_TypeDef CRC_Type) {
    ErrorStatus result = SUCCESS;

    CRC_Init(CRC_Type);
    CRC_ResultReversion(ENABLE);

    if (CRC_Type == CRC_Type_32) {
        if (CRC_CalcBlockCRC(arr, ARR_SIZE(arr)) != 0x8D12FF07) {
            result = ERROR;
        }
    }
    else {
        if (CRC_CalcBlockCRC(arr16, ARR_SIZE(arr16)) != 0xD3C0) {
            result = ERROR;
        }
    }

    return result;
}

ErrorStatus CRC_InputBytesReversionFunction(CRCType_TypeDef CRC_Type) {
    ErrorStatus result = SUCCESS;

    CRC_Init(CRC_Type);
    CRC_InputBytesReversion(ENABLE);

    if (CRC_Type == CRC_Type_32) {
        if (CRC_CalcBlockCRC(arr, ARR_SIZE(arr)) != 0x1ADD8D24) {
            result = ERROR;
        }
    }
    else {
        if (CRC_CalcBlockCRC(arr16, ARR_SIZE(arr16)) != 0xAC83) {
            result = ERROR;
        }
    }

    return result;
}

ErrorStatus CRC_InputBitsReversionFunction(CRCType_TypeDef CRC_Type) {
    ErrorStatus result = SUCCESS;

    CRC_Init(CRC_Type);
    CRC_InputBitsReversion(ENABLE);

    if (CRC_Type == CRC_Type_32) {
        if (CRC_CalcBlockCRC(arr, ARR_SIZE(arr)) != 0x1D9516FB) {
            result = ERROR;
        }
    }
    else {
        if (CRC_CalcBlockCRC(arr16, ARR_SIZE(arr16)) != 0xC637) {
            result = ERROR;
        }
    }

    return result;
}

ErrorStatus CRC_InitValueFunction(CRCType_TypeDef CRC_Type) {
    ErrorStatus result = SUCCESS;

    CRC_Init(CRC_Type);

    if (CRC_Type == CRC_Type_32) {
        CRC->INI = 0x20220907;
        CRC_ResetDR();

        if (CRC_CalcBlockCRC(arr, ARR_SIZE(arr)) != 0xDADA6D44) {
            result = ERROR;
        }
    }
    else {
        CRC->INI = 0x0928;
        CRC_ResetDR();

        if (CRC_CalcBlockCRC(arr16, ARR_SIZE(arr16)) != 0xBBAD) {
            result = ERROR;
        }
    }

    return result;
}

ErrorStatus CRC_PolyChangeCRC16Function(void) {
    ErrorStatus result = SUCCESS;

    CRC_Init(CRC_Type_16);
    CRC_PolyChangeCRC16(ENABLE);

    if (CRC_CalcBlockCRC(arr16, ARR_SIZE(arr16)) != 0x3A15) {
        result = ERROR;
    }

    return result;
}
