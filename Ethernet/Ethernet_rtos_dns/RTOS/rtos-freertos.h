#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

#define RTOSStackSizeKB(size) (size * 1024 / sizeof(StackType_t))
