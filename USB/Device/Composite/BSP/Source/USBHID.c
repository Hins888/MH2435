#include "USBHID.h"
#include "USBDevice.h"

InterfaceHIDStruct InterfaceHID;

#if (USBHID_DEMO == USBHID_VENDOR_DEMO)

#include "CircularBuffer.h"

static uint32_t HIDVendorInputReportData[ToUintSize(HID_VENDOR_REPORT_SIZE)];
static uint32_t HIDVendorOutputReportData[ToUintSize(HID_VENDOR_REPORT_SIZE)];

static CircularBufferStruct HIDVendorOutputCircularBuffer;

static uint32_t HIDVendorOutputReportBuffer[ToUintSize(HID_VENDOR_REPORT_SIZE * USBHID_VENDOR_BUFFER_REPORT_COUNT)];

bool HIDVendorPutInput(uint8_t* inputReport, uint32_t inputSize) {
    return InterfaceHID.PutInputReport(&InterfaceHID, inputReport, inputSize);
}

static bool HIDGetReportEvent(InterfaceHIDStruct* self, uint8_t* inputReport, uint32_t inputSize) {
    printf_dbg("HID has put input report.\n");
    // return fasle if HID Get/Input report not updated.
    // In application the hid->PutInputReport(...) could be used to update input report.
    return false;
}

bool HIDVendorGetOutput(uint8_t* outputReport, uint32_t outputSize) {
    return HIDVendorOutputCircularBuffer.Pop(&HIDVendorOutputCircularBuffer, outputReport, outputSize, false) == outputSize;
}

static bool HIDSetReportEvent(InterfaceHIDStruct* self, uint8_t* outputReport, uint32_t outputSize) {
    printf_dbg("HID has gotten output report.\n");
    HIDVendorOutputCircularBuffer.Push(&HIDVendorOutputCircularBuffer, outputReport, outputSize, true);
    // return true if HID Set/Output report is dealed.
    // In application the hid->GetOutputReport(...) could not get any output report.
    return true;
}

void HIDVendorSetup(USBDeviceStruct* device) {
    printf("HID-Vendor: HID Vendor device.\n");

    CircularBufferConstractor(&HIDVendorOutputCircularBuffer, 0);
    HIDVendorOutputCircularBuffer.Init(&HIDVendorOutputCircularBuffer, (uint8_t*)HIDVendorOutputReportBuffer, sizeof(HIDVendorOutputReportBuffer));

    InterfaceHIDStruct* hid = &InterfaceHID;
    InterfaceHIDConstractor(hid);

    hid->base.Name         = L"InterfaceHIDVendor";
    hid->IsOutputEPEnabled = true;
    hid->InputReportData   = HIDVendorInputReportData;
    hid->InputReportSize   = HID_VENDOR_REPORT_SIZE;
    hid->OutputReportData  = HIDVendorOutputReportData;
    hid->OutputReportSize  = HID_VENDOR_REPORT_SIZE;
    hid->SetReportCallback = HIDSetReportEvent;
    hid->GetReportCallback = HIDGetReportEvent;

    // Setup HID Endpoint index
    hid->RxEPConfig.Index = 3;
    hid->TxEPConfig.Index = 3;

    device->RegisterInterface(device, &hid->base);
}

void HIDVendorLoop(void) {
    uint32_t hidPacket[ToUintSize(HID_VENDOR_REPORT_SIZE)];
    if (!HIDVendorGetOutput((uint8_t*)hidPacket, HID_VENDOR_REPORT_SIZE))
        return;

    while (!HIDVendorPutInput((uint8_t*)hidPacket, HID_VENDOR_REPORT_SIZE)) {}
}
#elif (USBHID_DEMO == USBHID_KEYBOARD_DEMO)

static uint32_t HIDKeyboardInputReportData[ToUintSize(8)];
static uint32_t HIDKeyboardLEDReportData[ToUintSize(1)];

