/*
 * uart.h
 *
 *  Created on: 2020-4-27
 */

#ifndef __DEV_NETIF_H_
#define __DEV_NETIF_H_

#include "bsp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip\sockets.h"
#include "lwip\netif.h"
#include "lwip\dns.h"
#include "lwip\api.h"
#include "lwip\tcp.h"

#define MAX_DHCP_TRIES 4

typedef enum
{
    DHCP_IDLE   = 0,
    DHCP_START   = 1,
    DHCP_WAIT_ADDRESS = 2,
    DHCP_ADDRESS_ASSIGNED = 3,
    DHCP_LINK_DOWN   = 4,
    DHCP_TIMEOUT    = 5,
} DHCP_SETP_STATE;


int dev_netif_init(int8_t type);
int dev_netif_status(int8_t type);


int auto_get_ip(struct netif *p_netif);
void show_tcpip_info(struct netif *p_netif);


#ifdef __cplusplus
}
#endif

#endif /* __DEV_NETIF_H_ */
