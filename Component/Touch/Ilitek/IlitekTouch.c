#include "IlitekTouch.h"

#define ILITEK_I2C_ADDRS (0x41)

// ILITEK Regs
#define ILITEK_REG_ICE_MODE   0x181062
#define ILITEK_REG_FLASH_BASE 0x041000
#define ILITEK_REG_PID1       0x04009C
#define ILITEK_REG_PID2       0x040098

#define ILITEK_MAKE_REG(reg, mode) (((reg) << 8) | (mode))

#define ILITEK_REG_TOUCH1_STATUS   0xD000
#define ILITEK_REG_TOUCH1_POINT    0xD001
#define ILITEK_REG_TOUCH1_PRESSURE 0xD004

#define IlitekTouchReset(isReset) IOSet(touch->ResetIO, !isReset)
#define IlitekTouchInt(isHigh)    IOSet(touch->IntIO, isHigh)

#define CORE_VER_1410 0x01040100
#define CORE_VER_1420 0x01040200
#define CORE_VER_1430 0x01040300
#define CORE_VER_1460 0x01040600
#define CORE_VER_1470 0x01040700
#define CORE_VER_1600 0x01060000
#define CORE_VER_1700 0x01070000
#define CORE_VER_2100 0x02010000

#define PROTOCOL_VER_500 0x050000
#define PROTOCOL_VER_510 0x050100
#define PROTOCOL_VER_520 0x050200
#define PROTOCOL_VER_530 0x050300
#define PROTOCOL_VER_540 0x050400
#define PROTOCOL_VER_550 0x050500
#define PROTOCOL_VER_560 0x050600
#define PROTOCOL_VER_570 0x050700

#define P5_X_READ_DATA_CTRL                  0xF6
#define P5_X_GET_TP_INFORMATION              0x20
#define P5_X_GET_KEY_INFORMATION             0x27
#define P5_X_GET_TOOL_VERSION                0x28
#define P5_X_GET_PANEL_INFORMATION           0x29
#define P5_X_GET_FW_VERSION                  0x21
#define P5_X_GET_PROTOCOL_VERSION            0x22
#define P5_X_GET_CORE_VERSION                0x23
#define P5_X_GET_CORE_VERSION_NEW            0x24
#define P5_X_GET_REPORT_INFORMATION          0x2B
#define P5_X_GET_ALL_INFORMATION             0x2F
#define P5_X_GET_REPORT_FORMAT               0x37
#define P5_X_GET_BLOCK_INFOMATION            0x38
#define P5_X_MODE_CONTROL                    0xF0
#define P5_X_NEW_CONTROL_FORMAT              0xF2
#define P5_X_SET_CDC_INIT                    0xF1
#define P5_X_GET_CDC_DATA                    0xF2
#define P5_X_CDC_BUSY_STATE                  0xF3
#define P5_X_MP_TEST_MODE_INFO               0xFE
#define P5_X_I2C_UART                        0x40
#define CMD_GET_FLASH_DATA                   0x41
#define CMD_CTRL_INT_ACTION                  0x1B
#define P5_X_FW_UNKNOWN_MODE                 0xFF
#define P5_X_FW_AP_MODE                      0x00
#define P5_X_FW_TEST_MODE                    0x01
#define P5_X_FW_GESTURE_MODE                 0x0F
#define P5_X_FW_SIGNAL_DATA_MODE             0x03
#define P5_X_FW_RAW_DATA_MODE                0x08
#define P5_X_DEMO_PACKET_ID                  0x5A
#define P5_X_DEMO_AXIS_PACKET_ID             0x5B
#define P5_X_DEBUG_PACKET_ID                 0xA7
#define P5_X_DEBUG_AXIS_PACKET_ID            0xA8
#define P5_X_TEST_PACKET_ID                  0xF2
#define P5_X_GESTURE_PACKET_ID               0xAA
#define P5_X_GESTURE_FAIL_ID                 0xAE
#define P5_X_I2CUART_PACKET_ID               0x7A
#define P5_X_DEBUG_LITE_PACKET_ID            0x9A
#define P5_X_SLAVE_MODE_CMD_ID               0x5F
#define P5_X_INFO_HEADER_PACKET_ID           0xB7
#define P5_X_DEMO_DEBUG_INFO_PACKET_ID       0x5C
#define P5_X_EDGE_PLAM_CTRL_1                0x01
#define P5_X_EDGE_PLAM_CTRL_2                0x12
#define P5_X_EDGE_PALM_TUNING_PARA           0x1E
#define SPI_WRITE                            0x82
#define SPI_WRITE_SLAVE0                     0x87
#define SPI_READ                             0x83
#define SPI_ACK                              0xA3
#define P5_X_DEMO_PROXIMITY_ID               0xBC
#define P5_X_DEMO_HIGH_RESOLUTION_PACKET_ID  0x5B
#define P5_X_DEBUG_HIGH_RESOLUTION_PACKET_ID 0xA8
#define P5_X_DEMO_FINGER_PACKET_ID           0x81
#define P5_X_DEMO_PEN_PACKET_ID              0x82

