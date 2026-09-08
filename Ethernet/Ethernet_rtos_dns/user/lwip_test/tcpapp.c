#include "netconf.h"
#include "lwip/tcp.h"
#include "lwip/errno.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/api.h"

#include "test_data.h"

#define p_err printf

//#define TCP_SERVICE_IP1         192
//#define TCP_SERVICE_IP2         168
//#define TCP_SERVICE_IP3         10
//#define TCP_SERVICE_IP4         9
char *server_ip = "192.168.10.9";
    
#define SERVER_URL_PORT         7888
#define SERVER_TCP_PORT         7777

/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

/** Current state of the netconn. Non-TCP netconns are always
 * in state NETCONN_NONE! */


struct ret_connect_stat
{
    uint32_t local_addr;
    uint32_t remote_addr;
    uint16_t local_port;
    uint16_t remote_port;
    uint8_t socket_num;
    uint8_t type; //tcp udp
};

#define NUM_SOCKETS   MEMP_NUM_NETCONN

extern struct netif gnetif;

/*
 * @brief  配置本地ip地址
 *
 */
int set_ipaddr(struct netif *p_netif,  ip_addr_t *ipaddr, ip_addr_t *netmask,  ip_addr_t *gw, ip_addr_t *dns) 
{
    return 0;
}

/*
 * @brief  域名解析
 *
 */
int get_host_by_name(char *hostname, uint32_t *addr)
{
    char err_t;
    ip_addr_t ip_addr;
    
    /**
     * Execute a DNS query, only one IP address is returned
     *
     * @param name a string representation of the DNS host name to query
     * @param addr a preallocated ip_addr_t where to store the resolved IP address
     * @return ERR_OK: resolving succeeded
     *         ERR_MEM: memory error, try again later
     *         ERR_ARG: dns client not initialized or invalid hostname
     *         ERR_VAL: dns server response was invalid
     */
    err_t = netconn_gethostbyname(hostname, &ip_addr);
    if (err_t == ERR_OK)
    {
        *addr = ip_addr.addr;
        return 0;
    }

    return  - 1;
}

int tcp_client_create(char *ip, uint16_t port)
{
    int socket_c, err;
    struct sockaddr_in addr;

    printf("enter %s\n", __FUNCTION__);
    addr.sin_family = AF_INET;
    addr.sin_len = sizeof(struct sockaddr_in);
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    socket_c = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_c < 0)
    {
        printf("get socket err:%d", errno);
        return  - 1;
    }
    //mutex_lock(socket_mutex);
    err = connect(socket_c, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    //mutex_unlock(socket_mutex);
    if (err ==  - 1)
    {
        printf("link err:%d\n", errno);
        close(socket_c);
        return  - 1;
    }

    //设置keepalive，如果远程段非正常关闭，我们可以尽快察觉到并释放已经断开的链接
    err = 1;
    if (setsockopt(socket_c, SOL_SOCKET, SO_KEEPALIVE, &err, sizeof(int)) ==  - 1)
        printf("SO_KEEPALIVE err\n");

    printf("link ok\n");
    return socket_c;
}


/*
*强制发送数据
*send函数一次发送不一定能全部发送出去(比如缓冲区满了)
*这里判断剩下的数据长度并且继续发送
*/
int send_data(int socket, uint8_t *buff, int size)
{
    int ret;
again: 
    //mutex_lock(socket_mutex);    
    ret = send(socket, buff, size, 0);
    //mutex_unlock(socket_mutex);
    if (ret ==  - 1)
    {
        //p_err("send data err:%d", ret);
        return 0;
    }
    else if (ret < size)
    {
        size -= ret;
        buff += ret;
        printf("send remain:%d", size);
        goto again;
    }
    return size;
}

//
/*
 *UDP通信不要求预先连接，
 *预先连接的好处是可以直接调用send发送数据
 *
 */
