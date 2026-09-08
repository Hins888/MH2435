/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "mh2435.h"

#include "rtos-freertos.h"
#include "lvgl_config.h"

/*********************
 *      DEFINES
 *********************/

lv_color_t DisplayData1[MY_DISP_HOR_RES * MY_DISP_VER_RES] __attribute__((section(".bss.ARM.__at_0x60000000")));
lv_color_t DisplayData2[MY_DISP_HOR_RES * MY_DISP_VER_RES] __attribute__((section(".bss.ARM.__at_0x60300000")));// 1280*800*3

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    disp_init();

    static lv_disp_draw_buf_t draw_buf_dsc_1;
	
	lv_disp_draw_buf_init(&draw_buf_dsc_1, DisplayData1, DisplayData2, MY_DISP_HOR_RES * MY_DISP_VER_RES);   /*Initialize the display buffer*/
    
    static lv_disp_drv_t disp_drv;                  /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/
    
    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;
	
	
    disp_drv.direct_mode = 1;
    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;
    
    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_1;
    
    lv_disp_drv_register(&disp_drv);
}


/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
	DisplayStruct* displayPort = NULL;
	displayPort = &DPI_display;
	DPIPanelConstractor(displayPort,(uint32_t)DisplayData2);
    LTDC_Config(displayPort);
	
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

volatile lv_color_t * Color_p;
volatile uint8_t isFlush = 0;


#include "bsp_led/bsp_led.h"
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint16_t lines;
    LOG_DEBUG("*%d,%d  %d,%d %p \r\n",area->x1,area->y1,area->x2,area->y2,color_p);

    if(disp_flush_enabled) {
        if (lv_disp_flush_is_last(disp_drv)) {
            Color_p = color_p;
            isFlush = 1;
			
            lines = LTDC->CPSR & 0xFFFF;
            
            while(isFlush)
			{
				vTaskDelay(2);
			}
        }
        lv_disp_flush_ready(disp_drv);
    }
    else 
    {
        lv_disp_flush_ready(disp_drv);

    }
}

void LTDC_IRQHandler(void)
{
    if (LTDC_GetITStatus(LTDC_IT_FU) == SET)
    {
        uint32_t cr = DMA2D->CR;
        LOG_WARN("LTDC FU .\r\n");
        LOG_WARN("DMA2D_CR = 0x%08X.\r\n", cr);
        LTDC_ClearITPendingBit(LTDC_IT_FU);
    }
    
    if (LTDC_GetITStatus(LTDC_IT_LI) == SET)
    {
        if (disp_flush_enabled)
        {
            if (isFlush == 1)
            {
                LTDC_Layer1->CFBAR = (uint32_t)(&Color_p->full);
                LTDC->SRCR |= LTDC_SRCR_VBR;
                isFlush = 0;
            }
        }
        
        LTDC_ClearITPendingBit(LTDC_IT_LI);
    }
    
    if (LTDC_GetITStatus(LTDC_IT_TERR) == SET)
    {
        LOG_WARN("LTDC TERR .\r\n");
        LTDC_ClearITPendingBit(LTDC_IT_TERR);
    }
}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