/* FW data format */
#define DATA_FORMAT_DEMO_CMD                0x00
#define DATA_FORMAT_DEBUG_CMD               0x02
#define DATA_FORMAT_DEMO_DEBUG_INFO_CMD     0x04
#define DATA_FORMAT_GESTURE_NORMAL_CMD      0x01
#define DATA_FORMAT_GESTURE_INFO_CMD        0x02
#define DATA_FORMAT_DEBUG_LITE_CMD          0x05
#define DATA_FORMAT_DEBUG_LITE_ROI_CMD      0x01
#define DATA_FORMAT_DEBUG_LITE_WINDOW_CMD   0x02
#define DATA_FORMAT_DEBUG_LITE_AREA_CMD     0x03
#define DATA_FORMAT_DEBUG_LITE_PEN_CMD      0x04
#define DATA_FORMAT_DEBUG_LITE_PEN_AREA_CMD 0x06

/* Report Format Resolution */
#define POSITION_LOW_RESOLUTION          0X00
#define POSITION_HIGH_RESOLUTION         0x01
#define POSITION_CUSTOMER_TYPE_ON        0x00
#define POSITION_CUSTOMER_TYPE_OFF       0x1F
#define POSITION_PEN_TYPE_ON             0x00
#define POSITION_PEN_TYPE_OFF            0x03
#define POSITION_CUSTOMER_TYPE_OFF_3BITS 0x07 /*core ver 1700, CustomerType 3 bits*/

static bool IlitekTouchPowerUp(TouchStruct* touch) {
    IOSetup(touch->ResetIO, IO_DEFAULT_OUTPUT_CONFIG);
    IOSetup(touch->IntIO, IO_DEFAULT_OUTPUT_CONFIG);

    // set reset state
    IlitekTouchReset(true);

    // keep reset 1ms
    SystemDelay(1);

    // release reset
    IlitekTouchReset(false);

    SystemDelay(5);

    return true;
}

static const uint8_t IlitekAddrs[] = {ILITEK_I2C_ADDRS};

static uint32_t IlitekGetID(I2CPortStruct* i2cPort) {
    // enter ice mode
    if (!i2cPort->Write(i2cPort, I2CTransferLA4, ILITEK_MAKE_REG(ILITEK_REG_ICE_MODE, 0x25), NULL, 0))
        return 0;

    if (!i2cPort->WriteValue(i2cPort, I2CTransferLA4LD1, ILITEK_MAKE_REG(ILITEK_REG_FLASH_BASE, 0x25), 0x01))
        return 0;

    uint32_t pid1 = 0;
    if (!i2cPort->ReadValue(i2cPort, I2CTransferLA4LD4, ILITEK_MAKE_REG(ILITEK_REG_PID1, 0x25), &pid1))
        return 0;

    // read pid1 / pid2
    uint32_t chipID = 0;
    if (((pid1 >> 28) & 0xF) == 0xF) {
        uint32_t pid2 = 0;
        if (!i2cPort->ReadValue(i2cPort, I2CTransferLA4LD4, ILITEK_MAKE_REG(ILITEK_REG_PID2, 0x25), &pid2))
            return 0;
        chipID = ((pid2 & 0x0000FFFF) << 12) + ((pid1 & 0x0FFF0000) >> 16);
    }
    else {
        chipID = pid1 >> 16;
    }

    // exit ice mode
    if (!i2cPort->Write(i2cPort, I2CTransferLA4, ILITEK_MAKE_REG(ILITEK_REG_ICE_MODE, 0x1B), NULL, 0))
        return 0;

    return chipID;
}

