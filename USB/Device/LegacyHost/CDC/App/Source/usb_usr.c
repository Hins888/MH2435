#include "usb_usr.h"
#include "usb_bsp.h"

extern USB_OTG_CORE_HANDLE   USB_OTG_Core;
extern USBH_HOST             USB_Host;
extern CDC_LineCodingTypeDef CDC_GetLineCode;
extern CDC_LineCodingTypeDef CDC_SetLineCode;
extern CDC_Usercb_TypeDef    UserCb;

static uint8_t usb_test_string[] = "Megahunt USB host vcp running...\n";

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
                                     USBH_USR_Application,
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
    USB_UserLog("> Device Disconnected\n");
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

    USB_UserLog("VID : %04X\n", (*hs).idVendor);
    USB_UserLog("PID : %04X\n", (*hs).idProduct);
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
 * @brief  Displays received font char
 * @param  ptr: pointer to string to display.
 * @retval None
 */
/*
static void CDC_ReceiveData(uint8_t *ptr)
{
  USB_Log("> usb rx: %s", ptr);
}
*/

static bool CDCReadEvent(uint32_t* buffer, uint32_t bytesRead) {
    USB_Log("> CDC read %d bytes data from device\n", bytesRead);

#if CONFIG_USBCDC_SERVICE_READ_IN_APP
    // If you want read buffer in any other process of application use `VCPHostReadBytes()`, return false here.
    return false;
#else
    // Return true if dealed all the buffer data, and the data in ReadCircularBuffer will be freed.
    return true;
#endif
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
    // UserCb.Receive = CDC_ReceiveData;
    UserCb.ReadCallback = CDCReadEvent;
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

/**
 * @brief  USBH_USR_Application
 *         CDC Main application
 * @param  None
 * @retval None
 */
int USBH_USR_Application(void) {
    static CDC_Demo_State demo_state     = CDC_DEMO_IDLE;
    static uint32_t       tx_delay_count = 0x10000;
    static uint32_t       loop_count     = 0;
    static char           buf[100]       = {0};

    if (demo_state == CDC_DEMO_IDLE) {
        demo_state                    = CDC_DEMO_LOOPBACK;
        CDC_SetLineCode.b.dwDTERate   = CDC_GetLineCode.b.dwDTERate;
        CDC_SetLineCode.b.bDataBits   = CDC_GetLineCode.b.bDataBits;
        CDC_SetLineCode.b.bParityType = CDC_GetLineCode.b.bParityType;
        CDC_SetLineCode.b.bCharFormat = CDC_GetLineCode.b.bCharFormat;
        CDC_ChangeStateToIssueSetConfig(&USB_OTG_Core, &USB_Host);
    }
    /*
    if (demo_state == CDC_DEMO_RECEIVE)
    {
      CDC_StartReception(&USB_OTG_Core);
      // CDC_StopReception(&USB_OTG_Core);
      demo_state = CDC_DEMO_WAIT;
    }

    if (demo_state == CDC_DEMO_SEND)
    {
      USB_Log("> usb tx: %s", usb_test_string);
      CDC_SendData(usb_test_string, sizeof(usb_test_string));
      demo_state = CDC_DEMO_WAIT;
    }
    */

    if (demo_state == CDC_DEMO_LOOPBACK) {
        if (tx_delay_count == 0x10000) {
            tx_delay_count = 0;
            loop_count++;

            CDC_StartReception(&USB_OTG_Core);
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Loop times = %d %s", loop_count, usb_test_string);
            USB_Log("> usb tx: %s", buf);
            VCPHostSendBytes((uint8_t*)buf, strlen(buf));
        }
        tx_delay_count++;
    }

    if (demo_state == CDC_DEMO_WAIT) {}
    return 0;
}
