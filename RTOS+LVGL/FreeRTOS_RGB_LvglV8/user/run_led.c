#include "run_led.h"


#define IOT(io)        (IOPort(io)->ODR ^= (IOPin(io)))

TaskHandle_t RunLedHandle;

static void RunLed_Init(void);
static void RunLedTask_Handler(void* param);

void RunLed_Create(void)
{
    BaseType_t ret;
    
    ret = xTaskCreate(RunLedTask_Handler, "RunLed Task", 128, NULL, 3, &RunLedHandle);
    if (ret != pdPASS)
    {
        LOG_ERROR("MiniShell Task Create Error.\n");
    }
}
static void RunLedTask_Handler(void* param)
{
    RunLed_Init();
	LOG_INFO("RunLed Init OK\r\n");
	IOT(GPIO_GREE);
    while(1)
    {
        IOT(GPIO_RED);
        vTaskDelay(200);
        IOT(GPIO_GREE);
        vTaskDelay(800);

    }
	
}

void RunLed_Init(void)
{
	BSP_GPIO_Config();
}
