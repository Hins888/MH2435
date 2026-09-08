#ifndef __CAMERA_BSP_H__
#define __CAMERA_BSP_H__

// Includes
#include "Camera.h"

#if USE_COMPONENT_CAMERA_XS9950
#include "XS9950.h"
#endif

// Select Board Configs
#if CONFIG_BOARD == 15
// I2C Config
#define I2C0_PERIPH PeripheralI2C2
#define I2C0_SCL_IO PB10
#define I2C0_SDA_IO PB11

// CSI Port Config
#define CSI_RST_IO  PA7

#endif

// Default Config
// - CSI
#ifndef CSI_I2C_INDEX
#define CSI_I2C_INDEX 0
#endif

#ifndef CSI_PWDN_IO
#define CSI_PWDN_IO IONone
#endif

#ifndef CSI_RST_IO
#define CSI_RST_IO IONone
#endif

#ifdef CSI_VDD_IO
#ifndef CSI_VDD
#define CSI_VDD(isOn) IOSet(CSI_VDD_IO, isOn)
#endif
#endif


extern CameraStruct CSICamera;

extern bool CSICameraPreInit(I2CPortStruct* i2cPort, uint32_t* buffer);

extern bool CSICameraConfig(CameraStruct* camera);

extern bool CSICameraSetup(I2CPortStruct* i2cPort, uint32_t* buffer);

#endif // __CAMERA_BSP_H__
