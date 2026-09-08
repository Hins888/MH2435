/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_BSP__H__
#define __USB_BSP__H__

/* Includes ------------------------------------------------------------------*/
#include "usb_core.h"
#include "usb_hcd_int.h"
#include "stdio.h"

/** @addtogroup USB_OTG_DRIVER
 * @{
 */

/** @defgroup USB_BSP
 * @brief This file is the
 * @{
 */

/** @defgroup USB_BSP_Exported_Defines
 * @{
 */

#ifndef USB_IO_CONFIG_DATA
#define USB_IO_CONFIG_DATA MakeIOConfig(IOModeAlternate, GPIO_AF_USB, IOPullNone, IOSpeedHigh, IODriveHigh)
#endif

#ifndef USB_IO_CONFIG_ID
#define USB_IO_CONFIG_ID MakeIOConfig(IOModeAlternate, GPIO_AF_USB, IOPullUp, IOSpeedMedium, IODriveMedium)
#endif

#ifndef USB_IO_CONFIG_VBUS
#define USB_IO_CONFIG_VBUS MakeIOConfig(IOModeInput, 0, IOPullNone, IOSpeedMedium, IODriveMedium)
#endif
/**
 * @}
 */

/** @defgroup USB_BSP_Exported_Types
 * @{
 */
/**
 * @}
 */

/** @defgroup USB_BSP_Exported_Macros
 * @{
 */
/**
 * @}
 */

/** @defgroup USB_BSP_Exported_Variables
 * @{
 */
/**
 * @}
 */

/** @defgroup USB_BSP_Exported_FunctionsPrototype
 * @{
 */
void BSP_Init(void);

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE* pdev);
void USB_OTG_BSP_uDelay(const uint32_t usec);
void USB_OTG_BSP_mDelay(const uint32_t msec);
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE* pdev);
void USB_OTG_BSP_TimerIRQ(void);
#ifdef USE_HOST_MODE
void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE* pdev);
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE* pdev, uint8_t state);
void USB_OTG_BSP_Resume(USB_OTG_CORE_HANDLE* pdev);
void USB_OTG_BSP_Suspend(USB_OTG_CORE_HANDLE* pdev);

#endif /* USE_HOST_MODE */
/**
 * @}
 */

#endif /* __USB_BSP__H__ */

/**
 * @}
 */

/**
 * @}
 */
