#include "usart.h"

static void ADC_Config(void);
void NVIC_Configuration(void);
uint8_t GetCmd(void);

uint16_t uhADCxConvertedValue = 0;

#define ADCx                     ADC3
#define ADC_CHANNEL              ADC_Channel_0
#define ADCx_PERIPH              PeripheralADC3
#define ADCx_CHANNEL_GPIO_PERIPH PeripheralGPIOA
#define GPIO_PIN                 GPIO_Pin_0
#define GPIO_PORT                GPIOA
#define DMA_CHANNELx             DMA_Channel_2
#define DMA_STREAMx              DMA2_Stream0

int main(void)
{
    RCC_ClocksTypeDef RCC_ClocksStatus;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    
    USART_Configuration(115200);
    RCC_GetClocksFreq(&RCC_ClocksStatus);
	
    printf("****ADC Demo****\n");

    NVIC_Configuration();
    
    ADC_Config();
    ADC_SoftwareStartConv(ADCx);
    
    while (1);
}

uint8_t GetCmd(void)
{
    if (SET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
        return USART_ReceiveData(USART1);
    return 0;
}

void DMA2_Stream0_IRQHandler(void)
{
    printf("ADC data = %d, voltage = %f mV\n", uhADCxConvertedValue, (float)uhADCxConvertedValue * 3300 / 4096);
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF);
    NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  ADC3 channel04 with DMA configuration
  * @note   This function Configure the ADC peripheral  
            1) Enable peripheral clocks
            2) DMA2_Stream0 channel2 configuration
            3) Configure ADC Channel4 pin as analog input
            4) Configure ADC3 Channel4 
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;

    /* Enable ADCx, DMA and GPIO clocks ****************************************/ 
    PeripheralEnable(PeripheralDMA2, true);
    PeripheralEnable(ADCx_CHANNEL_GPIO_PERIPH, true);  
    PeripheralEnable(ADCx_PERIPH, true);

    /* DMA2 Stream0 channel2 configuration **************************************/
    DMA_InitStructure.DMA_Channel = DMA_CHANNELx;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&uhADCxConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA_STREAMx, &DMA_InitStructure);

    DMA_ITConfig(DMA_STREAMx, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA_STREAMx, ENABLE);

    /* Configure ADC3 Channel4 pin as analog input ******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

    /* ADC Common Init **********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC3 Init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADCx, &ADC_InitStructure);

    /* ADC3 regular channel4 configuration **************************************/
    ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_3Cycles);

    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADCx, ENABLE);

    /* Enable ADC3 DMA */
    ADC_DMACmd(ADCx, ENABLE);

    /* Enable ADC3 */
    ADC_Cmd(ADCx, ENABLE);
}
