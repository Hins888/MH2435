#include "usart.h"
#include "ethconf.h"
#include "netconf.h"
#include "lwip/tcp.h"

__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
extern __IO uint8_t EthLinkStatus; 
extern __IO uint32_t EthStatus;

static void   TIM3_Config(uint16_t period, uint16_t prescaler);
static void stop_timer3(void);

#if LWIP_DHCP 
extern int8_t lwip_get_dhcp_status(void);
void dhcp_handle()
{
    while (1)
    {
        /* check if any packet received */
        if (ETH_CheckFrameReceived())
        {
            /* process received ethernet packet */
            LwIP_Pkt_Handle();
            // printf("recv packet \r\n");
        }
        /* handle periodic timers for LwIP */
        LwIP_Periodic_Handle(LocalTime);
        
        if(lwip_get_dhcp_status() == 0)
        {
            printf("  dhcp end...\n");
            break;
        }
        //ping_timer_check();
        //heart_tick_update();
        
    }
}
#endif

int main(void)
{
    RCC_ClocksTypeDef RCC_ClocksStatus;

    USART_Configuration(115200);
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    printf("ETH Demo.\n");
    printf("****CPU: %3.1fMhz, HCLK: %3.1fMhz, PCLK: %3.1fMhz*****\n", (float)RCC_ClocksStatus.SYSCLK_Frequency / 1000000,
           (float)RCC_ClocksStatus.HCLK_Frequency / 1000000, (float)RCC_ClocksStatus.PCLK1_Frequency / 1000000);

    printf("note: Use the computer in the net ping local ip.\n");

    /* local ip and port can modify in netconf.h file */

    /* Configure 10ms Timer for lwip */
    TIM3_Config(999, 1799);
    printf("eth init... \n");
    /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
    ETH_BSP_Config();
    if(!(EthStatus & ETH_INIT_FLAG))
    {
        printf("no network\n");
        stop_timer3();
        while(1);
    }
    /* Initilaize the LwIP stack */
    printf("lwip init...\n");
    LwIP_Init();
    if(!(EthStatus & ETH_INIT_FLAG))
    {
        printf("no network\n");
        stop_timer3();
        while(1);
    }
    
#if LWIP_DHCP
    printf("start dhcp please wait...\n");
    dhcp_handle();
#else
    printf("Static ip: %d.%d.%d.%d\n", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
#endif
    
    printf("start lwip test......\n");
    while (1)
    {
        /* check if any packet received */
        if (ETH_CheckFrameReceived())
        {
            /* process received ethernet packet */
            LwIP_Pkt_Handle();
        }
        /* handle periodic timers for LwIP */
        LwIP_Periodic_Handle(LocalTime);
    }
}

static void stop_timer3(void)
{
    TIM_Cmd(TIM3, DISABLE);
}
/**
 * @brief  timer 3 init
 * @param  period : period value
 * @param  prescaler : prescaler value
 * @retval None
 * @note   Tout=((period+1)*(prescaler+1))/Ft us.
 *         Ft=pclk*2=180,Mhz
 */
static void TIM3_Config(uint16_t period, uint16_t prescaler)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Prescaler     = prescaler;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period        = period;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  Timer 3 interrupt service funcations
 * @param  None
 * @retval None
 */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        /* 10ms increment */
        LocalTime += 10;
    }
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

u32_t sys_now()
{
    return LocalTime;
}
