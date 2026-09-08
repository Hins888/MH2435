#ifndef __BSP_API_H
#define __BSP_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_cpu.h"
#include "bsp_uart.h"
#include "bsp_gpio.h"

typedef struct
{
    int tmSec;   /*  Seconds:  0-59  (K&R  says  0-61?)  */
    int tmMin;   /*  Minutes:  0-59  */
    int tmHour;  /*  Hours  since  midnight:  0-23  */
    int tmMday;  /*  Day  of  the  month:  1-31  */
    int tmMon;   /*  Months  *since*  january:  0-11  */
    int tmYear;  /*  Years  since  1900  */
    int tmWday;  /*  Days  since  Sunday  (0-6)  */
    int tmYday;  /*  Days  since  Jan.  1:  0-365  */
    int tmIsdst; /*  +1  Daylight  Savings  Time,  0  No  DST,
                  *  -1  don't  know  */
} ST_DATE;


typedef struct {
    long time;
}ST_TIMER;

void bsp_rcc_init(void);
uint32_t bsp_get_tick(void);
void bsp_delayms(uint32_t nms);
void bsp_delayus(uint32_t nus);
void bsp_taskdelay(uint32_t msec);


void bsp_adc_init(void);
int bsp_get_adc(uint8_t channel);

int bsp_gen_random(unsigned char *random, unsigned int len);

uint32_t bsp_timer_remain(ST_TIMER *end);
void bsp_timer_cutdown(ST_TIMER *timer, uint32_t millisecond);
uint32_t bsp_timer_expired(ST_TIMER *timer);

#ifdef __cplusplus
}
#endif

#endif //__BSP_API_H
