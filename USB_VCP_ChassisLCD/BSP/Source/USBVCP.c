#include "USBVCP.h"

static InterfaceCDCStruct    InterfaceCDC;
static InterfaceSerialStruct InterfaceSerial;

#ifdef SERIAL_SUPPORT_SEND_BYTES
uint32_t InterfaceSerialSendByteBuffer[ToUintSize(USBVCP_SEND_BYTE_BUFFER_SIZE)];
#endif

#ifdef SERIAL_SUPPORT_READ_BYTES
uint32_t InterfaceSerialReadByteBuffer[ToUintSize(USBVCP_READ_BYTE_BUFFER_SIZE)];
#endif

// DO NOT CALL 'static Callback' FUNCTIONS BY APPLICATION !!!
// TRY NOT TO EXECUTE BLOCKING CODE in 'static Callback' FUNCTIONS!!!
// All 'static Callback' functions will called by interrupt.
// To treat these 'static Callback' functions as VCP specific interrupts.

// All sample codes can be rewritten

#if (USBVCP_DEMO == 1)
static void VCPLoopbackBySentBufferCallback(BlockStruct* block) {
    // Sample code start - An example to send back read data to host.
    // Will free sample malloc buffer when sent back read data.
    printf_dbg("VCP Sent %d bytes\n", block->Size);
    free(block->Buffer);
    // Sample code end
}

void VCPLoopbackBySendBuffer(uint32_t* buffer, uint32_t bytesRead) {
    uint32_t* txbuffer = (uint32_t*)malloc((bytesRead + 3) & 0xFFFFFFFC);
    if (txbuffer) {
        memcpy(txbuffer, buffer, bytesRead);
        if (!VCPSendBufferWithCallback(txbuffer, bytesRead, VCPLoopbackBySentBufferCallback)) {
            printf("Send queue full!\n");
            free(txbuffer);
        }
    }
    else {
        printf("Heap full, malloc failed!\n");
    }
}

extern uint32_t __heap_limit, __heap_base;
#elif (USBVCP_DEMO > 2) && (USBVCP_DEMO <= 4)
static uint32_t loopbackBuffer[ToUintSize(1024)];

void VCPLoopbackByReadBytes(void) {
    uint32_t readSize = VCPReadBytes((uint8_t*)loopbackBuffer, 1024);
    if (!readSize)
        return;
#if (USBVCP_DEMO == USBVCP_LOOPBACK_DEMO_BY_READBYTES_SENDBUFFER)
    if (!VCPSendBufferSync(loopbackBuffer, readSize)) {
        printf("Send queue full!\n");
    }
#elif (USBVCP_DEMO == USBVCP_LOOPBACK_DEMO_BY_READBYTES_SENDBYTES)
//    uint32_t sendOffset = 0;
//    do {
//        sendOffset += VCPSendBytes((uint8_t*)loopbackBuffer + sendOffset, readSize - sendOffset);
//    }
//    while (sendOffset < readSize);
#endif
}
#elif (USBVCP_DEMO == USBVCP_SENDTEST_DEMO)
uint8_t TestArray[58] = {
    0x00, 0x00, 0x00, 0x01, //
    0xC3, 0x00, 0x00, 0x00, //
    0x00, 0x00, 0x00, 0x00, //
    0x00, 0x00, 0x55, 0x55, //
    0x55, 0x55, 0x55, 0x55, //
    0x55, 0x55, 0x77, 0x77, //
    0x77, 0x77, 0x77, 0x77, //
    0x77, 0x77, 0x7F, 0xFF, //
    0xFF, 0xFF, 0xFF, 0xFF, //
    0xFF, 0xFF, 0xFF, 0xFF, //
    0xFF, 0xFF, 0xFE, 0xFD, //
    0xFB, 0xF7, 0xEF, 0xDF, //
    0xBF,                   //
    0x3F, 0x7E, 0xFD, 0xFB, //
    0xF7, 0xEF, 0xDF, 0xBF, //
    0x7E,
};

static void VCPStartSendTestArray(void);

static void EndSentTestArray(BlockStruct* block) {
    VCPStartSendTestArray();
}

static void VCPStartSendTestArray(void) {
    VCPSendBufferWithCallback((uint32_t*)TestArray, 58, EndSentTestArray);
}
#endif

// Callback when host SetLineCoding.
static void VCPSetLineCodingCallback(InterfaceCDCStruct* self, uint32_t baud, uint8_t stop, uint8_t bits, uint8_t parity) {
    // Sample code start
    printf_dbg("SetLineCoding Baudrate: %d, Stop: %d, Bits: %d, Parity: %d.\n", baud, stop, bits, parity);
    // Sample code end
}

// Callback when device state changed.
static void VCPDeviceCallback(InterfaceCDCStruct* self, USBDeviceStateEnum state, USBDeviceEventEnum event) {
    // Sample code start
    switch (event) {
        case USBDeviceConfiguredEvent:
            printf_dbg("VCP Configured.\n");
#if (USBVCP_DEMO == USBVCP_SENDTEST_DEMO)
            VCPStartSendTestArray();
#endif
            break;
        default:
            break;
    }
    // Sample code end
}

// Callback when serial port connect status changed. (Avaliabled only when host software enabled DTR config.)
static void VCPConnectCallback(InterfaceSerialStruct* self, bool isConnected) {
    // Sample code start
    printf_dbg("%s\n", isConnected ? "Serial port connected!" : "Serial port disconnected!");
    // Sample code end
}

