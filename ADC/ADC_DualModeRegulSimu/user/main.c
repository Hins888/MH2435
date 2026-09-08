#include "usart.h"

void ADC_Config(void);
void NVIC_Configuration(void);
void DAC_DMA_Config(void);
void ADC_GPIO_Config(void);
uint8_t GetCmd(void);
void ADCShowData(void);

uint16_t aADCDualConvertedValue[4];

#define ADCx                     ADC1

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
    printf("ADC1_ch0(pa0) data = %d, voltage = %f mV\n", aADCDualConvertedValue[0], (float)aADCDualConvertedValue[0] * 3300 / 4096);
    printf("ADC2_ch3(pa3) data = %d, voltage = %f mV\n", aADCDualConvertedValue[1], (float)aADCDualConvertedValue[1] * 3300 / 4096);
    printf("ADC1_ch3(pa3) data = %d, voltage = %f mV\n", aADCDualConvertedValue[2], (float)aADCDualConvertedValue[2] * 3300 / 4096);
    printf("ADC2_ch0(pa0) data = %d, voltage = %f mV\n", aADCDualConvertedValue[3], (float)aADCDualConvertedValue[3] * 3300 / 4096);
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
  * @brief  ADC1 regular channels 0 and 1 configuration
  * @param  None
  * @retval None
  */
static void ADC1_CH0_CH1_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 2;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channels 0, 1 configuration */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_3Cycles);
}

/**
  * @brief  ADC2 regular channels 1, 0 configuration
  * @param  None
  * @retval None
  */
static void ADC2_CH1_CH0_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 2;
    ADC_Init(ADC2, &ADC_InitStructure);

    /* ADC2 regular channels 1, 0 configuration */ 
    ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 1, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 2, ADC_SampleTime_3Cycles);
}

/**
  * @brief  DMA Configuration
  * @param  None
  * @retval None
  */
static void DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_InitStructure.DMA_Channel = DMA_Channel_0; 
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&aADCDualConvertedValue;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC->CDR;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);

    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
    /* DMA2_Stream0 enable */
    DMA_Cmd(DMA2_Stream0, ENABLE);
}

/**
  * @brief Configure ADC Channels 0, 1 pins as analog inputs
  * @param  None
  * @retval None
  */
static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* ADC Channel 0 -> PC0
       ADC Channel 3 -> PC3
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  ADC configuration
  * @note   This function Configure the ADC peripheral  
            1) Enable peripheral clocks
            2) Configure ADC Channel 0 and 1 pin as analog input
            3) DMA2_Stream0 channel0 configuration
            4) Configure ADC1 Channel 0 and Channel 1
            5) Configure ADC2 Channel 1 and Channel 0
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
      ADC_CommonInitTypeDef ADC_CommonInitStructure;
      
      /* Enable peripheral clocks *************************************************/
      PeripheralEnable(PeripheralDMA2, true);
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
        
      /* DMA2 Stream0 channel0 configuration **************************************/
      DMA_Config();
      
      /* ADCs configuration ------------------------------------------------------*/
      /* Configure ADC Channel0, 1 pin as analog input */
      GPIO_Config();

      /* ADC Common Init */
      ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;
      ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
      ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
      ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
      ADC_CommonInit(&ADC_CommonInitStructure);

      /* ADC1 regular channels 0, 1 configuration */
      ADC1_CH0_CH1_Config();

      /* ADC2 regular channels 1, 0 configuration */
      ADC2_CH1_CH0_Config();

      /* Enable DMA request after last transfer (Multi-ADC mode) */
      ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

      /* Enable ADC1 */
      ADC_Cmd(ADC1, ENABLE);

      /* Enable ADC2 */
      ADC_Cmd(ADC2, ENABLE);
}  
