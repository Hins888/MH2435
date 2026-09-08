#include "GAUPort.h"

static uint8_t colorCode(GAUColorEnum color) {
#define __COLOR_CODE_ARGB8888 0x00U
#define __COLOR_CODE_RGB888   0x01U
#define __COLOR_CODE_RGB565   0x02U
#define __COLOR_CODE_ARGB1555 0x03U
#define __COLOR_CODE_ARGB4444 0x04U
#define __COLOR_CODE_L8       0x05U
#define __COLOR_CODE_AL44     0x06U
#define __COLOR_CODE_AL88     0x07U
#define __COLOR_CODE_L4       0x08U
#define __COLOR_CODE_A8       0x09U
#define __COLOR_CODE_A4       0x0AU

    uint8_t colorCode = 0;
    switch (color) {
        case GAUColorL8:
            colorCode = __COLOR_CODE_L8;
            break;
        case GAUColorRGB565:
            colorCode = __COLOR_CODE_RGB565;
            break;
        case GAUColorRGB888:
            colorCode = __COLOR_CODE_RGB888;
            break;
        case GAUColorARGB8888:
            colorCode = __COLOR_CODE_ARGB8888;
            break;
        default:
            break;
    }
    return colorCode;
}

static void deleteTask(GAUPortStruct* self, GAUTaskStruct* task);

static bool runTask(GAUPortStruct* self, GAUTaskStruct* task) {
    DMA2D_TypeDef* gau = self->GAU;

    bool isRunning = false;
    if (task->Type == GAUTaskCopy) {
        gau->CR = 0;

        // source buffer
        gau->FGPFCCR = colorCode(task->SourceColor);
        gau->FGMAR   = (uint32_t)task->Source;
        gau->FGOR    = task->SourceOffset;
        gau->FGCOLR  = 0;

        // targets buffer
        gau->OPFCCR = colorCode(task->TargetColor) | (task->Rotation << 9);
        gau->OMAR   = (uint32_t)task->Target;
        gau->OOR    = task->TargetOffset;
        gau->OCOLR  = 0;

        gau->NLR = (task->TargetWidth << DMA2D_NLR_PL_Pos) | (task->TargetHeight << DMA2D_NLR_NL_Pos);

        // start
        gau->IFCR  = 0x3FU; // reset flags
        gau->AMTCR = 0;
        if (task->Rotation || (task->SourceColor != task->TargetColor)) {
            gau->CR = BIT16; // Enable PFC
        }
        gau->CR |= //
            BIT9 | // Transfer Complete Interrupt
            BIT0;  // Start
        isRunning = true;
    }
    else {
        // TODO: Fill / Blend / Decode / Event Tasks
#if !USE_RTOS_FREERTOS
        if (task->EndCallback)
            task->EndCallback(task);

        deleteTask(self, task);
#endif
    }

    return isRunning;
}

#if USE_RTOS_FREERTOS
static SemaphoreHandle_t GAUSemaphore;

static GAUTaskStruct* createTask(GAUPortStruct* self) {
    return pvPortMalloc(sizeof(GAUTaskStruct));
}

static void deleteTask(GAUPortStruct* self, GAUTaskStruct* task) {
    vPortFree(task);
}

static bool startTask(GAUPortStruct* self, GAUTaskStruct* task) {
    return xQueueSend(self->TaskQueue, &task, portMAX_DELAY) == pdPASS;
}

static void DispatcherTask(GAUPortStruct* self) {
    GAUTaskStruct* task = NULL;
    while (xQueueReceive(self->TaskQueue, &task, portMAX_DELAY) == pdPASS) {
        // exit task
        if (task == NULL)
            break;

        if (uxSemaphoreGetCount(GAUSemaphore)) {
            if (xSemaphoreTake(GAUSemaphore, pdMS_TO_TICKS(CONFIG_GAU_TIMEOUT)) != pdPASS) {
                __BKPT();
            }
        }

        if (runTask(self, task))
            if (xSemaphoreTake(GAUSemaphore, pdMS_TO_TICKS(CONFIG_GAU_TIMEOUT)) != pdPASS)
                __BKPT();

        if (task->EndCallback)
            task->EndCallback(task);

        deleteTask(self, task);
    }

    vTaskDelete(NULL);
}

