#include "CameraBSP.h"

#ifndef I2C1_PERIPH
#define CAMERA_I2C_PORT_COUNT 1
#else
#define CAMERA_I2C_PORT_COUNT 2
#endif

static I2CPortStruct CameraI2CPort[CAMERA_I2C_PORT_COUNT];

static I2CPortStruct* CameraI2CPortSetup(uint8_t portIndex) {
    if (portIndex >= CAMERA_I2C_PORT_COUNT)
        return NULL;

    PeripheralEnum peripheralI2C;

    IOEnum scl, sda;

    if (portIndex == 0) {
        peripheralI2C = I2C0_PERIPH;
        scl           = I2C0_SCL_IO;
        sda           = I2C0_SDA_IO;
    }
#ifdef I2C1_PERIPH
    else {
        peripheralI2C = I2C1_PERIPH;
        scl           = I2C1_SCL_IO;
        sda           = I2C1_SDA_IO;
    }
#endif

    I2CPortStruct* i2cPort = &CameraI2CPort[portIndex];

    // if I2C Port is already setup
    if (i2cPort->Peripheral == peripheralI2C && i2cPort->SCL == scl && i2cPort->SDA == sda)
        return i2cPort;

    I2CPortConstractor(i2cPort, peripheralI2C, scl, sda);
    if (!i2cPort->Init(i2cPort)) {
        memset(i2cPort, 0, sizeof(I2CPortStruct));
        return NULL;
    }

    return i2cPort;
}

CSIStruct    CSIHandle;
CameraStruct CSICamera;

bool CSICameraPreInit(I2CPortStruct* i2cPort, uint32_t* buffer) {
    CameraStruct* camera = &CSICamera;

    CameraCSIConstractor(camera, i2cPort, &CSIHandle, buffer);
    camera->PowerIO = CSI_PWDN_IO;
    camera->ResetIO = CSI_RST_IO;

#ifdef CSI_VDD_IO
    CSI_VDD(true);
#endif

#ifdef CSI_XCLK_IO
    camera->ClockIO      = CSI_XCLK_IO;
    camera->Timer        = CSI_XCLK_TIMER;
    camera->TimerChannel = CSI_XCLK_TIMER_CHANNEL;
#endif

#if USE_COMPONENT_CAMERA_OV5640
    if (OV5640CSIConstractor(camera)) {
        return true;
    }
#endif

#if USE_COMPONENT_CAMERA_XS9950
    if (XS9950Constractor(camera)) {
        return true;
    }
#endif

    return false;
}

__WEAK bool CSICameraConfig(CameraStruct* camera) {
    return true;
}

bool CSICameraSetup(I2CPortStruct* i2cPort, uint32_t* buffer) {
    if (i2cPort == NULL && ((i2cPort = CameraI2CPortSetup(CSI_I2C_INDEX)) == NULL))
        return false;

    if (!CSICameraPreInit(i2cPort, buffer))
        return false;

    if (!CSICameraConfig(&CSICamera))
        return false;

    if (!CSICamera.Init(&CSICamera))
        return false;

    if (!CSICamera.Start(&CSICamera, true))
        return false;

    return true;
}
