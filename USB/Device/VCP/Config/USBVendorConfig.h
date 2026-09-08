#ifndef __USB_VENDOR_CONFIG_H__
#define __USB_VENDOR_CONFIG_H__

// set 1 to enalbe debug
#define CONFIG_DEBUG 0

/* USB Device Config Start                                                    */
// Change the default USB Version only for legacy device
// #define CONFIG_USB_VERSION USB_VERSION_1_1

// Max interface count
#define CONFIG_USB_DEVICE_MAX_INTERFACE_COUNT (6)

// USB Device Descriptor buffer size = max size of DEVICE_DESCRIPTOR_LENGTH, USB_DEVICE_MAX_STRING_DESCRIPTOR_SIZE,
// USB_DEVICE_MAX_CONFIGURATION_DESCRIPTOR_SIZE, USB_DEVICE_MAX_REPORT_DESCRIPTOR_SIZE and USB_DEVICE_MAX_VENDOR_DESCRIPTOR_SIZE
#define CONFIG_USB_DEVICE_MAX_STRING_DESCRIPTOR_SIZE        (128)
#define CONFIG_USB_DEVICE_MAX_CONFIGURATION_DESCRIPTOR_SIZE (256)
#define CONFIG_USB_DEVICE_MAX_REPORT_DESCRIPTOR_SIZE        (64)
#define CONFIG_USB_DEVICE_MAX_VENDOR_DESCRIPTOR_SIZE        (128)

/* USB Device Config End                                                      */

/* USB Interfaces Default Config Start                                        */
#if USE_COMPONENT_USBVCP
// defined the interface->SendPush(...) queue length
#define SERIAL_TX_QUEUE_LENGTH 16

// defined the first level read buffer size of serial interface
#define SERIAL_MAX_RX_SIZE 1024

// defined to support send bytes buffer by application call the method of interface->SendPushByte(...)
#define SERIAL_SUPPORT_SEND_BYTES

// defined to support read bytes buffer by application call the method of interface->ReadPopByte(...)
#define SERIAL_SUPPORT_READ_BYTES

#endif // USE_COMPONENT_USBVCP

#if USE_COMPONENT_USBCCID
#define CCID_MAX_COMMAND_BUFFER  (271)
#define CCID_MAX_RESPONSE_BUFFER (271)
// uncomment the definition to add notification support for CCID
// #define CCID_SUPPORT_NOTIFICATION
#endif // USE_COMPONENT_USBCCID

#if USE_COMPONENT_USBHID
// uncomment the definition to change the default HID Vendor Report Size (only for HID Vendor Demo)
// #define HID_VENDOR_REPORT_SIZE 512
#endif // USE_COMPONENT_USBHID

/* USB Interfaces Default Config End                                          */

#endif
