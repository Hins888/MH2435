#include "JPEGPort.h"

#define DBG_OUT(...) 

/**
 * @brief 查找并返回 DQT 和 SOF 标记的位置。
 *
 * @param[in] buf 输入的字节缓冲区。
 * @param[in] length 缓冲区的长度。
 * @param[out] dqt_start 输出 DQT 开始位置的指针。
 * @param[out] dqt_end 输出 DQT 结束位置的指针。
 * @param[out] sof_start 输出 SOF 开始位置的指针。
 * @param[out] sof_end 输出 SOF 结束位置的指针。
 * 
 * @return
 * - -1 表示图像错误或传参有问题。
 * -  0 表示不需要调换位置。
 * -  1 表示需要调换位置。
 */
static int8_t findDqtAndSof(uint8_t *buf, long length, long *dqt_start, long *dqt_end, long *sof_start, long *sof_end)
{
    uint32_t offset = 0;
    uint16_t lengthAtom = 0;
    uint32_t i = 0;
    uint32_t atomLength = 0;
    uint8_t markCount = 0;
    uint8_t lastAtomIsDQT = 0;

    if(buf == NULL)return -1;
    if(length <= 0)return -1;
    if(dqt_start == NULL)return -1;
    if(dqt_end == NULL)return -1;
    if(sof_start == NULL)return -1;
    if(sof_end == NULL)return -1;

    //帧头不匹配
    if(buf[0] != 0xff || buf[1] != 0xd8)return -1;

    for(offset = 0; offset < length; offset++)
    {
        if(buf[offset] == 0xff && buf[offset+1] != 0xd8)
            break;
    }
    
    *dqt_start = 0;
    *dqt_end = 0;
    *sof_start = 0;
    *sof_end = 0;

    for(i = 0; i < 30; i++)
    {
        atomLength = (buf[offset+2] << 8 | buf[offset+3]) + 2;
        if(markCount >= 5)break;
        if(buf[offset] != 0xff)break;

        if(buf[offset+1] == 0xc0)
        {
            *sof_start = offset; 
            *sof_end = offset + atomLength - 1;
            markCount ++;
        }
        if(buf[offset+1] == 0xdb)
        {
            // if(*sof_start == 0)return 0;

            if(*dqt_start == 0)*dqt_start = offset;

            *dqt_end = offset + atomLength - 1;
            markCount ++;
            lastAtomIsDQT = 1;
        }
        else
            lastAtomIsDQT = 0;
        
        if((*sof_start) && (*dqt_start) && (!lastAtomIsDQT))
            return 1;

        offset += atomLength;
    }
    
    if(markCount > 1)return 1;
    
    return -1;
}

//数据构成：无关数据0，start1, end 1，无关数据3，start2，end2，无关数据4, 数据交换为ABC->BCA
static uint8_t myFileSwap(uint8_t *buf, long bufLength, long start1, long end1, long start2, long end2)
{
    if(start1 > bufLength)return -1;
    if(end1 > bufLength)return -1;
    if(start2 > bufLength)return -1;
    if(end2 > bufLength)return -1;
    
    uint16_t len1 = end1 - start1 + 1;
    uint16_t len2 = end2 - start2 + 1;
    uint8_t *tempBuf = pvPortMalloc(len1 + 1);
    if(tempBuf == NULL)return -1;

    memset(tempBuf, 0, len1 + 1);

    memcpy(tempBuf, buf + start1, len1);
    memcpy(buf + start1, buf + end1 + 1, end2 - end1);

    memcpy(buf + end2 - len1 + 1, tempBuf, len1);
 
    vPortFree(tempBuf);
    return 1;
}

