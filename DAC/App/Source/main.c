#include "RetargetIO.h"
#include "wave_data.h"

void DAC_Configuration(void);
void DAC_TIM_Config(void);
void DAC_DMA_Config(void);
void NVIC_Configuration(void);
uint8_t GetCmd(void);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    
    RetargetIOSetup(CONFIG_RETARGETIO_DEFAULT_SERIAL);

    RCC_ClocksTypeDef RCC_ClocksStatus;
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    printf("DAC Demo.\n");

    wave_DataHandle();
    NVIC_Configuration();
    DAC_Configuration();
    DAC_DMA_Config();
    DAC_TIM_Config();
    TIM_Cmd(TIM2, ENABLE);
	
    while (1)
    {
        switch (GetCmd())
        {
            case 'p':
                printf("play start.\n");
                DAC_DMA_Config();
                TIM_Cmd(TIM2, ENABLE);
            break;
        }
    }
}

uint8_t GetCmd(void)
{
    if (SET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
        return USART_ReceiveData(USART1);
    return 0;
}

void DAC_TIM_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimBaseStruct;
    RCC_ClocksTypeDef RCC_ClocksStatus;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    
    TIM_TimBaseStruct.TIM_ClockDivision = 0;
    TIM_TimBaseStruct.TIM_Period = (RCC_ClocksStatus.PCLK1_Frequency / 1000000) - 1;
    TIM_TimBaseStruct.TIM_Prescaler = 1000000 / (SAMPLE_RATE / 2);
    TIM_TimBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimBaseStruct);
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    
    TIM_Cmd(TIM2, ENABLE);
}

void DMA1_Stream5_IRQHandler(void)
{
    printf("DMA irq.\nplay end.\n");
    DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF);
    TIM_Cmd(TIM2, DISABLE);
    DAC_ClearITPendingBit(DAC_Channel_1, DAC_IT_DMAUDR);
    NVIC_ClearPendingIRQ(DMA1_Stream5_IRQn);
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void DAC_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStruct;
    PeripheralEnable(PeripheralDMA1, true);
    DMA_DeInit(DMA1_Stream5);
    
    memset(&DMA_InitStruct, 0, sizeof(DMA_InitStruct));
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStruct.DMA_Channel = DMA_Channel_7;
    
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)&wavData[0];
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    
    DMA_InitStruct.DMA_BufferSize = sizeof(wavData)/2;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMA1_Stream5, &DMA_InitStruct);
    
    DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);
    
    DMA_Cmd(DMA1_Stream5, ENABLE);
}

void DAC_Configuration(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    DAC_DeInit();
    DAC_InitTypeDef DAC_InitStruct;
    
    DAC_InitStruct.DAC_Trigger = DAC_Trigger_T2_TRGO;
    DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits8_0;
    DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStruct);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_DMACmd(DAC_Channel_1, ENABLE);
}
