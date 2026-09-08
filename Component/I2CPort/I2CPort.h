#ifndef __I2C_PORT_H__
#define __I2C_PORT_H__

#include "mh2435.h"

typedef struct {
    uint16_t Address;
    uint16_t Value;
    uint16_t Mask;
    uint16_t Delay;
} I2CShortConfigStruct;

typedef struct {
    uint8_t  Address;
    uint8_t  Value;
    uint16_t Mask;
    uint16_t Delay;
} I2CByteConfigStruct;

typedef enum {
    I2CTransferA1   = 0x10,
    I2CTransferA1D1 = 0x11,
    I2CTransferA1D2 = 0x12,
    I2CTransferA1D3 = 0x13,
    I2CTransferA1D4 = 0x14,

    I2CTransferA2   = 0x20,
    I2CTransferA2D1 = 0x21,
    I2CTransferA2D2 = 0x22,
    I2CTransferA2D3 = 0x23,
    I2CTransferA2D4 = 0x24,

    I2CTransferA3   = 0x30,
    I2CTransferA3D1 = 0x31,
    I2CTransferA3D2 = 0x32,
    I2CTransferA3D3 = 0x33,
    I2CTransferA3D4 = 0x34,

    I2CTransferA4   = 0x40,
    I2CTransferA4D1 = 0x41,
    I2CTransferA4D2 = 0x42,
    I2CTransferA4D3 = 0x42,
    I2CTransferA4D4 = 0x44,

    // No Address needed
    I2CTransferNA = 0x00,

    // Address Little Endian + Data Big Endian
    I2CTransferLA1   = I2CTransferA1,
    I2CTransferLA1D1 = I2CTransferA1D1,
    I2CTransferLA1D2 = I2CTransferA1D2,
    I2CTransferLA1D3 = I2CTransferA1D3,
    I2CTransferLA1D4 = I2CTransferA1D4,

    I2CTransferLA2   = I2CTransferA2 | 0x80,
    I2CTransferLA2D1 = I2CTransferA2D1 | 0x80,
    I2CTransferLA2D2 = I2CTransferA2D2 | 0x80,
    I2CTransferLA2D3 = I2CTransferA2D3 | 0x80,
    I2CTransferLA2D4 = I2CTransferA2D4 | 0x80,

    I2CTransferLA3   = I2CTransferA3 | 0x80,
    I2CTransferLA3D1 = I2CTransferA3D1 | 0x80,
    I2CTransferLA3D2 = I2CTransferA3D2 | 0x80,
    I2CTransferLA3D3 = I2CTransferA3D3 | 0x80,
    I2CTransferLA3D4 = I2CTransferA3D4 | 0x80,

    I2CTransferLA4   = I2CTransferA4 | 0x80,
    I2CTransferLA4D1 = I2CTransferA4D1 | 0x80,
    I2CTransferLA4D2 = I2CTransferA4D2 | 0x80,
    I2CTransferLA4D3 = I2CTransferA4D3 | 0x80,
    I2CTransferLA4D4 = I2CTransferA4D4 | 0x80,

    // Address Little Endian + Data Little Endian
    I2CTransferLA1LD1 = I2CTransferLA1D1,
    I2CTransferLA1LD2 = I2CTransferLA1D2 | 0x08,
    I2CTransferLA1LD3 = I2CTransferLA1D3 | 0x08,
    I2CTransferLA1LD4 = I2CTransferLA1D4 | 0x08,

    I2CTransferLA2LD1 = I2CTransferLA2D1,
    I2CTransferLA2LD2 = I2CTransferLA2D2 | 0x08,
    I2CTransferLA2LD3 = I2CTransferLA2D3 | 0x08,
    I2CTransferLA2LD4 = I2CTransferLA2D4 | 0x08,

    I2CTransferLA3LD1 = I2CTransferLA3D1,
    I2CTransferLA3LD2 = I2CTransferLA3D2 | 0x08,
    I2CTransferLA3LD3 = I2CTransferLA3D3 | 0x08,
    I2CTransferLA3LD4 = I2CTransferLA3D4 | 0x08,

    I2CTransferLA4LD1 = I2CTransferLA4D1,
    I2CTransferLA4LD2 = I2CTransferLA4D2 | 0x08,
    I2CTransferLA4LD3 = I2CTransferLA4D3 | 0x08,
    I2CTransferLA4LD4 = I2CTransferLA4D4 | 0x08,
} I2CTransferTypeEnum;

typedef struct I2CPortBase I2CPortStruct;

struct I2CPortBase {
    bool (*Init)(I2CPortStruct* self);
    void (*SetAddress)(I2CPortStruct* self, uint8_t address);

    bool (*Read)(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, uint8_t* data, uint32_t length);
    bool (*Write)(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, uint8_t* data, uint32_t length);

    bool (*ReadValue)(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, void* value);
    bool (*WriteValue)(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, uint32_t value);
    bool (*ModifyValue)(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, uint32_t mask, uint32_t value);

    bool (*ByteLoad)(I2CPortStruct* self, const I2CByteConfigStruct* configs);
    bool (*ShortLoad)(I2CPortStruct* self, const I2CShortConfigStruct* configs);

    PeripheralEnum Peripheral : 8;

    IOEnum SCL : 8;
    IOEnum SDA : 8;

    uint8_t  Address;
    uint16_t Speed; // In KHz
    union {
        uint32_t     I2CBase;
        I2C_TypeDef* I2CV2;
    };
};

extern bool I2CPortConstractor(I2CPortStruct* self, PeripheralEnum peripheralI2C, IOEnum scl, IOEnum sda);

#endif // __I2C_PORT_H__
