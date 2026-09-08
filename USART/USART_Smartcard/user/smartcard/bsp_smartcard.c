/* Includes ------------------------------------------------------------------*/
#include "./smartcard/bsp_smartcard.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables definition and initialization ----------------------------*/
static __IO uint8_t SCData = 0;


/* Private function prototypes -----------------------------------------------*/
/* Transport Layer -----------------------------------------------------------*/


/* Physical Port Layer -------------------------------------------------------*/

static void SC_DeInit(void);
static ErrorStatus USART_ByteReceive(uint8_t *Data, uint32_t TimeOut);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Enables or disables the power to the Smartcard.
  * @param  NewState: new state of the Smartcard power supply. 
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SC_PowerCmd(FunctionalState NewState)
{
	if(NewState != DISABLE)
	{
		GPIO_ResetBits(SC_CMDVCC_GPIO_PORT, SC_CMDVCC_PIN);
	}
	else
	{
		GPIO_SetBits(SC_CMDVCC_GPIO_PORT, SC_CMDVCC_PIN);
	} 
}

/**
  * @brief  Sets or clears the Smartcard reset pin.
  * @param  ResetState: this parameter specifies the state of the Smartcard 
  *         reset pin. BitVal must be one of the BitAction enum values:
  *                 @arg Bit_RESET: to clear the port pin.
  *                 @arg Bit_SET: to set the port pin.
  * @retval None
  */
void SC_Reset(BitAction ResetState)
{
	GPIO_WriteBit(SC_RESET_GPIO_PORT, SC_RESET_PIN, ResetState);
}


/**
  * @brief  Manages the Smartcard transport layer: send APDU commands and receives
  *   the APDU responce.
  * @param  SC_ADPU: pointer to a SC_ADPU_Commands structure which will be initialized.  
  * @param  SC_Response: pointer to a SC_ADPU_Responce structure which will be initialized.
  * @retval None
  */
 void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_ResponceStatus)
{
  uint32_t i = 0;
  uint8_t locData = 0;
  /* Reset responce buffer ---------------------------------------------------*/
  for(i = 0; i < LC_MAX; i++)
  {
    SC_ResponceStatus->Data[i] = 0;
  }
  
  SC_ResponceStatus->SW1 = 0;
  SC_ResponceStatus->SW2 = 0;

  /* Enable the DMA Receive (Set DMAR bit only) to enable interrupt generation
     in case of a framing error FE */  
  USART_DMACmd(SC_USART, USART_DMAReq_Rx, ENABLE);

  /* Send header -------------------------------------------------------------*/
  SCData = SC_ADPU->Header.CLA;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  }  
  
  SCData = SC_ADPU->Header.INS;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  }
   
  SCData = SC_ADPU->Header.P1;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  } 
  
  SCData = SC_ADPU->Header.P2;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  }   

  /* Send body length to/from SC ---------------------------------------------*/
  if(SC_ADPU->Body.LC)
  {
    SCData = SC_ADPU->Body.LC;
    USART_SendData(SC_USART, SCData);
    while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
    {
    }     
  }
  else if(SC_ADPU->Body.LE)
  { 
    SCData = SC_ADPU->Body.LE;
    USART_SendData(SC_USART, SCData);
    while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
    {
    }     
  }
  /* Flush the SC_USART DR */
  (void)USART_ReceiveData(SC_USART);

  /* --------------------------------------------------------
    Wait Procedure byte from card:
    1 - ACK
    2 - NULL
    3 - SW1; SW2
   -------------------------------------------------------- */

  if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
  {
    if(((locData & (uint8_t)0xF0) == 0x60) || ((locData & (uint8_t)0xF0) == 0x90))
    {
      /* SW1 received */
      SC_ResponceStatus->SW1 = locData;

      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        /* SW2 received */
        SC_ResponceStatus->SW2 = locData;
      }
    }
    else if (((locData & (uint8_t)0xFE) == (((uint8_t)~(SC_ADPU->Header.INS)) & \
             (uint8_t)0xFE))||((locData & (uint8_t)0xFE) == (SC_ADPU->Header.INS & (uint8_t)0xFE)))
    {
      SC_ResponceStatus->Data[0] = locData;/* ACK received */
    }
  }

  /* If no status bytes received ---------------------------------------------*/
  if(SC_ResponceStatus->SW1 == 0x00)
  {
    /* Send body data to SC--------------------------------------------------*/
    if (SC_ADPU->Body.LC)
    {
      for(i = 0; i < SC_ADPU->Body.LC; i++)
      {
        SCData = SC_ADPU->Body.Data[i];
        
        USART_SendData(SC_USART, SCData);
        while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
        {
        } 
      }
      /* Flush the SC_USART DR */
      (void)USART_ReceiveData(SC_USART);
      /* Disable the DMA Receive (Reset DMAR bit only) */  
      USART_DMACmd(SC_USART, USART_DMAReq_Rx, DISABLE);
    }

    /* Or receive body data from SC ------------------------------------------*/
    else if (SC_ADPU->Body.LE)
    {
      for(i = 0; i < SC_ADPU->Body.LE; i++)
      {
        if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
        {
          SC_ResponceStatus->Data[i] = locData;
        }
      }
    }
    /* Wait SW1 --------------------------------------------------------------*/
    i = 0;
    while(i < 10)
    {
      if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
      {
        SC_ResponceStatus->SW1 = locData;
        i = 11;
      }
      else
      {
        i++;
      }
    }
    /* Wait SW2 ------------------------------------------------------------*/   
    i = 0;
    while(i < 10)
    {
      if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
      {
        SC_ResponceStatus->SW2 = locData;
        i = 11;
      }
      else
      {
        i++;
      }
    }
  }
}
/**
  * @brief  Resends the byte that failed to be received (by the Smartcard) correctly.
  * @param  None
  * @retval None
  */
