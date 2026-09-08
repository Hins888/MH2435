#include "LVGLPort.h"

#include "RetargetIO.h"

void LVGLRenderTask(void* param) {
    LVGLSetup();

//    lv_display_set_rotation(NULL, LV_DISPLAY_ROTATION_90);

#if USE_OSS_LVGL_DEMO_BENCHMARK
    lv_demo_benchmark();
#elif USE_OSS_LVGL_DEMO_MUSIC
    lv_demo_music();
#elif USE_OSS_LVGL_DEMO_WIDGETS
    lv_demo_widgets();
#elif USE_OSS_LVGL_DEMO_STRESS
    lv_demo_stress();
#endif

    while (1) {
        lv_timer_handler();
    }
}

int main() {
    RetargetIOSetup(CONFIG_RETARGETIO_DEFAULT_SERIAL);
    printf("LVGL Demo Start\n");

#if USE_RTOS_FREERTOS
    NVIC_SetPriorityGrouping(0 + (7 - __NVIC_PRIO_BITS));

    BaseType_t ret = xTaskCreate(LVGLRenderTask, "LVGL", RTOSStackSizeKB(8), NULL, 3, NULL);
    if (ret != pdPASS) {
        printf("LVGL Render Task Create Error.\n");
        while (1);
    }

    vTaskStartScheduler();

    while (1);
#else
    SysTick_Config(SystemCoreClock / 1000);
    LVGLRenderTask(NULL);
#endif

    return 0;
}

#if !USE_RTOS_FREERTOS
void SysTick_Handler(void) {
    lv_tick_inc(1);
}
#endif
