
#include "lwip/udp.h"
#include "string.h"
#include "stdio.h"

#include "netconf.h"

#define LWIP_DEMO_RX_BUFSIZE         2000

struct udp_pcb* upcb = NULL;

uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE]; 

extern ip_addr_t ipaddr;

/**
 * @brief       UDP 
 * @param       upcb: 
 * @retval      
 */
void lwip_udp_senddata(struct udp_pcb *upcb)
{
    struct pbuf *ptr;
    ptr = pbuf_alloc(PBUF_TRANSPORT, strlen((char *)g_lwip_demo_recvbuf), PBUF_POOL); /*  */

    if (ptr)
    {
        pbuf_take(ptr, (char *)g_lwip_demo_recvbuf, strlen((char *)g_lwip_demo_recvbuf)); /* */
        udp_send(upcb, ptr);    /*   */
        pbuf_free(ptr);         /*  */
    }
}

static void udp_echo_callback(void* arg, struct udp_pcb* upcb, struct pbuf* p, const ip_addr_t* addr, u16_t port)
{
    uint32_t data_len = 0;
    struct pbuf *q;

    if (p != NULL) /* */
    {
        memset(g_lwip_demo_recvbuf, 0, LWIP_DEMO_RX_BUFSIZE); /*  */

        for (q = p; q != NULL; q = q->next) /* */
        {
            if (q->len > (LWIP_DEMO_RX_BUFSIZE - data_len)) memcpy(g_lwip_demo_recvbuf + data_len, q->payload, (LWIP_DEMO_RX_BUFSIZE - data_len)); 
            else memcpy(g_lwip_demo_recvbuf + data_len, q->payload, q->len);

            data_len += q->len;

            if (data_len > LWIP_DEMO_RX_BUFSIZE) break;
        }

        upcb->remote_ip = *addr;  
        upcb->remote_port = port; 
        
        printf("get remoteip=%x, data=%s\n", upcb->remote_ip.addr, g_lwip_demo_recvbuf);
        
        lwip_udp_senddata(upcb);
        
        pbuf_free(p); 
    }
    else
    {
        udp_disconnect(upcb);
    }
}

void udp_echo_init(void)
{
    err_t err;
    ip_addr_t rmtipaddr;         /*  */
    
    upcb = udp_new();
    if(upcb)
    {
//        IP4_ADDR(&rmtipaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3);
//        err = udp_connect(upcb, &ipaddr, 5001);      /* UDP */
//        if (err == ERR_OK)
//        {
//            return ;
//        }
        err = udp_bind(upcb, IP_ADDR_ANY, 5001);    /* */
        if (err == ERR_OK)  /* */
        {
            udp_recv(upcb, udp_echo_callback, NULL);
        }
    }
    else return ;
}

