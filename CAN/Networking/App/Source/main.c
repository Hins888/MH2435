#include "mh2435.h"
#include "bsp_debug_usart.h"
#include "bsp_keyboard.h"

/* Private function prototypes -----------------------------------------------*/
#if 0
#define DEBUG_CANx                           	CAN1
#define DEBUG_CANx_CLK                       	PeripheralCAN1

#define DEBUG_CANx_RX_PIN                   	GPIO_Pin_8                 
#define DEBUG_CANx_RX_GPIO_PORT             	GPIOB                       
#define DEBUG_CANx_RX_GPIO_CLK              	PeripheralGPIOB
#define DEBUG_CANx_RX_PINSOURCE             	GPIO_PinSource8
#define DEBUG_CANx_RX_AF                    	GPIO_AF_CAN1

#define DEBUG_CANx_TX_PIN                  		GPIO_Pin_9                
#define DEBUG_CANx_TX_GPIO_PORT            		GPIOB                   
#define DEBUG_CANx_TX_GPIO_CLK             		PeripheralGPIOB
#define DEBUG_CANx_TX_PINSOURCE            		GPIO_PinSource9
#define DEBUG_CANx_TX_AF                   		GPIO_AF_CAN1

#define DEBUG_CANx_IRQHandler                   CAN1_RX0_IRQHandler
#define DEBUG_CANx_IRQn							CAN1_RX0_IRQn
#else

#define DEBUG_CANx                           	CAN2
#define DEBUG_CANx_CLK                       	PeripheralCAN2

#define DEBUG_CANx_RX_PIN                   	GPIO_Pin_12                 
#define DEBUG_CANx_RX_GPIO_PORT             	GPIOB                       
#define DEBUG_CANx_RX_GPIO_CLK              	PeripheralGPIOB
#define DEBUG_CANx_RX_PINSOURCE             	GPIO_PinSource12
#define DEBUG_CANx_RX_AF                    	GPIO_AF_CAN2

#define DEBUG_CANx_TX_PIN                  		GPIO_Pin_13               
#define DEBUG_CANx_TX_GPIO_PORT            		GPIOB                   
#define DEBUG_CANx_TX_GPIO_CLK             		PeripheralGPIOB
#define DEBUG_CANx_TX_PINSOURCE            		GPIO_PinSource13
#define DEBUG_CANx_TX_AF                   		GPIO_AF_CAN2

#define DEBUG_CANx_IRQHandler                   CAN2_RX0_IRQHandler
#define DEBUG_CANx_IRQn							CAN2_RX0_IRQn

#endif

void NVIC_Config(void);
int  CAN_Config(void);

#define MaxMessage 10
uint32_t MsgCnt      = 0;
uint32_t MsgValidCnt = 0;
CanRxMsg RxMessage[MaxMessage];

int main(void) {
    uint32_t keyStatus;
    CanTxMsg TxMessage;

	Debug_USART_Config();

    printf("CAN Networking Demo.\n");

    KBD_ExtiConfig();
	KBD_NvicConfig();
    NVIC_Config();
    CAN_Config();

    while (1) {
        if (0 != key_code) {
            keyStatus = key_code;
			key_code = 0;

            TxMessage.StdId   = 0x321;
            TxMessage.IDE     = CAN_ID_STD;
            TxMessage.RTR     = CAN_RTR_DATA;
            TxMessage.DLC     = 1;
            TxMessage.Data[0] = keyStatus;
            CAN_Transmit(DEBUG_CANx, &TxMessage);

            while ((CAN_GetFlagStatus(DEBUG_CANx, CAN_FLAG_RQCP0) != RESET) || (CAN_GetFlagStatus(DEBUG_CANx, CAN_FLAG_RQCP1) != RESET) ||
                   (CAN_GetFlagStatus(DEBUG_CANx, CAN_FLAG_RQCP2) != RESET)) {}

            printf("Transmit OK. KeyStatus = 0x%02X.\n", keyStatus);
        }

        if (MsgCnt != MsgValidCnt) {
            // 缓存非空
            if (RxMessage[MsgCnt].IDE == CAN_ID_STD) {
                printf("Stand ID: 0x%08X.\n", RxMessage[MsgCnt].StdId);
            }
            else {
                printf("Ext ID: 0x%08X.\n", RxMessage[MsgCnt].ExtId);
            }

            if (RxMessage[MsgCnt].RTR == CAN_RTR_Data) {
                uint32_t i;
                printf("Data: ");
                for (i = 0; i < RxMessage[MsgCnt].DLC; i++) {
                    printf("0x%02X ", RxMessage[MsgCnt].Data[i]);
                }
                printf("\n");
            }
            else {
                printf("Remote.\n");
            }

            printf("FMI: %d.\n", RxMessage[MsgCnt].FMI);

            MsgCnt = (MsgCnt + 1) % MaxMessage;
        }
    }
}

void NVIC_Config(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                   = DEBUG_CANx_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_Init(&NVIC_InitStructure);
}

int CAN_Config(void) {
    CAN_InitTypeDef       CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;
	
    PeripheralEnable(PeripheralCAN1, true);
    PeripheralEnable(DEBUG_CANx_CLK, true);
	
    PeripheralEnable(DEBUG_CANx_RX_GPIO_CLK, true);
    PeripheralEnable(DEBUG_CANx_TX_GPIO_CLK, true);

    GPIO_PinAFConfig(DEBUG_CANx_RX_GPIO_PORT, DEBUG_CANx_RX_PINSOURCE, DEBUG_CANx_RX_AF);
    GPIO_PinAFConfig(DEBUG_CANx_TX_GPIO_PORT, DEBUG_CANx_TX_PINSOURCE, DEBUG_CANx_TX_AF);

    GPIO_InitStructure.GPIO_Pin   = DEBUG_CANx_RX_PIN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(DEBUG_CANx_RX_GPIO_PORT, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin   = DEBUG_CANx_TX_PIN ;
    GPIO_Init(DEBUG_CANx_TX_GPIO_PORT, &GPIO_InitStructure);

    CAN_DeInit(DEBUG_CANx);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;

    // PCLK=60MHz, BAUD=200KHz
    CAN_InitStructure.CAN_BS1       = CAN_BS1_11tq;
    CAN_InitStructure.CAN_BS2       = CAN_BS2_3tq;
    CAN_InitStructure.CAN_Prescaler = 20;
    CAN_Init(DEBUG_CANx, &CAN_InitStructure);
	
	if(DEBUG_CANx == CAN2)
		CAN_FilterInitStructure.CAN_FilterNumber         = 15;
	else
		CAN_FilterInitStructure.CAN_FilterNumber         = 0;
    CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;

    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    CAN_ITConfig(DEBUG_CANx, CAN_IT_FMP0, ENABLE);

    return 0;
}

void DEBUG_CANx_IRQHandler(void) {
    if ((MsgValidCnt + 1) % MaxMessage == MsgCnt) {
        CAN_Receive(DEBUG_CANx, CAN_FIFO0, &RxMessage[MsgValidCnt]);
    }
    else {
        CAN_Receive(DEBUG_CANx, CAN_FIFO0, &RxMessage[MsgValidCnt]);
        MsgValidCnt = (MsgValidCnt + 1) % MaxMessage;
    }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {}
}
#endif
