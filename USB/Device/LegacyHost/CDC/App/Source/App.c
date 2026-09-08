#include "app.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST USB_Host               __ALIGN_END;

int main(void) {
    RetargetIOSetup(CONFIG_RETARGETIO_DEFAULT_SERIAL);

    printf("LegacyHost CDC Demo\n");

    /* Init Host Library */
    USBH_Init(&USB_OTG_Core,
#ifdef USE_USB_OTG_FS
              USB_OTG_FS_CORE_ID,
#else
              USB_OTG_HS_CORE_ID,
#endif
              &USB_Host, &CDC_cb, &USR_Callbacks);

    while (1) {
        /* Host Task handler */
        USBH_Process(&USB_OTG_Core, &USB_Host);

#if CONFIG_USBCDC_SERVICE_READ_IN_APP
        uint32_t       readLength = VCPHostReadBufferSize(false);
        static uint8_t buffer[256];
        if (readLength) {
            readLength = VCPHostReadBytes(buffer, sizeof(buffer));
            printf("Read %d bytes in application from CDC buffer\n", readLength);
            // data in buffer with readLength could be used
            // ...
        }
#endif
    }
}

#ifdef USE_USB_OTG_FS
void USB1_IRQHandler(void)
#else
void USB2_IRQHandler(void)
#endif
{
    USBH_OTG_ISR_Handler(&USB_OTG_Core);
}
