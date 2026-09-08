demo说明：
点亮RGB屏幕demo。

使用说明：
1)、根据实际电路配置IO
     根据实际电路及屏幕类型在对应头文件修改IO配置，
     "DPI-DisplayBSP.h"     //RGB接口LCD IO配置

     如果屏幕支持Touch，则在"TouchBSP.h"文件修改I2C、RST、INT信号配置

2)、屏幕适配
     "DPI-DisplayBSP.h"  
     选择或新增分辨率支持，例如：
     #define CONFIG_DISPLAY_DPI_PRESET DISPLAY_DPI_PRESET_800x480
