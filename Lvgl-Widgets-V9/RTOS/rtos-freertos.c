#include "rtos-freertos.h"

#if USE_RTOS_FREERTOS
// Override the `__WEAK` SystemDelay()
void SystemDelay(volatile uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}
#endif

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    while (1) __NOP();
}

void vApplicationMallocFailedHook(void) {
    while (1) __NOP();
}
