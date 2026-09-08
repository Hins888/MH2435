#include "jpegdecode.h"
#include "bsp_rgb_lcd.h"
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
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_INC4;//DMA_MemoryBurst_Single;//DMA_MemoryBurst_INC4;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//DMA_MemoryDataSize_Byte;//DMA_MemoryDataSize_HalfWord;
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
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_INC4;//DMA_MemoryBurst_Single;//DMA_MemoryBurst_INC4;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//DMA_MemoryDataSize_Byte;//DMA_MemoryDataSize_Word;
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
	
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;

	/* 配置中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
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
    DMA2D_SetLineOffset(DMA2D, 0);
	
	if(DisplayColorRGB888 == DPI_DISPLAY_COLOR)
	{
		DMA2D_SetOutputColorMode(DMA2D, DMA2D_OUTPUT_MODE_RGB888);
	}
	else
	{
		DMA2D_SetOutputColorMode(DMA2D, DMA2D_OUTPUT_MODE_RGB565);
	}
	
    DMA2D_SetLineOffsetMode(DMA2D, DMA2D_LINE_OFFSET_PIXELS);
    DMA2D_SetOutputRotationMode(DMA2D, DMA2D_ROTATION_0);
	
	
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 配置中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2D_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    

}

void JpegDecodeYuv_interrupt(uint8_t* jpegSrc, uint32_t jpegSize, uint8_t* yuv)
{
    DMA2_Stream5->M0AR = (uint32_t)jpegSrc;
    DMA2_Stream5->NDTR = jpegSize;
    DMA2_Stream0->M0AR = (uint32_t)yuv;
    DMA2_Stream0->NDTR = 0;

    JPEG_Cmd(ENABLE);
    DMA_Cmd(DMA2_Stream0, ENABLE);
    DMA_Cmd(DMA2_Stream5, ENABLE);
}


void Yuv2Rgb(uint8_t* yuv, uint8_t*rgb)
{
    DMA2D_FGND_SetMemAddr(DMA2D, (uint32_t)yuv);
    DMA2D_SetOutputMemAddr(DMA2D, (uint32_t)rgb);
    DMA2D_EnableIT_TC(DMA2D);
    DMA2D_Start(DMA2D);
}
extern void JpegDecodeYuv_IRQHandler(void);
void DMA2_Stream0_IRQHandler(void)
{
	JpegDecodeYuv_IRQHandler();
}
extern void custom_DMA2D_IRQHandler(void);
void DMA2D_IRQHandler(void)
{
	custom_DMA2D_IRQHandler();
}
