#include "SitronixTouch.h"

#define SITRONIX_I2C_ADDR 0x55, 0x38, 0x70, 0x60

// Regs
#define SITRONIX_REG_FIRMWARE_VERSION    0x00
#define SITRONIX_REG_STATUS_REG          0x01
#define SITRONIX_REG_DEVICE_CONTROL_REG  0x02
#define SITRONIX_REG_TIMEOUT_TO_IDLE_REG 0x03
#define SITRONIX_REG_XY_RESOLUTION_HIGH  0x04
#define SITRONIX_REG_X_RESOLUTION_LOW    0x05
#define SITRONIX_REG_Y_RESOLUTION_LOW    0x06
#define SITRONIX_REG_DEVICE_CONTROL_REG2 0x09
#define SITRONIX_REG_FIRMWARE_REVISION_3 0x0C
#define SITRONIX_REG_FIRMWARE_REVISION_2 0x0D
#define SITRONIX_REG_FIRMWARE_REVISION_1 0x0E
#define SITRONIX_REG_FIRMWARE_REVISION_0 0x0F
#define SITRONIX_REG_FINGERS             0x10
#define SITRONIX_REG_KEYS_REG            0x11
#define SITRONIX_REG_XY0_COORD_H         0x12
#define SITRONIX_REG_X0_COORD_L          0x13
#define SITRONIX_REG_Y0_COORD_L          0x14
#define SITRONIX_REG_I2C_PROTOCOL        0x3E
#define SITRONIX_REG_MAX_NUM_TOUCHES     0x3F
#define SITRONIX_REG_DATA_0_HIGH         0x40
#define SITRONIX_REG_DATA_0_LOW          0x41
#define SITRONIX_REG_MISC_CONTROL        0xF1
#define SITRONIX_REG_SMART_WAKE_UP_REG   0xF2
#define SITRONIX_REG_CHIP_ID             0xF4
#define SITRONIX_REG_PAGE_REG            0xFF

#define SitronixTouchReset(isReset) IOSet(touch->ResetIO, !isReset)

static bool SitronixTouchPowerUp(TouchStruct* touch) {
    IOSetup(touch->ResetIO, IO_DEFAULT_OUTPUT_CONFIG);
    IOSetup(touch->IntIO, IO_DEFAULT_OUTPUT_CONFIG);

    // set reset state
    SitronixTouchReset(true);

    // keep reset 10ms
    SystemDelay(10);

    // release reset
    SitronixTouchReset(false);

    // wait for stable
    //  - ST1912/ST1727: 65ms
    //  - ST1x32/ST1x28/ST1x30/ST1x34/ST1x36/ST1x33i/ST1x33/ST1x24/ST1615: 50ms
    SystemDelay(65);

    return true;
}

static const uint8_t SitronixAddrs[] = {SITRONIX_I2C_ADDR};

static bool SitronixTouchDetect(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    uint8_t id[3] = {0};

    uint32_t touchID1 = 0, touchID2 = 0;

    for (int i = 0; i < sizeof(SitronixAddrs); i++) {
        i2cPort->SetAddress(i2cPort, SitronixAddrs[i]);

        if (!i2cPort->Read(i2cPort, I2CTransferA2, SITRONIX_REG_CHIP_ID, id, 3)) {
            continue;
        }
        touchID2 = id[0] << 16 | (id[1] / 10) << 12 | (id[1] % 10) << 8 | (id[2] / 10) << 4 | (id[2] % 10);

        if (!i2cPort->Read(i2cPort, I2CTransferA1, SITRONIX_REG_CHIP_ID, id, 3)) {
            continue;
        }
        touchID1 = id[0] << 16 | (id[1] / 10) << 12 | (id[1] % 10) << 8 | (id[2] / 10) << 4 | (id[2] % 10);

        break;
    }

    if (touchID1 == 0xF1120) {
        touch->Points                = 5;
        touch->IsRasingEvent         = true;
        touch->NativeCoordConversion = false;

        touch->ID = touchID1;
    }
    else if (touchID2 == 0x860000) {
        if (!i2cPort->Read(i2cPort, I2CTransferA2, 0x0009, &touch->Points, 1))
            return false;

        touch->IsRasingEvent         = false;
        touch->NativeCoordConversion = false;

        touch->ID = touchID2;
    }
    else {
        return false;
    }

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

    uint8_t coordinate[8] = {0};

    bool isTouched = false;

    uint16_t x = 0, y = 0;

    if (touch->ID == 0x860000) {
        if (!i2cPort->Read(i2cPort, I2CTransferA2, 0x0010, coordinate, 1))
            return false;

        if (coordinate[0] & BIT3) {
            if (!i2cPort->Read(i2cPort, I2CTransferA2, 0x0014, (coordinate + 1), 7))
                return false;

            if (coordinate[1] & BIT7) {
                x = (coordinate[1 + 0] & 0x3F) << 8 | coordinate[1 + 1];
                y = (coordinate[1 + 2] & 0x3F) << 8 | coordinate[1 + 3];

                isTouched = true;
            }
        }

        if (!i2cPort->Read(i2cPort, I2CTransferA2, 0x0014 + (touch->Points - 1) * 7 + 6, coordinate, 1))
            return false;
    }
    else {
        if (!i2cPort->Read(i2cPort, I2CTransferA1, SITRONIX_REG_FINGERS, coordinate, 5))
            return false;

        if (coordinate[0 + 2] & BIT7) {
            x = (coordinate[0 + 2] & 0x70) << 4 | coordinate[1 + 2];
            y = (coordinate[0 + 2] & 0x07) << 8 | coordinate[2 + 2];

            isTouched = true;
        }
    }

    touch->SetCoordinate(touch, isTouched, x, y);
    return true;
}

bool SitronixTouchConstractor(TouchStruct* touch) {
    if (!SitronixTouchPowerUp(touch))
        return false;

    if (!SitronixTouchDetect(touch))
        return false;

    touch->Init   = Init;
    touch->Update = Update;

    return true;
}
