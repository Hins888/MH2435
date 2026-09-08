#include "AXSTouch.h"

#define AXS_I2C_ADDRS 0x3B

#define AXS_TOUCH_ONE_POINT_LEN 6
#define AXS_TOUCH_BUF_HEAD_LEN  2
#define AXS_MAX_TOUCH_NUMBER    1

#define AXS_TOUCH_REPORT_LEN (AXS_TOUCH_BUF_HEAD_LEN + AXS_MAX_TOUCH_NUMBER * AXS_TOUCH_ONE_POINT_LEN)

#define AXSTouchReset(isReset) IOSet(touch->ResetIO, !isReset)
#define AXSTouchInt(isHigh)    IOSet(touch->IntIO, isHigh)

static bool AXSTouchPowerUp(TouchStruct* touch) {
    IOSetup(touch->ResetIO, IO_DEFAULT_OUTPUT_CONFIG);

    // set reset state
    AXSTouchReset(true);

    // keep reset 10ms
    SystemDelay(20);

    // release reset
    AXSTouchReset(false);

    SystemDelay(20);

    return true;
}

static const uint8_t AXSAddrs[] = {AXS_I2C_ADDRS};

static bool AXSTouchDetect(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    uint32_t id = 0;

    for (int i = 0; i < sizeof(AXSAddrs); i++) {
        i2cPort->SetAddress(i2cPort, AXSAddrs[i]);

        if (!touch->Update(touch))
            continue;

        id = 0x015260;
        break;
    }

    if (!id)
        return false;

    touch->Points = 1;

    touch->NativeCoordConversion = false;

    touch->IsRasingEvent = false;

    touch->I2CAddress = i2cPort->Address;

    return true;
}

static bool Init(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    i2cPort->SetAddress(i2cPort, touch->I2CAddress);

    return true;
}

static bool Update(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    uint8_t writeCommand[] = {0xb5, 0xab, 0xa5, 0x5a, 0x00, 0x00, AXS_TOUCH_REPORT_LEN, 0x01, 0x00, 0x00, 0x00};
    if (!i2cPort->Write(i2cPort, I2CTransferNA, NULL, writeCommand, sizeof(writeCommand)))
        return false;

    uint8_t coordinate[AXS_TOUCH_REPORT_LEN] = {0};

    if (!i2cPort->Read(i2cPort, I2CTransferNA, NULL, coordinate, sizeof(coordinate)))
        return false;

    bool isTouched = false;

    uint16_t x = 0, y = 0;
    // touched
    if ((coordinate[1] == 1) && (coordinate[2] >> 6 != 1)) {
        isTouched = true;

        x = (coordinate[2] & 0x0F) << 8 | coordinate[3];
        y = (coordinate[4] & 0x0F) << 8 | coordinate[5];
    }
    touch->SetCoordinate(touch, isTouched, x, y);

    return true;
}

bool AXSTouchConstractor(TouchStruct* touch) {
    if (!AXSTouchPowerUp(touch))
        return false;

    touch->Update = Update;
    if (!AXSTouchDetect(touch)) {
        touch->Update = NULL;
        return false;
    }

    touch->Init   = Init;
    touch->Update = Update;

    return true;
}
