#ifndef __BSP_RTC_H
#define __BSP_RTC_H

#include "mh2435.h"

typedef struct 
{
    int tmSec;   
    int tmMin;   
    int tmHour; 
    int tmMday;  
    int tmMon;   
    int tmYear;  
    int tmWday;  
    int tmYday;  
}CALENDAR_TIME;

typedef enum
{
	CALENDAR_OK = 0,
	CALENDAR_YEAR_ERR,
	CALENDAR_MON_ERR, 
	CALENDAR_MDAY_ERR, 
	CALENDAR_HOUR_ERR,  
	CALENDAR_MIN_ERR,  
	CALENDAR_SEC_ERR,  
}CALENDAR_Error;

void RTC_Config(void);
unsigned int mkTime(CALENDAR_TIME* time);
void bsp_RtcGetTime(CALENDAR_TIME* Time);	
CALENDAR_Error bsp_RtcSetTime(CALENDAR_TIME* Time);
#endif
