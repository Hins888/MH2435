#define DEBUG

#include "drivers.h"
#include "app.h"
#include "api.h"

#include "lwip\sockets.h"
#include "lwip\netif.h"
#include "lwip\dns.h"
#include "lwip\api.h"
#include "lwip\tcp.h"

#include "type.h"

#include "tcpapp.h"
#include "util_debug.h"
#include "lwip/dhcp.h"
#include "sys_misc.h"

#include "os_socket.h"

#include "test_data.h"


//全局变量声明
extern int errno; //lwip错误号

/*
 * socket互斥锁，对socket的所有线程外的操作需要上锁
 * 如果不加锁，lwip会很不稳定，原因是lwip不支持多线程同时操作一个socket
 * 需要注意的是像send这样可能会阻塞的函数，如果加了锁将会降低数据
 * 收发的效率，所以收发数据时采用非阻塞方式要好些(MSG_DONTWAIT)
 */
extern mutex_t socket_mutex;

//函数声明
void test_tcp_recv(void *_fd);

//摄像头服务器socket
int server_socket_fd =  - 1;
//记录客户端的连接，用于向客户端发送摄像头数据
int remote_socket_fd =    - 1;

struct sockaddr udp_remote_client __attribute__((aligned(4))); //用于保存远程udp客户端信息

//thread 句柄
int server_accept_thread_fd =  - 1;

/*
 * @brief  连接到IP地址为192.168.0.100，端口号4700的TCP服务器
 *  连接成功后发送数据并关闭连接
 */
void test_tcp_client()
{
    char *ip = "192.168.10.26";
    char *data = "hello, Im a TCP CLIENT";
    uint16_t port = 4700;
    int fd = -1;
    p_dbg_enter;
    
    p_dbg("TCP connect ip addr %s, port:%d", ip, port);

    fd = tcp_client_create(ip, port);
    if(fd > 0)
    {
        send(fd, data, strlen(data), 0);
        close(fd);
        fd = -1;
    }
    p_dbg_exit;
}

/*
 * @brief  连接到IP地址为192.168.0.100，端口号4701的UDP服务器
 * 连接成功后，发送数据并关闭连接
 */
void test_udp_client()
{
    char *ip = "192.168.10.36";
    char *data = "hello, Im an UDP CLIENT";
    uint16_t port = 4701;
    int fd = -1;
    p_dbg_enter;
    p_dbg("UDP ip addr %s, port:%d", ip, port);

    fd = udp_client_create(ip, port);
    if(fd > 0)
    {
            send(fd, data, strlen(data), 0);
            close(fd);
            fd = -1;
    }
    
    p_dbg_exit;
}


/*
 *这里测试发送udp数据到远程端
 *这里新建一个socket并发送数据到192.168.1.101:4703
 *未绑定远端端口，使用sendto发送数据
 */
void test_udp_client2(char *pstr)
{
    int fd;
    struct sockaddr_in addr;
    int len = strlen(pstr);
    
    p_dbg_enter;

    p_dbg("enter %s\n", __FUNCTION__);
    addr.sin_family = AF_INET;
    addr.sin_len = sizeof(struct sockaddr_in);
    addr.sin_port = htons(4703);
    addr.sin_addr.s_addr = inet_addr("192.168.10.101");

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        p_err("get socket err:%d", errno);
        return;
    } 

    mutex_lock(socket_mutex);
    len = sendto(fd, (u8*)pstr, len, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr));
    mutex_unlock(socket_mutex);
    p_dbg("ret:%d", len);
    close(fd);
    
    p_dbg_exit;
}

/*
 * @brief  添加到多播组
 * 组播地址为224.0.0.2，端口号4702
 */
