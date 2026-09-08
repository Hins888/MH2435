#include "rtos-freertos.h"

// Override the `__WEAK` SystemDelay()
void SystemDelay(volatile uint32_t ms) {
    vTaskDelay(ms);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    while(1) {}
}

void vApplicationMallocFailedHook(void) {
    while(1) {}
}
