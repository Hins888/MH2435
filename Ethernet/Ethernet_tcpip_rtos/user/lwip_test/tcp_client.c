#include "netconf.h"
#include "lwip/tcp.h"
//#include "lwip/timers.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/err.h"

//typedef s8_t err_t;
#define TCP_RX_BUFSIZE         2000  /* 定义udp最大接收数据长度 */

/* TCP Client 测试全局状态标记变量
 * bit7:0,没有数据要发送;1,有数据要发送
 * bit6:0,没有收到数据;1,收到数据了.
 * bit5:0,没有连接上服务器;1,连接上服务器了.
 * bit4~0:保留 */
uint8_t g_lwip_send_flag;
uint8_t g_tcp_step = 0;

/* tcp服务器连接状态 */
enum tcp_client_states
{
    ES_TCPCLIENT_NONE = 0,          /* 没有连接 */
    ES_TCPCLIENT_CONNECTED,         /* 连接到服务器了 */
    ES_TCPCLIENT_CLOSING,           /* 关闭连接 */
};

/* LWIP回调函数使用的结构体 */
struct tcp_client_struct
{
    uint8_t state;                  /* 当前连接状 */
    struct tcp_pcb *pcb;            /* 指向当前的pcb */
    struct pbuf *p;                 /* 指向接收/或传输的pbuf */
};  

uint8_t g_tcp_recvbuf[TCP_RX_BUFSIZE]; 
uint8_t g_tcp_sendbuf[TCP_RX_BUFSIZE]; 

void  tcp_client_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es );
err_t tcp_client_usersent(struct tcp_pcb *tpcb, u8 *sdata, u32 len);


err_t tcp_client_connected(void *arg);
void  tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es);
 
static err_t tcp_recv_callback(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err);
static err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void  tcp_error_callback(void *arg,err_t err);
static err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err);


struct tcp_client_struct g_tcp_rxtx_ctx;
//struct tcp_client_struct g_tcp_rx_ctx;

#define TCP_SERVICE_IP1         192
#define TCP_SERVICE_IP2         168
#define TCP_SERVICE_IP3         10
#define TCP_SERVICE_IP4         12

#define SERVER_URL_PORT         7888
#define SERVER_TCP_PORT         7777

//static u8 lwip_test_buf[TCP_RX_BUFSIZE *2] = { 0};
uint8_t remoteip[4] = { 0 };            /* */

void lwip_demo_test_non_blocking(void)
{   

    switch(g_tcp_step)
    {
        case 0:
            remoteip[0] = TCP_SERVICE_IP1;
            remoteip[1] = TCP_SERVICE_IP2;
            remoteip[2] = TCP_SERVICE_IP3;
            remoteip[3] = TCP_SERVICE_IP4;
    
            tcp_client_connected(0);
            //printf("s:%d ERR=%d\n", g_tcp_step, err);
            g_tcp_step = 1;
            break;
        case 1:
            //lwip_test_buf =
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
    }

}

/**
 * @brief       lwIP TCP连接建立后调用回调函数
 * @param       arg   : 回调函数传入的参数
 * @param       tpcb  : TCP控制块
 * @param       err   : 错误码
 * @retval      返回错误码
 */
err_t tcp_client_connected(void *arg)
{
    //struct tcp_client_struct *es = NULL;
    struct tcp_pcb *tcppcb;   /* 定义一个TCP客户端控制块 */
    ip_addr_t rmtipaddr;      /* 远端ip地址 */

    printf("tcp connected start \n");
    
    tcppcb = tcp_new();  /* 创建一个新的pcb */
    if (tcppcb)           /* 创建成功 */
    {
        IP4_ADDR(&rmtipaddr, remoteip[0], remoteip[1], remoteip[2], remoteip[3]);
        printf("service IP:%d.%d.%d.%d; port:%d", remoteip[0], remoteip[1], remoteip[2], remoteip[3], SERVER_TCP_PORT); /* Ô¶¶ËIP */
        //printf("server ip addr = %x \n", rmtipaddr.addr);
        tcp_connect(tcppcb, &rmtipaddr, SERVER_TCP_PORT, tcp_connected_callback); /* 连接到目的地址的指定端口上,当连接成功后回调lwip_tcp_client_connected()函数 */
    }
    //g_tcp_rxtx_ctx.pcb = tcppcb;
    return  ERR_OK;
}

/**
 * @brief       lwIP TCP连接建立后调用回调函数
 * @param       arg   : 回调函数传入的参数
 * @param       tpcb  : TCP控制块
 * @param       err   : 错误码
 * @retval      返回错误码
 */
