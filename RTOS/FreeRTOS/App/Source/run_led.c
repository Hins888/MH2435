#include "run_led.h"

TaskHandle_t RunLedHandle;

static void RunLed_Init(void);
static void RunLedTask_Handler(void* param);

void RunLed_Create(void)
{
    BaseType_t ret;
    
    ret = xTaskCreate(RunLedTask_Handler, "RunLed Task", 128, NULL, 2, &RunLedHandle);
    if (ret != pdPASS)
    {
        printf("MiniShell Task Create Error.\n");
    }
}

static void RunLedTask_Handler(void* param)
{
    RunLed_Init();
    
    while(1)
    {
        GPIO_ResetBits(GPIOE, GPIO_Pin_2);
        vTaskDelay(200);
        GPIO_SetBits(GPIOE, GPIO_Pin_2);
        vTaskDelay(800);
    }
}

void RunLed_Init(void)
{
    GPIO_InitTypeDef gpio;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    
    GPIO_SetBits(GPIOE, GPIO_Pin_2);
    
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Pin = GPIO_Pin_2;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &gpio);
}
