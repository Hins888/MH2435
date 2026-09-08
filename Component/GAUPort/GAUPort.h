#ifndef __GAU_PORT_H__
#define __GAU_PORT_H__

/*!
    Graphic Acceleration Unit
*/

#include "mh2435.h"

#if USE_RTOS_FREERTOS
#include "rtos-freertos.h"
#endif

// Configs
#ifndef CONFIG_GAU_MAX_TASK_QUEUE_LENGTH
#define CONFIG_GAU_MAX_TASK_QUEUE_LENGTH 32
#endif

#ifndef CONFIG_GAU_TASK_STACK_SIZE_KB
#define CONFIG_GAU_TASK_STACK_SIZE_KB 1
#endif

#ifndef CONFIG_GAU_TIMEOUT
#define CONFIG_GAU_TIMEOUT 100
#endif

#ifndef GONFIG_GAU_INTERRUPT_PRIORITY
#define GONFIG_GAU_INTERRUPT_PRIORITY 3
#endif

#ifndef GONFIG_GAU_TASK_PRIORITY
#define GONFIG_GAU_TASK_PRIORITY 5
#endif

// Types
typedef enum {
    GAURotation0   = 0x00,
    GAURotation90  = 0x01,
    GAURotation180 = 0x02,
    GAURotation270 = 0x03,
} GAURotationEnum;

typedef enum {
    GAUTaskCopy,
    GAUTaskBlend,
    GAUTaskFill,
    GAUTaskDecode,
    GAUTaskEvent,
} GAUTaskTypeEnum;

typedef enum {
    GAUColorL8       = 0x01,
    GAUColorRGB565   = 0x02,
    GAUColorRGB888   = 0x03,
    GAUColorARGB8888 = 0x04,
    GAUColorJYUV420  = 0x10,
    GAUColorJYUV422  = 0x11,
} GAUColorEnum;

typedef struct {
    uint16_t X;
    uint16_t Y;
    uint16_t Width;
    uint16_t Height;
    uint16_t DrawWidth;
    uint16_t DrawHeight;
} GAUAreaStruct;

typedef struct GAUTaskBase GAUTaskStruct;

typedef void (*GAUTaskCallbackType)(GAUTaskStruct* task);

struct GAUTaskBase {
    GAUTaskTypeEnum Type : 8;
    GAURotationEnum Rotation : 8;
    union {
        uint8_t      _color;
        GAUColorEnum BackgroundColor : 8;
        GAUColorEnum SourceColor : 8;
    };
    GAUColorEnum TargetColor : 8;
    GAUColorEnum ForegroundColor : 8;

    void* Source;
    void* Target;

    uint16_t SourceOffset;
    uint16_t TargetOffset;

    uint16_t TargetWidth;
    uint16_t TargetHeight;

    GAUTaskCallbackType EndCallback;
    void*               UserData;
};

typedef struct GAUPortBase GAUPortStruct;

struct GAUPortBase {
    bool (*Init)(GAUPortStruct* self, bool isInit);
    bool (*AddCopyTask)(          //
        GAUPortStruct*  self,     //
        GAURotationEnum rotation, //

        void*        source,       //
        GAUColorEnum sourceColor,  //
        uint16_t     sourceOffset, //

        void*        target,       //
        GAUColorEnum targetColor,  //
        uint16_t     targetOffset, //

        uint16_t width,  //
        uint16_t height, //

        GAUTaskCallbackType endCallback, //
        void*               userData     //
    );
    bool (*AddEventTask)(                //
        GAUPortStruct*      self,        //
        GAUTaskCallbackType endCallback, //
        void*               userData     //
    );

#if USE_RTOS_FREERTOS
    QueueHandle_t TaskQueue;
    TaskHandle_t  TaskHandle;
#else
    GAUTaskStruct TaskQueue[CONFIG_GAU_MAX_TASK_QUEUE_LENGTH];

    uint8_t TaskOffset;
    uint8_t TaskCount;
#endif

    PeripheralEnum Peripheral : 8;

    union {
        uint32_t       GAUBase;
        DMA2D_TypeDef* GAU;
    };
};

// Functions
extern bool GAUPortConstractor(GAUPortStruct* self, PeripheralEnum peripheralGAU);

#endif // __GAU_PORT_H__
