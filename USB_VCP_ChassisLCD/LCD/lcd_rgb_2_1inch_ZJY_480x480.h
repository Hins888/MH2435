#ifndef __DIP_ZJY_480_480_H__
#define __DIP_ZJY_480_480_H__

#include <stdio.h>
#include "Display.h"
#if (USE_480_480_565 == USE_LCD_TYPE )

#define DPI_DISPLAY_COLOR		DisplayColorRGB565
#define DPI_USE_SDRAM			0
#define CONFIG_DPI_IO_LIST                                                          \
    ((IOEnum[]) {                                                                   \
        PG7, PI10, PI9,  PF10,                         /* Sync Pins: CK HS VS DE */ \
        PG6, PH12, PH11, PH10, PH9,  IONone,  IONone,  IONone, /* R Pins: R7-R0          */ \
        PI2, PI1,  PI0,  PH15, PH14, PH13, IONone,  IONone,  /* G Pins: R7-R0          */ \
        PI7, PI6,  PI5,  PI4,  PG11, IONone, IONone, IONone, /* B Pins: R7-R0          */ \
    })
	
#ifndef CONFIG_DPI_IO_COUNT
#define CONFIG_DPI_IO_COUNT (sizeof(CONFIG_DPI_IO_LIST) / sizeof(IOEnum))
#endif

#define DPI_BACKLIGHT_IO PA5


#define DPI_HSA  20
#define DPI_HBP  40
#define DPI_HFP  80
#define DPI_HACT 480	

#define DPI_VSA  4
#define DPI_VBP  18
#define DPI_VFP  22
#define DPI_VACT 480

#define DPI_PIXEL_CLOCK	20000	//kHz
//#define DPI_DISPLAY_ROTATION 	DISPLAT_ROTATION_0		//Try to avoid software rotation



#ifndef DPI_RESET_IO
#define DPI_RESET_IO IONone
#endif

#ifndef DPI_BACKLIGHT_IO
#define DPI_BACKLIGHT_IO IONone
#endif
	
#define MY_DISP_HOR_RES 	DPI_HACT
#define MY_DISP_VER_RES 	DPI_VACT

#define DEBUG_SPI_SCK_PIN                   GPIO_Pin_13                  
#define DEBUG_SPI_SCK_GPIO_PORT             GPIOB                       
#define DEBUG_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB

#define DEBUG_SPI_MOSI_PIN                  GPIO_Pin_15               
#define DEBUG_SPI_MOSI_GPIO_PORT            GPIOB                     
#define DEBUG_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB

#define DEBUG_CS_PIN                        GPIO_Pin_12              
#define DEBUG_CS_GPIO_PORT                  GPIOB                     
#define DEBUG_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOB

#define SPI_SCK_LOW()					{DEBUG_SPI_SCK_GPIO_PORT->BSRRH = DEBUG_SPI_SCK_PIN;}
#define SPI_SCK_HIGH()					{DEBUG_SPI_SCK_GPIO_PORT->BSRRL = DEBUG_SPI_SCK_PIN;}

#define SPI_MOSI_LOW()					{DEBUG_SPI_MOSI_GPIO_PORT->BSRRH = DEBUG_SPI_MOSI_PIN;}
#define SPI_MOSI_HIGH()					{DEBUG_SPI_MOSI_GPIO_PORT->BSRRL = DEBUG_SPI_MOSI_PIN;}

#define SPI_CS_LOW()					{DEBUG_CS_GPIO_PORT->BSRRH = DEBUG_CS_PIN;}
#define SPI_CS_HIGH()					{DEBUG_CS_GPIO_PORT->BSRRL = DEBUG_CS_PIN;}


#if defined(DPI_BACKLIGHT_IO) && !defined(DPI_BACKLIGHT_ON)
#define DPI_BACKLIGHT_ON(isOn) IOSet(DPI_BACKLIGHT_IO, isOn)
#endif

	
bool DPIPanelConstractor(DisplayStruct* display,uint32_t Pointer);
	
#endif

#endif  /* __DIP_ZJY_480_480_H__ */

