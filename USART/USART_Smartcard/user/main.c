#include "./usart/bsp_debug_usart.h"

/** @addtogroup USART_SmartCard
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
#include "./smartcard/bsp_smartcard.h"
#include "./usart/bsp_debug_usart.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Directories & Files ID */
const uint8_t MasterRoot[2] = {0x3F, 0x00};
const uint8_t GSMDir[2] = {0x7F, 0x20};
const uint8_t ICCID[2] = {0x2F, 0xE2};
const uint8_t IMSI[2] = {0x6F, 0x07};
const uint8_t CHV1[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};

/* APDU Transport Structures */
SC_ADPU_Commands SC_ADPU;
SC_ADPU_Responce SC_Responce;
__IO uint32_t TimingDelay = 0;
static __IO ErrorStatus HSEStartUpStatus = SUCCESS;
__IO uint32_t CardInserted = 0;
uint32_t CHV1Status = 0, i = 0;
__IO uint8_t ICCID_Content[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
__IO uint8_t IMSI_Content[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Private function prototypes -----------------------------------------------*/
static void Delay(uint32_t nCount);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int main(void)
{
	uint8_t card_data[0x100];
	SC_ADPU_Responce SC_Response;
	uint8_t count = 0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	/* Setup SysTick Timer for 1 msec interrupts  */
	SysTick_Config(SystemCoreClock / 1000);

	Debug_USART_Config();;
	printf("USART smartcard Demo V1.0.\n");

	/* Configure Smartcard Interface GPIO pins */
	SC_IOConfig();
	SC_Init();

	/*-------------------------------- Idle task ---------------------------------*/
	while(1)
	{
		/* Loop while no Smartcard is detected */  
		while(CardInserted == 0)
		{
		}

		SC_PowerCmd(ENABLE);
		//Reset
		SC_Reset(Bit_RESET);
		Delay(20);
		SC_Reset(Bit_SET);
		memset(card_data,0x00,0x100);
		count = SC_AnswerReq(&card_data[0], 40); /*读取上电时的IC卡数据 */
		printf("recv ATR : ");
		for(i = 0; i < count;i++)
		{
			printf("%#02x ",card_data[i]);
		}
		printf("\r\n");

		//获取随机数指令填充
		SC_ADPU.Header.CLA = 00;
		SC_ADPU.Header.INS = 0x84;
		SC_ADPU.Header.P1 = 0x00;
		SC_ADPU.Header.P2 = 0x00;
		SC_ADPU.Body.LC = 0x00;
		SC_ADPU.Body.LE = 0x08;

		SC_SendData(&SC_ADPU, &SC_Response);//数据发送

		printf("recv data: ");
		for(i = 0; i < SC_ADPU.Body.LE;i++)
		{
			printf("%#02x ",SC_Response.Data[i]);
		}
		printf("\r\nSW: ");
		printf("%#02x ",SC_Response.SW1);
		printf("%#02x ",SC_Response.SW2);
		printf("\r\n");
		CardInserted = 0;
	}
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length (time base 1 ms).
  * @retval None
  */
static void Delay(uint32_t nCount)
{
  TimingDelay = nCount;
 
  while(TimingDelay != 0)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number where 
  *         the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
