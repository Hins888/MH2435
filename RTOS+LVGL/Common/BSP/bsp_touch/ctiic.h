#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H

#include "mh2435.h"	  

#define USE_HW_IIC

#ifdef USE_HW_IIC
/*  I2C 快速模式 */
#define I2C_Speed              400000

/*I2C接口*/
#define TOUCH_I2C                          I2C1
#define TOUCH_I2C_CLK                      PeripheralI2C1

#define TOUCH_I2C_SCL_PIN                  GPIO_Pin_6                 
#define TOUCH_I2C_SCL_GPIO_PORT            GPIOB                       
#define TOUCH_I2C_SCL_GPIO_CLK             PeripheralGPIOB
#define TOUCH_I2C_SCL_SOURCE               GPIO_PinSource6
#define TOUCH_I2C_SCL_AF                   GPIO_AF_I2C1

#define TOUCH_I2C_SDA_PIN                  GPIO_Pin_7                  
#define TOUCH_I2C_SDA_GPIO_PORT            GPIOB                       
#define TOUCH_I2C_SDA_GPIO_CLK             PeripheralGPIOB
#define TOUCH_I2C_SDA_SOURCE               GPIO_PinSource7
#define TOUCH_I2C_SDA_AF                   GPIO_AF_I2C1

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
#else
//IO方向设置
#define CT_SDA_IN()  {GPIOC->MODER&=~(3<<(14*2));GPIOB->MODER|=0<<(14*2);}
#define CT_SDA_OUT() {GPIOC->MODER&=~(3<<(14*2));GPIOB->MODER|=1<<(14*2);} 
//IO操作函数
#define CT_IIC_SCL_H  GPIO_SetBits(GPIOB, GPIO_Pin_6)  //SCL
#define CT_IIC_SCL_L  GPIO_ResetBits(GPIOB, GPIO_Pin_6)

#define CT_IIC_SDA_H  GPIO_SetBits(GPIOB, GPIO_Pin_7)  //SDA
#define CT_IIC_SDA_L  GPIO_ResetBits(GPIOB, GPIO_Pin_7)

#define CT_READ_SDA (GPIO_ReadInputData(GPIOB) & GPIO_Pin_7)   //输入SDA 

#endif

uint8_t I2C_Write(uint8_t DevAddr7bits, uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite);
uint8_t I2C_Read(uint8_t DevAddr7bits,  uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);

void CT_IIC_Init(void);
#endif







