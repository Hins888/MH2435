#ifndef __USBMSD_H__
#define __USBMSD_H__

#include "InterfaceMSD.h"
#include "Block.h"

#define USBMSD_DEMO_VFS_STREAM 1
#define USBMSD_DEMO_VFS_SWKEY  2
#define USBMSD_DEMO_VFS_CDROM  3

#define USBMSD_DEMO USBMSD_DEMO_VFS_STREAM

extern void MSDSetup(USBDeviceStruct* device);

#if (USBMSD_DEMO == USBMSD_DEMO_VFS_STREAM)
extern bool MSDStreamSend(uint32_t* buffer, uint32_t bytesToSend, void (*popCallback)(BlockStruct* block));
#endif

#endif
