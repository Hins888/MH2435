demo说明：
运行 LVGL V9版本官方demo。

使用说明：
1)、根据项目需要配置是否需要Freertos
      //该宏为1表示支持RTOS，为0表示不支持RTOS
      #define USE_RTOS_FREERTOS 1

2)、是否启用DMA2D加速
     //定义CONFIG_LVGL_GAU_CACHE_SIZE大小表示启用加速，大小改为0表示不启用加速，CACHE大小可根据SRAM空闲情况配置
     #define CONFIG_LVGL_GAU_CACHE_SIZE (512 * 1024)  

3)、是否支持旋转
     //该宏为1表示支持旋转，为0不需要旋转。注意启用旋转需要CONFIG_LVGL_GAU_CACHE_SIZE宏定义不为0
     #define CONFIG_LVGL_PORT_SUPPORT_ROTATION 1      

4)、根据实际电路配置IO
     根据实际电路及屏幕类型在对应头文件修改IO配置，
     "DPI-DisplayBSP.h"     //RGB接口LCD IO配置

     如果屏幕支持Touch，则在"TouchBSP.h"文件修改I2C、RST、INT信号配置

