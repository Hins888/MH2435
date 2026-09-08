// The file contains global config definitons of the project.
// It is force included to MDK4/5 projects by append `--preinclude global-config.h`(ARMCC) or `-include global-config.h`(ARMClang) to `C/C++ - Misc Controls`
// All the global config defintions can be set here, such as `USE_FULL_ASSERT`, `USE_STDPERIPH_DRIVER`, etc...
// Note: The `C/C++ - Preprocessor Symbols - Define` should not be set if this file has been included!

// Definitions of Lvgl-Widgets

// Definitions of lvgl-demo
#define USE_OSS_LVGL_DEMO 1
#define USE_OSS_LVGL_DEMO_WIDGETS 1

// Definitions of lvgl-core
#define USE_OSS_LVGL 1

// Definitions of rtos-freertos
#define USE_RTOS_FREERTOS 0
#define USE_RTOS_FREERTOS_HEAP4 1

#define CONFIG_LVGL_GAU_CACHE_SIZE (512 * 1024)

#define CONFIG_LVGL_PORT_SUPPORT_ROTATION 1

// Definitions of component
#define USE_COMPONENT_DISPLAY 12
#define USE_COMPONENT_DISPLAY_DPI 18
#define USE_COMPONENT_GAUPORT 41
#define USE_COMPONENT_I2CPORT 42
#define USE_COMPONENT_RETARGETIO 44
#define USE_COMPONENT_TOUCH 49
#define USE_COMPONENT_TOUCH_CHIPSEMI 52
#define USE_COMPONENT_TOUCH_GOODIX 53
#define USE_COMPONENT_TOUCH_HYNITRON 54
#define USE_COMPONENT_TOUCH_ILITEK 55
#define USE_COMPONENT_TOUCH_SITRONIX 56
