#define DEBUG

#include "drivers.h"
#include "app.h"
#include "api.h"
#include "sha1.h"
#include "netif/etharp.h"

//#include "defs.h"
#include "type.h"
//#include "types.h"

#include "sys_misc.h"

//#include "web_cfg.h"

#include "lwip/netif.h"
#include "test.h"

struct iot_work_struct  iot_work;


extern void handle_cmd(void);
extern int set_ipaddr(struct netif *p_netif, struct ip_addr *ipaddr, struct ip_addr *netmask, struct ip_addr *gw, struct ip_addr *dns) ;

/*
 *发送事件到主线程
 *
*/
void send_work_event(uint32_t event)        //释放二值信号量，唤醒对应的任务  
{
    iot_work.event_flag |= event;
    if(iot_work.event)
        wake_up(iot_work.event);
}

/*
 *主工作线程
 *
*/
extern void dev_monitor_task(void *arg);
void test_app_stop(void);
void test_app_runing(void);
void test_app_restart(void);

void main_process()
{
    uint32_t pending_event;
    uint32_t  test_app_tag = 0;
    //struct ethernetif *eth_state = (struct ethernetif *)p_eth_netif->state;
    
    p_dbg("main_process func enter");
    memset(&iot_work, 0, sizeof(struct iot_work_struct));
    iot_work.event = init_event();

    thread_create(dev_monitor_task, 0, TASK_MONITOR_PRIO, 0, TASK_MONITOR_STACK_SIZE, "monitor_thread");


    if(dev_netif_status(0) == 2)
    {
        p_dbg("****** network connection successful  *******");
        test_app_runing();
        test_app_tag = 1;
    }

    // 进入主循环
    while(1)
    {
        // 等待事件通知，1s超时
        wait_event_timeout(iot_work.event, 1000);
        pending_event = iot_work.event_flag;
        iot_work.event_flag = 0;

          if(pending_event & INTERFACE_MODE_SHIBIE)
          {
            p_dbg("****** 进入识别模式 *******");
              //rgb565_test();
            
          }

        if(pending_event & ETH_LINK_OK_EVENT)
        {
            p_dbg("****** network connection successful  *******");
            if(test_app_tag == 0)
            {
                test_app_runing();
                test_app_tag = 1;
            }
            else
            {
                test_app_restart();
                test_app_tag = 1;
            }
        }

        if(pending_event & ETH_LINK_OK_EVENT)
        {
            p_dbg("****** network disconnection  *******");
            test_app_stop();
            test_app_tag = 0;
        }
    }
}
