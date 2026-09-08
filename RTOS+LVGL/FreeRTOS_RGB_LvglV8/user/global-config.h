// The file contains global config definitons of the project.
// It is force included to MDK4/5 projects by append `--preinclude global-config.h`(ARMCC) or `-include global-config.h`(ARMClang) to `C/C++ - Misc Controls`
// All the global config defintions can be set here, such as `USE_FULL_ASSERT`, `USE_STDPERIPH_DRIVER`, etc...
// Note: The `C/C++ - Preprocessor Symbols - Define` should not be set if this file has been included!

// Definitions of Lvgl-Widgets-V8

#define LCD_EVB_4_3INCH		0
#define LCD_EVB_5INCH		1
#define LCD_EVB_7INCH		2

#define DPI_LCD				LCD_EVB_5INCH	

//#define USE_SIMULATION		//No touch IC, using software to simulate touch events

// Definitions of lvgl-demo-v9
#define USE_OSS_LVGL_DEMO 1
#define USE_OSS_LVGL_DEMO_WIDGETS 1

// Definitions of lvgl-core-v9
#define USE_OSS_LVGL 1





