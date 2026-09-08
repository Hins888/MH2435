#include "app.h"
#include "usbh_video_core.h"
#include "jpeg_check.h"
#include "SDRAMBSP.h"
#include "DisplayBSP.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST USB_Host               __ALIGN_END;

#define DISPLAY_DATA_BUFFER0         (SDRAM_MEM_BASE)
#define DISPLAY_DATA_BUFFER_SIZE0    (800 * 600 * 2)
#define DISPLAY_DATA_BUFFER1         (DISPLAY_DATA_BUFFER0 + DISPLAY_DATA_BUFFER_SIZE0)
#define DISPLAY_DATA_BUFFER_SIZE1    (800 * 600 * 2)

#define JPEG_OUTPUT_DATA_BUFFER0 (DISPLAY_DATA_BUFFER1 + DISPLAY_DATA_BUFFER_SIZE1)
#define JPEG_OUTPUT_DATA_BUFFER1 (JPEG_OUTPUT_DATA_BUFFER0 + UVC_TARGET_WIDTH*UVC_TARGET_HEIGHT*2)

typedef enum{
    DISPLAY_IDLE = 0,
    DISPLAY_RUN
}DISPLAY_STATE_E;

#if USE_UVC_MJPEG
uint32_t uvc_data_buf0[(UVC_MAX_FRAME_SIZE + 3) >> 2];
uint32_t uvc_data_buf1[(UVC_MAX_FRAME_SIZE + 3) >> 2];
#else
uint8_t*  uvc_data_buf0 = (uint8_t*)JPEG_OUTPUT_DATA_BUFFER0;
uint8_t*  uvc_data_buf1 = (uint8_t*)JPEG_OUTPUT_DATA_BUFFER1;
#endif
uint8_t usb_connect_flag = false;

void JpegInit(void);
void JpegDecodeYuv(uint8_t* usb_connect_st, uint8_t* jpegSrc, uint32_t jpegSize, uint8_t* yuv, JPEG_InfoTypeDef *jpegInfo);
void Yuv2RgbInit(JPEG_InfoTypeDef *jpegInfo);
void Yuv2Rgb(uint8_t* yuv, uint8_t*rgb);
void USBH_TIMConfig(uint32_t time_us);
void DisplayProcess(uint8_t* usb_connect_st,uvc_data_struct* uvc_data);

DisplayStruct*      display = NULL;
DisplayLayerStruct* layer   = NULL;

int main(void) {
    uint32_t dislpay_st = DISPLAY_IDLE;
    
    RetargetIOSetup(CONFIG_RETARGETIO_DEFAULT_SERIAL);
    
    SDRAMSetup();
    
    printf("LegacyHost UVC Demo\n");

    // Display Test Pin
    IOSetup(PE15, IO_DEFAULT_OUTPUT_CONFIG);    
    
    // select and setup an available display interface
    display = DisplaySelectSetup((uint32_t*)DISPLAY_DATA_BUFFER0, DISPLAY_DATA_BUFFER_SIZE0);
    
    if (display) {
        DisplayTestPattern(display);
		
        display->Start(display, true);
        layer = display->Layer;
    }
    
    JpegInit();
    
    /* Init Host Library */
    USBH_Init(&USB_OTG_Core,
#ifdef USE_USB_OTG_FS
              USB_OTG_FS_CORE_ID,
#else
              USB_OTG_HS_CORE_ID,
#endif
              &USB_Host, &UVC_cb, &USR_Callbacks);
    
    uvc_stream_init((uint8_t*)uvc_data_buf0, (uint8_t*)uvc_data_buf1);
    
    #ifdef USE_USB_OTG_FS
    //for usb fs make sure less than 1 ms
    USBH_TIMConfig(800);
    #else
    //for usb hs make sure less than 125 us
    USBH_TIMConfig(80);
    #endif
    memset((uint8_t*)DISPLAY_DATA_BUFFER0,0xff,DISPLAY_DATA_BUFFER_SIZE0);
    memset((uint8_t*)DISPLAY_DATA_BUFFER1,0xff,DISPLAY_DATA_BUFFER_SIZE1);
    while (1) {
        switch(dislpay_st)
        {
            case DISPLAY_IDLE:
                if(usb_connect_flag)dislpay_st = DISPLAY_RUN;
                break;
            case DISPLAY_RUN:
                DisplayProcess(&usb_connect_flag,&uvc_data);
                if(!usb_connect_flag)
                {
                    uvc_stream_init((uint8_t*)uvc_data_buf0, (uint8_t*)uvc_data_buf1);
                    dislpay_st = DISPLAY_IDLE;
                }
                break;
        }
    }
}

void DisplayChangedCallback(DisplayLayerStruct* layer)
{
    while (layer->NextBuffer) {
        __NOP();
    }
}

