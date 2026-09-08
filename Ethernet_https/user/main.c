#define DEBUG

#include "bsp_api.h"
#include "drivers.h"
#include "app.h"
#include "api.h"

#include "dev_netif.h"
#include "web_cfg.h"
#include "test.h"

#define VIRSION     "V2.0"

extern void Debug_USART_Config(void);

void main_thread(void *pdata)
{
    //init Timer,Systick
    driver_misc_init();
    
    //init LED, Button, relays for ONENET
    usr_gpio_init();

#ifdef OS_UCOS
#ifdef UCOS_V3
    OSStatTaskCPUUsageInit((OS_ERR*)&ret);
#else
    OSStatInit();
#endif
#endif
    bsp_uart_init(115200);

    interface_display();

    //init a thread for timeout task 
    init_work_thread();

    //read the configurations 
    // load_web_cfg(&web_cfg);     //把flash里面对应的的参数读出来，放到全局变量web_cfg里面
    default_web_cfg(&web_cfg);     //把flash里面对应的的参数读出来，放到全局变量web_cfg里面

    //init LwIP kernel
    init_lwip();          //初始化  LWIP 协议栈   工作于多线程，不能基于回调去完成   同一接口，用socket编程

    dev_netif_init(0);

    misc_init();

    p_dbg("code version:%s", VIRSION);
    p_dbg("startup time:%d.%d S", os_time_get() / 1000, os_time_get() % 1000);
    p_dbg("mem pool1 remain:%d/%d", mem_get_free() , mem_get_size());
    //p_dbg("mem pool2 remain:%d/%d", mem_get_free2() , mem_get_size2());

    //bsp_get_random();
   // p_dbg("gen random test :%d", i);
//    init_sensers(); 
#if ONENET_ENABLE
    //update senser value automatically when Onenet enabled
    start_senser_poll();
#endif

//  web_server_init();

    main_process();
}

int main(void)
{
#ifdef DEBUG
    RCC_ClocksTypeDef RCC_ClocksStatus;
#endif
    
#ifdef OS_UCOS
#ifdef UCOS_V3
    OS_ERR err;
    OSInit(&err);
#else
    OSInit();
#endif
#endif
    
    bsp_rcc_init();
 
    Debug_USART_Config();

    //init memory configurations
    sys_mem_init();
    
    //msg_q_init();
    bsp_uart_init(115200);

#ifdef DEBUG
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    p_dbg("SYSCLK_Frequency:%d,HCLK_Frequency:%d,PCLK1_Frequency:%d,PCLK2_Frequency:%d,ADCCLK_Frequency:%d\n", 
        RCC_ClocksStatus.SYSCLK_Frequency, 
        RCC_ClocksStatus.HCLK_Frequency, 
        RCC_ClocksStatus.PCLK1_Frequency, 
        RCC_ClocksStatus.PCLK2_Frequency, 
        0);
#endif

    thread_create(main_thread, 0, TASK_MAIN_PRIO, 0, TASK_MAIN_STACK_SIZE, "main_thread");
    
#ifdef OS_UCOS
#ifdef UCOS_V3
    OSStart(&err);
#else
    OSStart();
#endif
#endif

#ifdef OS_FREE_RTOS
    vTaskStartScheduler();
#endif
    return 0;
}

/**********************
=================================================
任务名      任务状态 优先级   剩余栈 任务序号
printf_thread      R                1        318                2
IDLE               R                0        81                3
tcpip_thread       B                18        319                6
eth_int_thread     B                20        165                9
MOAL_WORK_QUEUE    B                22        66                7
main_thread        B                11        162                1
timer_tasklet      B                17        218                5
adc recv           B                10        955                10
AI audio client    B                6        1975                13
Tmr Svc            B                19        59                4
baidu AI client    B                5        417                12
woal_reassoc_se    B                15        315                8
img_snd            B                12        470                11





B:  BLOCK       阻塞
R:  READY       就绪
S:  SUSPENDED   挂起
D:  DELETE      删除

 printf("\r\n ****退出 img_send_thread *****\r\n");
 
 
**********************/