static const uint8_t HIDKeyboardConvertArray[98][3] = {
    /* ascii  CtrlValue  UsageID  */
    {'\0', 0x00, 0x00},
    {'a', 0x00, KEY_A},
    {'b', 0x00, KEY_B},
    {'c', 0x00, KEY_C},
    {'d', 0x00, KEY_D},
    {'e', 0x00, KEY_E},
    {'f', 0x00, KEY_F},
    {'g', 0x00, KEY_G},
    {'h', 0x00, KEY_H},
    {'i', 0x00, KEY_I},
    {'j', 0x00, KEY_J},
    {'k', 0x00, KEY_K},
    {'l', 0x00, KEY_L},
    {'m', 0x00, KEY_M},
    {'n', 0x00, KEY_N},
    {'o', 0x00, KEY_O},
    {'p', 0x00, KEY_P},
    {'q', 0x00, KEY_Q},
    {'r', 0x00, KEY_R},
    {'s', 0x00, KEY_S},
    {'t', 0x00, KEY_T},
    {'u', 0x00, KEY_U},
    {'v', 0x00, KEY_V},
    {'w', 0x00, KEY_W},
    {'x', 0x00, KEY_X},
    {'y', 0x00, KEY_Y},
    {'z', 0x00, KEY_Z},
    {'A', 0x02, KEY_A},
    {'B', 0x02, KEY_B},
    {'C', 0x02, KEY_C},
    {'D', 0x02, KEY_D},
    {'E', 0x02, KEY_E},
    {'F', 0x02, KEY_F},
    {'G', 0x02, KEY_G},
    {'H', 0x02, KEY_H},
    {'I', 0x02, KEY_I},
    {'J', 0x02, KEY_J},
    {'K', 0x02, KEY_K},
    {'L', 0x02, KEY_L},
    {'M', 0x02, KEY_M},
    {'N', 0x02, KEY_N},
    {'O', 0x02, KEY_O},
    {'P', 0x02, KEY_P},
    {'Q', 0x02, KEY_Q},
    {'R', 0x02, KEY_R},
    {'S', 0x02, KEY_S},
    {'T', 0x02, KEY_T},
    {'U', 0x02, KEY_U},
    {'V', 0x02, KEY_V},
    {'W', 0x02, KEY_W},
    {'X', 0x02, KEY_X},
    {'Y', 0x02, KEY_Y},
    {'Z', 0x02, KEY_Z},
    {'1', 0x00, KEY_1},
    {'2', 0x00, KEY_2},
    {'3', 0x00, KEY_3},
    {'4', 0x00, KEY_4},
    {'5', 0x00, KEY_5},
    {'6', 0x00, KEY_6},
    {'7', 0x00, KEY_7},
    {'8', 0x00, KEY_8},
    {'9', 0x00, KEY_9},
    {'0', 0x00, KEY_0},
    {'!', 0x02, KEY_1},
    {'@', 0x02, KEY_2},
    {'#', 0x02, KEY_3},
    {'$', 0x02, KEY_4},
    {'%', 0x02, KEY_5},
    {'^', 0x02, KEY_6},
    {'&', 0x02, KEY_7},
    {'*', 0x02, KEY_8},
    {'(', 0x02, KEY_9},
    {')', 0x02, KEY_0},
    {'\n', 0x00, KEY_ENTER},
    {0x1B, 0x00, KEY_ESCAPE},
    {' ', 0x00, KEY_SPACEBAR},
    {'-', 0x00, KEY_UNDERSCORE},
    {'_', 0x02, KEY_UNDERSCORE},
    {'=', 0x00, KEY_PLUS},
    {'+', 0x02, KEY_PLUS},
    {'[', 0x00, KEY_OPEN_BRACKET},
    {'{', 0x02, KEY_OPEN_BRACKET},
    {']', 0x00, KEY_CLOSE_BRACKET},
    {'}', 0x02, KEY_CLOSE_BRACKET},
    {'\\', 0x00, KEY_BACKSLASH},
    {'|', 0x02, KEY_BACKSLASH},
    {';', 0x00, KEY_COLON},
    {':', 0x02, KEY_COLON},
    {'\'', 0x00, KEY_QUOTE},
    {'"', 0x02, KEY_QUOTE},
    {',', 0x00, KEY_COMMA},
    {'<', 0x02, KEY_COMMA},
    {'.', 0x00, KEY_DOT},
    {'>', 0x02, KEY_DOT},
    {'.', 0x00, KEY_DOT},
    {'>', 0x02, KEY_DOT},
    {'/', 0x00, KEY_SLASH},
    {'?', 0x02, KEY_SLASH},
};

