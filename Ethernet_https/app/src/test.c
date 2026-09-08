#define DEBUG

#include "drivers.h"
#include "app.h"
#include "api.h"

#include "test.h"
#include "web_cfg.h"
#include "os_socket.h"


u8_t  command[100] = {0,};
int cmd_index = 0;
static int test_app_work = 0;

void handle_cmd(void)
{
    //sprintf(cmd_str, "user cmd:[%02x]", cmd);
}

void test_app_thread(void *arg)
{
    while(1)
    {
        //mbedtls_demo();

        //test_http();

        sslLoopTest();

        //tcpLoopTest(SOCKET_PRT_TCP, NULL);
    }
}
void test_app_restart(void)
{
    test_app_work = 1;
}

void test_app_stop(void)
{
    test_app_work = 0;
}

/*
 * @brief 满负荷发送，请先建立TCP连接
 * 建立单独的线程
 *
 */
void test_app_runing(void)
{
    p_dbg_enter;
    test_app_work = 1;
    thread_create(test_app_thread, 0, TASK_TCP_SEND_PRIO, 0, TASK_TCP_SEND_STACK_SIZE, "test_app_thread");
    p_dbg_exit;
}
