#ifndef __Touch_H__
#define __Touch_H__

#include "I2CPort.h"

#if USE_RTOS_FREERTOS
#include "rtos-freertos.h"

#ifndef CONFIG_TOUCH_MAX_COORDINATE_QUEUE_LENGTH
#define CONFIG_TOUCH_MAX_COORDINATE_QUEUE_LENGTH 32
#endif

#ifndef CONFIG_TOUCH_TASK_STACK_SIZE_KB
#define CONFIG_TOUCH_TASK_STACK_SIZE_KB 1
#endif

#ifndef GONFIG_TOUCH_TASK_PRIORITY
#define GONFIG_TOUCH_TASK_PRIORITY 5
#endif

#endif

typedef struct {
    uint16_t X;
    uint16_t Y : 15;

    bool IsTouched : 1;
} TouchCoordinateStruct;

typedef struct TouchStruct_s TouchStruct;

struct TouchStruct_s {
    I2CPortStruct* I2CPort;
    uint8_t        I2CAddress;

    uint32_t ID;

    IOEnum IntIO : 8;
    IOEnum ResetIO : 8;

    uint16_t XOffset;
    uint16_t YOffset;
    uint16_t X;
    uint16_t Y;

    bool NativeCoordConversion : 1;
    bool ReverseX : 1;
    bool ReverseY : 1;
    bool SwitchXY : 1;

    bool IsRasingEvent : 1;

    uint8_t Points;

    // Support one point now
#if USE_RTOS_FREERTOS
    QueueHandle_t     CoordinateQueue;
    SemaphoreHandle_t EventSemaphore;
#endif

    TouchCoordinateStruct Coordinate;

    bool (*Init)(TouchStruct* touch);
    bool (*Update)(TouchStruct* touch);

    void (*SetCoordinate)(TouchStruct* touch, bool isTouched, uint16_t x, uint16_t y);
    bool (*GetCoordinate)(TouchStruct* touch, bool* isTouched, uint16_t* x, uint16_t* y);
};

extern bool TouchConstractor(TouchStruct* touch, I2CPortStruct* i2cPort);

extern void TouchEventInit(TouchStruct* touch);

extern void TouchEvent(TouchStruct* touch);

#endif