static bool IlitekExcuteCommand(I2CPortStruct* i2cPort, uint8_t control, uint8_t command, uint8_t* data, uint8_t length, uint8_t tryCount) {
    uint32_t address;
    if (control && !i2cPort->Write(i2cPort, I2CTransferLA2, address = command << 8 | control, NULL, 0))
        return false;

    address = command;
    for (int i = 0; i < tryCount; i++) {
        if (!i2cPort->Write(i2cPort, I2CTransferA1, command, NULL, 0))
            return false;

        if (i2cPort->Read(i2cPort, 0, NULL, data, length) && data[0] == command)
            return true;
        SystemDelay(2);
    }

    return false;
}

static bool IlitekTouchDetect(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    uint32_t id = 0;

    for (int i = 0; i < sizeof(IlitekAddrs); i++) {
        i2cPort->SetAddress(i2cPort, IlitekAddrs[i]);

        id = IlitekGetID(i2cPort);

        if (id) {
            touch->I2CAddress = IlitekAddrs[i];
            break;
        }
    }

    switch (id) {
        case 0x00794501:
            touch->Points = 1;

            touch->NativeCoordConversion = false;

            touch->IsRasingEvent = true;

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

    uint8_t data[6] = {0};

    // get core version
    if (!IlitekExcuteCommand(i2cPort, P5_X_READ_DATA_CTRL, P5_X_GET_CORE_VERSION_NEW, data, 6, 3) &&
        !IlitekExcuteCommand(i2cPort, P5_X_READ_DATA_CTRL, P5_X_GET_CORE_VERSION, data, 6, 3))
        return false;

    if (data[0] == P5_X_GET_CORE_VERSION)
        data[4] = 0;

    uint32_t coreVersion = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];

    // get report format
    uint8_t penType         = POSITION_PEN_TYPE_OFF;
    uint8_t resolutionMode  = POSITION_LOW_RESOLUTION;
    uint8_t customerType    = POSITION_CUSTOMER_TYPE_OFF;
    uint8_t customerTypeOff = POSITION_CUSTOMER_TYPE_OFF;

    if (coreVersion >= CORE_VER_1470) {
        if (!IlitekExcuteCommand(i2cPort, 0, P5_X_GET_REPORT_FORMAT, data, 2, 3))
            return false;
        if (coreVersion >= CORE_VER_1700) {
            customerType    = (data[1] >> 3) & 0x07;
            penType         = data[1] >> 6;
            customerTypeOff = POSITION_CUSTOMER_TYPE_OFF_3BITS;
        }
        else {
            customerType    = data[1] >> 3;
            customerTypeOff = POSITION_CUSTOMER_TYPE_OFF;
        }
    }

    if (customerType != customerTypeOff || resolutionMode != POSITION_LOW_RESOLUTION || penType != POSITION_PEN_TYPE_OFF)
        return false;

    // set control format
    if (coreVersion >= CORE_VER_1700) {
        uint32_t address = P5_X_FW_SIGNAL_DATA_MODE << 16 | DATA_FORMAT_DEMO_CMD << 8 | P5_X_NEW_CONTROL_FORMAT;
        if (!i2cPort->Write(i2cPort, I2CTransferLA3, address, NULL, 0))
            return false;
    }
    else {
        uint32_t address = DATA_FORMAT_DEMO_CMD << 8 | P5_X_MODE_CONTROL;
        if (!i2cPort->Write(i2cPort, I2CTransferLA2, address, NULL, 0))
            return false;
    }

    return true;
}

static bool Update(TouchStruct* touch) {
    I2CPortStruct* i2cPort = touch->I2CPort;

    uint8_t packet[43] = {0};
    if (!i2cPort->Read(i2cPort, I2CTransferNA, NULL, packet, sizeof(packet)) || packet[0] != P5_X_DEMO_PACKET_ID)
        return false;

    bool isTouched = false;

    uint16_t x = 0, y = 0;
    // touched
    if (packet[1] != 0xFF && packet[2] != 0xFF && packet[3] != 0xFF) {
        isTouched = true;

        x = ((((packet[1] & 0xF0) << 4) | (packet[2])) * (uint32_t)touch->X) / 2048UL;
        y = ((((packet[1] & 0x0F) << 8) | (packet[3])) * (uint32_t)touch->Y) / 2048UL;
    }
    touch->SetCoordinate(touch, isTouched, x, y);

    return true;
}

bool IlitekTouchConstractor(TouchStruct* touch) {
    if (!IlitekTouchPowerUp(touch))
        return false;

    if (!IlitekTouchDetect(touch))
        return false;

    touch->Init   = Init;
    touch->Update = Update;

    return true;
}