// Callback when read data from host.
static void VCPReadCallback(InterfaceSerialStruct* self, uint32_t* buffer, uint32_t bytesRead) {
    // Sample code start - An example to send back read data to host.
    printf_dbg("Read %d bytes data from host!\n", bytesRead);

#if (defined USBVCP_LOOPBACK_DEMO_BY_READCALLBACK_SENDBUFFER) && (USBVCP_DEMO == USBVCP_LOOPBACK_DEMO_BY_READCALLBACK_SENDBUFFER)
    VCPLoopbackBySendBuffer(buffer, bytesRead);
#elif (defined USBVCP_LOOPBACK_DEMO_BY_READCALLBACK_SENDBYTES) && (USBVCP_DEMO == USBVCP_LOOPBACK_DEMO_BY_READCALLBACK_SENDBYTES)
    VCPSendBytes((uint8_t*)buffer, bytesRead);
#endif
    // Sample code end
}

void VCPSetup(USBDeviceStruct* device) {
    printf("VCP: Send all received data from VCP Port.\n");

    InterfaceSerialStruct* serial = &InterfaceSerial;
    InterfaceSerialConstractor(serial);

    serial->ReadCallback = VCPReadCallback;
#ifdef SERIAL_SUPPORT_SEND_BYTES
    serial->SendByteBuffer     = InterfaceSerialSendByteBuffer;
    serial->SendByteBufferSize = USBVCP_SEND_BYTE_BUFFER_SIZE;
#endif
#ifdef SERIAL_SUPPORT_READ_BYTES
    serial->ReadByteBuffer     = InterfaceSerialReadByteBuffer;
    serial->ReadByteBufferSize = USBVCP_READ_BYTE_BUFFER_SIZE;
#endif

    // Setup VCP Endpoint
    serial->RxEPConfig.Index   = 2;
    serial->RxEPConfig.Timeout = 3;
    serial->TxEPConfig.Index   = 2;
    serial->TxEPConfig.Timeout = 10;
    serial->ConnectCallback    = VCPConnectCallback;

    // Improve the transfer performance of USB

    serial->RxEPConfig.MaxPacket        = 512;
    serial->RxEPConfig.AdditionalBuffer = 1;
    serial->TxEPConfig.MaxPacket        = 512;
    serial->TxEPConfig.AdditionalBuffer = 1;

    InterfaceCDCStruct* cdc = &InterfaceCDC;
    InterfaceCDCConstractor(cdc, serial);
    // Set the index of CDC TxEPConfig to enable the management endpoint
    // cdc->TxEPConfig.Index      = 3;
    cdc->Serial                = serial;
    cdc->base.Name             = L"USB-ESL VCP";
    cdc->base.DeviceCallback   = (DeviceCallbackType)VCPDeviceCallback;
    cdc->SetLineCodingCallback = VCPSetLineCodingCallback;

    device->RegisterInterface(device, &cdc->base);
    device->RegisterInterface(device, &serial->base);

#if (USBVCP_DEMO == 1)
    util_assert((__heap_limit - __heap_base < 0x1000));
#endif
}

#ifdef SERIAL_SUPPORT_SEND_BYTES
// It is a thread safe async function.
// The function copyed buffer to CircularBuffer.
// If the CircularBuffer is full or less than size returned 0, else returned pushed size.
uint32_t VCPSendBytes(uint8_t* buffer, uint32_t size) {
    return InterfaceSerial.SendPushByte(&InterfaceSerial, buffer, size, true);
}

uint32_t VCPSendBufferSize(bool isFreeSize) {
    return InterfaceSerial.SendBufferSize(&InterfaceSerial, isFreeSize);
}
#endif

#ifdef SERIAL_SUPPORT_READ_BYTES
// It is a thread safe async function.
// The function copyed buffer from CircularBuffer.
// If the CircularBuffer is empty returned 0, else returned popped size.
uint32_t VCPReadBytes(uint8_t* buffer, uint32_t size) {
    if (!InterfaceSerial.ReadPopByte)
        return 0;
    return InterfaceSerial.ReadPopByte(&InterfaceSerial, buffer, size, true);
}

uint32_t VCPReadBufferSize(bool isFreeSize) {
    return InterfaceSerial.ReadBufferSize(&InterfaceSerial, isFreeSize);
}
#endif

// It is an ASYNC send function!!!
// The function is high-level VCPSend function.
// It will call sentCallback function pointer (e.g. VCPSentCallback(BlockStruct* block)) when VCP has sent all buffer data.
bool VCPSendBufferWithCallback(uint32_t* buffer, uint32_t size, void (*sentCallback)(BlockStruct* block)) {
    return InterfaceSerial.SendPush(&InterfaceSerial, buffer, size, NULL, sentCallback);
}

static volatile bool IsSyncSending = false;

static void VCPSyncSentCallback(BlockStruct* block) {
    IsSyncSending = false;
}

// This is a SYNC send function, which is not recommended because of its low efficiency
// The function can be called by application to send data to host by VCP.
// The function will return false if send queue is full.
// DO NOT CALL the function in interrupt (include 'static Callback' functions).
bool VCPSendBufferSync(uint32_t* buffer, uint32_t size) {
    IsSyncSending = true;
    if (!VCPSendBufferWithCallback(buffer, size, VCPSyncSentCallback)) {
        IsSyncSending = false;
        return false;
    }
    while (IsSyncSending) {}
    return true;
}
