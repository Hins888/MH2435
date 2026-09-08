#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include  "bsp_cpu.h"

#define GPIO_BASE_ADDR GPIOA_BASE

typedef enum
{ 
  GPIO_Mode_A_IN = 0x0,                 // 模拟输入
  GPIO_Mode_IN_FLOATING = 0x04,         // 浮空输入
  GPIO_Mode_IPD = 0x28,                 // 下拉输入
  GPIO_Mode_IPU = 0x48,                 // 上拉输入
  GPIO_Mode_Out_OD = 0x14,              // 开漏输出
  GPIO_Mode_Out_PP = 0x10,              // 推免输出
  GPIO_Mode_AF_OD = 0x1C,               // 复用开漏输出
  GPIO_Mode_AF_PP = 0x18,               // 复用推免输出
  GPIO_Mode_AF_IF = 0x19,               // 复用浮空输入
  GPIO_Mode_AF_IPU = 0x1A,               // 复用上拉输入
}GPIOMode_TypeDef_User;


#define GPIO_PIN_NONE 0xFFFF 

#define GPIO_GROUP_MAX 16

#define PA(n)     (0 * 16 + n)
#define PB(n)     (1 * 16 + n)
#define PC(n)     (2 * 16 + n)
#define PD(n)     (3 * 16 + n)
#define PE(n)     (4 * 16 + n)
#define PF(n)     (5 * 16 + n)
#define PG(n)     (6 * 16 + n)
#define PH(n)     (7 * 16 + n)
#define PI(n)     (8 * 16 + n)

#define GPIO_AF_GPIO    0XFF

int bsp_gpio_config(unsigned int gpio, int mode, int gpio_af);
int bsp_gpio_request(unsigned int gpio, const char* label);
void bsp_gpio_free(unsigned gpio);
void bsp_gpio_direction_input(unsigned gpio);
void bsp_gpio_direction_output(unsigned int gpio, int value);
void bsp_gpio_set_value(unsigned gpio, int value);
int bsp_gpio_get_value(unsigned gpio);
inline int bsp_gpio_get_output_value(unsigned gpio);
void bsp_gpio_set_debounce(unsigned gpio, unsigned debounce);
int bsp_gpio_to_irq(unsigned int gpio, int tirgger, void (*handler)(void* data));


#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

#ifdef __cplusplus
}
#endif

#endif //__BSP_GPIO_H
