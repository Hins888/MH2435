// The file contains global config definitons of the project.
// It is force included to MDK4/5 projects by append `--preinclude global-config.h`(ARMCC) or `-include global-config.h`(ARMClang) to `C/C++ - Misc Controls`
// All the global config defintions can be set here, such as `USE_FULL_ASSERT`, `USE_STDPERIPH_DRIVER`, etc...
// Note: The `C/C++ - Preprocessor Symbols - Define` should not be set if this file has been included!


// Definitions of component
#define USE_COMPONENT_RETARGETIO 19
#define USE_COMPONENT_USBBSP 67
#define USE_COMPONENT_VISUALFATFS 74
#define USE_COMPONENT_USBMSD 70
