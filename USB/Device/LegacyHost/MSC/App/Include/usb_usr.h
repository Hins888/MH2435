#ifndef __USB_USR_H__
#define __USB_USR_H__
#include "usbh_core.h"

typedef enum {
    APP_FS_INIT,
    APP_FS_READLIST,
    APP_FS_WRITEFILE,
    APP_FS_READFILE,
    APP_FS_IDLE
} APP_FS_STATE;

#define User_FATFS_Log(...) printf(__VA_ARGS__)

extern USBH_Usr_cb_TypeDef USR_Callbacks;

void            USBH_USR_Init(void);
void            USBH_USR_DeInit(void);
void            USBH_USR_DeviceAttached(void);
void            USBH_USR_ResetDevice(void);
void            USBH_USR_DeviceDisconnected(void);
void            USBH_USR_OverCurrentDetected(void);
void            USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed);
void            USBH_USR_Device_DescAvailable(void*);
void            USBH_USR_DeviceAddressAssigned(void);
void            USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef* cfgDesc, USBH_InterfaceDesc_TypeDef* itfDesc, USBH_EpDesc_TypeDef* epDesc);
void            USBH_USR_Manufacturer_String(void*);
void            USBH_USR_Product_String(void*);
void            USBH_USR_SerialNum_String(void*);
void            USBH_USR_EnumerationDone(void);
USBH_USR_Status USBH_USR_UserInput(void);
int             USBH_USR_Application(void);
void            USBH_USR_DeviceNotSupported(void);
void            USBH_USR_UnrecoveredError(void);

#endif
