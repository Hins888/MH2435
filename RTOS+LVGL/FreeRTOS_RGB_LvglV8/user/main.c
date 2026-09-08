#include "rtos-freertos.h"
#include "lvgl.h"
#include "run_led.h"
#include "run_lvgl.h"

static void MainTask_Handler(void* param);

TaskHandle_t MainTaskHandle;

int main(void)
{
    BaseType_t ret;
    RCC_ClocksTypeDef RCC_Clocks;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	BSP_USART_Configuration(115200);
	log_level = LOG_LEVEL_INFO;

    LOG_INFO("FreeRTOS Demo.\n");
    RCC_GetClocksFreq(&RCC_Clocks);
    LOG_INFO("SYSCLK = %d.\n", RCC_Clocks.SYSCLK_Frequency);
    LOG_INFO("AHBCLK = %d.\n", RCC_Clocks.HCLK_Frequency);
    LOG_INFO("PCLK1  = %d.\n", RCC_Clocks.PCLK1_Frequency);
    LOG_INFO("PCLK2  = %d.\n", RCC_Clocks.PCLK2_Frequency);
	
    SDRAM_PinConfig();
    SDRAM_Config();
	
    ret = xTaskCreate(MainTask_Handler, "Main Task", 1024, NULL, 9, &MainTaskHandle);
    if (ret != pdPASS) {
        LOG_ERROR("Main Task Create Error.\n");
        while (1) {}
    }

    vTaskStartScheduler();

    while (1) {}

    return 0;
}

void MainTask_Handler(void* param) {
    RunLed_Create();
	RunLvgl_Create();
    vTaskDelete(NULL);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif
