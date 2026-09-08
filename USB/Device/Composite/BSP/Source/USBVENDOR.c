#include "USBVENDOR.h"
#include "USBDevice.h"

InterfaceVendorStruct InterfaceVendor;

static bool VendorInputDataEvent(InterfaceVendorStruct* self, uint8_t* inputData, uint16_t* bytesToWrite) {
    return true;
}

static bool VendorOutputDataEvent(InterfaceVendorStruct* self, uint8_t* outputData, uint16_t bytesRead) {
    return true;
}

void VendorSetup(USBDeviceStruct* device) {
    printf("VENDOR: Vendor Specific device.\n");

    InterfaceVendorStruct* vendor = &InterfaceVendor;
    InterfaceVendorConstractor(vendor);

    vendor->InputDataCallback  = VendorInputDataEvent;
    vendor->OutputDataCallback = VendorOutputDataEvent;

    device->RegisterInterface(device, &vendor->base);
    device->VendorInterface = &vendor->base;
}
