#include "bsp_api.h"


static uint32_t timer_get_ms(void)
{
    return bsp_get_tick();
}

uint32_t bsp_timer_remain(ST_TIMER *end)
{
    uint32_t now, res;

    now = timer_get_ms();
    res = end->time - now;
    return res;
}

void bsp_timer_cutdown(ST_TIMER *timer, uint32_t millisecond)
{
    //NOTE it may overflow
    timer->time = timer_get_ms() + millisecond;
}

uint32_t bsp_timer_expired(ST_TIMER *timer)
{
    uint32_t cur_time = 0;
    cur_time = timer_get_ms();

    //hanle overflow
    if ((timer->time <= cur_time) && ((cur_time - timer->time) < 0x80000000))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
