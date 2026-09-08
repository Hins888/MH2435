/**
  ******************************************************************************
  * @file    main.c
  * @author  Megahunt
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   演示使用DMA进行SPI传输
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>

#include "./usart/bsp_debug_usart.h"
#include "./spi/bsp_spi_dma.h"


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
	uint32_t i = 0;
	uint32_t index = 0x00;
	
	Debug_USART_Config();;
	printf("SPI Master DMA Demo V1.0.\n");

    DEBUG_SPI_Init(0);
	
    for (index = 0; index < DEBUG_DATA_SIZE; index++) TransmitBuf[index] = index + 0xF0;
	
	SPI_DMACmd(DEBUG_SPI, SPI_DMAReq_Tx | SPI_DMAReq_Rx, ENABLE);
	SPI_DMA_Config();
	

     while (1)
    {
        memset(ReceiveBuf, 0x00, DEBUG_DATA_SIZE);
		SPI_DEBUG_CS_LOW();
        DMA_Cmd(DEBUG_SPIRX_DMA_STREAM, ENABLE);                      //开启DMA RX传输 
		DMA_Cmd(DEBUG_SPITX_DMA_STREAM, ENABLE);                      //开启DMA TX传输 

        while (RESET == DMA_GetFlagStatus(DEBUG_SPITX_DMA_STREAM, DEBUG_SPITX_DMA_TCIF));
		DMA_ClearFlag(DEBUG_SPITX_DMA_STREAM,DEBUG_SPITX_DMA_TCIF);
		while (RESET == DMA_GetFlagStatus(DEBUG_SPIRX_DMA_STREAM, DEBUG_SPIRX_DMA_TCIF));
		DMA_ClearFlag(DEBUG_SPIRX_DMA_STREAM,DEBUG_SPIRX_DMA_TCIF);
		
		SPI_DEBUG_CS_HIGH();
		
		DataPrint(ReceiveBuf, DEBUG_DATA_SIZE);
        if (DataCheck(TransmitBuf, ReceiveBuf, sizeof(TransmitBuf)) != 0)
        {
            printf("Transmission error...\n");
            printf("Suspend.\n");
			i++;
			if(i == 3)
            while (1);
        }
        else
        {
            printf("ok.\n");
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



