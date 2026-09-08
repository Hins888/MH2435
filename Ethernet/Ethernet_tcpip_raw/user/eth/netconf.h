/**
 ******************************************************************************
 * @file    netconf.h
 * @author  Megahunt
 * @version V1.0
 * @date    2023-xx-xx
 * @brief
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NETCONF_H
#define __NETCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mh2435.h"

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
#define DHCP_IDLE             0
#define DHCP_START            1
#define DHCP_WAIT_ADDRESS     2
#define DHCP_ADDRESS_ASSIGNED 3
#define DHCP_TIMEOUT          4
#define DHCP_LINK_DOWN        5
//#define DHCP_START_IDLE       0xff

// #define LWIP_DHCP       /* enable DHCP, if disabled static address is used */

/* Uncomment SERIAL_DEBUG to enables retarget of printf to  serial port
   for debug purpose */
#define SERIAL_DEBUG

//#define DEST_IP_ADDR0 192
//#define DEST_IP_ADDR1 168
//#define DEST_IP_ADDR2 10
//#define DEST_IP_ADDR3 12

#define DEST_PORT 6000

#define UDP_SERVER_PORT 5000 /* define the UDP local connection port */
#define UDP_CLIENT_PORT 5000 /* define the UDP remote connection port */

/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0 2
#define MAC_ADDR1 0
#define MAC_ADDR2 0
#define MAC_ADDR3 0
#define MAC_ADDR4 0
#define MAC_ADDR5 1

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 10
#define IP_ADDR3 102

/*NETMASK*/
#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 255
#define NETMASK_ADDR3 0

/*Gateway Address*/
#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 10
#define GW_ADDR3 1

/**
 * @brief  Number of milliseconds when to check for link status from PHY
 */
#ifndef LINK_TIMER_INTERVAL
#define LINK_TIMER_INTERVAL 1000
#endif

/* MII and RMII mode selection*/
#define RMII_MODE

//#define MII_MODE

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void LwIP_Init(void);
void LwIP_Pkt_Handle(void);
void LwIP_Periodic_Handle(__IO uint32_t localtime);

#ifdef __cplusplus
}
#endif

#endif /* __NETCONF_H */
