#define DEBUG

#include "drivers.h"
#include "app.h"
#include "api.h"

#include "dev_netif.h"

#include "rtl8201f.h"

#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "netif/etharp.h"

#include "web_cfg.h"

extern WEB_CFG web_cfg;

static __IO uint8_t dhcp_step = DHCP_IDLE;
struct netif if_wifi;
struct netif if_eth;
struct netif *p_netif = NULL, *p_eth_netif = NULL; //一个netif对应一个网络设备

wait_event_t eth_int_event = 0;
char eth_int_pending = 0;


extern err_t eth_ethernetif_init(struct netif *netif);
extern void eth_ethernetif_input(struct netif *netif, void *p_buf, int size);
extern int create_mac(unsigned char *mac);

void show_tcpip_info(struct netif *p_netif)
{
    ip_addr_t dns_server;

    dns_server = dns_getserver(0);

    p_dbg("ipaddr: %d.%d.%d.%d", ip4_addr1(&p_netif->ip_addr.addr), ip4_addr2(&p_netif->ip_addr.addr), ip4_addr3(&p_netif->ip_addr.addr), ip4_addr4(&p_netif->ip_addr.addr));

    p_dbg("netmask: %d.%d.%d.%d", ip4_addr1(&p_netif->netmask.addr), ip4_addr2(&p_netif->netmask.addr), ip4_addr3(&p_netif->netmask.addr), ip4_addr4(&p_netif->netmask.addr));

    p_dbg("gw: %d.%d.%d.%d", ip4_addr1(&p_netif->gw.addr), ip4_addr2(&p_netif->gw.addr), ip4_addr3(&p_netif->gw.addr), ip4_addr4(&p_netif->gw.addr));


    p_dbg("dns_server: %d.%d.%d.%d", ip4_addr1(&dns_server.addr), ip4_addr2(&dns_server.addr), ip4_addr3(&dns_server.addr), ip4_addr4(&dns_server.addr));
}

/*
 * @brief  自动获取ip地址
 *
 */
int auto_get_ip(struct netif *p_netif)
{
    struct ip_addr ipaddr;
	
    ipaddr.addr = 0;
    p_dbg_enter;
    netif_set_down(p_netif); //先关掉,dhcp_start会启动它
    netif_set_ipaddr(p_netif, &ipaddr); //清零
    netif_set_netmask(p_netif, &ipaddr);
    netif_set_gw(p_netif, &ipaddr);
    dns_setserver(0, &ipaddr);
    dhcp_start(p_netif);
    return 0;
}

#if 0
/**
 * @brief  LwIP_DHCP_Process_Handle
 * @param  None
 * @retval None
 */
static void LwIP_DHCP_Process(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;
    static uint32_t IPaddress = 0;


    if(dhcp_step == DHCP_IDLE) return;

    switch (dhcp_step)
    {
        case DHCP_START: {
            dhcp_step = DHCP_WAIT_ADDRESS;
            ipaddr.addr = 0;
            dns_setserver(0, &ipaddr);
            dhcp_start(p_eth_netif);
            /* IP address should be set to 0
               every time we want to assign a new DHCP address */
            IPaddress = 0;
        }
        break;

        case DHCP_WAIT_ADDRESS: {
            /* Read the new IP address */
            IPaddress = p_eth_netif->ip_addr.addr;
            if (IPaddress != 0)
            {
                dhcp_step = DHCP_ADDRESS_ASSIGNED;
                /* Stop DHCP */
                dhcp_stop(p_eth_netif);
            }
            else
            {
                /* DHCP timeout */
                if (p_eth_netif->dhcp->tries > MAX_DHCP_TRIES)
                {
                    dhcp_step = DHCP_TIMEOUT;
                    /* Stop DHCP */
                    dhcp_stop(p_eth_netif);
                    /* Static address used */
                    gw.addr = web_cfg.eth_ip.gw;
                    ipaddr.addr = web_cfg.eth_ip.ip;
                    netmask.addr = web_cfg.eth_ip.msk;
                    netif_set_addr(p_eth_netif, &ipaddr, &netmask, &gw);
                }
            }
        }
        break;
        default:
            dhcp_step = DHCP_IDLE;
            break;
    }
}
#endif

