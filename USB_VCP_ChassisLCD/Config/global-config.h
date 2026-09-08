// The file contains global config definitons of the project.
// It is force included to MDK4/5 projects by append `--preinclude global-config.h`(ARMCC) or `-include global-config.h`(ARMClang) to `C/C++ - Misc Controls`
// All the global config defintions can be set here, such as `USE_FULL_ASSERT`, `USE_STDPERIPH_DRIVER`, etc...
// Note: The `C/C++ - Preprocessor Symbols - Define` should not be set if this file has been included!

// Definitions of VCP
#define CONFIG_SYSCLK    true,  12, 240, 1, 5, 10, 2, 2

#define CONFIG_BOARD 15    //MH2435 QFN88 EVB

// Definitions of component
#define USE_COMPONENT_RETARGETIO 19
#define USE_COMPONENT_USBBSP 67
#define USE_COMPONENT_USBVCP 72

#define USE_800_480_888		0
#define USE_800_480_565		1
#define USE_480_480_565		2

#define USE_LCD_TYPE			USE_480_480_565




	