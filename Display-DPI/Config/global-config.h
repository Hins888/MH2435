// The file contains global config definitons of the project.
// It is force included to MDK4/5 projects by append `--preinclude global-config.h`(ARMCC) or `-include global-config.h`(ARMClang) to `C/C++ - Misc Controls`
// All the global config defintions can be set here, such as `USE_FULL_ASSERT`, `USE_STDPERIPH_DRIVER`, etc...
// Note: The `C/C++ - Preprocessor Symbols - Define` should not be set if this file has been included!


// Definitions of component
//#define USE_COMPONENT_SDRAM_BSP 10

#define USE_COMPONENT_I2CPORT 45
#define USE_COMPONENT_TOUCH 52
#define USE_COMPONENT_TOUCH_CHIPSEMI 56
#define USE_COMPONENT_TOUCH_GOODIX 57
#define USE_COMPONENT_TOUCH_HYNITRON 58
#define USE_COMPONENT_TOUCH_ILITEK 59
#define USE_COMPONENT_TOUCH_SITRONIX 60
