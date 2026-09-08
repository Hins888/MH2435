#ifndef __USBVCP_H__
#define __USBVCP_H__

#include "USBDevice.h"
#include "InterfaceCDC.h"
#include "InterfaceSerial.h"

#define USBVCP_LOOPBACK_DEMO_BY_READCALLBACK_SENDBUFFER 1

#ifdef SERIAL_SUPPORT_SEND_BYTES
#define USBVCP_SEND_BYTE_BUFFER_SIZE 2048

#define USBVCP_LOOPBACK_DEMO_BY_READCALLBACK_SENDBYTES 2
#endif

#ifdef SERIAL_SUPPORT_READ_BYTES
#define USBVCP_READ_BYTE_BUFFER_SIZE 2048

#define USBVCP_LOOPBACK_DEMO_BY_READBYTES_SENDBUFFER 3
#ifdef SERIAL_SUPPORT_SEND_BYTES
#define USBVCP_LOOPBACK_DEMO_BY_READBYTES_SENDBYTES 4
#endif
#endif

#define USBVCP_SENDTEST_DEMO 5

#define USBVCP_DEMO USBVCP_LOOPBACK_DEMO_BY_READBYTES_SENDBYTES

extern void VCPSetup(USBDeviceStruct* device);

#ifdef SERIAL_SUPPORT_SEND_BYTES
extern uint32_t VCPSendBytes(uint8_t* buffer, uint32_t size);
extern uint32_t VCPSendBufferSize(bool isFreeSize);
#endif

#ifdef SERIAL_SUPPORT_READ_BYTES
extern uint32_t VCPReadBytes(uint8_t* buffer, uint32_t size);
extern uint32_t VCPReadBufferSize(bool isFreeSize);
#endif

extern bool VCPSendBufferWithCallback(uint32_t* buffer, uint32_t size, void (*sentCallback)(BlockStruct* block));

extern bool VCPSendBufferSync(uint32_t* buffer, uint32_t size);

#if (USBVCP_DEMO > 2) && (USBVCP_DEMO <= 4)
extern void VCPLoopbackByReadBytes(void);
#define VCPLoop() VCPLoopbackByReadBytes()
#else
#define VCPLoop()
#endif

#endif
