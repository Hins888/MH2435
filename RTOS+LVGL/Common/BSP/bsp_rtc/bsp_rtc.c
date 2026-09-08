#include "./bsp_rtc/bsp_rtc.h"

#define START_YEAR 		(1970)
#define SECOND_DAY    	(86400)    
#define SECOND_HOUR    	(3600)   
#define SECOND_MIN    	(60)   

const unsigned short int mon_yday[][13] =
{
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/**
 * @brief  Set the second of the start clock.
 * @param  Sec
 * @retval None
 */
static void rtc_set_sec(unsigned int Sec)
{
	RTC_ResetCounter();
    RTC_SetRefRegister(Sec);
}

/**
 * @brief  To determine whether a year is a leap year
 * @param  year: The year to be judged
 * @retval Judgment result
 * 			0 Non-leap year
 *			1 Leap year
 */

static int isLeapYear(int year)
{
    return( (year%4 == 0 && year%100 != 0) || (year%400 == 0) );
}
 
/**
 * @brief  Get the number of days in a year
 * @param  year: The year to be queried
 * @retval Number of days
 */
static int getDaysForYear(int year)
{
    return (isLeapYear(year)?366:365);
}
 
/**
 * @brief  Gets the current count value
 * @param  None
 * @retval The final value of the RTC count
 */
static unsigned int rtc_get_sec(void)
{
    return RTC_GetRefRegister() + RTC_GetCounter();
}

/**
 * @brief  Zeller's formula determines the day of the week based on the date
 * @param  None
 * @retval Day of the week(0~6)
 */
u8 Get_Week(int year,int month,int day)
{
    if(month==1||month==2)
    {
        month+=12;
        year--;
    }
    int c=year/100;
    int y=year%100;
    int m=month;
    int d=day;
    int W=c/4-2*c+y+y/4+26*(m+1)/10+d-1;
    if(W<0)
        return (W+(-W/7+1)*7)%7;
    return W%7;
}

/**
 * @brief  Determine whether the days in the Date meet the requirements
 * @param  Time
 * @retval CALENDAR_Error: CALENDAR Error code
 */
static CALENDAR_Error checkDay(CALENDAR_TIME* time)
{
    unsigned char  day;
    int year, month;
    // u8 ret=0;
    unsigned char daycheck[] = "\x1f\x1c\x1f\x1e\x1f\x1e\x1f\x1f\x1e\x1f\x1e\x1f";

    year = time->tmYear;
    if (isLeapYear(year))
        daycheck[1] = 29;

    month = time->tmMon;
    day   = time->tmMday;
	
    if (day > daycheck[month - 1])
        return CALENDAR_MDAY_ERR;
    return CALENDAR_OK;
}

/**
 * @brief  Calculate the Greenwich time of the Date
 * @param  Time
 * @retval Sec
 */
unsigned int mkTime(CALENDAR_TIME* time)
{
    int year, month, i;
    unsigned char  daycheck[] = "\x1f\x1c\x1f\x1e\x1f\x1e\x1f\x1f\x1e\x1f\x1e\x1f";
    unsigned int days = 0, sec = 0;
	
	//calc month and day
    int isLeepYear = isLeapYear(year < time->tmYear);
	
	for(year = 1970; year < time->tmYear; year++)
	{
		days += getDaysForYear(year);	
	}

    month = time->tmMon;
	days += mon_yday[isLeepYear][month-1];
	
    days += time->tmMday - 1;
    sec = days * (24 * 3600) + time->tmHour * 3600 + time->tmMin * 60 + time->tmSec;
	
    return sec;
}


/**
 * @brief  Date based on Greenwich time
 * @param  second
 * @param  time
 * @retval Sec
 */
static void gmTime(unsigned int second, CALENDAR_TIME* time)
{
    int leftSeconds = second % SECOND_DAY;
    int days = second / SECOND_DAY;
    int curYear = START_YEAR;
    int leftDays = days;
 
    //calc year
    int daysCurYear = getDaysForYear(curYear);
    while (leftDays >= daysCurYear)
    {
        leftDays -= daysCurYear;
        curYear++;
        daysCurYear = getDaysForYear(curYear);
    }
    time->tmYear = curYear;
 
    //calc month and day
    int isLeepYear = isLeapYear(curYear);
    for (int i = 1; i < 13; i++)
    {
        if (leftDays < mon_yday[isLeepYear][i])
        {
            time->tmMon = i;
            time->tmMday = leftDays - mon_yday[isLeepYear][i-1] + 1;
            break;
        }
    }
	time->tmHour = leftSeconds / SECOND_HOUR;
    time->tmMin = (leftSeconds % SECOND_HOUR) / SECOND_MIN;
    time->tmSec = leftSeconds % SECOND_MIN;
	time->tmWday = Get_Week(time->tmYear,time->tmMon,time->tmMday);
}


/**
 * @brief  RTC Init.
 * 			Using RTC for accurate timing requires switching to external 32K
 * @param  None
 * @retval None
 */
void RTC_Config(void)
{
	uint32_t times_release = 0;
	
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_BPU, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	
	
	PWR_BackupAccessCmd(ENABLE);
	
	RCC_LSEConfig(RCC_LSE_ON);
	while(!RCC_GetFlagStatus(RCC_FLAG_LSERDY));
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
}

/**
 * @brief  Set the start time of the calendar
 * @param  Time
 * @retval CALENDAR_Error: CALENDAR Error code
 */
CALENDAR_Error bsp_RtcSetTime(CALENDAR_TIME* Time)
{
	unsigned int seconds;
    CALENDAR_Error ret;
    if ((Time->tmYear > 2099) || (Time->tmYear < 1970))
        return CALENDAR_YEAR_ERR;
    if ((Time->tmMon > 12) || (Time->tmMon == 0))
        return CALENDAR_YEAR_ERR;
    if ((Time->tmMday > 31) || (Time->tmMday == 0))
        return CALENDAR_MDAY_ERR;
    if (Time->tmHour > 23)
        return CALENDAR_MDAY_ERR;
    if (Time->tmMin > 59)
        return CALENDAR_MIN_ERR;
    if (Time->tmSec > 59)
        return CALENDAR_SEC_ERR;

    ret = checkDay(Time);
    if (ret)
        return ret;

    seconds = mkTime(Time);

    rtc_set_sec(seconds);

    return CALENDAR_OK;
}

/**
 * @brief  Get the latest calendar times
 * @param  Time
 * @retval None
 */
void bsp_RtcGetTime(CALENDAR_TIME* Time)
{
	unsigned int sec;

    sec = rtc_get_sec();
    gmTime(sec, Time);
}