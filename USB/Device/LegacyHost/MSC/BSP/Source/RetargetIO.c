#include "RetargetIO.h"


#define SerialRegisterType USART_TypeDef

#define SerialTxEmpty(uart)    USART_GetFlagStatus(uart, USART_FLAG_TXE)
#define SerialSend(uart, data) USART_SendData(uart, data)

#define SerialRxNotEmpty(uart) USART_GetFlagStatus(uart, USART_FLAG_RXNE)
#define SerialReceive(uart)    USART_ReceiveData(uart)

PeripheralStruct* SerialPeripheral = NULL;

void RetargetIOSetup(PeripheralEnum peripheral, IOEnum txIO, IOEnum rxIO) {
    SerialPeripheral = PeripheralMap + peripheral;

    SerialRegisterType* uart = (SerialRegisterType*)SerialPeripheral->Base;

    PeripheralEnable(peripheral, true);
    PeripheralReset(peripheral);


    USART_InitTypeDef serialConfig = {0};

    serialConfig.USART_BaudRate            = 115200;
    serialConfig.USART_WordLength          = USART_WordLength_8b;
    serialConfig.USART_StopBits            = USART_StopBits_1;
    serialConfig.USART_Parity              = USART_Parity_No;
    serialConfig.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    serialConfig.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(uart, &serialConfig);
    USART_Cmd(uart, ENABLE);

    uint8_t alternate = 0;

    alternate = peripheral >= PeripheralUART4 ? 8 : 7;


    IOConfigStruct uartIOConfig = MakeIOConfig(IOModeAlternate, alternate, IOPullNone, IOSpeedLow, IODriveLow);
    IOSetup(rxIO, uartIOConfig);
    IOSetup(txIO, uartIOConfig);
}

#if __GNUC__
int _write(int file, char* ptr, int len) {
    if (SerialPeripheral == NULL)
        return -1;

    SerialRegisterType* uart = (SerialRegisterType*)SerialPeripheral->Base;

    for (int i = 0; i < len; i++) {
        while (!SerialTxEmpty(uart)) {}
        SerialSend(uart, (uint8_t)*ptr++);
    }
    return len;
}
#endif

int fputc(int c, FILE* stream) {
    if (SerialPeripheral == NULL)
        return -1;

    SerialRegisterType* uart = (SerialRegisterType*)SerialPeripheral->Base;

    if (c == '\n') {
        while (!SerialTxEmpty(uart)) {}
        SerialSend(uart, (uint16_t)'\r');
    }
    while (!SerialTxEmpty(uart)) {}
    SerialSend(uart, (uint8_t)c);
    return c;
}

int fgetc(FILE* f) {
    if (SerialPeripheral == NULL)
        return -1;

    SerialRegisterType* uart = (SerialRegisterType*)SerialPeripheral->Base;

    while (!SerialRxNotEmpty(uart)) {}
    return (int)SerialReceive(uart);
}