void test_multicast_send_data(int udp_fd);
void test_multicast_join()
{
    //加入到这个多播组，可以接收这个多播组的所有数据
    char *ip = "239.118.0.0";
    uint16_t port = 10000;
    int udp_fd = -1;
    p_dbg_enter;
    
    p_dbg("multicast ip addr %s, port:%d", ip, port);

    udp_fd = udp_add_membership(ip, port);

    if(udp_fd != -1)
        test_multicast_send_data(udp_fd);
    //TODO:这里也可以修改程序，尝试接收UDP多播包

    
    close(udp_fd);//测试完成，关闭连接
    p_dbg_exit;
}

/*
 * @brief 像多播组发送数据
 * 组播地址为224.0.0.2，端口号4702
 */
void test_multicast_send_data(int udp_fd)
{
    //发送数据到一个多播组，UDP客户端可以发送任何数据到任何多播组
    char *ip = "224.0.0.2";
    uint16_t port = 4702;
    struct sockaddr_in multisnd_addr;
        
    p_dbg_enter;
    p_dbg("send data to ip:%s, port:%d", ip, port);

    if(udp_fd == -1)
    {
        p_err("udp socket not create");
        return ;
    }
    memset(&multisnd_addr, 0, sizeof(struct sockaddr_in));
    multisnd_addr.sin_len = sizeof(struct sockaddr);
    multisnd_addr.sin_family = AF_INET;
    multisnd_addr.sin_port=htons(port);
    multisnd_addr.sin_addr.s_addr=inet_addr(ip);
    mutex_lock(socket_mutex);
    sendto(udp_fd, "this is test data", 20, 0,(struct sockaddr*)&multisnd_addr,sizeof(struct sockaddr));
    mutex_unlock(socket_mutex);
    p_dbg_exit;
}


DECLARE_MONITOR_ITEM("tcp totol send", tcp_totol_send);

/*
 * @brief TCP数据发送线程
 * tcp_send_stop用于用户控制结束测试
 *
 */
int tcp_send_stop = 0;
#define TEST_PACKET_SIZE     1024
void tcp_send_thread(void *arg)
{
    int i;
    char *send_buff;
    char *ip = "192.168.10.26";
    uint16_t port = 4700;
    int fd = -1;

    send_buff = (char*)malloc(TEST_PACKET_SIZE);
    if(!send_buff) goto end;
    
    fd = tcp_client_create(ip, port);
    
    if(fd == -1){
        p_err("请先与服务器建立TCP连接");
        goto end;
    }

    for(i = 0; i < TEST_PACKET_SIZE; i++)
        send_buff[i] = i;
    p_dbg("full tcp send start");
    while(1)
    {

        mutex_lock(socket_mutex);
        //采用阻塞方式发送,这种方式发送会阻塞相当长时间
        i = send(fd, send_buff, TEST_PACKET_SIZE, /*MSG_DONTWAIT*/0);
        mutex_unlock(socket_mutex);

        ADD_MONITOR_VALUE(tcp_totol_send, TEST_PACKET_SIZE);
        if(tcp_send_stop)
        {
            p_dbg("stop tcp send test");
            goto end;
        }
        //sleep(10); //如果想减慢发送速度，可以在这里延时一下
    }

end:
    p_dbg("tcp send end");
    if(send_buff)
        mem_free(send_buff);
    thread_exit(thread_myself());
}
/*
 * @brief 满负荷发送，请先建立TCP连接
 * 建立单独的线程
 *
 */
void test_full_speed_send()
{
    p_dbg_enter;
    tcp_send_stop = 0;
    thread_create(tcp_send_thread, 0, TASK_TCP_SEND_PRIO, 0, TASK_TCP_SEND_STACK_SIZE, "tcp_send_thread");
    p_dbg_exit;
}

void test_full_speed_send_stop()
{
    p_dbg_enter;
    tcp_send_stop = 1;
    p_dbg_exit;
}

/*
 * @brief  关闭本地服务器
 *
 */
