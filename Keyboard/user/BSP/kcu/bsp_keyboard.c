#include "./kcu/bsp_keyboard.h"


volatile uint8_t key_code = 0;
#ifdef MH2435_QFN88
GPIO_TypeDef *row_ports[KBD_ROWS] = {GPIOB, GPIOC};	
uint16_t row_pins[KBD_ROWS] = {GPIO_Pin_14, GPIO_Pin_6};

GPIO_TypeDef *col_ports[KBD_COLS] = {GPIOC, GPIOA};
uint16_t col_pins[KBD_COLS] = {GPIO_Pin_7, GPIO_Pin_8};
#else

GPIO_TypeDef *row_ports[KBD_ROWS] = {GPIOC, GPIOC};	
uint16_t row_pins[KBD_ROWS] = {GPIO_Pin_6, GPIO_Pin_7};

GPIO_TypeDef *col_ports[KBD_COLS] = {GPIOC, GPIOC};
uint16_t col_pins[KBD_COLS] = {GPIO_Pin_8, GPIO_Pin_9};
#endif



void KBD_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

#ifdef MH2435_QFN88	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
#endif
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    for (int i = 0; i < KBD_ROWS; i++)
	{
        GPIO_InitStruct.GPIO_Pin = row_pins[i];
        GPIO_Init(row_ports[i], &GPIO_InitStruct);
    }

    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;	
    for (int i = 0; i < KBD_COLS; i++)
	{
        GPIO_InitStruct.GPIO_Pin = col_pins[i];
        GPIO_Init(col_ports[i], &GPIO_InitStruct);
		GPIO_ResetBits(col_ports[i],col_pins[i]);
    }
}

void KBD_NvicConfig(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
#ifdef MH2435_QFN88	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

void KBD_ExtiConfig(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	
	KBD_Config();
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
#ifdef MH2435_QFN88	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6 | EXTI_Line14;
#else
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource7);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6 | EXTI_Line7;
#endif
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}
