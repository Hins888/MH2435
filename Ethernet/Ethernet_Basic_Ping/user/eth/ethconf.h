/**
 ******************************************************************************
 * @file    ethconf.h
 * @author  Megahunt
 * @version V1.0
 * @date    2023-xx-xx
 * @brief
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ETHCONF_H
#define __ETHCONF_H

#ifdef __cplusplus
extern "C" {
#endif


#include "lwip/netif.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define ETHERNET_PHY_ADDRESS 0x03 /* Relative to 8201F */

/* PHY configuration section **************************************************/

/*******************  PHY Extended Registers section : ************************/

/* These values are relatives to RTL8201F/LAN8720A PHY and change from PHY to another,
   so the user have to update this value depending on the used external PHY */

/* The RTL8201F PHY status register  */
#define PHY_RTL8201F_SR            ((uint16_t)0x0000) 
#define PHY_RTL8201F_SPEED_STATUS  ((uint16_t)0x2000) 
#define PHY_RTL8201F_DUPLEX_STATUS ((uint16_t)0x0100) 

/* The LAN8720A PHY status register  */
#define PHY_LAN8720A_SR            ((uint16_t)0x001F) 
#define PHY_LAN8720A_SPEED_STATUS  ((uint16_t)0x0008) 
#define PHY_LAN8720A_DUPLEX_STATUS ((uint16_t)0x0010) 

/*
    ETH_MDIO -------------------------> PA2
    ETH_MDC --------------------------> PC1
    ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
    ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
    ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
    ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
    ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PB11
    ETH_MII_TXD0/ETH_RMII_TXD0 -------> PB12
    ETH_MII_TXD1/ETH_RMII_TXD1 -------> PB13
                                                                                        */
/* ETH_MDIO */
#define ETH_MDIO_GPIO_CLK RCC_AHB1Periph_GPIOA
#define ETH_MDIO_PORT     GPIOA
#define ETH_MDIO_PIN      GPIO_Pin_2
#define ETH_MDIO_AF       GPIO_AF_ETH
#define ETH_MDIO_SOURCE   GPIO_PinSource2

/* ETH_MDC */
#define ETH_MDC_GPIO_CLK RCC_AHB1Periph_GPIOC
#define ETH_MDC_PORT     GPIOC
#define ETH_MDC_PIN      GPIO_Pin_1
#define ETH_MDC_AF       GPIO_AF_ETH
#define ETH_MDC_SOURCE   GPIO_PinSource1

/* ETH_RMII_REF_CLK */
#define ETH_RMII_REF_CLK_GPIO_CLK RCC_AHB1Periph_GPIOA
#define ETH_RMII_REF_CLK_PORT     GPIOA
#define ETH_RMII_REF_CLK_PIN      GPIO_Pin_1
#define ETH_RMII_REF_CLK_AF       GPIO_AF_ETH
#define ETH_RMII_REF_CLK_SOURCE   GPIO_PinSource1

/* ETH_RMII_CRS_DV */
#define ETH_RMII_CRS_DV_GPIO_CLK RCC_AHB1Periph_GPIOA
#define ETH_RMII_CRS_DV_PORT     GPIOA
#define ETH_RMII_CRS_DV_PIN      GPIO_Pin_7
#define ETH_RMII_CRS_DV_AF       GPIO_AF_ETH
#define ETH_RMII_CRS_DV_SOURCE   GPIO_PinSource7

/* ETH_RMII_RXD0 */
#define ETH_RMII_RXD0_GPIO_CLK RCC_AHB1Periph_GPIOC
#define ETH_RMII_RXD0_PORT     GPIOC
#define ETH_RMII_RXD0_PIN      GPIO_Pin_4
#define ETH_RMII_RXD0_AF       GPIO_AF_ETH
#define ETH_RMII_RXD0_SOURCE   GPIO_PinSource4

/* ETH_RMII_RXD1 */
#define ETH_RMII_RXD1_GPIO_CLK RCC_AHB1Periph_GPIOC
#define ETH_RMII_RXD1_PORT     GPIOC
#define ETH_RMII_RXD1_PIN      GPIO_Pin_5
#define ETH_RMII_RXD1_AF       GPIO_AF_ETH
#define ETH_RMII_RXD1_SOURCE   GPIO_PinSource5

/* ETH_RMII_TX_EN */
#define ETH_RMII_TX_EN_GPIO_CLK RCC_AHB1Periph_GPIOB
#define ETH_RMII_TX_EN_PORT     GPIOB
#define ETH_RMII_TX_EN_PIN      GPIO_Pin_11
#define ETH_RMII_TX_EN_AF       GPIO_AF_ETH
#define ETH_RMII_TX_EN_SOURCE   GPIO_PinSource11

/* ETH_RMII_TXD0 */
#define ETH_RMII_TXD0_GPIO_CLK RCC_AHB1Periph_GPIOB
#define ETH_RMII_TXD0_PORT     GPIOB
#define ETH_RMII_TXD0_PIN      GPIO_Pin_12
#define ETH_RMII_TXD0_AF       GPIO_AF_ETH
#define ETH_RMII_TXD0_SOURCE   GPIO_PinSource12

/* ETH_RMII_TXD1 */
#define ETH_RMII_TXD1_GPIO_CLK RCC_AHB1Periph_GPIOB
#define ETH_RMII_TXD1_PORT     GPIOB
#define ETH_RMII_TXD1_PIN      GPIO_Pin_13
#define ETH_RMII_TXD1_AF       GPIO_AF_ETH
#define ETH_RMII_TXD1_SOURCE   GPIO_PinSource13

/* MAC RST */
#define ETH_PHY_RST_GPIO_CLK   RCC_AHB1Periph_GPIOB
#define ETH_PHY_RST_PORT       GPIOB
#define ETH_PHY_RST_PIN        GPIO_Pin_10

#define ETH_PHY_RST_LOW()      {ETH_PHY_RST_PORT->BSRRH=ETH_PHY_RST_PIN;}
#define ETH_PHY_RST_HIGH()     {ETH_PHY_RST_PORT->BSRRL=ETH_PHY_RST_PIN;}

/* Ethernet Flags for EthStatus variable */
#define ETH_INIT_FLAG 0x01 /* Ethernet Init Flag */
#define ETH_LINK_FLAG 0x10 /* Ethernet Link Flag */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void ETH_BSP_Config(void);
void ETH_link_callback(struct netif* netif);
void ETH_CheckLinkStatus(uint16_t PHYAddress);

#ifdef __cplusplus
}
#endif

#endif /* __ETHCONF_H */
