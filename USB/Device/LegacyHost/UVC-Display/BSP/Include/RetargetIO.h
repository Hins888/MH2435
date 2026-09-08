#ifndef __RETARGET_IO_H__
#define __RETARGET_IO_H__

#include <stdio.h>
#include "mh2435.h"

#ifndef CONFIG_RETARGETIO_DEFAULT_SERIAL
#define CONFIG_RETARGETIO_DEFAULT_SERIAL PeripheralUSART1, PA9, PA10
#endif

extern void RetargetIOSetup(PeripheralEnum peripheral, IOEnum txIO, IOEnum rxIO);
extern void Serial_Send_buffer(uint8_t* buffer,uint32_t len);
#endif // __RETARGET_IO_H__
