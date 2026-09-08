#include "GoodixTouch.h"

#define GOODIX_I2C_ADDRS (0xBA >> 1), (0x28 >> 1)

// ReadOnly registers (device and coordinates info)
// Product ID (MSB 4 bytes)
#define GOODIX_REG_ID 0x8140
// Firmware version (LSB 2 bytes)
#define GOODIX_REG_FW_VER 0x8144

// Read/write registers
// The version number of the configuration file
#define GOODIX_REG_GT9X_CONFIG_DATA 0x8047
#define GOODIX_REG_GT9X_CONFIG_SIZE 185

#define GOODIX_REG_GT1X_CONFIG_DATA 0x8050
#define GOODIX_REG_GT1X_CONFIG_SIZE 238

// Current output X resolution (LSB 2 bytes)
#define GOODIX_READ_X_RES 0x8146
// Current output Y resolution (LSB 2 bytes)
#define GOODIX_READ_Y_RES 0x8148
// Module vendor ID
#define GOODIX_READ_VENDOR_ID 0x814A

#define GOODIX_READ_COORD_ADDR 0x814E

#define GOODIX_POINT_X_ADDR 0x8150
#define GOODIX_POINT_Y_ADDR 0x8152

#define GoodixTouchReset(isReset) IOSet(touch->ResetIO, !isReset)
#define GoodixTouchInt(isHigh)    IOSet(touch->IntIO, isHigh)

static bool GoodixTouchPowerUp(TouchStruct* touch) {
    IOSetup(touch->ResetIO, IO_DEFAULT_OUTPUT_CONFIG);
    IOSetup(touch->IntIO, IO_DEFAULT_OUTPUT_CONFIG);

    // set reset state
    GoodixTouchReset(true);
    // do not rise int io to select default I2C address
    GoodixTouchInt(false);
    // keep reset 10ms
    SystemDelay(10);

    // release reset
    GoodixTouchReset(false);
    SystemDelay(10);
    GoodixTouchInt(false);

    // wait for stable
    SystemDelay(60);

    return true;
}

static const uint8_t GoodixAddrs[] = {GOODIX_I2C_ADDRS};

static bool GoodixTouchDetect(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    uint8_t id[4] = {0};

    bool isSuccessful = false;

    for (int i = 0; i < sizeof(GoodixAddrs); i++) {
        i2cPort->SetAddress(i2cPort, GoodixAddrs[i]);
        isSuccessful = i2cPort->Read(i2cPort, I2CTransferA2, GOODIX_REG_ID, id, 4);
        if (isSuccessful) {
            touch->I2CAddress = GoodixAddrs[i];
            break;
        }
    }

    if (!isSuccessful)
        return false;

    touch->ID = 0;
    for (int i = 0; i < 4; i++) {
        if (id[i] < 0x30)
            break;
        touch->ID = (touch->ID << 4) | (id[i] & 0x0F);
    }

    switch (touch->ID) {
        case 0x9271: // GT9271
        case 0x0911: // GT911
            touch->Points = 5;

            touch->NativeCoordConversion = false;
            break;

        case 0x1158: // GT1151Q
            touch->Points = 5;

            touch->NativeCoordConversion = false;
            break;
        // unknown id
        default:
            touch->ID = 0;
            return false;
    }

    return true;
}

// GT1x Config
static uint16_t GoodixGT1xChecksum(uint8_t* config) {
    __IO uint16_t checksum = 0;
    for (uint8_t i = 0; i < GOODIX_REG_GT1X_CONFIG_SIZE - 2; i += 2) {
        checksum += (config[i] << 8) | config[i + 1];
    }
    return 0 - checksum;
}

static bool GoodixGT1xLoadConfig(TouchStruct* touch, I2CPortStruct* i2cPort) {
    uint8_t config[GOODIX_REG_GT1X_CONFIG_SIZE + 1];

    if (!i2cPort->Read(i2cPort, I2CTransferA2, GOODIX_REG_GT1X_CONFIG_DATA, config, GOODIX_REG_GT1X_CONFIG_SIZE))
        return false;

    uint16_t checksum = config[GOODIX_REG_GT1X_CONFIG_SIZE - 2] << 8 | config[GOODIX_REG_GT1X_CONFIG_SIZE - 1];
    if (checksum != GoodixGT1xChecksum(config))
        return false;

    return true;
}

