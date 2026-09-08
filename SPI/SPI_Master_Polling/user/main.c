#include <stdio.h>
#include <string.h>

#include "./usart/bsp_debug_usart.h"
#include "./spi/bsp_spi.h"

__IO uint16_t SendIndex = 0;
__IO uint16_t RecvIndex = 0;
__IO uint8_t RecvFlag = 0;

#define DEBUG_SPI_IRQHandler				SPI1_IRQHandler
#define	DEBUG_SPI_IRQn						SPI1_IRQn


int DataCheck(uint8_t *Src, uint8_t *Dst, uint32_t Len)
{
	return printf(memcmp(Src, Dst, Len) ? ">> Fail !\n" : ">> Ok !\n"), memcmp(Src, Dst, Len);
}
void DataPrint(uint8_t *Buff, uint32_t Len)
{
	while(printf(Len-- % 16 ? "%04X " : "\n%04X ", *Buff++), printf(Len ? "" : "\n"), Len);
}



int main(void)
{
	uint32_t i=0;
	Debug_USART_Config();
    
	printf("SPI Master Polling Demo V1.0.\n");
	
    DEBUG_SPI_Init(0);
    uint32_t index = 0x00;
    for (index = 0; index < DEBUG_DATA_SIZE; index++) TransmitBuf[index] = index + 0xF0;

    while(1)
    {
        memset(ReceiveBuf, 0x00, DEBUG_DATA_SIZE);
    
		SPI_DEBUG_CS_LOW();
        for(i = 0;i < DEBUG_DATA_SIZE; i++)
        {
				
            while(SPI_GetFlagStatus(DEBUG_SPI,SPI_FLAG_TXE) == RESET);
            SPI_SendData(DEBUG_SPI, TransmitBuf[i]);
		
            while(SPI_GetFlagStatus(DEBUG_SPI,SPI_FLAG_RXNE) == RESET);
            ReceiveBuf[i] = SPI_ReceiveData(DEBUG_SPI);

        }			
		SPI_DEBUG_CS_HIGH();
		
		DataPrint(ReceiveBuf,DEBUG_DATA_SIZE);
        i = 0;
		if (DataCheck(TransmitBuf, ReceiveBuf, sizeof(TransmitBuf)) != 0)
		{
			DataPrint(ReceiveBuf, DEBUG_DATA_SIZE);
			while (1);
		}
    }
}
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
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
  {}
}
#endif
