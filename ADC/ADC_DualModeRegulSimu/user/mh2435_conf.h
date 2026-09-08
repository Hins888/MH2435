
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MH2435_CONF_H
#define __MH2435_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment the line below to enable peripheral header file inclusion */
#include "mh2435_adc.h"
#include "mh2435_can.h"
#include "mh2435_crc.h"
#include "mh2435_dac.h"
#include "mh2435_dma.h"
#include "mh2435_exti.h"
#include "mh2435_gpio.h"
#include "mh2435_i2c.h"
#include "mh2435_iwdg.h"
#include "mh2435_pwr.h"
#include "mh2435_qspi.h"
#include "mh2435_sysctrl.h"
#include "mh2435_trng.h"
#include "mh2435_rtc.h"
#include "mh2435_sdio.h"
#include "mh2435_spi.h"
#include "mh2435_tim.h"
#include "mh2435_usart.h"
#include "mh2435_wwdg.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* If an external clock source is used, then the value of the following define
   should be set to the value of the external clock source, else, if no external
   clock is used, keep this define commented */
/*#define I2S_EXTERNAL_CLOCK_VAL   12288000 */ /* Value of the external clock in Hz */


/* Uncomment the line below to expanse the "assert_param" macro in the
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed.
  *   If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif  /* USE_FULL_ASSERT */

#endif  /* __MH2435_CONF_H */

/************************ (C) COPYRIGHT Megahunt *****END OF FILE****/