static err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    struct tcp_client_struct *es = &g_tcp_rxtx_ctx;
    
    printf("tcp connected state=%d \n", err);
    if (err == ERR_OK)
    {
        es->state = ES_TCPCLIENT_CONNECTED;         /* 状态为连接成功 */
        es->pcb = tpcb;
        es->p = NULL;
        
        tcp_arg(tpcb, es);                          /* 使用es更新tpcb的callback_arg */
        tcp_recv(tpcb, tcp_recv_callback);       /* 初始化LwIP的tcp_recv回调功能 */
        tcp_sent(tpcb, tcp_sent_callback);       /* 初始化LwIP的tcp_sent回调功能 */
        
        tcp_poll(tpcb, tcp_poll_callback, 2);    /* 初始化LwIP的tcp_poll回调功能 */
        
        tcp_err(tpcb, tcp_error_callback);       /* 初始化tcp_err()回调函数 */
        
        g_lwip_send_flag |= 1 << 5;                 /* 标记连接到服务器了 */
        err = ERR_OK;
    }
    else
    {
        tcp_client_close(tpcb, 0);      /* 关闭连接 */
    }

    return err;
}

/**
 * @brief       lwIP tcp_recv()函数的回调函数
 * @param       arg   : 回调函数传入的参数
 * @param       tpcb  : TCP控制块
 * @param       p     : 网络数据包
 * @param       err   : 错误码
 * @retval      返回错误码
 */
err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    uint32_t data_len = 0;
    struct pbuf *q;
    struct tcp_client_struct *es;
    err_t ret_err;

    
    LWIP_ASSERT("arg != NULL", arg != NULL);
    es = (struct tcp_client_struct *)arg;

    if (p == NULL)                                                  /* 如果从服务器接收到空的数据帧就关闭连接 */
    {
        es->state = ES_TCPCLIENT_CLOSING;                           /* 需要关闭TCP 连接了 */
        es->p = p;
        ret_err = ERR_OK;
    }
    else if (err != ERR_OK)                                         /* 当接收到一个非空的数据帧,但是err!=ERR_OK */
    {
        if (p)pbuf_free(p);                                         /* 释放接收pbuf */

        ret_err = err;
    }
    else if (es->state == ES_TCPCLIENT_CONNECTED)                   /* 当处于连接状态时 */
    {
        if (p != NULL)                                              /* 当处于连接状态并且接收到的数据不为空时 */
        {
            memset(g_tcp_recvbuf, 0, TCP_RX_BUFSIZE);   /* 数据接收缓冲区清零 */

            for (q = p; q != NULL; q = q->next)                     /* 遍历完整个pbuf链表 */
            {
                /* 判断要拷贝到LWIP_DEMO_RX_BUFSIZE中的数据是否大于LWIP_DEMO_RX_BUFSIZE的剩余空间，如果大于 */
                /* 的话就只拷贝LWIP_DEMO_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据 */
                if (q->len > (TCP_RX_BUFSIZE - data_len)) memcpy(g_tcp_recvbuf + data_len, q->payload, (TCP_RX_BUFSIZE - data_len)); /* 拷贝数据 */
                else memcpy(g_tcp_recvbuf + data_len, q->payload, q->len);
                   
                printf("recv len=%d!\n", q->len);
                //tcp_client_usersent(tpcb, g_tcp_recvbuf + data_len, q->len);
                tcp_write(tpcb, g_tcp_recvbuf + data_len, p->tot_len, 1);
                data_len += q->len;

                if (data_len > TCP_RX_BUFSIZE) break; /* 超出TCP客户端接收数组,跳出 */
            }

            g_lwip_send_flag |= 1 << 6;                     /* 标记接收到数据了 */
            tcp_recved(tpcb, p->tot_len);                   /* 用于获取接收数据,通知LWIP可以获取更多数据 */
            pbuf_free(p);                                   /* 释放内存 */
            ret_err = ERR_OK;
        }
    }
    else                                                    /* 接收到数据但是连接已经关闭 */
    {
        tcp_recved(tpcb, p->tot_len);                       /* 用于获取接收数据,通知LWIP可以获取更多数据 */
        es->p = NULL;
        pbuf_free(p);                                       /* 释放内存 */
        ret_err = ERR_OK;
    }

    return ret_err;
}

/**
 * @brief       lwIP tcp_err函数的回调函数
 * @param       arg   : 回调函数传入的参数
 * @retval      无
 */