// GT9x Config
static uint8_t GoodixGT9xChecksum(uint8_t* config) {
    __IO uint8_t checksum = 0;
    for (uint8_t i = 0; i < GOODIX_REG_GT9X_CONFIG_SIZE - 1; i++) {
        checksum += config[i];
    }
    return (~checksum) + 1;
}

static bool GoodixGT9xLoadConfig(TouchStruct* touch, I2CPortStruct* i2cPort) {
    uint8_t config[GOODIX_REG_GT9X_CONFIG_SIZE + 1];

    if (!i2cPort->Read(i2cPort, I2CTransferA2, GOODIX_REG_GT9X_CONFIG_DATA, config, GOODIX_REG_GT9X_CONFIG_SIZE))
        return false;

    if (config[GOODIX_REG_GT9X_CONFIG_SIZE - 1] != GoodixGT9xChecksum(config))
        return false;

    /*
    // Set X resolution
    config[1] = LSB(touch->X);
    config[2] = MSB(touch->X);
    // Set Y resolution
    config[3] = LSB(touch->Y);
    config[4] = MSB(touch->Y);
    // Set touch number
    config[5] = 1; //touch->Points;
    */

    touch->IsRasingEvent = (config[6] & 0x03) == 0 || (config[6] & 0x03) == 3;

    if (!touch->NativeCoordConversion)
        return true;

    // Check SwitchXY bit and load config if needed: [3]: Switch XY  [2]: Software Noise Reduction
    if ((config[6] & ~BIT3) != (touch->SwitchXY << 3)) {
        config[6] = (touch->SwitchXY << 3) | (config[6] & ~BIT3);

        config[184] = GoodixGT9xChecksum(config);
        config[185] = 0x01;

        if (!i2cPort->Write(i2cPort, I2CTransferA2, GOODIX_REG_GT9X_CONFIG_DATA, config, sizeof(config)))
            return false;
    }

    return true;
}

static bool GoodixTouchLoadConfig(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    switch (touch->ID) {
        case 0x1158:
            if (!GoodixGT1xLoadConfig(touch, i2cPort))
                return false;
            break;
        case 0x9271:
        case 0x0911:
            if (!GoodixGT9xLoadConfig(touch, i2cPort))
                return false;
            break;
        default:
            return false;
    }

    return true;
}

static bool Init(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    i2cPort->SetAddress(i2cPort, touch->I2CAddress);

    // Only load config when touch NativeCoordConversion is set
    if (!GoodixTouchLoadConfig(touch))
        return false;

    return true;
}

static bool Update(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    __IO uint8_t status = 0;
    if (!i2cPort->Read(i2cPort, I2CTransferA2, GOODIX_READ_COORD_ADDR, (uint8_t*)&status, 1))
        return false;

    if ((status & BIT7) == 0)
        return false;

    bool isTouched = false;

    uint16_t x = 0, y = 0;
    // touched
    if (status & BITS(3, 0)) {
        uint16_t coordinate[2] = {0};
        if (!i2cPort->Read(i2cPort, I2CTransferA2, GOODIX_POINT_X_ADDR, (uint8_t*)&coordinate, 4))
            return false;

        x = coordinate[0];
        y = coordinate[1];

        isTouched = true;
    }
    touch->SetCoordinate(touch, isTouched, x, y);

    i2cPort->WriteValue(i2cPort, I2CTransferA2D1, GOODIX_READ_COORD_ADDR, 0);

    return false;
}

bool GoodixTouchConstractor(TouchStruct* touch) {
    if (!GoodixTouchPowerUp(touch))
        return false;

    if (!GoodixTouchDetect(touch))
        return false;

    touch->Init   = Init;
    touch->Update = Update;

    return true;
}
