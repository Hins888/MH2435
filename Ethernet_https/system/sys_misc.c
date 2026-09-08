#define DEBUG

#include "drivers.h"
#include "app.h"
#include "api.h"

#include "test.h"
#include "tinyalloc.h"

timer_t ind1_timer;
static u8 ind1_timer_del = 0;

#define IROM_SIZE  0x140000

extern char Image$$RW_IRAM1$$ZI$$Limit[];

void sys_mem_init(void)
{
    uint32_t malloc_start, malloc_size;

    malloc_start = (uint32_t) &Image$$RW_IRAM1$$ZI$$Limit; //取空闲内存最低地址
    malloc_start = (malloc_start + 3) & (~0x03);// ALIGN TO word

    // IROM_SIZE为板子内存总大小，更换MCU时需注意
    // malloc_size = 0x20000000 + IROM_SIZE - malloc_start;
    // @veis20230416,  sram size=1250KB
    malloc_size = 0x20000000 + IROM_SIZE - malloc_start;
    sys_meminit((void*)malloc_start, malloc_size);
    //ta_init((const void *)(uint32_t *)malloc_start, (const void *)(uint32_t *)(malloc_start + malloc_size), 256, 16, 8);
    //sys_meminit((void*)malloc_start, malloc_size);

}

uint8_t led_value = 0;
uint8_t led_bright_value = 10;

void led_switch(uint8_t value)
{
    p_dbg("led_switch:%x", value);
    led_value = value;
    
}

//不再支持亮度
void led_bright(uint8_t value)
{
    p_dbg("led_bright:%d", value);
    if (value > 99)
        value = 99;

    led_bright_value = value / 10;
}


SWITCH_EVENT g_switch = SWITCH_EVENT_OFF;
void button_stat_callback()
{
    static uint8_t last_stat = 1;
    BUTTON_EVENT button_event = BUTTON_EVENT_IDLE;

    if (BUTTON_STAT && !last_stat)
    {
        last_stat = BUTTON_STAT;
        button_event = BUTTON_EVENT_UP;
        //p_dbg("button up");
    }

    if (!BUTTON_STAT && last_stat)
    {
        last_stat = BUTTON_STAT;
        button_event = BUTTON_EVENT_DOWN;
        //p_dbg("button down");
    }

    if (button_event == BUTTON_EVENT_UP)
    {
        if(g_switch == SWITCH_EVENT_OFF)
            g_switch = SWITCH_EVENT_ON;
        else
            g_switch = SWITCH_EVENT_ON;
#if SUPPORT_AUDIO
        //camera_button_event(g_switch);
        audio_button_event(g_switch);
#endif
    }
}


void assert_failed(uint8_t *file, uint32_t line)
{
    p_err("assert_failed in:%s,line:%d \n", file ? file : "n", line);
    while (1)
        ;
}
void ind2_status_set(int on)
{
    
}

void ind2_timer_callback(void *arg)
{
    
}

void ind1_timer_callback(void *arg)
{


}

void indicate_led_twink_start(int fre)
{
    if(!ind1_timer_del)
    {
        ind1_timer_del = 1;
        ind1_timer = timer_setup(10000, 1, ind1_timer_callback, NULL);
        add_timer(ind1_timer);
    }
    mod_timer(ind1_timer, fre);    
}

void indicate_led_twink_stop()
{
    ind1_timer_del = 1;
    del_timer(ind1_timer);
}

/**
 * @brief 定义两个定时器用于led的闪烁
 */
void misc_init()
{
    //indicate_led_twink_start(10000);

}

void show_sys_info(struct netif *p_netif)
{
    //p_dbg("mac:%02x-%02x-%02x-%02x-%02x-%02x", p_netif->hwaddr[0], p_netif->hwaddr[1], p_netif->hwaddr[2], p_netif->hwaddr[3], p_netif->hwaddr[4], p_netif->hwaddr[5]);
    show_tcpip_info(p_netif);
}

void soft_reset()
{
    p_err("system well reset\n");
    NVIC_SystemReset();
      __DSB();  
    while (1);
}


#ifdef OS_UCOS
#if OS_APP_HOOKS_EN > 0u
void App_TaskCreateHook(OS_TCB *ptcb)
{
    ptcb = ptcb;
}

void App_TaskDelHook(OS_TCB *ptcb)
{
    ptcb = ptcb;
}

void App_TaskReturnHook(OS_TCB *ptcb)
{
    ptcb = ptcb;
}

void App_TCBInitHook(OS_TCB *ptcb)
{
    ptcb = ptcb;
}

void App_TaskSwHook(void){

}

void App_TimeTickHook(void){}

//uC/OS-II Stat线程中调用此函数，每100MS一次
void App_TaskStatHook()
{
    #if USE_MEM_DEBUG
    mem_slide_check(0);
    #endif
    //button_stat_callback();
}

#endif
#endif

#ifdef OS_FREE_RTOS
void vApplicationIdleHook( void )
{
    #if USE_MEM_DEBUG
    mem_slide_check(0);
    #endif
}
#endif

extern uint32_t drvdbg;
extern uint32_t drvdbg_save;
void switch_dbg(void)
{
    if(dbg_level)
    {
        p_dbg("close dbg");
    }
    dbg_level = !dbg_level;
    if(dbg_level)
    {
        p_dbg("open dbg");
#ifdef DEBUG_LEVEL1
        drvdbg = drvdbg_save;
#endif
    }
#ifdef DEBUG_LEVEL1    
    else
        drvdbg = 0;
#endif
}

void usr_gpio_init()
{
	//BUTTON status
    bsp_gpio_config((uint32_t)BUTTON_GPIO, GPIO_Mode_IPU, GPIO_AF_GPIO);
    
    //LED2
    bsp_gpio_config((uint32_t)IND1_GPIO, GPIO_Mode_Out_PP, GPIO_AF_GPIO);
    //init the relay on board
}

void update_relay_status(int relay, int value)
{

}

//get button status
int get_alarm_value()
{
    int stat = GPIO_STAT(GPIOE, GPIO_Pin_14); //读取按键状态(0:1)
    //p_dbg("current BUTTON VALUE: [%d]", flg); //打印按键状态
    return stat;
}
