#ifndef __RETARGET_IO_H__
#define __RETARGET_IO_H__

#include <stdio.h>
#include "mh2435.h"

#ifndef CONFIG_RETARGETIO_DEFAULT_SERIAL
#define CONFIG_RETARGETIO_DEFAULT_SERIAL PeripheralUSART1, PA9, PA10
#endif

extern void RetargetIOSetup(PeripheralEnum peripheral, IOEnum txIO, IOEnum rxIO);

#endif // __RETARGET_IO_H__