void SC_ParityErrorHandler(void)
{
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  } 
}
/**
  * @brief  Receives a new data while the time out not elapsed.
  * @param  None
  * @retval An ErrorStatus enumuration value:
  *          - SUCCESS: New data has been received
  *          - ERROR: time out was elapsed and no further data is received
  */
 ErrorStatus USART_ByteReceive(uint8_t *Data, uint32_t TimeOut)
{
  uint32_t Counter = 0;

  while((USART_GetFlagStatus(SC_USART, USART_FLAG_RXNE) == RESET) && (Counter != TimeOut))
  {
    Counter++;
  }

  if(Counter != TimeOut)
  {
    *Data = (uint8_t)USART_ReceiveData(SC_USART);
    return SUCCESS;    
  }
  else 
  {
    return ERROR;
  }
}
/**
  * @brief  Requests the reset answer from card.
  * @param  card: pointer to a buffer which will contain the card ATR.
  * @param  length: maximum ATR length
  * @retval None
  */
uint8_t SC_AnswerReq(uint8_t *card, uint8_t length)
{
  uint8_t Data = 0;
  uint8_t i = 0;

	for(i = 0; i < length;)
	{
		if((USART_ByteReceive(&Data, SC_RECEIVE_TIMEOUT)) == SUCCESS)
		{
		  card[i++] = Data;
		}
		else
			break;
	}
	return i; 
}



/**
  * @brief  Initializes all peripheral used for Smartcard interface.
  * @param  None
  * @retval None
  */
