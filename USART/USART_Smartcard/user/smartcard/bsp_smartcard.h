/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_SMARTCARD_H
#define __BSP_SMARTCARD_H

/* Includes ------------------------------------------------------------------*/
#include "mh2435.h"

#define SC_USART                           USART3
#define SC_USART_CLK                       RCC_APB1Periph_USART3
#define SC_USART_APBPERIPHCLOCK            RCC_APB1PeriphClockCmd
#define SC_USART_IRQn                      USART3_IRQn
#define SC_USART_IRQHandler                USART3_IRQHandler

#define SC_USART_TX_PIN                    GPIO_Pin_10                
#define SC_USART_TX_GPIO_PORT              GPIOB                      
#define SC_USART_TX_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define SC_USART_TX_SOURCE                 GPIO_PinSource10
#define SC_USART_TX_AF                     GPIO_AF_USART3

#define SC_USART_CK_PIN                    GPIO_Pin_12                
#define SC_USART_CK_GPIO_PORT              GPIOB                    
#define SC_USART_CK_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define SC_USART_CK_SOURCE                 GPIO_PinSource12
#define SC_USART_CK_AF                     GPIO_AF_USART3


/* Smartcard Interface GPIO pins */
#define SC_3_5V_PIN                        GPIO_Pin_15
#define SC_3_5V_GPIO_PORT                  GPIOH
#define SC_3_5V_GPIO_CLK                   RCC_AHB1Periph_GPIOH

#define SC_RESET_PIN                       GPIO_Pin_7
#define SC_RESET_GPIO_PORT                 GPIOF
#define SC_RESET_GPIO_CLK                  RCC_AHB1Periph_GPIOF

#define SC_CMDVCC_PIN                      GPIO_Pin_12
#define SC_CMDVCC_GPIO_PORT                GPIOG
#define SC_CMDVCC_GPIO_CLK                 RCC_AHB1Periph_GPIOG

#define SC_OFF_PIN                         GPIO_Pin_6
#define SC_OFF_GPIO_PORT                   GPIOF
#define SC_OFF_GPIO_CLK                    RCC_AHB1Periph_GPIOF
#define SC_OFF_EXTI_LINE                   EXTI_Line6
#define SC_OFF_EXTI_PORT_SOURCE            EXTI_PortSourceGPIOF
#define SC_OFF_EXTI_PIN_SOURCE             EXTI_PinSource6
#define SC_OFF_EXTI_IRQn                   EXTI9_5_IRQn 
#define SC_OFF_EXTI_IRQHandler             EXTI9_5_IRQHandler

/* Exported constants --------------------------------------------------------*/
#define T0_PROTOCOL        0x00  /* T0 protocol */
#define DIRECT             0x3B  /* Direct bit convention */
#define INDIRECT           0x3F  /* Indirect bit convention */
#define SETUP_LENGTH       20
#define HIST_LENGTH        20
#define LC_MAX             20
#define SC_RECEIVE_TIMEOUT 0xFFFF  /* Direction to reader */

/* SC Tree Structure -----------------------------------------------------------
                              MasterFile
                           ________|___________
                          |        |           |
                        System   UserData     Note
------------------------------------------------------------------------------*/

/* SC ADPU Command: Operation Code -------------------------------------------*/
#define SC_CLA_GSM11       0xA0

/*------------------------ Data Area Management Commands ---------------------*/
#define SC_SELECT_FILE     0xA4
#define SC_GET_RESPONCE    0xC0
#define SC_STATUS          0xF2
#define SC_UPDATE_BINARY   0xD6
#define SC_READ_BINARY     0xB0
#define SC_WRITE_BINARY    0xD0
#define SC_UPDATE_RECORD   0xDC
#define SC_READ_RECORD     0xB2

/*-------------------------- Administrative Commands -------------------------*/ 
#define SC_CREATE_FILE     0xE0

/*-------------------------- Safety Management Commands ----------------------*/
#define SC_VERIFY          0x20
#define SC_CHANGE          0x24
#define SC_DISABLE         0x26
#define SC_ENABLE          0x28
#define SC_UNBLOCK         0x2C
#define SC_EXTERNAL_AUTH   0x82
#define SC_GET_CHALLENGE   0x84

/*-------------------------- Answer to reset Commands ------------------------*/ 
#define SC_GET_A2R         0x00

/* SC STATUS: Status Code ----------------------------------------------------*/
#define SC_EF_SELECTED     0x9F
#define SC_DF_SELECTED     0x9F
#define SC_OP_TERMINATED   0x9000

/* Smartcard Voltage */
#define SC_VOLTAGE_5V      0
#define SC_VOLTAGE_3V      1




/* ADPU-Header command structure ---------------------------------------------*/
typedef struct
{
  uint8_t CLA;  /* Command class */
  uint8_t INS;  /* Operation code */
  uint8_t P1;   /* Selection Mode */
  uint8_t P2;   /* Selection Option */
} SC_Header;

/* ADPU-Body command structure -----------------------------------------------*/
typedef struct 
{
  uint8_t LC;           /* Data field length */
  uint8_t Data[LC_MAX];  /* Command parameters */
  uint8_t LE;           /* Expected length of data to be returned */
} SC_Body;

/* ADPU Command structure ----------------------------------------------------*/
typedef struct
{
  SC_Header Header;
  SC_Body Body;
} SC_ADPU_Commands;

/* SC response structure -----------------------------------------------------*/
typedef struct
{
  uint8_t Data[LC_MAX];  /* Data returned from the card */
  uint8_t SW1;          /* Command Processing status */
  uint8_t SW2;          /* Command Processing qualification */
} SC_ADPU_Responce;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* APPLICATION LAYER ---------------------------------------------------------*/
void SC_PowerCmd(FunctionalState NewState);
void SC_Reset(BitAction ResetState);
void SC_IOConfig(void);
void SC_Init(void);uint8_t SC_Detect(void);
void SC_ParityErrorHandler(void);


/*--------------APDU-----------*/
void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_ResponceStatus);
/*------------ ATR ------------*/
uint8_t SC_AnswerReq(uint8_t *card, uint8_t length);

#endif /* __SMARTCARD_H */

/************************ (C) COPYRIGHT megahunt *****END OF FILE****/