// 用于读取内存中JPEG文件的图像尺寸和色彩空间
void read_jpeg_info_from_memory(const uint8_t* data, uint32_t Sof0offset, uint16_t* width, uint16_t* height, JPEGYuvModeEnum* yuvMode)
{
    Sof0offset += 5;
    *height = (data[Sof0offset + 0] << 8) | data[Sof0offset + 1];
    *width = (data[Sof0offset + 2] << 8) | data[Sof0offset + 3];
    
    if (data[Sof0offset + 4] == 3 && 
        data[Sof0offset + 6] == 0x11 && 
        data[Sof0offset + 9] == 0x11 &&
        data[Sof0offset + 12] == 0x11)
    {
        *yuvMode = JPEGYuv444With8x8; // YUV444
    }
    else if (data[Sof0offset + 4] == 3 && 
        data[Sof0offset + 6] == 0x21 && 
        data[Sof0offset + 9] == 0x11 &&
        data[Sof0offset + 12] == 0x11)
    {
        *yuvMode = JPEGYuv422With8x8; // YUV422
    }
    else if (data[Sof0offset + 4] == 3 && 
        data[Sof0offset + 6] == 0x22 && 
        data[Sof0offset + 9] == 0x11 &&
        data[Sof0offset + 12] == 0x11)
    {
        *yuvMode = JPEGYuv420With8x8; // YUV420
    }
    else
    {
        *yuvMode = JPEGYuvModeUnkmow; // 未知
    }
}

int8_t checkJPEGhead(uint8_t *buf, long length, JPEGImageInfoStruct* jpegInfo)
{
    // 查找 DQT 和 SOF 的位置与长度
    long dqtStartAddress = 0, dqtEndAddress = 0;
    long sofStartAddress = 0, sofEndAddress = 0;

    int ret = findDqtAndSof(buf, length, &dqtStartAddress, &dqtEndAddress, &sofStartAddress, &sofEndAddress);
    
    if (ret == -1)
    {
        DBG_OUT("sof or dqt can not find.\n");
        return -1;
    }
    
    DBG_OUT("dqtStartAddress:0x%x, dqtEndAddress:0x%x, sofStartAddress:0x%x, sofEndAddress:0x%x\n", dqtStartAddress, dqtEndAddress, sofStartAddress, sofEndAddress);
    
    // 从sof0段中获取jpeg信息
    read_jpeg_info_from_memory(buf, sofStartAddress, &jpegInfo->Width, &jpegInfo->Height, &jpegInfo->YuvMode);
    
    if (jpegInfo->YuvMode == JPEGYuv444With8x8)
    {
        jpegInfo->YuvOutSize = ((jpegInfo->Width+7)&(~0x07))*((jpegInfo->Height+7)&(~0x07))*3;
    }
    else if (jpegInfo->YuvMode == JPEGYuv422With8x8)
    {
        jpegInfo->YuvOutSize = ((jpegInfo->Width+15)&(~0x0f))*((jpegInfo->Height+7)&(~0x07))*2;
    }
    else if (jpegInfo->YuvMode == JPEGYuv420With8x8)
    {
        jpegInfo->YuvOutSize = ((jpegInfo->Width+15)&(~0x0f))*((jpegInfo->Height+15)&(~0x0f))*3/2;
    }
    else
    {
        return -1;
    }
    
    if(sofStartAddress < dqtStartAddress)
    {
        myFileSwap(buf, length, sofStartAddress, sofEndAddress, dqtStartAddress, dqtEndAddress);
        return 1;
    }
    else
    {
        return 0;
    }
}



static void deleteTask(JPEGPortStruct* self, JPEGTaskStruct* task);

