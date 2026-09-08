#define DEBUG

#include "drivers.h"
#include "app.h"
#include "api.h"

//#include <absacc.h>

void driver_gpio_init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); //外部中断需要用到
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | 
        RCC_AHB1Periph_GPIOB | 
        RCC_AHB1Periph_GPIOC | 
        RCC_AHB1Periph_GPIOD | 
        RCC_AHB1Periph_GPIOE | 
        RCC_AHB1Periph_GPIOF | 
        RCC_AHB1Periph_GPIOH | 
        RCC_AHB1Periph_GPIOI, ENABLE);
}

int check_rst_stat()
{
    uint32_t stat;
    stat = RCC->CSR;
    RCC->CSR = 1L << 24; //清除复位标志

    p_err("reset:");
    if (stat &(1UL << 31))
    // 低功耗复位
    {
        p_err("low power\n");
    }
    if (stat &(1UL << 30))
    //窗口看门狗复位
    {
        p_err("windw wdg\n");
    }
    if (stat &(1UL << 29))
    //独立看门狗复位
    {
        p_err("indep wdg\n");
    }
    if (stat &(1UL << 28))
    //软件复位
    {
        p_err("soft reset\n");
    }
    if (stat &(1UL << 27))
    //掉电复位
    {
        p_err("por reset\n");
    }
    if (stat &(1UL << 26))
    //rst复位
    {
        p_err("user reset\n");
    }

    return 0;
}


void driver_misc_init()
{
    driver_gpio_init();
}

/**
 * @brief  Gets Time from RTC 
 * @param  None
 * @retval Time in DWORD
 */
DWORD get_fattime(void)
{
    return os_time_get();
}
