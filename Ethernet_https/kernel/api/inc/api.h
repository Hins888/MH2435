#ifndef _API_H_
#define _API_H_

//kernel
#ifdef OS_FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#endif
#ifdef OS_UCOS
#ifdef UCOS_V3
#include "os.h"
#include "os_cfg_app.h"
#else
#include "ucos_ii.h"
#endif
#endif
#include "atomic.h"
#include "mbox.h"
#include "mutex.h"
#include "sem.h"
#include "wait.h"
#include "task_api.h"
#include "timer.h"
#include "msg_q.h"
#include "memory.h"

extern volatile uint32_t jiffies;
#ifdef OS_FREE_RTOS
extern volatile int           OSIntNesting;      
#endif


typedef struct
{
    uint32  addrType;    //
    uint32  dhcpEnable;  //
    char ipAddr[64];  //
    char gateWay[64]; //
    char subMask[64]; //
    char dns1[64];    //
    char dns2[64];    //
} ST_IP_PROPERTIES;  //properties//

#endif
