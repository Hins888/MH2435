/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_STDREQ_H
#define __USBH_STDREQ_H

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"
#include "usb_hcd.h"
#include "usbh_core.h"
#include "usbh_def.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_STDREQ
  * @brief This file is the 
  * @{
  */ 


/** @defgroup USBH_STDREQ_Exported_Defines
  * @{
  */
#ifndef USER_SELECTED_DESC_CONFIGURATION_NUM
#define USER_SELECTED_DESC_CONFIGURATION_NUM 0
#endif
  
/*Standard Feature Selector for clear feature command*/
#define FEATURE_SELECTOR_ENDPOINT         0X00
#define FEATURE_SELECTOR_DEVICE           0X01


#define INTERFACE_DESC_TYPE               0x04
#define ENDPOINT_DESC_TYPE                0x05
#define INTERFACE_DESC_SIZE               0x09


#define USBH_HID_CLASS                    0x03

/**
  * @}
  */ 


/** @defgroup USBH_STDREQ_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_STDREQ_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_STDREQ_Exported_Variables
  * @{
  */ 
extern uint8_t USBH_CfgDesc[CFG_DESC_MAX_SIZE];
/**
  * @}
  */ 

/** @defgroup USBH_STDREQ_Exported_FunctionsPrototype
  * @{
  */
USBH_Status USBH_GetDescriptor(USB_OTG_CORE_HANDLE *pdev, 
                               USBH_HOST           *phost,                                
                               uint8_t  req_type,
                               uint16_t value_idx, 
                               uint8_t* buff, 
                               uint16_t length );

USBH_Status USBH_Get_DevDesc(USB_OTG_CORE_HANDLE *pdev,
                             USBH_HOST *phost,
                             uint8_t length);

USBH_Status USBH_Get_StringDesc(USB_OTG_CORE_HANDLE *pdev, 
                                USBH_HOST           *phost,                                 
                                uint8_t string_index, 
                                uint8_t *buff, 
                                uint16_t length);

USBH_Status USBH_SetCfg(USB_OTG_CORE_HANDLE *pdev, 
                        USBH_HOST *phost,
                        uint16_t configuration_value);

USBH_Status USBH_Get_CfgDesc(USB_OTG_CORE_HANDLE *pdev,
                             USBH_HOST           *phost,                                 
                             uint16_t length);

USBH_Status USBH_SetAddress(USB_OTG_CORE_HANDLE *pdev, 
                            USBH_HOST           *phost,                             
                            uint8_t DeviceAddress);

USBH_Status USBH_ClrFeature(USB_OTG_CORE_HANDLE *pdev,
                            USBH_HOST           *phost,                             
                            uint8_t ep_num, uint8_t hc_num); 

USBH_Status USBH_SetInterface(USB_OTG_CORE_HANDLE *pdev, 
                        USBH_HOST *phost,
                        uint8_t itf_idx, uint8_t altSetting);

USBH_Status USBH_Issue_ClrFeature(USB_OTG_CORE_HANDLE *pdev, 
                                  USBH_HOST           *phost, 
                                  uint8_t ep_num);

USBH_DescHeader_t      *USBH_GetNextDesc (uint8_t   *pbuf, 
                                                  uint16_t  *ptr);

USBH_Status USBH_SetDeviceFeature(USB_OTG_CORE_HANDLE *pdev, 
                                  USBH_HOST *phost,
                                  uint8_t FeatureSelector, uint16_t wIndex);

USBH_Status USBH_ClearDeviceFeature(USB_OTG_CORE_HANDLE *pdev, 
                                    USBH_HOST *phost,
                                    uint8_t FeatureSelector, uint16_t wIndex);

/**
  * @}
  */ 

#endif /* __USBH_STDREQ_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
* @}
*/ 