void SC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;  
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(SC_USART_TX_GPIO_CLK | SC_USART_CK_GPIO_CLK, ENABLE);
						 
	/* Enable SC_USART clock */
	SC_USART_APBPERIPHCLOCK(SC_USART_CLK, ENABLE);

	/* Connect PXx to SC_USART_TX */
	GPIO_PinAFConfig(SC_USART_TX_GPIO_PORT, SC_USART_TX_SOURCE, SC_USART_TX_AF);

	/* Connect PXx to SC_USART_CK */
	GPIO_PinAFConfig(SC_USART_CK_GPIO_PORT, SC_USART_CK_SOURCE, SC_USART_CK_AF); 

	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = SC_USART_CK_PIN;
	GPIO_Init(SC_USART_CK_GPIO_PORT, &GPIO_InitStructure);

	/* Configure USART Tx pin as alternate function open-drain */
	GPIO_InitStructure.GPIO_Pin = SC_USART_TX_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(SC_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	/* Enable SC_USART IRQ */
	NVIC_InitStructure.NVIC_IRQChannel = SC_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
	NVIC_Init(&NVIC_InitStructure);

	/* SC_USART configuration ----------------------------------------------------*/
	/* SC_USART configured as follow:
		- Word Length = 9 Bits
		- 1.5 Stop Bit
		- Even parity
		- BaudRate = 8064 baud
		- Hardware flow control disabled (RTS and CTS signals)
		- Tx and Rx enabled
		- USART Clock enabled
	*/

	/* USART Clock set to 3,5 MHz (PCLK2 (84 MHz) / 24) */
	USART_SetPrescaler(SC_USART, 0x0C);

	/* USART Guard Time set to 2 Bit */
	USART_SetGuardTime(SC_USART, 2);

	USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
	USART_ClockInit(SC_USART, &USART_ClockInitStructure);


	USART_InitStructure.USART_BaudRate = 9408;  /* Starting baudrate = 3,5MHz / 372etu */
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(SC_USART, &USART_InitStructure); 

	/* Enable the SC_USART Parity Error Interrupt */
	USART_ITConfig(SC_USART, USART_IT_PE, ENABLE);

	/* Enable the SC_USART Framing Error Interrupt */
	USART_ITConfig(SC_USART, USART_IT_ERR, ENABLE);

	/* Enable SC_USART */
	USART_Cmd(SC_USART, ENABLE);

	/* Enable the NACK Transmission */
	USART_SmartCardNACKCmd(SC_USART, ENABLE);

	/* Enable the Smartcard Interface */
	USART_SmartCardCmd(SC_USART, ENABLE);

	/* Set RSTIN HIGH */  
	SC_Reset(Bit_SET);
}

/**
  * @brief  Deinitializes all ressources used by the Smartcard interface.
  * @param  None
  * @retval None
  */
static void SC_DeInit(void)
{
  /* Disable CMDVCC */
  SC_PowerCmd(DISABLE);

  /* Deinitializes the SC_USART */
  USART_DeInit(SC_USART);
                         
  /* Disable SC_USART clock */
  SC_USART_APBPERIPHCLOCK(SC_USART_CLK, DISABLE);
}



/**
  * @brief  Configures GPIO hardware resources used for Samrtcard.
  * @param  None
  * @retval None
  */
void SC_IOConfig(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(SC_OFF_GPIO_CLK | SC_CMDVCC_GPIO_CLK | 
                         SC_3_5V_GPIO_CLK | SC_RESET_GPIO_CLK, ENABLE);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure Smartcard CMDVCC pin */
  GPIO_InitStructure.GPIO_Pin = SC_CMDVCC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(SC_CMDVCC_GPIO_PORT, &GPIO_InitStructure);

  /* Configure Smartcard Reset pin */
  GPIO_InitStructure.GPIO_Pin = SC_RESET_PIN;
  GPIO_Init(SC_RESET_GPIO_PORT, &GPIO_InitStructure);

  /* Configure Smartcard 3/5V pin */
  GPIO_InitStructure.GPIO_Pin = SC_3_5V_PIN;
  GPIO_Init(SC_3_5V_GPIO_PORT, &GPIO_InitStructure);


  /* Disable CMDVCC */
  SC_PowerCmd(DISABLE);

  /* Set RSTIN HIGH */  
  SC_Reset(Bit_SET);
    						                                
  /* Configure Smartcard OFF pin */
  GPIO_InitStructure.GPIO_Pin = SC_OFF_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SC_OFF_GPIO_PORT, &GPIO_InitStructure);
  
  /* Configure EXTI line connected to Smartcard OFF Pin */
  SYSCFG_EXTILineConfig(SC_OFF_EXTI_PORT_SOURCE, SC_OFF_EXTI_PIN_SOURCE);

  EXTI_ClearITPendingBit(SC_OFF_EXTI_LINE);
  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_Line = SC_OFF_EXTI_LINE;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Clear the SC_OFF_EXTI_IRQn Pending Bit */
  NVIC_ClearPendingIRQ(SC_OFF_EXTI_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = SC_OFF_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
}

/**
  * @brief  Detects whether the Smartcard is present or not.
  * @param  None. 
  * @retval 0 - Smartcard inserted
  *         1 - Smartcard not inserted
  */
uint8_t SC_Detect(void)
{
  return GPIO_ReadInputDataBit(SC_OFF_GPIO_PORT, SC_OFF_PIN);
}




