#ifndef __TOUCH_GOODIX_H__
#define __TOUCH_GOODIX_H__

#ifndef USE_SIMULATION

#include "mh2435.h"

#define GOODIX_SCCB_ADDRS  0x28,0xBA	//8bit

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


bool Touch_GetXY(uint16_t *x, uint16_t *y);
bool Touch_IsPressed(void);
bool Touch_Init(void);
#endif 

#endif  /* __TOUCH_GOODIX_H__ */

