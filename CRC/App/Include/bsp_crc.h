#ifndef __BSP_CRC_H
#define __BSP_CRC_H

#include "mh2435.h"
#include "RetargetIO.h"

typedef enum {
    CRC_Type_16 = 0x00,
    CRC_Type_32 = 0x01
} CRCType_TypeDef;

#define CRC_CSR_BytesNum_1 (0x00UL << 6)
#define CRC_CSR_BytesNum_2 (0x01UL << 6)
#define CRC_CSR_BytesNum_3 (0x02UL << 6)
#define CRC_CSR_BytesNum_4 (0x03UL << 6)

ErrorStatus CRC_CalculationFunction(void);
ErrorStatus CRC_BytesNumFunction(CRCType_TypeDef CRC_Type);
ErrorStatus CRC_XORFunction(CRCType_TypeDef CRC_Type);
ErrorStatus CRC_ResultReversionFunction(CRCType_TypeDef CRC_Type);
ErrorStatus CRC_InputBytesReversionFunction(CRCType_TypeDef CRC_Type);
ErrorStatus CRC_InputBitsReversionFunction(CRCType_TypeDef CRC_Type);
ErrorStatus CRC_InitValueFunction(CRCType_TypeDef CRC_Type);
ErrorStatus CRC_PolyChangeCRC16Function(void);

#endif /* __CRC_H */
