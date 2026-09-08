#ifndef __TOUCH_BSP_H__
#define __TOUCH_BSP_H__

#include "Touch.h"

#if USE_COMPONENT_TOUCH_GOODIX
#include "GoodixTouch.h"
#endif
#if USE_COMPONENT_TOUCH_SITRONIX
#include "SitronixTouch.h"
#endif
#if USE_COMPONENT_TOUCH_HYNITRON
#include "HynitronTouch.h"
#endif
#if USE_COMPONENT_TOUCH_CHIPSEMI
#include "ChipsemiTouch.h"
#endif
#if USE_COMPONENT_TOUCH_ILITEK
#include "IlitekTouch.h"
#endif
#if USE_COMPONENT_TOUCH_AXS
#include "AXSTouch.h"
#endif

#define TOUCH_I2C_PERIPH PeripheralI2C1
#define TOUCH_I2C_SCL_IO PB6
#define TOUCH_I2C_SDA_IO PB7
#define TOUCH_RST_IO     PA15
#define TOUCH_INT_IO     PF7

#define TOUCH_INT_EXTI EXTI9_5_

#ifndef TOUCH_I2C_PERIPH
#define TOUCH_I2C_PERIPH PeripheralNull
#endif

#ifndef TOUCH_I2C_SCL_IO
#define TOUCH_I2C_SCL_IO IONone
#endif

#ifndef TOUCH_I2C_SDA_IO
#define TOUCH_I2C_SDA_IO IONone
#endif

#ifndef TOUCH_I2C_SPEED
#define TOUCH_I2C_SPEED 400
#endif

#ifndef TOUCH_RST_IO
#define TOUCH_RST_IO IONone
#endif

#ifndef TOUCH_INT_IO
#define TOUCH_INT_IO IONone
#endif

extern TouchStruct TouchPort;

extern bool TouchSetup(uint16_t x, uint16_t y, bool reverseX, bool reverseY, bool switchXY);

#endif // __TOUCH_BSP_H__
