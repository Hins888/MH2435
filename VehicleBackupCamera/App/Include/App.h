#ifndef __APP_H__
#define __APP_H__

// Select Board Configs
#if CONFIG_BOARD == 15
// I2C Config

#define I2C0_PERIPH PeripheralI2C3
#define I2C0_SCL_IO PC15
#define I2C0_SDA_IO PC14

#endif

// Default Config
#ifndef CSI_I2C
#define CSI_I2C i2cPort0
#endif

#ifndef DVP_I2C
#define DVP_I2C i2cPort0
#endif

#include "SDRAMBSP.h"

#include "DisplayBSP.h"

#include "CameraBSP.h"

#endif
