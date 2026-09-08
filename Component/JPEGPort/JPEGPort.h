#ifndef __JPEG_PORT_H__
#define __JPEG_PORT_H__

/*!
    Graphic Acceleration Unit
*/

#include "mh2435.h"

#if USE_RTOS_FREERTOS
#include "rtos-freertos.h"
#endif

// Configs
#ifndef CONFIG_JPEG_MAX_TASK_QUEUE_LENGTH
#define CONFIG_JPEG_MAX_TASK_QUEUE_LENGTH 32
#endif

#ifndef CONFIG_JPEG_TASK_STACK_SIZE_KB
#define CONFIG_JPEG_TASK_STACK_SIZE_KB 1
#endif

#ifndef CONFIG_JPEG_TIMEOUT
#define CONFIG_JPEG_TIMEOUT 500
#endif

#ifndef GONFIG_JPEG_INTERRUPT_PRIORITY
#define GONFIG_JPEG_INTERRUPT_PRIORITY 3
#endif

#ifndef GONFIG_JPEG_STEAM_INTERRUPT_PRIORITY
#define GONFIG_JPEG_STEAM_INTERRUPT_PRIORITY 3
#endif

#ifndef GONFIG_JPEG_TASK_PRIORITY
#define GONFIG_JPEG_TASK_PRIORITY 5
#endif

// Types
typedef enum {
    JPEGYuv444With8x8   = 0x00,
    JPEGYuv422With8x8   = 0x01,
    JPEGYuv420With8x8   = 0x02,
    JPEGYuvModeUnkmow   = 0xFF,
    // TODO: YUV Planar and Packed
} JPEGYuvModeEnum;

typedef enum {
    JPEGDecode,
    JPEGTaskEvent,
} JPEGTaskTypeEnum;

typedef struct {
    uint16_t Width;
    uint16_t Height;
    uint32_t YuvOutSize;
    JPEGYuvModeEnum YuvMode;
} JPEGImageInfoStruct;

typedef struct JPEGTaskBase JPEGTaskStruct;

typedef void (*JPEGTaskCallbackType)(JPEGTaskStruct* task);

struct JPEGTaskBase {
    JPEGTaskTypeEnum Type : 8;
    JPEGYuvModeEnum YuvMode : 8;
    void* Source;
    uint32_t SourceSize;
    void* Target;
    JPEGImageInfoStruct ImageInfo;
    uint32_t TargetSize;
    uint32_t TaskStauts;
    JPEGTaskCallbackType EndCallback;
    void*               UserData;
};

typedef struct JPEGPortBase JPEGPortStruct;

struct JPEGPortBase {
    bool (*Init)(JPEGPortStruct* self, bool isInit);
    bool (*AddDecodeTask)(          //
        JPEGPortStruct*  self,     //

        void*        source,       //
        uint32_t     sourceSize,
        void*        target,       //
        uint32_t     targetSize,
        JPEGTaskCallbackType endCallback, //
        void*               userData     //
    );
    bool (*AddEventTask)(                //
        JPEGPortStruct*      self,        //
        JPEGTaskCallbackType endCallback, //
        void*               userData     //
    );

#if USE_RTOS_FREERTOS
    QueueHandle_t TaskQueue;
    TaskHandle_t  TaskHandle;
#endif

    PeripheralEnum JPEGHandlerPeripheral : 8;

    union {
        uint32_t       JPEGInBase;
        DMA_Stream_TypeDef* JPEGIn;
    };

    union {
        uint32_t       JPEGOutBase;
        DMA_Stream_TypeDef* JPEGOut;
    };

    union {
        uint32_t       JPEGHandlerBase;
        JPEGD_TypeDef* JPEGHandler;
    };
};


// Functions
extern bool JPEGPortConstractor(JPEGPortStruct* self, 
                    DMA_Stream_TypeDef* JPEGInPeripheral, 
                    DMA_Stream_TypeDef* JPEGOutPeripheral,
                    PeripheralEnum JPEGHandlerPeripheral);

int8_t checkJPEGhead(uint8_t *buf, long length, JPEGImageInfoStruct* jpegInfo);

#endif // __GAU_PORT_H__
