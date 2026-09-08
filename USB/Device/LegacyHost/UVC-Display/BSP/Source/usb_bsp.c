#include "usb_bsp.h"

/**
  * @brief  USB_OTG_BSP_Init
  *         Initializes BSP configurations
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE * pdev)
{
    #ifdef USE_USB_OTG_FS
    IOSetup(PA11, USB_IO_CONFIG_DATA);
    IOSetup(PA12, USB_IO_CONFIG_DATA);
    IOSetup(PA9, USB_IO_CONFIG_VBUS);
    IOSetup(PA10, USB_IO_CONFIG_ID);
    #else
    IOSetup(PB12, USB_IO_CONFIG_ID);
    #endif
	
    IOSetup(PC13, IO_DEFAULT_OUTPUT_CONFIG);
    IOH(PC13);	
    
    if ((ClockGet(ClockNodeREF) / 1000000 != 48)) {
        ClockMultiply(ClockNodePLL3, ClockRatio(867.84 / 12.0));
        ClockDivide(ClockNodePLL3R, ClockRatio(867.84 / 48.0));
        ClockSelect(ClockNodeREF, ClockNodePLL3R);
        ClockEnable(ClockNodePLL3G, true);
    }
    
    #ifdef USE_USB_OTG_FS
    PeripheralEnable(PeripheralUSB1, true);
    #else
    PeripheralEnable(PeripheralUSB2, true);
    #endif
}

/**
  * @brief  USB_OTG_BSP_EnableInterrupt
  *         Configures USB Global interrupt
  * @param  None
  * @retval None
  */
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE * pdev)
{
    
    NVIC_SetPriority(USB1_IRQn, 0);
    NVIC_SetPriority(USB2_IRQn, 1);
    NVIC_DisableIRQ(USB1_IRQn);
    NVIC_DisableIRQ(USB2_IRQn);
    
    #ifdef USE_USB_OTG_FS
    NVIC_EnableIRQ(USB1_IRQn);
    #else
    NVIC_EnableIRQ(USB2_IRQn);
    #endif
}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE * pdev, uint8_t state)
{
}

/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE * pdev)
{
}

/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay(const uint32_t usec)
{
    SystemDelayUs(usec);
}


/**
  * @brief  USB_OTG_BSP_mDelay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
void USB_OTG_BSP_mDelay(const uint32_t msec)
{
    SystemDelay(msec);
}
