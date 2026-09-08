#include <stdio.h>
#include <string.h>
#include "./usart/bsp_debug_usart.h"
#include "./spi/bsp_spi.h"

__IO uint16_t SendIndex = 0;
__IO uint16_t RecvIndex = 0;
__IO uint8_t RecvFlag = 0;


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
	uint32_t index = 0x00;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

    Debug_USART_Config();
	
	printf("SPI Master Interrupt Demo V1.0.\n");
    DEBUG_SPI_Init(0);
	
	  
	
    NVIC_Configuration();
	
    for (index = 0; index < DEBUG_DATA_SIZE; index++) TransmitBuf[index] = index + 0xF0;
	
	SPI_DEBUG_CS_LOW();	
	
	SPI_ITConfig(DEBUG_SPI, SPI_IT_RXNE, ENABLE);
    SPI_ITConfig(DEBUG_SPI, SPI_IT_TXE, ENABLE);
    
    while (1)
    {
        if (RecvFlag)
        {
            RecvFlag = 0;
			SPI_DEBUG_CS_HIGH();
			DataPrint(ReceiveBuf,DEBUG_DATA_SIZE);
            if (DataCheck(TransmitBuf, ReceiveBuf, sizeof(TransmitBuf)) != 0)
            {
                printf("Transmission error...\n");
                printf("Suspend.\n");
				
                while (1);
            }
            else
            {
                printf("ok.\n");
            }
            SPI_Cmd(DEBUG_SPI, ENABLE);
			SPI_DEBUG_CS_LOW();
			SPI_ITConfig(DEBUG_SPI, SPI_IT_RXNE, ENABLE);
            SPI_ITConfig(DEBUG_SPI, SPI_IT_TXE, ENABLE);
        }
    }
}

void DEBUG_SPI_IRQHandler(void)
{

    if(SPI_GetITStatus(DEBUG_SPI,SPI_IT_RXNE) == SET)
    {
		
        ReceiveBuf[RecvIndex++] = SPI_ReceiveData(DEBUG_SPI);
        if (RecvIndex == DEBUG_DATA_SIZE)
        {
            RecvFlag = 1;
            RecvIndex = 0;
            SPI_ITConfig(DEBUG_SPI, SPI_IT_RXNE, DISABLE);
            SPI_Cmd(DEBUG_SPI,DISABLE);
        }
    }

    if(SPI_GetITStatus(DEBUG_SPI,SPI_IT_TXE) == SET)
    {
        SPI_SendData(DEBUG_SPI, TransmitBuf[SendIndex++]);
        if (DEBUG_DATA_SIZE == SendIndex)
        {
            SendIndex = 0;
            SPI_ITConfig(DEBUG_SPI, SPI_IT_TXE, DISABLE);
        }
    }

    NVIC_ClearPendingIRQ(SPI1_IRQn);
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
