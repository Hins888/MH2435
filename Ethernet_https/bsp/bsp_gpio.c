#include "bsp_api.h"
#include "bsp_gpio.h"

uint32_t GPIOx_ADDR_LIST[9] = 
{
    (GPIOA_BASE + 0x0000),
    (GPIOA_BASE + 0x0400),
    (GPIOA_BASE + 0x0800),
    (GPIOA_BASE + 0x0C00),
    (GPIOA_BASE + 0x1000),
    (GPIOA_BASE + 0x1400),
    (GPIOA_BASE + 0x1800),
    (GPIOA_BASE + 0x1C00),
    (GPIOA_BASE + 0x2000) ,
};
uint32_t GPIO_Request[10] = {0};

#define gpio_to_addr(gpio)      GPIOx_ADDR_LIST[((gpio & 0xf0) >> 4) & (0x0f)]
#define gpio_to_pin(gpio)       (0x01 <<(gpio & 0x0f))
#define gpio_to_pinsrc(gpio)    (gpio & 0x0f)

int bsp_gpio_config(unsigned int gpio, int mode, int gpio_af)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_TypeDef * GPIOx = (GPIO_TypeDef *)gpio_to_addr(gpio);
    
    //printf("gpio init: gpiox=%d, pin=%p", (((gpio & 0xf0) >> 4) & (0x0f)), GPIOx_ADDR_LIST[2]);
    GPIO_StructInit(&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = gpio_to_pin(gpio);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    if (mode == GPIO_Mode_A_IN)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else if (mode == GPIO_Mode_IN_FLOATING)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else if (mode == GPIO_Mode_IPD)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    }
    else if (mode == GPIO_Mode_IPU)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    }
    else if (mode == GPIO_Mode_Out_OD)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else if (mode == GPIO_Mode_Out_PP)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else
    if (mode == GPIO_Mode_AF_OD)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else if (mode == GPIO_Mode_AF_PP)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    }
    else if (mode == GPIO_Mode_AF_IF)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else if (mode == GPIO_Mode_AF_IPU)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    }
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    
    if(gpio_af != GPIO_AF_GPIO)
    {
       GPIO_PinAFConfig(GPIOx, gpio_to_pinsrc(gpio), gpio_af);
    }
    return 0;
}

int bsp_gpio_request(unsigned int gpio, const char* label)
{
    uint32_t index, gpiox;
    gpiox = gpio &0xf0 >> 4;
    index = gpio &0x0f;
    
    if (GPIO_Request[gpiox] & (BIT(index)))
        return -1;
    GPIO_Request[gpiox] |= BIT(index);

    return 0;
}


void bsp_gpio_free(unsigned gpio)
{
    uint32_t gpiox, pinx;
    gpiox = gpio &0xf0 >> 4;
    pinx = gpio &0x0f;
 
    GPIO_Request[gpiox] &= ~(pinx);
    
    return;
}

void bsp_gpio_direction_input(unsigned gpio)
{
    //uint32_t pinx;
    //GPIO_TypeDef * GPIOx = (GPIO_TypeDef *)gpio_to_addr(gpio);
    //pinx =  gpio_to_pin(gpio);

    //GPIOx->OEN |= (pinx); // Input Mode
}


void bsp_gpio_direction_output(unsigned int gpio, int value)
{
    uint32_t pinx;
    GPIO_TypeDef * GPIOx = (GPIO_TypeDef *)gpio_to_addr(gpio);
    pinx =  gpio_to_pin(gpio);

    if (value)
        GPIO_SetBits(GPIOx, pinx);
    else
        GPIO_ResetBits(GPIOx, pinx);
}


inline void bsp_gpio_set_value(unsigned gpio, int value)
{
    uint32_t pinx;
    GPIO_TypeDef * GPIOx = (GPIO_TypeDef *)gpio_to_addr(gpio);
    pinx =  gpio_to_pin(gpio);

    if (value)
        GPIO_SetBits(GPIOx, pinx);
    else
        GPIO_ResetBits(GPIOx, pinx);
}


inline int bsp_gpio_get_value(unsigned gpio)
{
    uint32_t pinx;
    GPIO_TypeDef * GPIOx = (GPIO_TypeDef *)gpio_to_addr(gpio);
    pinx =  gpio_to_pin(gpio);
    
    return GPIO_ReadInputDataBit(GPIOx, pinx);
}


int bsp_gpio_get_output_value(unsigned gpio)
{
    uint32_t pinx;
    GPIO_TypeDef * GPIOx = (GPIO_TypeDef *)gpio_to_addr(gpio);
    pinx =  gpio_to_pin(gpio);
    
    return GPIO_ReadOutputDataBit(GPIOx, pinx);
}


void bsp_gpio_set_debounce(unsigned gpio, unsigned debounce)
{

}

int bsp_gpio_to_irq(unsigned int gpio, int tirgger, void (*handler)(void* data))
{
    //int index, gpiox;
    //gpiox = gpio / 16;
    //index = gpio % 16;

    //LogInfo("%s%d gpio to irq x:%d index:%d ", __func__, __LINE__, gpiox, index);

    // LogInfo("%s%d gpio to irq x:%d handle:%p ", __func__, __LINE__, gpio, gpio_handle[gpiox][index].handler);
    return 0;
}