static void tcp_error_callback(void *arg, err_t err)
{
    printf("connect error=%d! closed by core!!\n", err);
    printf("try to connect to server again!!\n");
    /* 这里我们不做任何处理 */
    //Á¬½ÓÊ§°ÜµÄÊ±ºòÊÍ·ÅTCP¿ØÖÆ¿éµÄÄÚ´æ
    tcp_close(g_tcp_rxtx_ctx.pcb);  

}

/**
 * @brief       LWIP数据发送，用户应用程序调用此函数来发送数据
 * @param       tpcb: TCP控制块
 * @retval      返回值:0，成功；其他，失败
 */
err_t tcp_client_usersent(struct tcp_pcb *tpcb, u8 *sdata, u32 len)
{
    err_t ret_err;
    uint32_t once_len = 0;
    
    struct tcp_client_struct *es;
    es = tpcb->callback_arg;

    if (es != NULL) /* 连接处于空闲可以发送数据 */
    {
        uint16_t sendCount = (len + TCP_RX_BUFSIZE - 1) / TCP_RX_BUFSIZE;
        uint8_t* sendPoint = sdata;
        for (int reportIndex = 0; reportIndex < sendCount; reportIndex++)
        {
            if (len < TCP_RX_BUFSIZE)
            {
                //uint8_t reportAlignBuffer[TCP_RX_BUFSIZE];
                //memcpy(reportAlignBuffer, reportPoint, len);
                //reportPoint = reportAlignBuffer;
                once_len = len;
            }
            else once_len = TCP_RX_BUFSIZE;
            
            pbuf_take(es->p, (char *)sendPoint, once_len); /* 将tcp_client_sentbuf[]中的数据拷贝到es->p_tx中 */
            tcp_client_senddata(tpcb, es);                  /* 将tcp_client_sentbuf[]里面复制给pbuf的数据发送出去 */
            g_lwip_send_flag &= ~(1 << 7);                  /* 清除数据发送标志 */

            len -= once_len; //MIN(len, once_len);
            sendPoint += once_len;
        }
        ret_err = sendPoint - sdata;
    }
    else
    {
        tcp_abort(tpcb);                        /* 终止连接,删除pcb控制块 */
        ret_err = ERR_ABRT;
    }

    return ret_err;
}

/**
 * @brief       lwIP tcp_poll的回调函数
 * @param       arg : 回调函数传入的参数
 * @param       tpcb: TCP控制块
 * @retval      ERR_OK
 */
err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct tcp_client_struct *es;
    es = (struct tcp_client_struct *)arg;

    if (es->state == ES_TCPCLIENT_CLOSING)              /* 连接断开 */
    {
        tcp_client_close(tpcb, es);     /* 关闭TCP连接 */
    }

    ret_err = ERR_OK;
    return ret_err;
}

/**
 * @brief       lwIP tcp_sent的回调函数(当从远端主机接收到ACK信号后发送数据)
 * @param       arg : 回调函数传入的参数
 * @param       tpcb: TCP控制块
 * @param       len : 长度
 * @retval      ERR_OK
 */
err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct tcp_client_struct *es;
    LWIP_UNUSED_ARG(len);
    es = (struct tcp_client_struct *)arg;

    if (es->p)tcp_client_senddata(tpcb, es); /* 发送数据 */

    return ERR_OK;
}

/**
 * @brief       用来发送数据
 * @param       tpcb: TCP控制块
 * @param       es  : LWIP回调函数使用的结构体
 * @retval      无
 */
void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct *es)
{
    struct pbuf *ptr;
    err_t wr_err = ERR_OK;

    while ((wr_err == ERR_OK) && es->p && (es->p->len <= tcp_sndbuf(tpcb))) /* 将要发送的数据加入到发送缓冲队列中 */
    {
        ptr = es->p;
        wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);

        if (wr_err == ERR_OK)
        {
            es->p = ptr->next;             /* 指向下一个pbuf */

            if (es->p)pbuf_ref(es->p);     /* pbuf的ref加一 */

            pbuf_free(ptr);                /* 释放ptr */
        }
        else if (wr_err == ERR_MEM)es->p = ptr;

        tcp_output(tpcb);                  /* 将发送缓冲队列中的数据立即发送出去 */
    }
}

/**
 * @brief       关闭与服务器的连接
 * @param       tpcb: TCP控制块
 * @param       es  : LWIP回调函数使用的结构体
 * @retval      无
 */
void tcp_client_close(struct tcp_pcb *tpcb, struct tcp_client_struct *es)
{
    /* 移除回调 */
    tcp_abort(tpcb);                        /* 终止连接,删除pcb控制块 */
    
    tcp_arg(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_poll(tpcb, NULL, 0);

    g_lwip_send_flag &= ~(1 << 5);          /* 标记连接断开了 */
}
