#include "usb_usr.h"
#include "usb_bsp.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"

extern USB_OTG_CORE_HANDLE USB_OTG_Core;
extern USBH_HOST           USB_Host;
extern void                put_char(uint8_t c);

/* Points to the DEVICE_PROP structure of current device */
/* The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_Callbacks = {USBH_USR_Init,
                                     USBH_USR_DeInit,
                                     USBH_USR_DeviceAttached,
                                     USBH_USR_ResetDevice,
                                     USBH_USR_DeviceDisconnected,
                                     USBH_USR_OverCurrentDetected,
                                     USBH_USR_DeviceSpeedDetected,
                                     USBH_USR_Device_DescAvailable,
                                     USBH_USR_DeviceAddressAssigned,
                                     USBH_USR_Configuration_DescAvailable,
                                     USBH_USR_Manufacturer_String,
                                     USBH_USR_Product_String,
                                     USBH_USR_SerialNum_String,
                                     USBH_USR_EnumerationDone,
                                     USBH_USR_UserInput,
                                     0,
                                     USBH_USR_DeviceNotSupported,
                                     USBH_USR_UnrecoveredError};

/**
 * @}
 */

/**
 * @brief  USBH_USR_Init
 *         Displays the message for host lib initialization
 * @param  None
 * @retval None
 */
void USBH_USR_Init(void) {
    USB_UserLog("> USB Host library started.\n");
}

/**
 * @brief  USBH_USR_DeInit
 *         Deint User state and associated variables
 * @param  None
 * @retval None
 */
void USBH_USR_DeInit(void) {}

/**
 * @brief  USBH_USR_DeviceAttached
 *         Displays the message for device attached
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceAttached(void) {
    USB_UserLog("> Device Attached\n");
}

/**
 * @brief  USBH_USR_ResetUSBDevice
 *         Reset USB Device
 * @param  None
 * @retval None
 */
void USBH_USR_ResetDevice(void) {
    /* Users can do their application actions here for the USB-Reset */
}

/**
 * @brief  USBH_DisconnectEvent
 *         Device disconnect event
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceDisconnected(void) {
    USB_ErrLog("> Device Disconnected\n");
}

/**
 * @brief  USBH_USR_OverCurrentDetected
 *         Device Overcurrent detection event
 * @param  None
 * @retval None
 */
void USBH_USR_OverCurrentDetected(void) {
    USB_ErrLog("Overcurrent detected.\n");
}

/**
 * @brief  USBH_USR_DeviceSpeedDetected
 *         Displays the message for device speed
 * @param  Devicespeed : Device Speed
 * @retval None
 */
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed) {
    if (DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED) {
        USB_UserLog("> High speed device detected\n");
    }
    else if (DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED) {
        USB_UserLog("> Full speed device detected\n");
    }
    else if (DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED) {
        USB_UserLog("> Low speed device detected\n");
    }
    else {
        USB_UserLog("> Device fault\n");
    }
}

/**
 * @brief  USBH_USR_Device_DescAvailable
 *         Displays the message for device descriptor
 * @param  DeviceDesc : device descriptor
 * @retval None
 */
void USBH_USR_Device_DescAvailable(void* DeviceDesc) {
    USBH_DevDesc_TypeDef* hs;
    hs = DeviceDesc;

    USB_UserLog("VID : %04luh\n", (unsigned long)(*hs).idVendor);
    USB_UserLog("PID : %04luh\n", (unsigned long)(*hs).idProduct);
}

/**
 * @brief  USBH_USR_DeviceAddressAssigned
 *         USB device is successfully assigned the Address
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceAddressAssigned(void) {}

/**
 * @brief  USBH_USR_Conf_Desc
 *         Displays the message for configuration descriptor
 * @param  ConfDesc : Configuration descriptor
 * @retval None
 */
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef* cfgDesc, USBH_InterfaceDesc_TypeDef* itfDesc, USBH_EpDesc_TypeDef* epDesc) {
    USBH_InterfaceDesc_TypeDef* id;

    id = itfDesc;

    switch ((*id).bInterfaceClass) {
        case 0x08:
            USB_UserLog("> MSC device connected\n");
            break;
        case 0x03:
            USB_UserLog("> HID device connected\n");
            break;
        case 0x02:
            USB_UserLog("> CDC device connected\n");
            break;
        default:
            USB_UserLog("> Unknown device connected,class value is %d\n", (*id).bInterfaceClass);
            break;
    }
}

/**
 * @brief  USBH_USR_Manufacturer_String
 *         Displays the message for Manufacturer String
 * @param  ManufacturerString : Manufacturer String of Device
 * @retval None
 */
void USBH_USR_Manufacturer_String(void* ManufacturerString) {
    USB_UserLog("Manufacturer : %s\n", (char*)ManufacturerString);
}

/**
 * @brief  USBH_USR_Product_String
 *         Displays the message for Product String
 * @param  ProductString : Product String of Device
 * @retval None
 */
void USBH_USR_Product_String(void* ProductString) {
    USB_UserLog("Product : %s\n", (char*)ProductString);
}

/**
 * @brief  USBH_USR_SerialNum_String
 *         Displays the message for SerialNum_String
 * @param  SerialNumString : SerialNum_String of device
 * @retval None
 */
void USBH_USR_SerialNum_String(void* SerialNumString) {
    USB_UserLog("Serial Number : %s\n", (char*)SerialNumString);
}

/**
 * @brief  USBH_USR_DeviceNotSupported
 *         Device is not supported
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceNotSupported(void) {
    USB_UserLog("> Device not supported\n");
}

/**
 * @brief  USBH_USR_UnrecoveredError
 * @param  None
 * @retval None
 */
void USBH_USR_UnrecoveredError(void) {
    USB_ErrLog("> Unrecovered error\n");
}

/**
 * @brief  EnumerationDone
 *         User response request is displayed to ask for
 *         application jump to class
 * @param  None
 * @retval None
 */
void USBH_USR_EnumerationDone(void) {
    /* Enumeration complete */
    USB_UserLog("> Enumeration completed\n");
}

/**
 * @brief  USBH_USR_UserInput
 *         User Action for application state entry
 * @param  None
 * @retval USBH_USR_Status : User response for key button
 */
USBH_USR_Status USBH_USR_UserInput(void) {
    USBH_USR_Status usbh_usr_status;

    usbh_usr_status = USBH_USR_RESP_OK;
    return usbh_usr_status;
}

void USR_KEYBRD_Init(void) {
    USB_UserLog("> HID Demo Device : Keyboard\n");
}

void USR_KEYBRD_ProcessData(uint8_t data) {
    put_char(data);
}

void USR_MOUSE_Init(void) {
    USB_UserLog("> HID Demo Device : Mouse\n");
}

void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef* data) {
    printf("\n");
    printf("mouse x value: %d\n", data->x);
    printf("mouse y value: %d\n", data->y);
    printf("mouse z value: %d\n", data->z);
    printf("mouse button value: %d\n", data->button);
}
