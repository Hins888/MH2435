#include "bsp_api.h"
#include "api.h"

// static uint32_t osCoreclock;
#define RCC_SYSTIC_MODE_REG     1
#define RCC_SYSTIC_MODE_INT     2
#define RCC_SYSTIC_MODE_OS      3

#define RCC_SYSTIC_MODE RCC_SYSTIC_MODE_OS

extern uint32_t SystemCoreClock;

volatile uint64_t g_current_tick = 0;

static void rcc_clkConfiguration(void);

#if (RCC_SYSTIC_MODE == RCC_SYSTIC_MODE_REG)
static uint8_t    UsNumber       = 0;
//static uint16_t MsNumber = 0;

void bsp_rcc_init(void)
{
    RCC_ClocksTypeDef clocks;
 
    RCC_GetClocksFreq(&clocks);

    SystemCoreClock = clocks.SYSCLK_Frequency;

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    UsNumber = SystemCoreClock / 8000000;
    //MsNumber = (u16)UsNumber * 1000;
}

SRAM_FUNCTION void bsp_delayus(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * UsNumber;
    SysTick->VAL  = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    }
    while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
}

SRAM_FUNCTION void bsp_delayms(u32 nms)
{
    u32 retime = 0;

    retime = nms;
    while (retime--)
    {
        bsp_delayus(1000);
    }
}

#elif (RCC_SYSTIC_MODE == RCC_SYSTIC_MODE_INT)
void bsp_rcc_init(void)
{
    RCC_ClocksTypeDef clocks;

    RCC_GetClocksFreq(&clocks);
    SystemCoreClock = clocks.SYSCLK_Frequency;

    g_current_tick = 0;
    SysTick_Config(clocks.SYSCLK_Frequency / 1000); // 1ms
}

SRAM_FUNCTION void bsp_delayus(uint32_t nus)
{
    u32 ticks;
    u32 told, tnow, reload, tcnt = 0;

    reload = SysTick->LOAD;                     //获取重装载寄存器值
    ticks  = nus * (SystemCoreClock / 1000000); //计数时间值
    told   = SysTick->VAL;                      //获取当前数值寄存器值（开始时数值）

    while (1)
    {
        tnow = SysTick->VAL; //获取当前数值寄存器值
        if (tnow != told)    //当前值不等于开始值说明已在计数
        {
            if (tnow < told)
                tcnt += told - tnow; //当前值小于开始数值，说明未计到0
                                     //计数值=开始值-当前值
            else
                tcnt += reload - tnow + told; //当前值大于开始数值，说明已计到0并重新计数
                                              //计数值=重装载值-当前值+开始值(已从开始值计到0)
            told = tnow;                      //更新开始值
            if (tcnt >= ticks)
                break; //时间超过/等于要延迟的时间,则退出.
        }
    }
}

SRAM_FUNCTION void bsp_delayms(uint32_t nms)
{
    uint64_t temp;
    temp = g_current_tick;
    while ((g_current_tick - temp) < nms) {}
}

SRAM_FUNCTION uint32_t bsp_get_tick(void)
{
    return g_current_tick;
}

void SysTick_Handler(void)
{
    g_current_tick++;
    if (g_current_tick > 0xFFFFFFFFFFF)
        g_current_tick = 0;
}

#elif (RCC_SYSTIC_MODE == RCC_SYSTIC_MODE_OS)
#define SYSTICK_RATE_HZ         ((u32)configTICK_RATE_HZ) /*  */
#define SYSTICK_EACH_ADD        (1000 / SYSTICK_RATE_HZ) /*ms*/
/*****************************************************************************
 * Name : udelay
 * Function	: Millisecond delay.
 * ---------------------------------------------------------------------------
 * Input Parameters:
 * msec  Milliseconds of delay
 * Output Parameters:None
 * Return Value:none
 * ---------------------------------------------------------------------------
 * Description:
 *****************************************************************************/
SRAM_FUNCTION void bsp_delayus(uint32_t usec)
{
    u32 ticks;
    u32 told, tnow, reload, tcnt = 0;

    reload = SysTick->LOAD;                     //获取重装载寄存器值
    ticks  = usec * (SystemCoreClock / 1000000); //计数时间值
    told   = SysTick->VAL;                      //获取当前数值寄存器值（开始时数值）

    while (1)
    {
        tnow = SysTick->VAL; //获取当前数值寄存器值
        if (tnow != told)    //当前值不等于开始值说明已在计数
        {
            if (tnow < told)
                tcnt += told - tnow; //当前值小于开始数值，说明未计到0
                                     //计数值=开始值-当前值
            else
                tcnt += reload - tnow + told; //当前值大于开始数值，说明已计到0并重新计数
                                              //计数值=重装载值-当前值+开始值  （已从开始值计到0）
            told = tnow;                      //更新开始值
            if (tcnt >= ticks)
                break; //时间超过/等于要延迟的时间,则退出.
        }
    }
}

SRAM_FUNCTION void bsp_delayms(uint32_t msec)
{
    portTickType currentTick;
    portTickType futureTick;

    if (0 >= msec) return;

    currentTick = xTaskGetTickCount();
    futureTick  = pdMS_TO_TICKS(msec);

    if (futureTick <= 0) futureTick = 1;

    vTaskDelayUntil(&currentTick, futureTick);
}

u32 bsp_get_tickus(void)
{
    unsigned int tmp;
    unsigned int cnt;

    cnt = xTaskGetTickCount() * SYSTICK_EACH_ADD;
    tmp = SystemCoreClock / SYSTICK_RATE_HZ - SysTick->VAL;
    tmp = tmp / (SystemCoreClock / SYSTICK_RATE_HZ / 4000);
    tmp += cnt * 1000;

    return tmp;
}

u32 bsp_get_tick(void)
{
    return xTaskGetTickCount() * SYSTICK_EACH_ADD;
}

SRAM_FUNCTION void bsp_taskdelay(uint32_t msec)
{
    bsp_delayms(msec);
}

void bsp_rcc_init(void)
{
    //RCC_ClocksTypeDef clocks;
 
    //RCC_GetClocksFreq(&clocks);
    
    rcc_clkConfiguration();
    
    //SystemCoreClock = clocks.SYSCLK_Frequency;
 
    //SysTick_Config(SystemCoreClock /1000);
}
#endif

static void rcc_clkConfiguration(void)
{

}
