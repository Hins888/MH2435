#include "app.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST USB_Host               __ALIGN_END;
static uint32_t                                uart_port = PeripheralUSART1;

int main(void) {
    RetargetIOSetup(uart_port, PB6, PB7);

    printf("usb host hid test v1.0\n\r");
    USBH_Init(&USB_OTG_Core,
#ifdef USE_USB_OTG_FS
              USB_OTG_FS_CORE_ID,
#else
              USB_OTG_HS_CORE_ID,
#endif
              &USB_Host, &HID_cb, &USR_Callbacks);

    while (1) {
        USBH_Process(&USB_OTG_Core, &USB_Host);
    }
}

void put_char(uint8_t c) {

    while (!USART_GetFlagStatus((USART_TypeDef*)(PeripheralMap + uart_port)->Base, USART_FLAG_TXE)) {}
    USART_SendData((USART_TypeDef*)(PeripheralMap + uart_port)->Base, c);
}

#ifdef USE_USB_OTG_FS
void USB1_IRQHandler(void)
#else
void USB2_IRQHandler(void)
#endif
{
    USBH_OTG_ISR_Handler(&USB_OTG_Core);
}
