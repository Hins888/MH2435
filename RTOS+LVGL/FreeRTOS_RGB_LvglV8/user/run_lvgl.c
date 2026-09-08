#include "run_Lvgl.h"

TaskHandle_t RunLvglHandle;

 void RunLvgl_Init(void);
static void RunLvglTask_Handler(void* param);
void RunLvgl_Create(void)
{
    BaseType_t ret;
    
    ret = xTaskCreate(RunLvglTask_Handler, "RunLvgl Task", 2048, NULL, 2, &RunLvglHandle);
    if (ret != pdPASS)
    {
        LOG_ERROR("MiniShell Task Create Error.\n");
    }
}

static void RunLvglTask_Handler(void* param)
{
	static uint32_t count = 0;
    RunLvgl_Init();
	BL_Unit_Config(100);
	
	LOG_INFO("RunLvgl Init OK\r\n");
    while(1)
    {
        lv_timer_handler();
		vTaskDelay(2);
		count++;
#ifdef USE_SIMULATION
		if(0 == count%400)
			Touch_offset = (Touch_offset+1)%6;
#endif
    }
}


void RunLvgl_Init(void)
{
	lv_init();

	lv_port_disp_init();
	lv_port_indev_init();

#ifdef SELECT_DEMO_WIDGETS
	lv_demo_widgets();
#endif

}

void vApplicationTickHook()
{
    lv_tick_inc(1);
}
