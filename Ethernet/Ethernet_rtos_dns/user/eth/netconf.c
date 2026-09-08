/**
 ******************************************************************************
 * @file    netconf.c
 * @author  Megahunt
 * @version V1.0
 * @date    2023-xx-xx
 * @brief   Network connection configuration
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "lwip/udp.h"
#include "lwip/timeouts.h"
//#include "lwip/timers.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "netconf.h"
#include "ethconf.h"

#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
#define MAX_DHCP_TRIES 3

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif gnetif;
ip_addr_t ipaddr;
ip_addr_t netmask;
ip_addr_t gw;

uint32_t     TCPTimer  = 0;
uint32_t     ARPTimer  = 0;
uint32_t     LinkTimer = 0;
uint32_t     IPaddress = 0;

#if LWIP_DHCP
struct dhcp g_dhcp;
uint32_t     DHCPfineTimer   = 0;
uint32_t     DHCPcoarseTimer = 0;
__IO uint8_t DHCP_state = DHCP_IDLE;
#endif

extern __IO uint32_t EthStatus;

/* Private functions ---------------------------------------------------------*/



/**
 * @brief  Initializes the lwIP stack
 * @param  None
 * @retval None
 */
void LwIP_Init(void)
{
#if LWIP_DHCP
    ipaddr.addr  = 0;
    netmask.addr = 0;
    gw.addr      = 0;
#else
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif

    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
    struct ip_addr *netmask, struct ip_addr *gw,
    void *state, err_t (* init)(struct netif *netif),
    err_t (* input)(struct pbuf *p, struct netif *netif))

    Adds your network interface to the netif_list. Allocate a struct
    netif and pass a pointer to this structure as the first argument.
    Give pointers to cleared ip_addr structures when using DHCP,
    or fill them with sane numbers otherwise. The state pointer may be NULL.

    The init function pointer must point to a initialization function for
    your ethernet netif interface. The following code illustrates it's use.*/
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /*  Registers the default network interface.*/
    netif_set_default(&gnetif);

    if (EthStatus == (ETH_INIT_FLAG | ETH_LINK_FLAG))
    {
        /* Set Ethernet link flag */
        gnetif.flags |= NETIF_FLAG_LINK_UP;

        /* When the netif is fully configured this function must be called.*/
        netif_set_up(&gnetif);
#if LWIP_DHCP
        DHCP_state = DHCP_START;
        ipaddr.addr = 0;
        netmask.addr = 0;
        gw.addr = 0;
#else
    #ifdef SERIAL_DEBUG
        printf("\n  Static IP address   \n");
        printf("IP: %d.%d.%d.%d\n", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        printf("NETMASK: %d.%d.%d.%d\n", NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
        printf("Gateway: %d.%d.%d.%d\n", GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* SERIAL_DEBUG */
#endif /* LWIP_DHCP */
    }
    else
    {
        /*  When the netif link is down this function must be called.*/
        netif_set_down(&gnetif);
#if LWIP_DHCP
        DHCP_state = DHCP_LINK_DOWN;
#endif /* LWIP_DHCP */
#ifdef SERIAL_DEBUG
        printf("\n  Network Cable is  \n");
        printf("    not connected   \n");
#endif /* SERIAL_DEBUG */
    }
    /* Set the link callback function, this function is called on change of link status*/
    netif_set_link_callback(&gnetif, ETH_link_callback);
}

/**
 * @brief  Called when a frame is received
 * @param  None
 * @retval None
 */
void LwIP_Pkt_Handle(void)
{
    /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
    while(ETH_CheckFrameReceived())
    {
        ethernetif_input(&gnetif);
    }
}

/**
 * @brief  LwIP periodic tasks
 * @param  localtime the current LocalTime value
 * @retval None
 */
void LwIP_Periodic_Handle(__IO uint32_t localtime)
{
    sys_check_timeouts();
    
#if LWIP_DHCP       /* 如果使用DHCP */
    /* 每500ms调用一次dhcp_fine_tmr() */
    if (localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)    /* DHCP_FINE_TIMER_MSECS(500) lwip_localtime心跳 T3 毫秒单位 */
    {
        DHCPfineTimer = localtime;
        dhcp_fine_tmr();

        if ((DHCP_state >= DHCP_START) && (DHCP_state < DHCP_LINK_DOWN))
        {
            LwIP_DHCP_Process_Handle();  /* DHCP处理 */
        }
    }

    /* DHCP Coarse periodic process every 60s */
    if (localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
    {
        DHCPcoarseTimer = localtime;
        dhcp_coarse_tmr();
    }
#endif
}
static u8_t auto_ip_gen()
{
    u16_t v = gnetif.hwaddr[4] + gnetif.hwaddr[5] + gnetif.hwaddr[3];
    return ((v >> 8) + (v & 0xFF));
}

#if LWIP_DHCP
int8_t lwip_get_dhcp_status(void)
{
    if ((DHCP_state > DHCP_IDLE) && (DHCP_state < DHCP_LINK_DOWN))
    {
       return 1;
    }
    else return 0;
}
/**
 * @brief  LwIP_DHCP_Process_Handle
 * @param  None
 * @retval None
 */
void LwIP_DHCP_Process_Handle(void)
{
    switch (DHCP_state)
    {
        case DHCP_START: {
            ip_addr_set_zero(&ipaddr);
            ip_addr_set_zero(&netmask);
            ip_addr_set_zero(&gw);
            DHCP_state = DHCP_WAIT_ADDRESS;
            dhcp_start(&gnetif);
            /* IP address should be set to 0
               every time we want to assign a new DHCP address */
            IPaddress = 0;
            printf("正在查找DHCP服务器,请稍等...........\r\n");
        }
        break;

        case DHCP_WAIT_ADDRESS: {
            /* Read the new IP address */
            IPaddress = gnetif.ip_addr.addr;

            if (IPaddress != 0)
            {
                DHCP_state = DHCP_ADDRESS_ASSIGNED;


#ifdef SERIAL_DEBUG
                printf("\nIP address assigned by DHCP = %x\n", gnetif.ip_addr.addr);
                printf("    IP: %d.%d.%d.%d\n", (uint8_t)(gnetif.ip_addr.addr), (uint8_t)(gnetif.ip_addr.addr >> 8), 
                            (uint8_t)(gnetif.ip_addr.addr >> 16), (uint8_t)(gnetif.ip_addr.addr >> 24));
                printf("    NETMASK: %d.%d.%d.%d\n", (uint8_t)(gnetif.netmask.addr), (uint8_t)(gnetif.netmask.addr >> 8), 
                            (uint8_t)(gnetif.netmask.addr >> 16), (uint8_t)(gnetif.netmask.addr >> 24));
                printf("    Gateway: %d.%d.%d.%d\n", (uint8_t)(gnetif.gw.addr), (uint8_t)(gnetif.gw.addr >> 8), 
                            (uint8_t)(gnetif.gw.addr >> 16), (uint8_t)(gnetif.gw.addr >> 24));
#endif /* SERIAL_DEBUG */
                IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, auto_ip_gen());
                IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                
                /* Stop DHCP */
                //dhcp_stop(&gnetif);
                DHCP_state = DHCP_IDLE;
            }
            else
            {
                /* DHCP timeout */
                if (g_dhcp.tries > MAX_DHCP_TRIES)
                {
                    DHCP_state = DHCP_TIMEOUT;

                    /* Stop DHCP */
                    //dhcp_stop(&gnetif);

                    /* Static address used */
                    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, auto_ip_gen());
                    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                    //netif_set_addr(&gnetif, &ipaddr, &netmask, &gw);

#ifdef SERIAL_DEBUG
                    printf("\nDHCP timeout, Use static IP address \n");
                    printf("    IP: %d.%d.%d.%d\n", IP_ADDR0, IP_ADDR1, IP_ADDR2, auto_ip_gen());
                    printf("    NETMASK: %d.%d.%d.%d\n", NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                    printf("    Gateway: %d.%d.%d.%d\n", GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* SERIAL_DEBUG */
                }
            }
        }
        break;
        case DHCP_TIMEOUT:
            DHCP_state = DHCP_LINK_DOWN;
            //dhcp_stop(&gnetif);
            break;
        default:
            break;
    }
}
#endif