static const uint8_t HIDKeyboardReportDescriptor[] __ALIGNED(4) = {
    USAGE_PAGE(1),      0x01, // USAGE_PAGE (Generic Desktop)
    USAGE(1),           0x06, // USAGE (Keyboard)
    COLLECTION(1),      0x01, // Collection (Application)

    USAGE_PAGE(1),      0x07, // USAGE_PAGE (Keyboard)
    USAGE_MINIMUM(1),   0xe0, // USAGE_MINIMUM (Keyboard LeftControl)
    USAGE_MAXIMUM(1),   0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)

    LOGICAL_MINIMUM(1), 0x00, // LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1), 0x01, // LOGICAL_MAXIMUM (1)

    REPORT_SIZE(1),     0x01, // REPORT_SIZE (1)
    REPORT_COUNT(1),    0x08, // REPORT_COUNT (8)
    INPUT(1),           0x02, // INPUT (Data,Var,Abs)

    REPORT_COUNT(1),    0x01, // REPORT_COUNT (1)
    REPORT_SIZE(1),     0x08, // REPORT_SIZE (8)
    INPUT(1),           0x03, // INPUT (Cnst,Var,Abs)

    REPORT_COUNT(1),    0x05, // REPORT_COUNT (5)
    REPORT_SIZE(1),     0x01, // REPORT_SIZE (1)
    USAGE_PAGE(1),      0x08, // USAGE_PAGE (LEDs)
    USAGE_MINIMUM(1),   0x01, // USAGE_MINIMUM (Num Lock)
    USAGE_MAXIMUM(1),   0x05, // USAGE_MAXIMUM (Kana)
    OUTPUT(1),          0x02, // OUTPUT (Data,Var,Abs)

    REPORT_COUNT(1),    0x01, // REPORT_COUNT (1)
    REPORT_SIZE(1),     0x03, // REPORT_SIZE (3)
    OUTPUT(1),          0x03, // OUTPUT (Cnst,Var,Abs)

    REPORT_COUNT(1),    0x06, // REPORT_COUNT (6)
    REPORT_SIZE(1),     0x08, // REPORT_SIZE (8)
    LOGICAL_MINIMUM(1), 0x00, // LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1), 0xFF, // LOGICAL_MAXIMUM (255)
    USAGE_PAGE(1),      0x07, // USAGE_PAGE (Keyboard)
    USAGE_MINIMUM(1),   0x00, // USAGE_MINIMUM (Reserved (no event indicated))
    USAGE_MAXIMUM(1),   0x65, // USAGE_MAXIMUM (Keyboard Application)
    INPUT(1),           0x00, // INPUT (Data,Ary,Abs)
    END_COLLECTION(0),
};

static bool HIDKeyboardKeyInput(uint8_t keyReport[8]) {
    return InterfaceHID.PutInputReport(&InterfaceHID, keyReport, 8);
}

void HIDKeyboardPressKey(char* keys) {
    char*   key        = keys;
    uint8_t KeyBuff[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    char    lastKey    = 0;
    do {
        for (int i = 0; i < sizeof(HIDKeyboardConvertArray) / sizeof(HIDKeyboardConvertArray[0]); i++) {
            if (*key == HIDKeyboardConvertArray[i][0]) {
                if (HIDKeyboardConvertArray[i][2] == lastKey) {
                    memset(KeyBuff, 0x00, sizeof(KeyBuff));
                    while (!HIDKeyboardKeyInput(KeyBuff)) {}
                }
                KeyBuff[0] = HIDKeyboardConvertArray[i][1];
                KeyBuff[2] = HIDKeyboardConvertArray[i][2];
                lastKey    = KeyBuff[2];
                break;
            }
        }
        while (!HIDKeyboardKeyInput(KeyBuff)) {}
    }
    while (*(key++));
}

static bool HIDKeyboardKeyInputtedEvent(InterfaceHIDStruct* self, uint8_t* inputReport, uint32_t inputSize) {
    //printf("Last key inputted.\n");
    // return fasle if HID Get/Input report not updated.
    // In application hid->PutInputReport(...) could be used to update input report.
    return false;
}

static bool HIDKeyboardLEDUpdatedEvent(InterfaceHIDStruct* self, uint8_t* outputReport, uint32_t outputSize) {
    uint8_t keyboardLED = *outputReport;
    printf("LED updated %02X.\n", keyboardLED);
    // return true if HID Set/Output report is dealed.
    // In application hid->GetOutputReport(...) will got nothing.
    return true;
}

void HIDKeyboardSetup(USBDeviceStruct* device) {
    printf("HID-Keyboard: Loop send keys 'Hello World!!!'.\n");

    InterfaceHIDStruct* hid = &InterfaceHID;
    InterfaceHIDConstractor(hid);

    hid->base.Name         = L"InterfaceHIDKeyboard";
    hid->IsOutputEPEnabled = false;
    hid->InputReportData   = HIDKeyboardInputReportData;
    hid->InputReportSize   = 8;
    hid->OutputReportData  = HIDKeyboardLEDReportData;
    hid->OutputReportSize  = 1;
    hid->GetReportCallback = HIDKeyboardKeyInputtedEvent;
    hid->SetReportCallback = HIDKeyboardLEDUpdatedEvent;

    hid->ReportDescriptorData = (uint32_t*)HIDKeyboardReportDescriptor;
    hid->ReportDescriptorSize = sizeof(HIDKeyboardReportDescriptor);

    // Setup HID Endpoint
    hid->RxEPConfig.Index = 3;
    hid->TxEPConfig.Index = 3;

    device->RegisterInterface(device, &hid->base);
}

static uint32_t HIDKeyboardLoopCount = 0;
extern uint32_t HIDKeyboardInterval;

void HIDKeyboardLoopPressKey(void) {
    HIDKeyboardLoopCount++;
    if (HIDKeyboardLoopCount % 8000001 == 8000000)
        HIDKeyboardPressKey("Hello World!!!\n");
}
#endif
