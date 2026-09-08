#ifndef __USBBSP_H__
#define __USBBSP_H__

#include "USBDevice.h"

#if USE_COMPONENT_USBMSD
#include "USBMSD.h"
#endif // USE_COMPONENT_USBMSD

#if USE_COMPONENT_USBVCP
#include "USBVCP.h"
#endif // USE_COMPONENT_USBVCP

#if USE_COMPONENT_USBHID
#include "USBHID.h"
#endif // USE_COMPONENT_USBHID

#if USE_COMPONENT_USBCCID
#include "USBCCID.h"
#endif // USE_COMPONENT_USBCCID

#if USE_COMPONENT_USBVENDOR
#include "USBVENDOR.h"
#endif // USE_COMPONENT_USBVENDOR

#if USE_COMPONENT_USBUVC
#include "USBUVC.h"
#endif // USE_COMPONENT_USBUVC

extern USBDeviceStruct USBDevice;

extern void USBSetup(void);
extern void USBLoop(void);
extern void USBStop(void);

#endif