void test_close_camera_server()
{
    p_dbg_enter;
    mutex_lock(socket_mutex);
    if (remote_socket_fd !=  - 1)
    {
        close_socket(remote_socket_fd);
        remote_socket_fd =  - 1;
    }

    if (server_socket_fd !=  - 1)
    {
        close_socket(server_socket_fd);
        server_socket_fd =  - 1;
    }

    if (server_accept_thread_fd !=  - 1)
    {
        thread_exit(server_accept_thread_fd);
        server_accept_thread_fd =  - 1;
    }
    mutex_unlock(socket_mutex);
    p_dbg_exit;
}

/*
 * @brief 本地服务器监听线程
 *
 */
void tcp_camera_task(void *server_fd)
{
    int sockaddr_len, new_socket, opt;
    struct sockaddr_in addr;

    sockaddr_len = sizeof(struct sockaddr);

    while (1)
    {
        p_dbg("waiting for remote connect");
        new_socket = accept((int)server_fd, (struct sockaddr*) &addr, (socklen_t*) &sockaddr_len);
        if (new_socket ==  - 1)
        {
            p_err("accept err");
            break;
        } 
        p_dbg("accept a new client");
        if(remote_socket_fd == -1)
        {
            remote_socket_fd = new_socket;
        }
        else
        {
            p_err("a camera client already connected");
            close_socket(new_socket);
            continue;
        }
        opt = 1;
        if (setsockopt(new_socket, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(int)) ==  - 1)
            p_err("SO_KEEPALIVE err\n");

//        start_capture_img(); //开始摄像头捕获，发给本地客户端

    }
    thread_exit(server_accept_thread_fd);
}

/*
 * @brief  自动获取IP测试
 *
 *
 */
int auto_get_ip(struct netif *p_netif);
void test_auto_get_ip(struct netif *p_netif)
{
    int i, wait_time = 10;

    auto_get_ip(p_netif);

    for (i = 0; i < wait_time; i++)
    {
        p_dbg("%d", i);
        if (p_netif->ip_addr.addr)
            break;
        sleep(1000);
    }

    if (p_netif->ip_addr.addr)
    {
        show_tcpip_info(p_netif);
    }

}

char loopback_enable = 1;
void switch_loopback_test()
{
    loopback_enable = !loopback_enable;
    if(loopback_enable)
        p_dbg("使能回发测试");
    else
        p_dbg("关闭回发测试");
}

/**
 * @brief  处理接收到的socket通信数据，目前只作为测试命令使用
 * @比如，tcp客户端连接到了开发板，客户端发过来的数据将在这里处理
 * @
 */
void handle_test_recv(int socket, uint8_t *data, int len)
{
    if(loopback_enable)
    {
        mutex_lock(socket_mutex);
        send(socket, data, len, 0);
        mutex_unlock(socket_mutex);
    }else{
        //p_dbg("socket:%d, recv:%d byte", socket, len);
        //dump_hex("data", data, len);
        //send_cmd_to_self(data[0]);
    }
}


#define  TCP_TEST_LEN 2048
static unsigned char tcp_send_buffer[TCP_TEST_LEN];
static unsigned char tcp_read_buffer[TCP_TEST_LEN];

#define TCP_SERVICE_IP                  "192.168.10.36"
#define SERVER_URL_PORT                 7888
#define SERVER_TCP_PORT                 7777

static uint8_t loop_stop_tag = 0;

