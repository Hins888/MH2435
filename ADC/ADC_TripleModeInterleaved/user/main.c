#include "usart.h"

void ADC_Config(void);
void NVIC_Configuration(void);
void DAC_DMA_Config(void);
void ADC_GPIO_Config(void);
uint8_t GetCmd(void);
void ADCShowData(void);

uint16_t aADCTripleConvertedValue[3];

#define ADCx                     ADC1
#define ADC_CHANNEL              ADC_Channel_0
#define GPIO_PIN                 GPIO_Pin_0
#define GPIO_PORT                GPIOA
#define ADC1_0_CHANNEL_GPIO_CLK  RCC_AHB1Periph_GPIOA
#define DMA_CHANNELx             DMA_Channel_0
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
    printf("ADC1 data = %d, voltage = %f mV\n", aADCTripleConvertedValue[0], (float)aADCTripleConvertedValue[0] * 3300 / 4096);
    printf("ADC2 data = %d, voltage = %f mV\n", aADCTripleConvertedValue[1], (float)aADCTripleConvertedValue[1] * 3300 / 4096);
    printf("ADC3 data = %d, voltage = %f mV\n", aADCTripleConvertedValue[2], (float)aADCTripleConvertedValue[2] * 3300 / 4096);
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
  * @brief  ADC configuration
  * @note   This function Configure the ADC peripheral  
            1) Enable peripheral clocks
            2) Configure ADC Channel 0 pin as analog input
            3) DMA2_Stream0 channel0 configuration
            4) Configure ADC1 Channel 0
            5) Configure ADC2 Channel 0
            6) Configure ADC3 Channel 0
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
	DMA_InitTypeDef        DMA_InitStructure;
	ADC_InitTypeDef        ADC_InitStructure;
	ADC_CommonInitTypeDef  ADC_CommonInitStructure;  

	/* Enable peripheral clocks *************************************************/
	RCC_AHB1PeriphClockCmd( ADC1_0_CHANNEL_GPIO_CLK , ENABLE);
	PeripheralEnable( PeripheralDMA2 , ENABLE);
	PeripheralEnable( PeripheralADC1 , ENABLE);
	PeripheralEnable( PeripheralADC2 , ENABLE);
	PeripheralEnable( PeripheralADC3 , ENABLE);  

	/* Configure ADC Channel 0 pin as analog input *****************************/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

	/* DMA2 Stream0 channel0 configuration **************************************/
	DMA_InitStructure.DMA_Channel = DMA_CHANNELx;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC->CDR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&aADCTripleConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 3;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_ITConfig(DMA_STREAMx, DMA_IT_TC, ENABLE);
	DMA_Init(DMA_STREAMx, &DMA_InitStructure);

	/* DMA2_Stream0 enable */
	DMA_Cmd(DMA_STREAMx, ENABLE);

	/* ADC Common configuration *************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_TripleMode_Interl;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_2;  
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; 
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 regular channel 0 configuration ************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_CHANNEL, 1, ADC_SampleTime_3Cycles);
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* ADC2 regular channel 0 configuration ************************************/
	ADC_Init(ADC2, &ADC_InitStructure);
	/* ADC2 regular channel0 configuration */ 
	ADC_RegularChannelConfig(ADC2, ADC_CHANNEL, 1, ADC_SampleTime_3Cycles);

	/* ADC3 regular channel 0 configuration ************************************/
	ADC_Init(ADC3, &ADC_InitStructure); 
	/* ADC3 regular channel0 configuration */
	ADC_RegularChannelConfig(ADC3, ADC_CHANNEL, 1, ADC_SampleTime_3Cycles);

	/* Enable DMA request after last transfer (multi-ADC mode) ******************/
	ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

	/* Enable ADC1 **************************************************************/
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC2 **************************************************************/
	ADC_Cmd(ADC2, ENABLE);

	/* Enable ADC3 **************************************************************/
	ADC_Cmd(ADC3, ENABLE);
}  
