#include "usb_usr.h"
#include "usb_bsp.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "ff.h"

static FATFS        fatfs;
static FIL          file;
static APP_FS_STATE ApplicationState = APP_FS_INIT;

extern USB_OTG_CORE_HANDLE USB_OTG_Core;
extern USBH_HOST           USB_Host;

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
    ApplicationState = APP_FS_INIT;
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

/**
 * @brief  Explore_Disk
 *         Displays disk content
 * @param  path: pointer to root path
 * @retval None
 */
static uint8_t Explore_Disk(char* path, uint8_t recu_level) {

    FRESULT res;
    FILINFO fno;
    DIR     dir;
    char*   fn;
    char    tmp[14];

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        while (HCD_IsDeviceConnected(&USB_OTG_Core)) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) {
                break;
            }
            if (fno.fname[0] == '.') {
                continue;
            }

            fn = fno.fname;
            strcpy(tmp, fn);

            if (recu_level == 1) {
                User_FATFS_Log("   |__");
            }
            else if (recu_level == 2) {
                User_FATFS_Log("   |   |__");
            }
            if ((fno.fattrib & AM_MASK) == AM_DIR) {
                strcat(tmp, "\n");
                User_FATFS_Log("%s", tmp);
            }
            else {
                strcat(tmp, "\n");
                User_FATFS_Log("%s", tmp);
            }

            if (((fno.fattrib & AM_MASK) == AM_DIR) && (recu_level == 1)) {
                Explore_Disk(fn, 2);
            }
        }
    }
    return res;
}
static uint8_t file_read_buf[512] = {0};
/**
 * @brief  USBH_USR_Application
 * @param  None
 * @retval None
 */
int USBH_USR_Application(void) {
    FRESULT  res;
    uint16_t bytesWrite;
    uint16_t bytesRead;
    uint8_t  writeTextBuff[] = "Megahunt USB Host MSC demo application using FAT_FS   ";

    switch (ApplicationState) {
        case APP_FS_INIT:
            /* Initialises the File System */
            if (f_mount(&fatfs, "", 0) != FR_OK) {
                /* efs initialisation fails */
                User_FATFS_Log("> Cannot initialize File System.\n");
                return (-1);
            }

            User_FATFS_Log("> File System initialized.\n");
            User_FATFS_Log("> Disk capacity : %lu Bytes\n", (unsigned long)(USBH_MSC_Param.MSCapacity * USBH_MSC_Param.MSPageLength));

            if (USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED) {
                User_FATFS_Log("> The disk is write protected\n");
            }

            ApplicationState = APP_FS_READLIST;
            break;
        case APP_FS_READLIST:
            User_FATFS_Log("> Exploring disk flash ...\n");
            Explore_Disk("0:/", 1);
            ApplicationState = APP_FS_WRITEFILE;
            break;
        case APP_FS_WRITEFILE:
            f_mount(&fatfs, "", 0);
            if (f_open(&file, "0:Megahunt.TXT", FA_CREATE_NEW | FA_WRITE) == FR_OK) {
                res = f_write(&file, writeTextBuff, sizeof(writeTextBuff), (void*)&bytesWrite);
                if ((bytesWrite == 0) || (res != FR_OK)) {
                    User_FATFS_Log("> Megahunt.TXT can not be writen.\n");
                }
                else {
                    User_FATFS_Log("> 'Megahunt.TXT' file now is created\n");
                }

                /* close file and filesystem */
                f_close(&file);
                f_mount(NULL, "", 0);
            }
            else {
                User_FATFS_Log("> Megahunt.TXT already created in the disk\n");
            }
            ApplicationState = APP_FS_READFILE;
            break;
        case APP_FS_READFILE:
            f_mount(&fatfs, "", 0);
            if (f_open(&file, "0:Megahunt.TXT", FA_OPEN_EXISTING | FA_READ) == FR_OK) {
                memset(file_read_buf, 0, sizeof(file_read_buf));
                res = f_read(&file, file_read_buf, file.fsize, (void*)&bytesRead);
                if ((bytesRead == 0) || (res != FR_OK)) {
                    User_FATFS_Log("> Megahunt.TXT CANNOT be read.\n");
                }
                else {
                    User_FATFS_Log("> Megahunt.TXT Content is :\n");
                    User_FATFS_Log("    %s\n\n", file_read_buf);
                }
                /* close file and filesystem */
                f_close(&file);
                f_mount(NULL, "", 0);
            }
            else {
                User_FATFS_Log("> Megahunt.TXT is not exist in the disk\n");
            }
            ApplicationState = APP_FS_IDLE;
            break;
        case APP_FS_IDLE:

            break;
    }
    return 0;
}
