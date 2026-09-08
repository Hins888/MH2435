// The file contains global config definitons of the project.
// It is force included to MDK4/5 projects by append `--preinclude global-config.h`(ARMCC) or `-include global-config.h`(ARMClang) to `C/C++ - Misc Controls`
// All the global config defintions can be set here, such as `USE_FULL_ASSERT`, `USE_STDPERIPH_DRIVER`, etc...
// Note: The `C/C++ - Preprocessor Symbols - Define` should not be set if this file has been included!

#define CONFIG_BOARD 15       //MH2435_QFN88_EVB

// Definitions of component
#define USE_COMPONENT_CAMERA 1
#define USE_COMPONENT_CAMERA_XS9950 8
#define USE_COMPONENT_DISPLAY_DPI 18
#define USE_COMPONENT_I2CPORT 42