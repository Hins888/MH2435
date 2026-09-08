#include "HynitronTouch.h"

#define HYNITRON_I2C_ADDR0 (0x34 >> 1)
#define HYNITRON_I2C_ADDR1 (0xB4 >> 1)

#define HYNITRON_I2C_ADDRS HYNITRON_I2C_ADDR0, HYNITRON_I2C_ADDR1

// Regs
#define HYNITRON_REG_CHIP_ID      0xD204
#define HYNITRON_REG_DEBUG_INFO   0xD101
#define HYNITRON_REG_DEBUG_POINTS 0xD108
#define HYNITRON_REG_NORMAL       0xD109

#define HYNITRON_REG_TOUCH1_STATUS   0xD000
#define HYNITRON_REG_TOUCH1_POINT    0xD001
#define HYNITRON_REG_TOUCH1_PRESSURE 0xD004

#define HynitronTouchReset(isReset) IOSet(touch->ResetIO, !isReset)
#define HynitronTouchInt(isHigh)    IOSet(touch->IntIO, isHigh)

static bool HynitronTouchPowerUp(TouchStruct* touch) {
    IOSetup(touch->ResetIO, IO_DEFAULT_OUTPUT_CONFIG);
    IOSetup(touch->IntIO, IO_DEFAULT_OUTPUT_CONFIG);

    // set reset state
    HynitronTouchReset(true);

    // keep reset 10ms
    SystemDelay(2);

    // release reset
    HynitronTouchReset(false);

    SystemDelay(200);

    return true;
}

static const uint8_t HynitronAddrs[] = {HYNITRON_I2C_ADDRS};

static bool HynitronTouchDetect(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    uint32_t id = 0;

    for (int i = 0; i < sizeof(HynitronAddrs); i++) {
        i2cPort->SetAddress(i2cPort, HynitronAddrs[i]);

        if (!i2cPort->Write(i2cPort, I2CTransferA2, HYNITRON_REG_DEBUG_INFO, NULL, 0))
            continue;

        SystemDelay(1);

        if (!i2cPort->Read(i2cPort, I2CTransferA2, HYNITRON_REG_CHIP_ID, (uint8_t*)&id, 4))
            continue;

        if (id) {
            touch->I2CAddress = HynitronAddrs[i];
            break;
        }
    }

    if (!id)
        return false;

    touch->ID = id >> 16;
    if (touch->I2CAddress == HYNITRON_I2C_ADDR0)
        touch->ID = (touch->ID / 1000) << 12 | (touch->ID % 1000 / 100) << 8 | (touch->ID % 100 / 10) << 4 | (touch->ID % 10);

    switch (touch->ID) {
        case 0x0328:
        case 0x9217:
            touch->Points = 5;

            touch->NativeCoordConversion = false;

            touch->IsRasingEvent = false;

            break;

        // unknown id
        default:
            touch->ID = 0;
            return false;
    }

    return true;
}

static bool Init(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    i2cPort->SetAddress(i2cPort, touch->I2CAddress);

    if (!i2cPort->Write(i2cPort, I2CTransferA2, HYNITRON_REG_NORMAL, NULL, 0))
        return false;

    return true;
}

static bool Update(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    uint8_t coordinate[4] = {0};
    if (!i2cPort->Read(i2cPort, I2CTransferA2, HYNITRON_REG_TOUCH1_STATUS, coordinate, 4))
        return false;

    bool isTouched = false;

    uint16_t x = 0, y = 0;
    // touched
    if ((coordinate[0] & 0x0F) == 0x06) {
        isTouched = true;

        x = (coordinate[3] & 0xF0) >> 4 | (coordinate[1] << 4);
        y = (coordinate[3] & 0x0F) >> 0 | (coordinate[2] << 4);
    }
    touch->SetCoordinate(touch, isTouched, x, y);

    return true;
}

bool HynitronTouchConstractor(TouchStruct* touch) {
    if (!HynitronTouchPowerUp(touch))
        return false;

    if (!HynitronTouchDetect(touch))
        return false;

    touch->Init   = Init;
    touch->Update = Update;

    return true;
}
