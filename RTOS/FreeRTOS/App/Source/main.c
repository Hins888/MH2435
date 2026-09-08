#include "run_led.h"

/* Private function prototypes -----------------------------------------------*/

static void MainTask_Handler(void* param);

TaskHandle_t MainTaskHandle;

int main(void) {
    BaseType_t ret;

    RCC_ClocksTypeDef RCC_Clocks;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    RetargetIOSetup(CONFIG_RETARGETIO_DEFAULT_SERIAL);

    printf("FreeRTOS Demo.\n");


    RCC_GetClocksFreq(&RCC_Clocks);

    printf("SYSCLK = %d.\n", RCC_Clocks.SYSCLK_Frequency);
    printf("AHBCLK = %d.\n", RCC_Clocks.HCLK_Frequency);
    printf("PCLK1  = %d.\n", RCC_Clocks.PCLK1_Frequency);
    printf("PCLK2  = %d.\n", RCC_Clocks.PCLK2_Frequency);

    ret = xTaskCreate(MainTask_Handler, "Main Task", 1024, NULL, 9, &MainTaskHandle);
    if (ret != pdPASS) {
        printf("Main Task Create Error.\n");
        while (1) {}
    }

    vTaskStartScheduler();

    while (1) {}

    return 0;
}

void MainTask_Handler(void* param) {
    RunLed_Create();

    vTaskDelete(NULL);
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {}
}
#endif