int tcpLoopTest(uint8_t tcp_type, void *argv)
{
    int fd=-1;
    int ret = 0, i= 0;
    unsigned int tickStart;
    unsigned int hadsend = 0;
    unsigned int needSend = 0;
    unsigned int hadRecv = 0;
    unsigned int needRecv = 0;
    unsigned int t0 = bsp_get_tick();
    unsigned int tOpen = bsp_get_tick();
    unsigned int tSend = bsp_get_tick();
    unsigned int tRecv = bsp_get_tick();
    unsigned int tClose = bsp_get_tick();
    unsigned char *ptxBuf = tcp_send_buffer;
    unsigned char *prxBuf = tcp_read_buffer;
    unsigned int tcpStatus;
    ST_NET_SOCKET  ap_info;
    unsigned int max_timeout = 5000;
    unsigned int  time_count = 10;
    COMM_SSL_CONFIG *tls_config;

    memset(ptxBuf, 0, TCP_TEST_LEN);
    memcpy(ptxBuf, CONST_10K_STRING, TCP_TEST_LEN - 1);
    memcpy(ptxBuf, "MH_TCP_TEST_START", sizeof("mh_TCP_TEST_START") - 1);


    p_dbg("[%u] %s: socket mode =%d Start test!!!!\r\n", bsp_get_tick(), __func__, tcp_type);
    memset(&ap_info, 0x00, sizeof(ST_NET_SOCKET));
    ap_info.router = 0;
    ap_info.addrType= 0;
    ap_info.addr = (char *)TCP_SERVICE_IP;

    t0 = bsp_get_tick();
    if(tcp_type == SOCKET_PRT_SSL)
    {
        tls_config = (COMM_SSL_CONFIG *)argv;
        ap_info.port  =  SERVER_URL_PORT;
    }
    else
    {
        ap_info.port  =  SERVER_TCP_PORT;
    }

    while(1)
    {
        if(loop_stop_tag)
        {
            goto EXIT;
        }
if(tcp_type == SOCKET_PRT_SSL)
        ret = OS_socket_open(SOCKET_PRT_SSL, (void*)&ap_info, tls_config, 8000);
else
        ret = OS_socket_open(SOCKET_PRT_TCP, (void*)&ap_info, NULL, 2000);

        if(ret<0)
        {
            p_dbg("[%u] %s: socket Open error:%d\r\n", bsp_get_tick(), __func__, ret);
            goto EXIT;
        }
        else
        {
            fd = ret;
            break;
        }
    }

    i = 0;
    do
    {
TCP_LONG_LINK_START:
    tickStart = tOpen = tSend= tRecv= bsp_get_tick();
    hadsend = hadRecv = needSend = needRecv = 0;

    bsp_gen_random((unsigned char *)&needSend, 4);
    needSend %= TCP_TEST_LEN;
    if(needSend < 100) needSend = 100;
    if(needSend > TCP_TEST_LEN) needSend = TCP_TEST_LEN;

    hadsend = 0;
    tickStart = bsp_get_tick();
    while(1)
    {
        if(loop_stop_tag)
        {
            p_dbg("[%u]-%s: 222 stop=1\r\n", bsp_get_tick(), __func__);
            ret = -2;
            goto EXIT;
        }

        tcpStatus = 0;
        ret = OS_socket_status(fd, &tcpStatus);
        if(ret < 0)
        {
            p_dbg("[%u] %s: 111 socket Status get failed, ret=%d\r\n",
                     bsp_get_tick(), __func__, ret);
            goto EXIT;
        }
        else
        {
            if(0 == (tcpStatus & 0x1))
            {
                p_dbg("[%u] %s: socket Send tcp error, need=%u, had=%u\r\n",
                         bsp_get_tick(),  __func__, needSend, hadsend);
                ret = OS_ENODEV;
                goto EXIT;
            }
        }

        if((bsp_get_tick() - tickStart) > max_timeout)
        {
            p_dbg("[%u] %s:socket Send timeout\r\n", bsp_get_tick(), __func__);
            ret = -2;
            goto EXIT;
        }

        ret = OS_socket_send(fd, (unsigned char *)(&ptxBuf[hadsend]), needSend, 0);
        if((ret<0) && (-2 != ret))
        {
            p_dbg("[%u] %s:socket Send error:%d, need=%u, had=%u\r\n",
                     bsp_get_tick(),  __func__, ret, needSend, hadsend);
            goto EXIT;
        }
        else if(ret > 0)
        {
            hadsend += (unsigned int)ret;
            needSend -= (unsigned int)ret;
        }
        else
        {
            bsp_delayms(1);
        }

        if(needSend <= 0)
        {
            break;
        }
    }
    p_dbg("%s:socket Send need=%u, send=%d\r\n", __func__, needSend, hadsend);
    tSend = bsp_get_tick();

    memset(prxBuf, 0, TCP_TEST_LEN);
    hadRecv = 0;
    needRecv = hadsend;
    tickStart = bsp_get_tick();
    while(1)
    {
        if(loop_stop_tag)
        {
            p_dbg("[%u] %s: 333 stop=1\r\n", bsp_get_tick(), __func__);
            ret = -2;
            goto EXIT;
        }

        tcpStatus = 0;
        ret = OS_socket_status(fd, &tcpStatus);
        if(ret < 0)
        {
            p_dbg("[%u] %s: 222 socket Status get failed, ret=%d\r\n",
                     bsp_get_tick(),  __func__, ret);
            goto EXIT;
        }
        else
        {
            if(0 == (tcpStatus & 1))
            {
                p_dbg("[%u] %s:socket Recv tcp error, need=%u, had=%u\r\n",
                         bsp_get_tick(),  __func__, needSend, hadsend);
                ret = OS_ENODEV;
                goto EXIT;
            }
        }

        if((bsp_get_tick() - tickStart) > max_timeout)
        {
            p_dbg("[%u] %s:socket Send timeout\r\n",
                     bsp_get_tick(), __func__);
            ret = -2;
            goto EXIT;
        }

        ret = OS_socket_recv(fd, (unsigned char *)(&prxBuf[hadRecv]), needRecv, 100);
        if((ret<0) && (-2 != ret))
        {
            p_dbg("[%u] %s:socket Recv error:%d, need=%u, had=%u\r\n",
                     bsp_get_tick(), __func__, ret, hadsend, hadRecv);
            goto EXIT;
        }
        else if(ret > 0)
        {
            hadRecv += (unsigned int)ret;
            needRecv -= (unsigned int)ret;
        }
        else
        {
            bsp_delayms(1);
        }

        if(needRecv <= 0)
        {
            break;
        }
    }

    //p_dbg("%s: socket Recv need=%u, recv=%d, recv data\r\n", __func__, hadsend, hadRecv);
    tRecv = bsp_get_tick();
    if(0 != memcmp(prxBuf, ptxBuf, hadRecv))
    {
        p_dbg("[%u] %s: data compare error\r\n", bsp_get_tick(), __func__);
        p_dbg("[%u] %s: send:", bsp_get_tick(), __func__);
        for(unsigned int i = 0; i < hadsend; ++i)
        {
            if(0 == (i % 16)) printf("\r\n");
            printf("%02X ", ptxBuf[i]);
        }
        p_dbg("\r\n");

        p_dbg("[%u] %s: recv:", bsp_get_tick(),  __func__);
        for(unsigned int i = 0; i < hadRecv; ++i)
        {
            if(0 == (i % 16)) printf("\r\n");
            printf("%02X ", prxBuf[i]);
        }
        p_dbg("\r\n");
        ret = -100;
    }
    else
    {
        p_dbg("%s: CommRecv need=%u, recv=%d, recv data\r\n", __func__, hadsend, hadRecv);
        ret = 0;
        if(tcp_type == 1)
        {
            bsp_delayms(10);
            t0 = bsp_get_tick();
            goto TCP_LONG_LINK_START;
        }
    }
    } while(i++ < time_count);

EXIT:
    p_dbg("%s exit %d\r\n",__func__, fd);

    if(fd>=0) OS_socket_close(fd, 0);

    tClose = bsp_get_tick();

    p_dbg("[%u] %s: socket test failed need=%u, recv=%d, status=%u, timeout\r\n",
          bsp_get_tick(), __func__, hadsend, hadRecv, tcpStatus);

    return ret;
}
