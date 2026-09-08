#ifndef _DRIVER_MISC_H
#define _DRIVER_MISC_H

//define UART PORT

#define UART1_RX_GPIO           PA(9)
#define UART1_TX_GPIO           PA(10)
#define UART1_TX_IO_AF          GPIO_AF_USART1
#define UART1_RX_IO_AF          GPIO_AF_USART1

#define UART3_RX_GPIO           PB(14)
#define UART3_TX_GPIO           PB(15)
#define UART3_TX_IO_AF          GPIO_AF_USART3
#define UART3_RX_IO_AF          GPIO_AF_USART3


//define USB PORT

#define USB_DP_GPIO             PA(11)
#define USB_DM_GPIO             PA(12)

#define USB_DP_IO_AF            GPIO_AF_USB
#define USB_DM_IO_AF            GPIO_AF_USB

//define sdio PORT
#define SDIO_CLK_GPIO               PB(2)
#define SDIO_CLK_AF                 GPIO_AF_SDIO

#define SDIO_D0_GPIO                PB(4)
#define SDIO_D0_AF                  GPIO_AF_SDIO

#define SDIO_D1_GPIO                PA(8)
#define SDIO_D1_AF                  GPIO_AF_SDIO

#define SDIO_D2_GPIO                PC(10)
#define SDIO_D2_AF                  GPIO_AF_SDIO

#define SDIO_D3_GPIO                PC(11)
#define SDIO_D3_AF                  GPIO_AF_SDIO

#define SDIO_CMD_GPIO               PD(2)
#define SDIO_CMD_AF                 GPIO_AF_SDIO

#define WIFI_RESET_GPIO             GPIO_PIN_NONE
#define WIFI_RESET_AF               GPIO_AF_GPIO

#define WIFI_PDN_GPIO               GPIO_PIN_NONE
#define WIFI_PDN_AF                 GPIO_AF_GPIO

#define WIFI_RESET_HIGH             bsp_gpio_set_value(SDIO_RESET_GPIO, 1)
#define WIFI_RESET_LOW              bsp_gpio_set_value(SDIO_RESET_GPIO, 0)

#define WIFI_PDN_HIGH               bsp_gpio_set_value(SDIO_PDN_GPIO, 1)
#define WIFI_PDN_LOW                bsp_gpio_set_value(SDIO_PDN_GPIO, 0)


//define DCMI PORT
#define DCMI_HS_GPIO                PH(8)
#define DCMI_HS_IO_AF               GPIO_AF_DCMI

#define DCMI_VS_GPIO                PG(9)
#define DCMI_VS_IO_AF               GPIO_AF_DCMI

#define DCMI_PCLK_GPIO              PA(6)
#define DCMI_PCLK_IO_AF             GPIO_AF_DCMI

#define DCMI_D0_GPIO                PC(6)
#define DCMI_D0_IO_AF               GPIO_AF_DCMI

#define DCMI_D1_GPIO                PC(7)
#define DCMI_D1_IO_AF               GPIO_AF_DCMI

#define DCMI_D2_GPIO                PC(8)
#define DCMI_D2_IO_AF               GPIO_AF_DCMI

#define DCMI_D3_GPIO                PC(9)
#define DCMI_D3_IO_AF               GPIO_AF_DCMI

#define DCMI_D4_GPIO                PE(4)
#define DCMI_D4_IO_AF               GPIO_AF_DCMI

#define DCMI_D5_GPIO                PD(3)
#define DCMI_D5_IO_AF               GPIO_AF_DCMI

#define DCMI_D6_GPIO                PE(5)
#define DCMI_D6_IO_AF               GPIO_AF_DCMI

#define DCMI_D7_GPIO                PE(6)
#define DCMI_D7_IO_AF               GPIO_AF_DCMI

#define DCMI_MCO_GPIO               PA(5)
#define DCMI_MCO_IO_AF              GPIO_AF_DCMI

#define DCMI_RST_GPIO               PD(4)
#define DCMI_RST_IO_AF              GPIO_AF_GPIO

#define DCMI_PWD_GPIO               PI(5)
#define DCMI_PWD_IO_AF              GPIO_AF_GPIO

#define SIO_C_GPIO                  PH(4)
#define SIO_C_IO_AF                 GPIO_AF_I2C

#define SIO_D_GPIO                  PB(3)
#define SIO_D_IO_AF                 GPIO_AF_I2C
                                                
#define SIO_D_OUT           bsp_gpio_direction_output((uint32_t)SIO_D_GPIO, 1);
#define SIO_D_IN            bsp_gpio_direction_input((uint32_t)SIO_D_GPIO);

//end define DCMI PORT

//start define AUDIO PORT
#define DAC2_PORT                   GPIOA
#define DAC2_PIN                    GPIO_Pin_5

#define ADC_PORT                    GPIOA
#define ADC_PIN                     GPIO_Pin_3
//end define audio PORT


#define  ETH_MDIO_GPIO              PA(2)
#define  ETH_MDIO_GPIO_AF           GPIO_AF_ETH

#define  ETH_MDC_GPIO               PC(1)
#define  ETH_MDC_GPIO_AF            GPIO_AF_ETH

#define  ETH_TXD0_GPIO              PB(12)
#define  ETH_TXD0_GPIO_AF           GPIO_AF_ETH

#define  ETH_TXD1_GPIO              PB(13)
#define  ETH_TXD1_GPIO_AF           GPIO_AF_ETH

#define  ETH_TX_EN_GPIO             PB(11)
#define  ETH_TX_EN_GPIO_AF          GPIO_AF_ETH

#define  ETH_RXD0_GPIO              PC(4)
#define  ETH_RXD0_GPIO_AF           GPIO_AF_ETH

#define  ETH_RXD1_GPIO              PC(5)
#define  ETH_RXD1_GPIO_AF           GPIO_AF_ETH

#define  ETH_CRSDV_GPIO             PA(7)
#define  ETH_CRSDV_GPIO_AF          GPIO_AF_ETH

#define  ETH_RCLK_GPIO              PA(1)
#define  ETH_RCLK_GPIO_AF           GPIO_AF_ETH

#define  ETH_RESET_GPIO             PB(10)
#define  ETH_RESET_GPIO_AF          GPIO_AF_GPIO
//END ETH PORT

#define GPIO_SET(port,pin)          (port->BSRRL = pin)
#define GPIO_CLR(port,pin)          (port->BSRRH = pin)
#define GPIO_STAT(port,pin)         (!!(port->IDR & pin))


//LED
#define IND1_GPIO                   GPIO_PIN_NONE
#define IND1_ON                     do{bsp_gpio_set_value(IND1_GPIO, 1); }while(0)
#define IND2_ON                     do{bsp_gpio_set_value(IND1_GPIO, 0); }while(0)

//BUTTON
#define BUTTON_GPIO                 GPIO_PIN_NONE
#define BUTTON_STAT                 bsp_gpio_get_value(GPIO_PIN_NONE)     //按键1     PC4

void usr_gpio_init(void);
int check_rst_stat(void);
void driver_misc_init(void);
void driver_gpio_init(void);

#endif