static int lwip_eth_netif_init()
{
    struct ip_addr ipaddr, netmask, gw;

    if (p_eth_netif)
        netif_remove(p_eth_netif);

    p_eth_netif = &if_eth;

    create_mac(p_eth_netif->hwaddr);

    if(web_cfg.eth_ip.auto_get == 0)
    {
        gw.addr = web_cfg.eth_ip.gw;
        ipaddr.addr = web_cfg.eth_ip.ip;
        netmask.addr = web_cfg.eth_ip.msk;
    }
    else ipaddr.addr = 0;

    if (netif_add(p_eth_netif, &ipaddr, &netmask, &gw, NULL, eth_ethernetif_init, tcpip_input) == 0)
    {
        p_err("netif_add faild \r\n");
        return  - 1;
    }

    netif_set_default(p_eth_netif);    //网线插入后将以太网作为默认网卡,拔出后自动将wifi设为默认

    netif_set_up(p_eth_netif);

    return 0;
}


void eth_int_thread(void *pdata)    //以太网的任务
{
    int rval, last_link_status = 0;
    uint8_t auto_dhcp = 0;
    struct ethernetif *eth_state = NULL;
    uint16_t linked_status;

    rval=ETH_MACDMA_Config();

    if(rval != ETH_SUCCESS)
        goto end;

    rval = lwip_eth_netif_init();
    if(rval != 0) goto end;


    eth_int_event = init_event();

    eth_state = (struct ethernetif *)p_eth_netif->state;
    linked_status = ETH_ReadPHYRegister(RTL8201x_PHY_ADDRESS, PHY_BSR);
    if((linked_status & PHY_Linked_Status))
        eth_state->link_state = 1;

    
    if(web_cfg.eth_ip.auto_get){
        auto_dhcp = 1;
        p_eth_netif->ip_addr.addr = 0;
         auto_get_ip(p_eth_netif);
        
        p_dbg("%s:%d link_state =%d auto dhcping...", __func__, __LINE__, eth_state->link_state);
    }
    else
    {
        eth_state->link_state = 2;
    }
    
    p_dbg("%s:%d ip type=%s func link_state =%d", __func__, __LINE__, web_cfg.eth_ip.auto_get ? 
            "dhcp ip": "static ip", eth_state->link_state);
    
    while(1)
    {
        rval = wait_event_timeout(eth_int_event, 300);
        if(rval == WAIT_EVENT_TIMEOUT) //check PHY IS linked
        {
            uint16_t status = ETH_ReadPHYRegister(RTL8201x_PHY_ADDRESS, PHY_BSR);
            if(!(status & PHY_Linked_Status) && (last_link_status == 1))
            {
                last_link_status = 0;
                if(eth_state) eth_state->link_state = 0;
                p_dbg("eth link lost");
                p_eth_netif->ip_addr.addr = 0;
                send_work_event(ETH_LINK_LOST_EVENT);
            }
            else if((status & PHY_Linked_Status) && (last_link_status == 0))
            {
                last_link_status = 1;
                if(eth_state) eth_state->link_state = 1;

                netif_set_up(p_eth_netif);
                p_dbg("eth link ok");
                dhcp_step = DHCP_START;
            }
        }

        //p_dbg("eth rx frame check =0x%x %d", ETH_ReadPHYRegister(RTL8201x_PHY_ADDRESS, PHY_BSR), p_eth_netif->ip_addr.addr);
        while(ETH_CheckFrameReceived())
        {
            //p_dbg("eth rx frame");
            if(p_eth_netif)
            {
                eth_ethernetif_input(p_eth_netif, NULL, 0);
            }
            /* Clear the Eth DMA Rx IT pending bits */
            //ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
            //ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
            eth_int_pending = 0;
        }

        if((auto_dhcp) && (eth_state->link_state) &&(p_eth_netif->ip_addr.addr != 0))
        {
            show_tcpip_info(p_eth_netif);
            p_dbg("eth link ok and dhcp get ip!! \r\n");
            eth_state->link_state = 2;
            send_work_event(ETH_LINK_OK_EVENT);
            auto_dhcp = 0;
        }
    }
end:
    p_err_fun;
    thread_exit(thread_myself());
}

int dev_netif_status(int8_t type)
{
    int status = 0;
    status = ((struct ethernetif *)p_eth_netif->state)->link_state;
    return status;
}

int dev_netif_init(int8_t type)
{
    dev_rfl8201x_init();

    thread_create(eth_int_thread, 0, TASK_ETH_INT_PRIO, 0, 256, "eth_int_thread");

    return 0;
}

