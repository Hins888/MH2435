#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/inet_chksum.h"
#include "lwip/raw.h"

#include "stdio.h"
#define PING_DATA_SIZE 32
#define PING_FLAG_ID 0xE5F6

static struct raw_pcb* rpcb = NULL;
static u16_t ping_seq = 0;

extern u32_t sys_now(void);
static u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr);

void ping_init()
{
    if(rpcb != NULL) {
        raw_remove(rpcb);
    }
    rpcb = raw_new(IP_PROTO_ICMP);
    if(rpcb == NULL){
        return;
    }
    ip_addr_t destip;
    IP4_ADDR(&destip, 192,168,1,117);
    // IP4_ADDR(&destip, 127,0,0,1);
    raw_bind(rpcb, IP_ADDR_ANY);
    raw_connect(rpcb, &destip);
    
    raw_recv(rpcb, ping_recv, NULL);
}
/**
 * @param arg user supplied argument (raw_pcb.recv_arg)
 * @param pcb the raw_pcb which received data
 * @param p the packet buffer that was received
 * @param addr the remote IP address from which the packet was received
 * @return 1 if the packet was 'eaten' (aka. deleted),
 *         0 if the packet lives on
 * If returning 1, the callback is responsible for freeing the pbuf
 * if it's not used any more.
 */
static u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
    struct icmp_echo_hdr* iecho;
    // printf("%d.%d.%d.%d\r\n", ip4_addr1(addr), ip4_addr2(addr), ip4_addr3(addr), ip4_addr4(addr));
    if(p->len < PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr)) 
    {
        return 0;
    }
    iecho = (void*)((u8_t*)p->payload + PBUF_IP_HLEN);
    if(iecho->id != ntohs(PING_FLAG_ID) || iecho->type != ICMP_ER)
    {
        
        return 0;
    }
    printf("resp[%d] %d.%d.%d.%d\r\n", ntohs(iecho->seqno), ip4_addr1(addr), ip4_addr2(addr), ip4_addr3(addr), ip4_addr4(addr));
    pbuf_free(p);
    return 1;
}

static void ping_send(void);
#define PING_INTVL_MS   (3 * 1000)
void ping_timer_check()
{
    static u32_t ptm = 0;
    if(sys_now() - ptm < PING_INTVL_MS)
    {
        return;
    }
    ptm = sys_now();
    ping_send();
    
}

static void ping_send(void)
{
    if(rpcb == NULL)
    {
        return;
    }
    u16_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
    struct pbuf* p = pbuf_alloc(PBUF_IP, ping_size, PBUF_RAM);
    if(p == NULL) {
        return;
    }
    //fill icmp_hdr and data 
    struct icmp_echo_hdr* iehdr = p->payload;
    ICMPH_TYPE_SET(iehdr, ICMP_ECHO);
    ICMPH_CODE_SET(iehdr, 0);
    iehdr->id = htons(PING_FLAG_ID);
    iehdr->seqno = htons(++ping_seq);
    
    int i = 0;
    u8_t* ping_data = ((u8_t*)p->payload) + sizeof(struct icmp_echo_hdr);
    for(i = 0; i < PING_DATA_SIZE; i++)
    {
        ping_data[i] = i;
    }
    
    iehdr->chksum = 0;
    iehdr->chksum = inet_chksum(iehdr, ping_size);
    u8_t err = raw_send(rpcb, p);
    
    pbuf_free(p);
    printf("\r\n");
    if(err != ERR_OK)
    {
        printf("err: %d \r\n", err);
    } else {
        printf("ping[%d] %d.%d.%d.%d \r\n", ping_seq, ip4_addr1(&(rpcb->remote_ip)), ip4_addr2(&(rpcb->remote_ip)), ip4_addr3(&(rpcb->remote_ip)), ip4_addr4(&(rpcb->remote_ip)));
    }
}
