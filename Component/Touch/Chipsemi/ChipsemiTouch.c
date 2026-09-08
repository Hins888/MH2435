#include "ChipsemiTouch.h"

#define CHIPSEMI_I2C_ADDR 0x2E

// Regs
#define CHIPSMEI_REG_DIRECT_MODE 0x42BD
#define CHIPSMEI_REG_NORMAL_MODE 0x0003
#define CHIPSMEI_REG_GET_MODE    0x0001
#define CHIPSEMI_REG_CHIP_ID     0x00D0
#define CHIPSEMI_REG_VENDOR_ID   0x00A5

#define ChipsemiTouchReset(isReset) IOSet(touch->ResetIO, !isReset)
#define ChipsemiTouchInt(isHigh)    IOSet(touch->IntIO, isHigh)

static bool ChipsemiTouchPowerUp(TouchStruct* touch) {
    IOSetup(touch->ResetIO, IO_DEFAULT_OUTPUT_CONFIG);
    IOSetup(touch->IntIO, IO_DEFAULT_OUTPUT_CONFIG);

    // set reset state
    ChipsemiTouchReset(true);

    // keep reset 2ms
    SystemDelay(2);

    // release reset
    ChipsemiTouchReset(false);

    // wait for power on
    SystemDelay(200);

    return true;
}

static bool ChipsemiTouchGetMode(I2CPortStruct* i2cPort, bool* isDirectMode) {
    uint8_t data[3] = {0};

    if (!i2cPort->Read(i2cPort, I2CTransferA2, CHIPSMEI_REG_GET_MODE, data, 3))
        return false;

    *isDirectMode = (data[0] == (i2cPort->Address << 1)) && (data[2] == 0x01);
    return true;
}

static const uint8_t ChipsemiAddrs[] = {CHIPSEMI_I2C_ADDR};

static const uint32_t ChipsemiIDCodes[] = {
    0x000000, 0x6206A0, 0x630600, 0x620600, 0x632400, 0x644200, 0x644000, 0x644800, 0x643200, 0x642400, 0x6406BF, 0x641300, 0x641700, 0x654000,
};

static bool ChipsemiTouchDetect(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    touch->ID = 0;

    bool isDirectMode = false;
    for (int i = 0; i < sizeof(ChipsemiAddrs); i++) {
        i2cPort->SetAddress(i2cPort, ChipsemiAddrs[i]);
        for (int tryCount = 0; tryCount < 3; tryCount++) {
            // Enter direct mode
            if (!i2cPort->Write(i2cPort, I2CTransferA2, CHIPSMEI_REG_DIRECT_MODE, (uint8_t[]) {0x28, 0x35, 0xc1, 0x00, 0x35, 0xae}, 6))
                continue;

            SystemDelay(30);

            // Check if in direct mode
            if (!ChipsemiTouchGetMode(i2cPort, &isDirectMode) || !isDirectMode)
                continue;

            uint32_t id[4] = {0};
            if (!i2cPort->Read(i2cPort, I2CTransferA2, 0x8000, (uint8_t*)id, 12))
                continue;

            uint32_t chipID = 0;
            if ((id[2] == 0x544C4E4B || id[2] == 0x544C4EBD) && (id[0] == 0x35368008 || id[0] == 0x35358008))
                chipID = (id[0] & 0x00FF0000) << 8;
            else
                break;

            if (!i2cPort->Read(i2cPort, I2CTransferA2, (chipID == 0x36000000 ? 0x9E00 : 0xD6E0) + 107, (uint8_t*)(id + 3), 1))
                continue;

            if (id[3] > (sizeof(ChipsemiIDCodes) / 4))
                continue;

            // Enter normal mode
            if (!i2cPort->Write(i2cPort, I2CTransferA2, CHIPSMEI_REG_NORMAL_MODE, (uint8_t[]) {0x05}, 1))
                continue;

            SystemDelay(10);

            if (!ChipsemiTouchGetMode(i2cPort, &isDirectMode) || isDirectMode)
                continue;

            touch->ID = chipID | ChipsemiIDCodes[id[3]];
            break;
        }

        if (touch->ID) {
            touch->I2CAddress = ChipsemiAddrs[i];
            break;
        }
    }

    if (touch->ID == 0)
        return false;

    switch (touch->ID) {
        case 0x36654000: // CHSC6540
            touch->Points = 2;

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

    return true;
}

static bool Update(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    uint8_t touchData[8] = {0};
    if (!i2cPort->Read(i2cPort, I2CTransferA1, 0x00, touchData, 7))
        return false;

    bool isTouched = false;

    uint16_t x = 0, y = 0;
    // touched
    if ((touchData[2] & 0x07) > 0) {
        isTouched = true;

        x = ((touchData[3] & 0x0F) << 8) | touchData[4];
        y = ((touchData[5] & 0x0F) << 8) | touchData[6];
    }
    touch->SetCoordinate(touch, isTouched, x, y);

    return true;
}

bool ChipsemiTouchConstractor(TouchStruct* touch) {
    if (!ChipsemiTouchPowerUp(touch))
        return false;

    if (!ChipsemiTouchDetect(touch))
        return false;

    touch->Init   = Init;
    touch->Update = Update;

    return true;
}
