#include "ethconf.h"
#include "lwip/tcpip.h"
#include "netconf.h"
#include "usart.h"

#if !NO_SYS
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference
                                incremented by 10ms */

static TaskHandle_t AppTaskCreate_Handle = NULL; /* 创建任务句柄 */
static TaskHandle_t Test1_Task_Handle    = NULL; /* Test1句柄 */
static TaskHandle_t Test2_Task_Handle    = NULL; /* Test2句柄 */
// static TaskHandle_t Netif_Dev_Task_Handle = NULL;/* Test2句柄 */
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void); /* 用于创建任务 */

static void Test1_Task(void* pvParameters); /* Test1_Task任务实现 */
static void Test2_Task(void* pvParameters); /* Test2_Task任务实现 */
#endif

#define PRINT_DEBUG printf

extern __IO uint8_t  EthLinkStatus;
extern __IO uint32_t EthStatus;

extern void tcp_client_test(void);

#if LWIP_DHCP
extern int8_t lwip_get_dhcp_status(void);
void dhcp_handle()
{
    while (1)
    {
#if NO_SYS
        /* check if any packet received */
        if (ETH_CheckFrameReceived())
        {
            /* process received ethernet packet */
            LwIP_Pkt_Handle();
            // printf("recv packet \r\n");
        }
        /* handle periodic timers for LwIP */
        LwIP_Periodic_Handle(LocalTime);
#else
        LwIP_DHCP_Process_Handle();
        vTaskDelay(5);
#endif
        if (lwip_get_dhcp_status() == 0)
        {
            printf("  dhcp end...\n");
            break;
        }
        // ping_timer_check();
        // heart_tick_update();
    }
}
#endif

int main(void)
{
    RCC_ClocksTypeDef RCC_ClocksStatus;
    
#if !NO_SYS
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为pdPASS */
#endif
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    USART_Configuration(115200);

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    printf("ETH Demo.\n");
    printf("****CPU: %3.1fMhz, HCLK: %3.1fMhz, PCLK: %3.1fMhz*****\n", (float)RCC_ClocksStatus.SYSCLK_Frequency / 1000000,
           (float)RCC_ClocksStatus.HCLK_Frequency / 1000000, (float)RCC_ClocksStatus.PCLK1_Frequency / 1000000);

    // printf("start lwip test \n");
#if !NO_SYS
    /* 创建AppTaskCreate任务 */
    xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,         /* 任务入口函数 */
                          (const char*)"AppTaskCreate",          /* 任务名字 */
                          (uint16_t)512,                         /* 任务栈大小 */
                          (void*)NULL,                           /* 任务入口函数参数 */
                          (UBaseType_t)1,                        /* 任务的优先级 */
                          (TaskHandle_t*)&AppTaskCreate_Handle); /* 任务控制块指针 */
    /* 启动任务调度 */
    if (pdPASS == xReturn)
        vTaskStartScheduler(); /* 启动任务，开启调度 */
    else
        return -1;
#endif

    while (1)
        ; /* 正常不会执行到这里 */
}

#if !NO_SYS
/***********************************************************************
 * @ 函数名  ： AppTaskCreate
 * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
 * @ 参数    ： 无
 * @ 返回值  ： 无
 **********************************************************************/
static void AppTaskCreate(void)
{
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为pdPASS */

    printf("start main task... \n");

    /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
    ETH_BSP_Config();
    if (!(EthStatus & ETH_INIT_FLAG))
    {
        printf("no network\n");
    }

    /* Initilaize the LwIP stack */
    printf("lwip tcpip...\n");

    tcpip_init(NULL, NULL);

    LwIP_Init();
    if (!(EthStatus & ETH_INIT_FLAG))
    {
        printf("no network\n");
    }

    printf("本例程演示开发板发送数据到服务器\n\n");

    printf("网络连接模型如下：\n\t 电脑<--网线-->路由<--网线-->开发板\n\n");

    printf("实验中使用TCP协议传输数据，电脑作为TCP Server，开发板作为TCP "
           "Client\n\n");

    printf("本例程的IP地址均在对应的测试文件中修改\n\n");

    taskENTER_CRITICAL(); // 进入临界区

    /* 创建Test1_Task任务 */
    xReturn = xTaskCreate((TaskFunction_t)Test1_Task,         /* 任务入口函数 */
                          (const char*)"Test1_Task",          /* 任务名字 */
                          (uint16_t)512,                      /* 任务栈大小 */
                          (void*)NULL,                        /* 任务入口函数参数 */
                          (UBaseType_t)1,                     /* 任务的优先级 */
                          (TaskHandle_t*)&Test1_Task_Handle); /* 任务控制块指针 */
    if (pdPASS == xReturn)
        PRINT_DEBUG("Create Test1_Task sucess...\r\n");

    /* 创建Test2_Task任务 */
    xReturn = xTaskCreate((TaskFunction_t)Test2_Task,         /* 任务入口函数 */
                          (const char*)"Test2_Task",          /* 任务名字 */
                          (uint16_t)512,                      /* 任务栈大小 */
                          (void*)NULL,                        /* 任务入口函数参数 */
                          (UBaseType_t)2,                     /* 任务的优先级 */
                          (TaskHandle_t*)&Test2_Task_Handle); /* 任务控制块指针 */
    if (pdPASS == xReturn)
        PRINT_DEBUG("Create Test2_Task sucess...\n\n");

    vTaskDelete(AppTaskCreate_Handle); // 删除AppTaskCreate任务

    taskEXIT_CRITICAL(); // 退出临界区
}

/**********************************************************************
 * @ 函数名  ： Test1_Task
 * @ 功能说明： Test1_Task任务主体
 * @ 参数    ：
 * @ 返回值  ： 无
 ********************************************************************/
void dns_thread(void *arg);
static void Test1_Task(void* parameter)
{
    printf("start %s task ...\n", __func__);
#if LWIP_DHCP
    dhcp_handle();
    printf("dhcp end %s start tcpip...\n", __func__);
#endif

    while (1)
    {
        // LED2_TOGGLE;
        //  PRINT_DEBUG("LED2_TOGGLE\n");
        dns_thread(NULL);
        vTaskDelay(20); /* 延时2000个tick */
    }
}

/**********************************************************************
 * @ 函数名  ： Test2_Task
 * @ 功能说明： Test2_Task任务主体
 * @ 参数    ：
 * @ 返回值  ： 无
 ********************************************************************/
static void Test2_Task(void* parameter)
{
    printf("start %s task ...\n", __func__);
    while (1)
    {
        // LED1_TOGGLE;
        //  PRINT_DEBUG("LED1_TOGGLE\n");
        vTaskDelay(1000); /* 延时1000个tick */
    }
}
#endif

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */

    /* Infinite loop */
    while (1) {}
}
#endif