void DMA2D_IRQHandler(void) {
    DMA2D->IFCR = 0x3FU;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(GAUSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#else

static GAUPortStruct* gauPort;

static GAUTaskStruct* createTask(GAUPortStruct* self) {
    GAUTaskStruct* task = NULL;
    do {
        __disable_irq();
        if (self->TaskCount < CONFIG_GAU_MAX_TASK_QUEUE_LENGTH) {
            task = self->TaskQueue + ((self->TaskOffset + self->TaskCount) % CONFIG_GAU_MAX_TASK_QUEUE_LENGTH);
            break;
        }
        __enable_irq();
        SystemDelay(1);
    }
    while (task == NULL);
    return task;
}

static void deleteTask(GAUPortStruct* self, GAUTaskStruct* task) {
    __disable_irq();
    self->TaskOffset++;
    self->TaskOffset %= CONFIG_GAU_MAX_TASK_QUEUE_LENGTH;
    self->TaskCount--;
    __enable_irq();
}

static bool startTask(GAUPortStruct* self, GAUTaskStruct* task) {
    __disable_irq();

    self->TaskCount++;
    if (self->TaskCount == 1)
        runTask(self, task);

    __enable_irq();
    return true;
}

void DMA2D_IRQHandler(void) {
    DMA2D->IFCR = 0x3FU;

    GAUPortStruct* self = gauPort;

    if (!self)
        return;

    GAUTaskStruct* task = self->TaskQueue + self->TaskOffset;
    if (task->EndCallback)
        task->EndCallback(task);

    deleteTask(self, task);

    while (self->TaskCount) {
        task = self->TaskQueue + self->TaskOffset;
        if (runTask(self, task))
            break;
    }
}
#endif

static bool Init(GAUPortStruct* self, bool isInit) {
    if (isInit) {
        PeripheralEnable(self->Peripheral, true);
        PeripheralReset(self->Peripheral);

#if USE_RTOS_FREERTOS
        self->TaskQueue = xQueueCreate(CONFIG_GAU_MAX_TASK_QUEUE_LENGTH, sizeof(GAUTaskStruct*));

        if (self->TaskQueue == NULL)
            return false;

        if (xTaskCreate((TaskFunction_t)DispatcherTask, "GAU", RTOSStackSizeKB(CONFIG_GAU_TASK_STACK_SIZE_KB), self, GONFIG_GAU_TASK_PRIORITY,
                        &self->TaskHandle) != pdPASS)
            return false;

        GAUSemaphore = xSemaphoreCreateBinary();
#else
        self->TaskOffset = 0;
        self->TaskCount  = 0;

        gauPort = self;
#endif
        NVIC_SetPriority(DMA2D_IRQn, GONFIG_GAU_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(DMA2D_IRQn);
    }
    else {
#if USE_RTOS_FREERTOS
        xQueueSend(self->TaskQueue, NULL, portMAX_DELAY);
        vQueueDelete(self->TaskQueue);
        vSemaphoreDelete(GAUSemaphore);
#else
        gauPort = NULL;
#endif
        NVIC_DisableIRQ(DMA2D_IRQn);
    }

    return true;
}

static bool AddCopyTask(      //
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
) {
    GAUTaskStruct* task = createTask(self);

    if (!task)
        return false;

    task->Type = GAUTaskCopy;

    task->Rotation = rotation;

    task->Source       = source;
    task->SourceColor  = sourceColor;
    task->SourceOffset = sourceOffset;

    task->Target       = target;
    task->TargetColor  = targetColor;
    task->TargetOffset = targetOffset;

    task->TargetWidth  = width;
    task->TargetHeight = height;

    task->EndCallback = endCallback;
    task->UserData    = userData;

    return startTask(self, task);
}

bool AddEventTask(                   //
    GAUPortStruct*      self,        //
    GAUTaskCallbackType endCallback, //
    void*               userData     //
) {
    GAUTaskStruct* task = createTask(self);

    if (!task)
        return false;

    task->Type = GAUTaskEvent;

    task->EndCallback = endCallback;
    task->UserData    = userData;

    return startTask(self, task);
}

bool GAUPortConstractor(GAUPortStruct* self, PeripheralEnum peripheralGAU) {
    memset(self, 0, sizeof(GAUPortStruct));

    self->Init         = Init;
    self->AddCopyTask  = AddCopyTask;
    self->AddEventTask = AddEventTask;

    self->Peripheral = peripheralGAU;

    self->GAUBase = PeripheralMap[peripheralGAU].Base;

    return true;
}
