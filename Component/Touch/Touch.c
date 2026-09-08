#include "Touch.h"

static void ConvertCoordinate(TouchStruct* touch, uint16_t* x, uint16_t* y) {
    if (touch->NativeCoordConversion)
        return;

    if (touch->SwitchXY) {
        *x ^= *y;
        *y ^= *x;
        *x ^= *y;
    }

    if (touch->ReverseX)
        *x = touch->X - *x;

    if (touch->ReverseY)
        *y = touch->Y - *y;
}

static bool GetCoordinate(TouchStruct* touch, bool* isTouched, uint16_t* x, uint16_t* y) {
    TouchCoordinateStruct coordinate;
#if USE_RTOS_FREERTOS
    if (uxQueueMessagesWaiting(touch->CoordinateQueue))
        xQueueReceive(touch->CoordinateQueue, &coordinate, 0);
    else
#endif
        coordinate = touch->Coordinate;

    *isTouched = coordinate.IsTouched;

    if (*isTouched) {
        *x = coordinate.X;
        *y = coordinate.Y;
        ConvertCoordinate(touch, x, y);
    }
#if USE_RTOS_FREERTOS
    return uxQueueMessagesWaiting(touch->CoordinateQueue) > 0;
#else
    return false;
#endif
}

static void SetCoordinate(TouchStruct* touch, bool isTouched, uint16_t x, uint16_t y) {
    TouchCoordinateStruct coordinate = (TouchCoordinateStruct) {.X = x, .Y = y, .IsTouched = isTouched};
#if USE_RTOS_FREERTOS
    TouchCoordinateStruct lastCoordinate;
    if (!isTouched && (xQueuePeek(touch->CoordinateQueue, &lastCoordinate, (TickType_t)0) == pdTRUE) && !lastCoordinate.IsTouched) {
        return;
    }

    if (uxQueueSpacesAvailable(touch->CoordinateQueue) == 0) {
        void* dummy;
        xQueueReceive(touch->CoordinateQueue, &dummy, 0);
    }
    xQueueSend(touch->CoordinateQueue, &coordinate, 0);
#endif
    touch->Coordinate = coordinate;
}

static bool GetEvent(TouchStruct* touch) {
    // always fire event if no interrupt IO is set
    if (touch->IntIO == IONone)
        return true;
    if (EXTI->PR & IOPin(touch->IntIO)) {
        EXTI->PR = IOPin(touch->IntIO);
        return true;
    }
    return false;
}

#if USE_RTOS_FREERTOS
static void TouchEventTask(TouchStruct* touch) {
    while (xSemaphoreTake(touch->EventSemaphore, portMAX_DELAY) == pdPASS) {
        touch->Update(touch);
    }
}

void TouchEvent(TouchStruct* touch) {
    if (GetEvent(touch)) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(touch->EventSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

#else
void TouchEvent(TouchStruct* touch) {
    if (GetEvent(touch))
        touch->Update(touch);
}
#endif

bool TouchConstractor(TouchStruct* touch, I2CPortStruct* i2cPort) {
    memset(touch, 0, sizeof(TouchStruct));
    touch->I2CPort = i2cPort;

    touch->IntIO   = IONone;
    touch->ResetIO = IONone;

#if USE_RTOS_FREERTOS
    touch->CoordinateQueue = xQueueCreate(CONFIG_TOUCH_MAX_COORDINATE_QUEUE_LENGTH, sizeof(TouchCoordinateStruct));
    touch->EventSemaphore  = xSemaphoreCreateBinary();

    if (                                                      //
        !xTaskCreate(                                         //
            (TaskFunction_t)TouchEventTask,                   //
            "Touch",                                          //
            RTOSStackSizeKB(CONFIG_TOUCH_TASK_STACK_SIZE_KB), //
            touch,                                            //
            GONFIG_TOUCH_TASK_PRIORITY,                       //
            NULL                                              //
            )                                                 //
    )
        return false;
#endif

    touch->SetCoordinate = SetCoordinate;
    touch->GetCoordinate = GetCoordinate;

    return true;
}

void TouchEventInit(TouchStruct* touch) {
    if (touch->IntIO == IONone)
        return;

    PeripheralEnable(PeripheralEXTI, true);
    PeripheralEnable(PeripheralSYSCFG, true);

    IOSetup(touch->IntIO, MakeIOConfig(IOModeInput, 0, IOPullUp, IOSpeedLow, IODriveLow));

    // set the source pin of EXTI line
    uint8_t pinIndex  = IOSource(touch->IntIO);
    uint8_t portIndex = touch->IntIO >> 4;
    uint8_t maskPos   = (pinIndex & 0x3) << 2;

    SYSCFG->EXTICR[pinIndex >> 2] = (SYSCFG->EXTICR[pinIndex >> 2] & ~(0xF << maskPos)) | portIndex << maskPos;

    // enable the EXTI event
    if (touch->IsRasingEvent) {
        EXTI->RTSR |= IOPin(touch->IntIO);
        EXTI->FTSR &= ~IOPin(touch->IntIO);
    }
    else {
        EXTI->RTSR &= ~IOPin(touch->IntIO);
        EXTI->FTSR |= IOPin(touch->IntIO);
    }
    EXTI->IMR |= IOPin(touch->IntIO);
}
