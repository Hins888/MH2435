#include "mh2435.h"
#include "RetargetIO.h"

/* Private function prototypes -----------------------------------------------*/

int CAN_Polling(void);

#define USE_CAN1

#ifdef USE_CAN1
#define CANx    CAN1
#define CAN_CLK PeripheralCAN1
#else // USE_CAN2
#define CANx    CAN2
#define CAN_CLK PeripheralCAN2
#endif

int main(void) {
    int TestRx;

    RetargetIOSetup(CONFIG_RETARGETIO_DEFAULT_SERIAL);

    printf("CAN LoopBack Demo.\n");

    TestRx = CAN_Polling();

    if (TestRx) {
        printf("CAN Test Failed.\n");
    }
    else {
        printf("CAN Test OK.\n");
    }

    while (1);
}

int CAN_Polling(void) {
    CAN_InitTypeDef       CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    CanTxMsg              TxMessage;
    CanRxMsg              RxMessage;
    uint32_t              uwCounter       = 0;
    uint8_t               TransmitMailbox = 0;

#ifdef USE_CAN1
    PeripheralEnable(CAN_CLK, true);
#else
    PeripheralEnable(PeripheralCAN1, true);
    PeripheralEnable(CAN_CLK, true);
#endif

    CAN_DeInit(CANx);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
    CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;

    CAN_InitStructure.CAN_BS1       = CAN_BS1_12tq;
    CAN_InitStructure.CAN_BS2       = CAN_BS2_2tq;
    CAN_InitStructure.CAN_Prescaler = 24;
    CAN_Init(CANx, &CAN_InitStructure);

#ifdef USE_CAN1
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
#else
    CAN_FilterInitStructure.CAN_FilterNumber = 14;
#endif

    CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;

    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    TxMessage.StdId   = 0x11;
    TxMessage.RTR     = CAN_RTR_DATA;
    TxMessage.IDE     = CAN_ID_STD;
    TxMessage.DLC     = 2;
    TxMessage.Data[0] = 0xCA;
    TxMessage.Data[1] = 0xFE;

    TransmitMailbox = CAN_Transmit(CANx, &TxMessage);
    uwCounter       = 0;

    while ((CAN_TransmitStatus(CANx, TransmitMailbox) != CANTXOK) && (uwCounter != 0xFFFF)) {
        uwCounter++;
    }

    uwCounter = 0;

    while ((CAN_MessagePending(CANx, CAN_FIFO0) < 1) && (uwCounter != 0xFFFF)) {
        uwCounter++;
    }

    // Set Receive to default value
    RxMessage.StdId   = 0x00;
    RxMessage.IDE     = CAN_ID_STD;
    RxMessage.DLC     = 0;
    RxMessage.Data[0] = 0x00;
    RxMessage.Data[1] = 0x00;
    CAN_Receive(CANx, CAN_FIFO0, &RxMessage);

    if (RxMessage.StdId != 0x11) {
        return -1;
    }

    if (RxMessage.IDE != CAN_ID_STD) {
        return -1;
    }

    if (RxMessage.DLC != 2) {
        return -1;
    }

    if ((RxMessage.Data[0] << 8 | RxMessage.Data[1]) != 0xCAFE) {
        return -1;
    }

    return 0;
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
