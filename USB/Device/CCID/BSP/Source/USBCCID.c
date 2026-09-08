#include "USBCCID.h"
#include "USBDevice.h"

InterfaceCCIDStruct InterfaceCCID;

static uint32_t CCIDCommandBuffer[ToUintSize(CCID_MAX_COMMAND_BUFFER)];
static uint32_t CCIDResponseBuffer[ToUintSize(CCID_MAX_RESPONSE_BUFFER)];

static uint8_t TransferBlockEvent(InterfaceCCIDStruct* self, uint8_t slot, uint8_t* readData, uint32_t readLength, uint8_t* sendData, uint32_t* sendLength) {
    // This is an example to send back APDU commands to host

    memcpy(sendData, readData, readLength);
    // Must set *sendLength before return, *sendLength should be less than CCID_MAX_RESPONSE_DATA_SIZE
    *sendLength = readLength;

    // Return CCID_ERROR_OK if there is no error
    return CCID_ERROR_OK;
}

void CCIDSetup(USBDeviceStruct* device) {
    printf("CCID: Enumerates a CCID device.\n");

    InterfaceCCIDStruct* ccid = &InterfaceCCID;
    InterfaceCCIDConstractor(ccid);

    // Override CCID Descriptor
    ccid->ClassDescriptor = (CCIDClassDescriptorStruct) {
        .bLength                = CCID_DESCRIPTOR_SIZE,
        .bDescriptorType        = CCID_DESCRIPTOR_TYPE,
        .bcdCCID                = CCID_VERSION,
        .bMaxSlotIndex          = 0x00,
        .bVoltageSupport        = 0x03,
        .dwProtocols            = 0x00000001,
        .dwDefaultClock         = DEFAULT_CLOCK_FREQ,
        .dwMaximumClock         = DEFAULT_CLOCK_FREQ,
        .bNumClockSupported     = 0x00,
        .dwDataRate             = DEFAULT_DATA_RATE,
        .dwMaxDataRate          = DEFAULT_DATA_RATE,
        .bNumDataRatesSupported = 0x00,
        .dwMaxIFSD              = 0x00000000,
        .dwSynchProtocols       = 0x00000000,
        .dwMechanical           = 0x00000000,
        .dwFeatures             = 0x00010038,
        .dwMaxCCIDMessageLength = MAX(CCID_MAX_COMMAND_BUFFER, CCID_MAX_RESPONSE_BUFFER),
        .bClassGetResponse      = 0x00,
        .bClassEnvelope         = 0x00,
        .wLcdLayout             = 0x0000,
        .bPINSupport            = 0x00,
        .bMaxCCIDBusySlots      = 0x01,
    };

    // Setup CCID Buffers
    ccid->CommandBuffer  = CCIDCommandBuffer;
    ccid->ResponseBuffer = CCIDResponseBuffer;

    // Setup CCID Callbacks
    ccid->TransferBlockCallback = TransferBlockEvent;

    // Setup CCID Endpoint
    ccid->RxEPConfig.Index = 3;
    ccid->TxEPConfig.Index = 3;

    device->RegisterInterface(device, &ccid->base);
}