int udp_client_create(char *ip, uint16_t port)
{
    int socket_c, err;
    struct sockaddr_in addr;

    printf("enter %s\n", __FUNCTION__);
    addr.sin_family = AF_INET;
    addr.sin_len = sizeof(struct sockaddr_in);
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    socket_c = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_c < 0)
    {
        printf("get socket err:%d", errno);
        return  - 1;
    } 

    err = connect(socket_c, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    if (err ==  - 1)
    {
        printf("link err:%d\n", errno);
        close(socket_c);
        return  - 1;
    }

    //设置keepalive，如果远程段非正常关闭，我们可以尽快察觉到并释放已经断开的链接
    err = 1;
    if (setsockopt(socket_c, SOL_SOCKET, SO_KEEPALIVE, &err, sizeof(int)) ==  - 1)
        printf("SO_KEEPALIVE err\n");


    printf("link ok\n");
    return socket_c;
}

//
/*
 *添加到多播组，同一组内的设备可以互相收发广播数据
 *
 *ip地址范围224.0.0.1 - 239.255.255.255
 */
int udp_add_membership(char *ip, uint16_t port)
{
    int socket_c, err;
    struct sockaddr_in addr;
    struct ip_mreq imr;
    printf("enter %s\n", __FUNCTION__);

    socket_c = socket(AF_INET, SOCK_DGRAM, 0);
    printf("socket ret:%d", socket_c);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_len =  sizeof(struct sockaddr_in);
    addr.sin_port = htons(port);
    err = bind(socket_c, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    printf("bind ret:%d", err);
    if (err ==  - 1)
    {
        close(socket_c);
        return  - 1;
    }

    //imr.imr_interface.s_addr = htons(INADDR_ANY);
    imr.imr_interface.s_addr =  gnetif.ip_addr.addr;
    imr.imr_multiaddr.s_addr = inet_addr(ip);
    err = setsockopt(socket_c, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(struct ip_mreq));
    printf("IP_ADD_MEMBERSHIP ret:%d", err);
    if (err ==  - 1)
    {
        close(socket_c);
        return  - 1;
    } 

    printf("link ok\n");
    return socket_c;
}

/*
 *创建UDP服务器，并且设置为可以接收广播
 *
 *
 */
int udp_create_server(uint16_t port)
{
    int n;
    int socket_c, err;
    struct sockaddr_in addr;

    printf("enter %s\n", __FUNCTION__);
    addr.sin_family = AF_INET;
    addr.sin_len = sizeof(struct sockaddr_in);
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htons(INADDR_ANY);

    socket_c = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_c < 0)
    {
        p_err("get socket err:%d", errno);
        return  - 1;
    }
    err = bind(socket_c, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    printf("bind ret:%d", err);
    if (err ==  - 1)
    {
        close(socket_c);
        return  - 1;

    }
#if 1    //如果希望接收广播数据,请置1
    err = setsockopt(socket_c, SOL_SOCKET, SO_BROADCAST, (char *) &n, sizeof(n));
    printf("SO_BROADCAST ret:%d", err);
    if(err == -1)
    {
    close(socket_c);
    return -1;
    }
#endif
    printf("server ok:%d", socket_c);
    return socket_c;
}

/*
 *remote_port和remote_addr为大端模式
 */
int udp_data_send(int socket_id, void *data, int len, uint16_t remote_port, uint32_t remote_addr)
{
    int remen = len;

    //struct lwip_sock *sock;
    printf("enter %s ,%d\n", __FUNCTION__, socket_id);

    do
    {
        struct sockaddr udp_remote_addr;
        memset(&udp_remote_addr, 0, sizeof(struct sockaddr));
        udp_remote_addr.sa_len = sizeof(struct sockaddr);
        udp_remote_addr.sa_family = AF_INET;
        memcpy(udp_remote_addr.sa_data, &remote_port, 2);
        memcpy(&udp_remote_addr.sa_data[2], &remote_addr, 4);

        len = sendto(socket_id, (u8*)data, len, 0, &udp_remote_addr, sizeof(struct sockaddr));
        if (len ==  - 1)
        {
            //EAGAIN == timeout
//            if ((sockets_ctx[socket_id].err == EAGAIN) || (sockets_ctx[socket_id].err == ENOMEM) || (sockets_ctx[socket_id].err == EINTR))
//            {
//                printf("send fatal err:%d\n", sockets_ctx[socket_id].err);
//                return 0;
//            }
            close(socket_id);
            break;
        }

        if (len != remen)
        {
//            if (sockets_ctx[socket_id].err != 0)
//            {
//                printf("send err1:%d,%d\n", len, remen);
//                close(socket_id);
//            }

            //if (len == 0)
                //printf("send 0:%d\n", sockets_ctx[socket_id].err);
        }
    }
    while (0);
    return len;
}


extern uint8_t g_mac_addr[6];
#define CPU_ID              ((unsigned int*)0x1fff7a10)
/**
 * @brief  通过CPU_ID计算出一个随机数用于填充MAC地址的低3字节
 *
 *
 */
int create_mac(unsigned char *mac)
{
    char psk[33], id[12];

    int i, j, k, ret = 0;
    i =  *CPU_ID;
    j = *(CPU_ID + 1);
    k = *(CPU_ID + 2);

    memcpy(id, &i, 4);
    memcpy(id + 4, &j, 4);
    memcpy(id + 8, &k, 4);
    //ret = pbkdf2_sha1(ps, id, 12, 100, (u8*)psk, 32);
    //取cpu id号来生成mac地址
    psk[0] = i;
    psk[1] = j;
    psk[2] = k;

    //取三个字节作为mac地址
    mac[0] = 0x00;
    mac[1] = 0x0c;
    mac[2] = 0x43;
    memcpy(mac + 3, psk, 3);
    mac[5] = mac[5] &0xfe; //for 2-BSSID mode

    //dump_hex("mac", mac, 6);

    return ret;
}

#define  TCP_TEST_LEN 2048
static unsigned char tcp_send_buffer[TCP_TEST_LEN];
static unsigned char tcp_read_buffer[TCP_TEST_LEN];

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
    unsigned int t0 = sys_now();
    unsigned int tOpen = sys_now();
    unsigned int tSend = sys_now();
    unsigned int tRecv = sys_now();
    unsigned int tClose = sys_now();
    unsigned char *ptxBuf = tcp_send_buffer;
    unsigned char *prxBuf = tcp_read_buffer;
    unsigned int tcpStatus;
    unsigned int max_timeout = 5000;
    unsigned int  time_count = 10;

    memset(ptxBuf, 0, TCP_TEST_LEN);
    memcpy(ptxBuf, CONST_10K_STRING, TCP_TEST_LEN - 1);
    memcpy(ptxBuf, "MH_TCP_TEST_START", sizeof("mh_TCP_TEST_START") - 1);

    printf("[%u] %s: socket mode =%d Start test!!!!\r\n", sys_now(), __func__, tcp_type);

    while(1)
    {
        if(loop_stop_tag)
        {
            goto EXIT;
        }
        tcp_client_create(argv, SERVER_TCP_PORT);
        if(ret<0)
        {
            printf("[%u] %s: socket Open error:%d\r\n", sys_now(), __func__, ret);
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
    tickStart = tOpen = tSend= tRecv= sys_now();
    hadsend = hadRecv = needSend = needRecv = 0;

    needSend = rand();
    needSend %= TCP_TEST_LEN;
    if(needSend < 100) needSend = 100;
    if(needSend > TCP_TEST_LEN) needSend = TCP_TEST_LEN;

    hadsend = 0;
    tickStart = sys_now();
    while(1)
    {
        if(loop_stop_tag)
        {
            printf("[%u]-%s: 222 stop=1\r\n", sys_now(), __func__);
            ret = -2;
            goto EXIT;
        }

        tcpStatus = 0;
//        ret = OS_socket_status(fd, &tcpStatus);
//        if(ret < 0)
//        {
//            printf("[%u] %s: 111 socket Status get failed, ret=%d\r\n",
//                     sys_now(), __func__, ret);
//            goto EXIT;
//        }
//        else
//        {
//            if(0 == (tcpStatus & 0x1))
//            {
//                printf("[%u] %s: socket Send tcp error, need=%u, had=%u\r\n",
//                         sys_now(),  __func__, needSend, hadsend);
//                ret = -1;
//                goto EXIT;
//            }
//        }

        if((sys_now() - tickStart) > max_timeout)
        {
            printf("[%u] %s:socket Send timeout\r\n", sys_now(), __func__);
            ret = -2;
            goto EXIT;
        }

        ret = send_data(fd, (unsigned char *)(&ptxBuf[hadsend]), needSend); //OS_socket_send(fd, (unsigned char *)(&ptxBuf[hadsend]), needSend, 0);
        if((ret<0) && (-2 != ret))
        {
            printf("[%u] %s:socket Send error:%d, need=%u, had=%u\r\n",
                     sys_now(),  __func__, ret, needSend, hadsend);
            goto EXIT;
        }
        else if(ret > 0)
        {
            hadsend += (unsigned int)ret;
            needSend -= (unsigned int)ret;
        }
        else
        {
            vTaskDelay(1);
        }

        if(needSend <= 0)
        {
            break;
        }
    }
    printf("%s:socket Send need=%u, send=%d\r\n", __func__, needSend, hadsend);
    tSend = sys_now();

    memset(prxBuf, 0, TCP_TEST_LEN);
    hadRecv = 0;
    needRecv = hadsend;
    tickStart = sys_now();
    while(1)
    {
        if(loop_stop_tag)
        {
            printf("[%u] %s: 333 stop=1\r\n", sys_now(), __func__);
            ret = -2;
            goto EXIT;
        }

        tcpStatus = 0;
//        ret = OS_socket_status(fd, &tcpStatus);
//        if(ret < 0)
//        {
//            printf("[%u] %s: 222 socket Status get failed, ret=%d\r\n",
//                     sys_now(),  __func__, ret);
//            goto EXIT;
//        }
//        else
//        {
//            if(0 == (tcpStatus & 1))
//            {
//                printf("[%u] %s:socket Recv tcp error, need=%u, had=%u\r\n",
//                         sys_now(),  __func__, needSend, hadsend);
//                ret = -3;
//                goto EXIT;
//            }
//        }

        if((sys_now() - tickStart) > max_timeout)
        {
            printf("[%u] %s:socket Send timeout\r\n",
                     sys_now(), __func__);
            ret = -2;
            goto EXIT;
        }

        ret =  recv(fd, (unsigned char *)(&prxBuf[hadRecv]), needRecv, 100); //OS_socket_recv(fd, (unsigned char *)(&prxBuf[hadRecv]), needRecv, 100);
        if((ret<0) && (-2 != ret))
        {
            printf("[%u] %s:socket Recv error:%d, need=%u, had=%u\r\n",
                     sys_now(), __func__, ret, hadsend, hadRecv);
            goto EXIT;
        }
        else if(ret > 0)
        {
            hadRecv += (unsigned int)ret;
            needRecv -= (unsigned int)ret;
        }
        else
        {
            vTaskDelay(1);
        }

        if(needRecv <= 0)
        {
            break;
        }
    }

    //p_dbg("%s: socket Recv need=%u, recv=%d, recv data\r\n", __func__, hadsend, hadRecv);
    tRecv = sys_now();
    if(0 != memcmp(prxBuf, ptxBuf, hadRecv))
    {
        printf("[%u] %s: data compare error\r\n", sys_now(), __func__);
        printf("[%u] %s: send:", sys_now(), __func__);
        for(unsigned int i = 0; i < hadsend; ++i)
        {
            if(0 == (i % 16)) printf("\r\n");
            printf("%02X ", ptxBuf[i]);
        }
        printf("\r\n");

        printf("[%u] %s: recv:", sys_now(),  __func__);
        for(unsigned int i = 0; i < hadRecv; ++i)
        {
            if(0 == (i % 16)) printf("\r\n");
            printf("%02X ", prxBuf[i]);
        }
        printf("\r\n");
        ret = -100;
    }
    else
    {
        printf("%s: CommRecv need=%u, recv=%d, recv data\r\n", __func__, hadsend, hadRecv);
        ret = 0;
        if(tcp_type == 1)
        {
            vTaskDelay(10);
            t0 = sys_now();
            goto TCP_LONG_LINK_START;
        }
    }
    } while(i++ < time_count);

EXIT:
    printf("%s exit %d\r\n",__func__, fd);

    if(fd>=0) close(fd);

    tClose = sys_now();

    printf("[%u] %s: socket test failed need=%u, recv=%d, status=%u, timeout\r\n",
          sys_now(), __func__, hadsend, hadRecv, tcpStatus);

    return ret;
}

void tcp_client_test(void)
{
    tcpLoopTest(0, server_ip);
}