static bool runTask(JPEGPortStruct* self, JPEGTaskStruct* task) {
    JPEGD_TypeDef* jpegd = self->JPEGHandler;
    DMA_Stream_TypeDef* jpegIn = self->JPEGIn;
    DMA_Stream_TypeDef* jpegOut = self->JPEGOut;

    bool isRunning = false;
    if (task->Type == JPEGDecode) {
        jpegIn->M0AR = (uint32_t)task->Source;
        jpegIn->NDTR = task->SourceSize;
        jpegOut->M0AR = (uint32_t)task->Target;
        jpegOut->NDTR = task->TargetSize;
        
        DMA_Cmd(self->JPEGIn, DISABLE);
        DMA_Cmd(self->JPEGOut, DISABLE);
        
        DMA_ClearFlag(self->JPEGIn, DMA_FLAG_TCIF);
        DMA_ClearFlag(self->JPEGIn, DMA_FLAG_HTIF);
        DMA_ClearFlag(self->JPEGIn, DMA_FLAG_TEIF);
        DMA_ClearFlag(self->JPEGIn, DMA_FLAG_FEIF);
        DMA_ClearFlag(self->JPEGOut, DMA_FLAG_TCIF);
        DMA_ClearFlag(self->JPEGOut, DMA_FLAG_HTIF);
        DMA_ClearFlag(self->JPEGOut, DMA_FLAG_TEIF);
        DMA_ClearFlag(self->JPEGOut, DMA_FLAG_FEIF);
        
        JPEG_Cmd(ENABLE);
        DMA_Cmd(DMA2_Stream0, ENABLE);
        DMA_Cmd(DMA2_Stream5, ENABLE);
        
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
static SemaphoreHandle_t JPEGOutSemaphore;
static SemaphoreHandle_t JPEGHandlerSemaphore;

static JPEGTaskStruct* createTask(JPEGPortStruct* self) {
    return pvPortMalloc(sizeof(JPEGTaskStruct));
}

static void deleteTask(JPEGPortStruct* self, JPEGTaskStruct* task) {
    vPortFree(task);
}

static bool startTask(JPEGPortStruct* self, JPEGTaskStruct* task) {
    return xQueueSend(self->TaskQueue, &task, portMAX_DELAY) == pdPASS;
}

static void DispatcherTask(JPEGPortStruct* self) {
    JPEGTaskStruct* task = NULL;
    while (xQueueReceive(self->TaskQueue, &task, portMAX_DELAY) == pdPASS) {
        // exit task
        if (task == NULL)
            break;

        if (uxSemaphoreGetCount(JPEGOutSemaphore)) {
            if (xSemaphoreTake(JPEGOutSemaphore, pdMS_TO_TICKS(CONFIG_JPEG_TIMEOUT)) != pdPASS) {
                __BKPT();
            }
        }
        
        if (uxSemaphoreGetCount(JPEGHandlerSemaphore)) {
            if (xSemaphoreTake(JPEGHandlerSemaphore, pdMS_TO_TICKS(CONFIG_JPEG_TIMEOUT)) != pdPASS) {
                __BKPT();
            }
        }
        
        JPEGD_TypeDef* jpegd = self->JPEGHandler;
        DMA_Stream_TypeDef* jpegIn = self->JPEGIn;
        DMA_Stream_TypeDef* jpegOut = self->JPEGOut;
        
        if (runTask(self, task))
        {
            if (xSemaphoreTake(JPEGHandlerSemaphore, pdMS_TO_TICKS(CONFIG_JPEG_TIMEOUT)) != pdPASS) {
                __BKPT();
            }
            
            if (xSemaphoreTake(JPEGOutSemaphore, pdMS_TO_TICKS(CONFIG_JPEG_TIMEOUT)) != pdPASS) {
                __BKPT();
            }
        }
        if (task->EndCallback)
            task->EndCallback(task);

        deleteTask(self, task);
    }

    vTaskDelete(NULL);
}

void DMA2_Stream0_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF) == SET)
    {
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF);
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF);
        
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(JPEGOutSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void JPEGD_IRQHandler(void)
{
    if (JPEG_GetITStatus(JPEG_IT_EOC) == SET)
    {
        JPEG_ClearITPendingBit(JPEG_IT_EOC);
        
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(JPEGHandlerSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
#else


#endif

static void jpegDMAInit(JPEGPortStruct* self)
{
    DMA_InitTypeDef DMA_InitStruct;
    
    // Stream5 channel0  IN
    if (self->JPEGIn == DMA2_Stream5 )
    {
        DMA_InitStruct.DMA_Channel = DMA_Channel_0;
    }
    
    DMA_InitStruct.DMA_BufferSize = 0;
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStruct.DMA_Memory0BaseAddr = 0;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&JPEGD->DIR;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_INC16;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_Init(self->JPEGIn, &DMA_InitStruct);
    
    // Stream0 channel7  OUT
    if (self->JPEGOut == DMA2_Stream0 )
    {
        DMA_InitStruct.DMA_Channel = DMA_Channel_7;
    }
    DMA_InitStruct.DMA_BufferSize = 0;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStruct.DMA_Memory0BaseAddr = 0;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&JPEGD->DOR;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_INC16;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_Init(self->JPEGOut, &DMA_InitStruct);
    
    JPEG_SetInFifoThreshold(16);
    JPEG_SetOutFifoThreshold(16);
    
    DMA_ITConfig(self->JPEGOut, DMA_IT_TC, ENABLE);
    
    JPEG_ITConfig(JPEG_IT_EOC, ENABLE);
}



static bool Init(JPEGPortStruct* self, bool isInit) {
    if (isInit) {
        PeripheralEnable(PeripheralDMA2, true);
        PeripheralEnable(self->JPEGHandlerPeripheral, true);
        
        PeripheralReset(self->JPEGHandlerPeripheral);
        
#if USE_RTOS_FREERTOS
        self->TaskQueue = xQueueCreate(CONFIG_JPEG_MAX_TASK_QUEUE_LENGTH, sizeof(JPEGTaskStruct*));

        if (self->TaskQueue == NULL)
            return false;

        if (xTaskCreate((TaskFunction_t)DispatcherTask, "JPEG", RTOSStackSizeKB(CONFIG_JPEG_TASK_STACK_SIZE_KB), self, GONFIG_JPEG_TASK_PRIORITY,
                        &self->TaskHandle) != pdPASS)
            return false;

        JPEGOutSemaphore = xSemaphoreCreateBinary();
        if (JPEGOutSemaphore == NULL)
        {
            return false;
        }
        
        JPEGHandlerSemaphore = xSemaphoreCreateBinary();
        if (JPEGHandlerSemaphore == NULL)
        {
            return false;
        }
#else
        
#endif
        jpegDMAInit(self);
        
        NVIC_SetPriority(JPEGD_IRQn, GONFIG_JPEG_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(JPEGD_IRQn);
        
        if (self->JPEGOut == DMA2_Stream0)
        {
            NVIC_SetPriority(DMA2_Stream0_IRQn, GONFIG_JPEG_STEAM_INTERRUPT_PRIORITY);
            NVIC_EnableIRQ(DMA2_Stream0_IRQn);
        }
    }
    else {
#if USE_RTOS_FREERTOS
        xQueueSend(self->TaskQueue, NULL, portMAX_DELAY);
        vQueueDelete(self->TaskQueue);
        vSemaphoreDelete(JPEGHandlerSemaphore);
        vSemaphoreDelete(JPEGOutSemaphore);
        
        NVIC_DisableIRQ(JPEGD_IRQn);
        
        if (self->JPEGOut == DMA2_Stream0)
        {
            NVIC_DisableIRQ(DMA2_Stream0_IRQn);
        }
#else

#endif
    }

    return true;
}

static bool AddDecodeTask(      //
    JPEGPortStruct*  self,     //
    void*        source,       //
    uint32_t     sourceSize,    //
    void*        target,       //
    uint32_t     targetSize,
    JPEGTaskCallbackType endCallback, //
    void*               userData     //
) {
    JPEGTaskStruct* task = createTask(self);

    if (!task)
        return false;

    task->Type = JPEGDecode;

    task->Source       = source;
    task->SourceSize   = sourceSize;
    task->Target       = target;
    task->TargetSize   = targetSize;
    task->EndCallback = endCallback;
    task->UserData    = userData;

    return startTask(self, task);
}

static bool AddEventTask(                   //
    JPEGPortStruct*      self,        //
    JPEGTaskCallbackType endCallback, //
    void*               userData     //
) {
    JPEGTaskStruct* task = createTask(self);

    if (!task)
        return false;

    task->Type = JPEGTaskEvent;

    task->EndCallback = endCallback;
    task->UserData    = userData;

    return startTask(self, task);
}

bool JPEGPortConstractor(JPEGPortStruct* self, 
                    DMA_Stream_TypeDef* inPeripheral, 
                    DMA_Stream_TypeDef* outPeripheral,
                    PeripheralEnum handlerPeripheral
) {
    memset(self, 0, sizeof(JPEGPortStruct));

    self->Init         = Init;
    self->AddDecodeTask  = AddDecodeTask;
    self->AddEventTask = AddEventTask;

    self->JPEGIn = inPeripheral;
    self->JPEGOut = outPeripheral;
    self->JPEGHandlerPeripheral = handlerPeripheral;

    self->JPEGHandlerBase = PeripheralMap[handlerPeripheral].Base;

    return true;
}
