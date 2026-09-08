// The file contains global config definitons of the project.
// It is force included to MDK4/5 projects by append `--preinclude global-config.h`(ARMCC) or `-include global-config.h`(ARMClang) to `C/C++ - Misc Controls`
// All the global config defintions can be set here, such as `USE_FULL_ASSERT`, `USE_STDPERIPH_DRIVER`, etc...
// Note: The `C/C++ - Preprocessor Symbols - Define` should not be set if this file has been included!

// Definitions of USB-LegacyHost-CDC
#define CONFIG_USBCDC_SERVICE_READ_IN_APP 1

// Definitions of component
#define USE_COMPONENT_RETARGETIO 19
#define USE_COMPONENT_USBLEGACYHOST_BSP 58

// Definitions of USBLegacyHost-Core
#define USE_COMPONENT_USBLEGACYHOST_OTG 64
#define USE_COMPONENT_USBLEGACYHOST_CORE 63

// Definitions of USBLegacyHost-Class
#define USE_COMPONENT_USBLEGACYHOST_CLASS_CDC 60