void DisplayProcess(uint8_t* usb_connect_st,uvc_data_struct* uvc_data)
{
    static uint32_t jpegOutDataAdreess = JPEG_OUTPUT_DATA_BUFFER0;
    static uint32_t displayDataAddress = DISPLAY_DATA_BUFFER0;
    static JPEG_InfoTypeDef JpegInfo;
    static uint32_t isfirstFrame =1;
    static uint32_t dislpay_st = DISPLAY_IDLE;
    
    if (uvc_data->ready_frame_flag)
    {
        IOH(PE15);
        #if 0
        printf("Frame num %d, %d bytes :\n",uvc_data->frame_cnt,uvc_data->ready_frame_len);
        for(long i = 0;i < 64;i++)
        {
            printf("0x%02x ",uvc_data->ready_buffer[i]);
            if((i+1)%32 == 0)printf("\n");
        }
        printf("......\n");
        for(long i = uvc_data->ready_frame_len - 4;i < uvc_data->ready_frame_len;i++)
        {
            printf("0x%02x ",uvc_data->ready_buffer[i]);
        }
        printf("\n");
        #endif
        
        
        #if USE_UVC_MJPEG
        if(!jpeg_check_start(uvc_data->ready_buffer,uvc_data->ready_frame_len))
        {
            printf("err frame: no jpeg start info\n");
            uvc_data->ready_frame_flag = false;
            return;
        }
        
        if(!jpeg_check_end(uvc_data->ready_buffer,uvc_data->ready_frame_len))
        {
            printf("err frame: no jpeg end info\n");
            uvc_data->ready_frame_flag = false;
            return;
        }
        if(!jpeg_check_head_swap(uvc_data->ready_buffer,uvc_data->ready_frame_len))
        {
            printf("err frame: no jpeg head info\n");
            uvc_data->ready_frame_flag = false;
            return;
        }
        JpegDecodeYuv(usb_connect_st, uvc_data->ready_buffer, uvc_data->ready_frame_len, (uint8_t*)jpegOutDataAdreess, &JpegInfo);
        if(isfirstFrame == 1)
        {
            isfirstFrame = 0;
            Yuv2RgbInit(&JpegInfo);
        }
        Yuv2Rgb((uint8_t *)jpegOutDataAdreess, (uint8_t*)displayDataAddress);
        #else
        if(isfirstFrame == 1)
        {
            isfirstFrame = 0;
            JpegInfo.ChromaSubsampling = JPEG_422_SUBSAMPLING;
            JpegInfo.ImageWidth = UVC_TARGET_WIDTH;
            JpegInfo.ImageHeight = UVC_TARGET_HEIGHT;
            Yuv2RgbInit(&JpegInfo);
        }
        Yuv2Rgb((uint8_t *)uvc_data.ready_buffer, (uint8_t*)displayDataAddress);
        #endif
        IOL(PE15);
        #if USE_UVC_MJPEG
        jpegOutDataAdreess = (jpegOutDataAdreess == JPEG_OUTPUT_DATA_BUFFER0) ? JPEG_OUTPUT_DATA_BUFFER1 : JPEG_OUTPUT_DATA_BUFFER0;
        #endif
        display->Refresh(display,(uint8_t*)displayDataAddress,layer->Width,layer->Height,layer->XOffset,layer->YOffset,DisplayChangedCallback);
        
        displayDataAddress = (displayDataAddress == DISPLAY_DATA_BUFFER0) ? DISPLAY_DATA_BUFFER1 : DISPLAY_DATA_BUFFER0;
        uvc_data->ready_frame_flag = false;
        
    }
}

#ifdef USE_USB_OTG_FS
void USB1_IRQHandler(void)
#else
void USB2_IRQHandler(void)
#endif
{
    USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

void JpegInit(void)
{
    DMA_InitTypeDef DMA_InitStruct;
    
    PeripheralEnable(PeripheralJPEGD, 1);
    PeripheralEnable(PeripheralDMA2D, 1);
    PeripheralEnable(PeripheralDMA2, 1);
    
    // Stream5 channel0  IN
    DMA_InitStruct.DMA_Channel = DMA_Channel_0;
    DMA_InitStruct.DMA_BufferSize = 0;
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStruct.DMA_Memory0BaseAddr = 0;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&JPEGD->DIR;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_INC16;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_Init(DMA2_Stream5, &DMA_InitStruct);
    
    // Stream0 channel7  OUT
    DMA_InitStruct.DMA_Channel = DMA_Channel_7;
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
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_Init(DMA2_Stream0, &DMA_InitStruct);
    
    DMA_FlowControllerConfig(DMA2_Stream0, DMA_FlowCtrl_Peripheral);
    
    JPEG_SetInFifoThreshold(16);
    JPEG_SetOutFifoThreshold(16);
    
}

void JpegDecodeYuv(uint8_t* usb_connect_st, uint8_t* jpegSrc, uint32_t jpegSize, uint8_t* yuv, JPEG_InfoTypeDef *jpegInfo)
{
    DMA2_Stream5->M0AR = (uint32_t)jpegSrc;
    DMA2_Stream5->NDTR = jpegSize;
    DMA2_Stream0->M0AR = (uint32_t)yuv;
    DMA2_Stream0->NDTR = 0;
    
    JPEG_Cmd(ENABLE);
    DMA_Cmd(DMA2_Stream0, ENABLE);
    DMA_Cmd(DMA2_Stream5, ENABLE);
    
    while(DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF) == RESET)
    {
        if(*usb_connect_st == false)
        {
            DMA_Cmd(DMA2_Stream0, DISABLE);
            break;
        }
    }
    while(DMA_GetFlagStatus(DMA2_Stream5, DMA_FLAG_TCIF) == RESET)
    {
        if(*usb_connect_st == false)
        {
            DMA_Cmd(DMA2_Stream5, DISABLE);
            break;
        }
    }
    DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF);
    DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF);
    
    JPEG_GetInfo(jpegInfo);
    
    JPEG_Cmd(DISABLE);
}

void Yuv2RgbInit(JPEG_InfoTypeDef *jpegInfo)
{
    DMA2D_SetMode(DMA2D, DMA2D_MODE_M2M_PFC);
    DMA2D_FGND_SetColorMode(DMA2D, DMA2D_INPUT_MODE_YCBCR);
    
    if (jpegInfo->ChromaSubsampling == JPEG_420_SUBSAMPLING)
    {
        DMA2D_FGND_SetChrSubSampling(DMA2D, DMA2D_CSS_420);
    }
    else if (jpegInfo->ChromaSubsampling == JPEG_422_SUBSAMPLING)
    {
        DMA2D_FGND_SetChrSubSampling(DMA2D, DMA2D_CSS_422);
    }
    else if (jpegInfo->ChromaSubsampling == JPEG_444_SUBSAMPLING)
    {
        DMA2D_FGND_SetChrSubSampling(DMA2D, DMA2D_CSS_444);
    }
    
    if ((jpegInfo->ImageWidth & 0x07) == 0)
    {
        DMA2D_FGND_SetLineOffset(DMA2D, 0);
    }
    else 
    {
        DMA2D_FGND_SetLineOffset(DMA2D, 8-(jpegInfo->ImageWidth & 0x07));
    }
    
    DMA2D_SetNbrOfPixelsPerLines(DMA2D, jpegInfo->ImageWidth);
    DMA2D_SetNbrOfLines(DMA2D, jpegInfo->ImageHeight);
    
    if(jpegInfo->ImageWidth <= layer->Width)
    {
        DMA2D_SetLineOffset(DMA2D, layer->Width - jpegInfo->ImageWidth);
    }
    else
    {
        DMA2D_SetLineOffset(DMA2D, 0);
    }
    DMA2D_SetOutputColorMode(DMA2D, DMA2D_OUTPUT_MODE_RGB565);
    DMA2D_SetLineOffsetMode(DMA2D, DMA2D_LINE_OFFSET_PIXELS);
    DMA2D_SetOutputRotationMode(DMA2D, DMA2D_ROTATION_0);
}


void Yuv2Rgb(uint8_t* yuv, uint8_t*rgb)
{
    DMA2D_FGND_SetMemAddr(DMA2D, (uint32_t)yuv);
    DMA2D_SetOutputMemAddr(DMA2D, (uint32_t)rgb);
    
    DMA2D_Start(DMA2D);
    while(DMA2D_IsActiveFlag_TC(DMA2D) == 0);
    DMA2D_ClearFlag_TC(DMA2D);
}

void USBH_TIMConfig(uint32_t time_us)
{
    TIM_TimeBaseInitTypeDef TIM_TimBaseStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_ClocksTypeDef RCC_ClocksStatus;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_TimBaseStruct.TIM_ClockDivision = 0;
    TIM_TimBaseStruct.TIM_Period = time_us * 2;
    TIM_TimBaseStruct.TIM_Prescaler = RCC_ClocksStatus.PCLK1_Frequency / 1000000 - 1;
    TIM_TimBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &TIM_TimBaseStruct);
    
    TIM_ClearFlag(TIM7, TIM_FLAG_Update);
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM7, ENABLE);
}

void TIM7_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM7,TIM_IT_Update))
    {
        /* Host Task handler */
        USBH_Process(&USB_OTG_Core, &USB_Host);
        TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
    }
}
