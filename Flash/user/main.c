#include "./usart/bsp_debug_usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mh2435.h"


#define FLASH_SIZE_BYTES        (512 * 1024 - 64 * 1024)

#define FLASH_START_ADDR 	    0x8010000U
#define FLASH_SECTOR_SIZE 	    (0x1000)
#define FLASH_SECTOR_NUM 	    (FLASH_SIZE_BYTES / FLASH_SECTOR_SIZE)
#define FLASH_PAGE_NUM 	        (FLASH_SECTOR_NUM * 16)         



typedef enum
{
	DATA_TYPE_ALL_ZERO = 0x0,
	DATA_TYPE_ALL_ONE,
	DATA_TYPE_A5A5A5A5,
	DATA_TYPE_ADDRESS_SELF,
    DATA_TYPE_00TOFF,
}FLASH_TEST_DATA_TYPE;



static void DataPrintf(void *buf, uint32_t bufsize);
static int DataCheck(void *src, void *dst, uint32_t size);
static int EraseCheck(uint32_t addr, uint32_t pagNum);
static int FlashTest(uint32_t pagNum, uint32_t dataType);

static int useDMAFlag = 1;


int main(void)
{
	uint32_t i = 0;
   
	Debug_USART_Config();
	printf("Flash test Demo V1.0\n");
	
	QSPI_Init(NULL);
    QSPI_SetLatency(0);
	
  	PeripheralEnable(PeripheralDMA2, ENABLE);
	
	while (1)
	{
		printf("\n>>>  Sector Erase, Program 0x00~0xFF\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_00TOFF); 
		
		printf("\n>>>  Sector Erase, Program zero all\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{ 
			FLASH_EraseSector(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}		
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_ALL_ZERO);
		
		printf("\n>>>  Sector Erase, Program 0x00~0xFF\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_00TOFF);
		
		printf("\n>>>  Sector Erase, Program 0xA5A5A5A5 all\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_A5A5A5A5);

		printf("\n>>>  Sector Erase, Program one all\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		} 
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_ALL_ONE);
		printf("\n>>>  No Erase, Program zero all\n");
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_ALL_ZERO);

		printf("\n>>>  Sector Erase, Program address self\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_ADDRESS_SELF);
		
		printf("\nQSPI Flash test end\n");		
	}
	
    return 0;
}

static void DataPrintf(void *buf, uint32_t bufsize)
{
	uint32_t i = 0;
	uint8_t *pBuf = (uint8_t *)buf;
	
	if (0 != bufsize)
	{
		for (i = 0; i < bufsize; i++)
		{
			if (0 != i && 0 == i%16)
			{
				printf(" \n");			
			}

			printf("%02X ", pBuf[i]);
		}
	}
	printf("\n");
}

static int DataCheck(void *src, void *dst, uint32_t size)
{
	uint8_t *pSrc = (uint8_t *)src, *pDst = (uint8_t *)dst;
	
	if (memcmp(pDst, pSrc, size))
	{
		DataPrintf(pDst, size);
		return -1;
	}

	return 0;
}

static int EraseCheck(uint32_t addr, uint32_t pagNum)
{
	uint32_t i = 0;
	uint8_t erase_Buf[QSPI_PAGE_SIZE];

	memset(erase_Buf, 0xFF, QSPI_PAGE_SIZE);	
 
	CACHE_CleanAll(DCACHE);
	for (i = 0; i < pagNum; i++)
	{	
		if (-1 == DataCheck(erase_Buf, (uint8_t *)(addr + i * QSPI_PAGE_SIZE), sizeof(erase_Buf)))
		{
			printf("Erase Check failed!\n");
            while(1);
			return -1;
		}
	}	
	
	return 0;
}

static int FlashTest(uint32_t pagNum, uint32_t dataType)
{
	uint32_t i = 0, j = 0;
	uint32_t write_Buf[QSPI_PAGE_SIZE/4] = {0};
    uint8_t data_buf_00toFF[QSPI_PAGE_SIZE] = {0};
	QSPI_CommandTypeDef cmdType;
	uint32_t program_addr = FLASH_START_ADDR;
	
    for (i = 0; i < QSPI_PAGE_SIZE; i++)
    {
        data_buf_00toFF[i] = i;
    }
	
	switch (dataType)
	{
		case DATA_TYPE_ALL_ONE:
			memset(write_Buf, 0xFF, sizeof(write_Buf));			
			break;
	
		case DATA_TYPE_A5A5A5A5:
			memset(write_Buf, 0xA5, sizeof(write_Buf));
			break;
		
        case DATA_TYPE_00TOFF:
			memset(write_Buf, 0x00, sizeof(write_Buf));
            memcpy(write_Buf, data_buf_00toFF, sizeof(data_buf_00toFF));
            break;
        
		case DATA_TYPE_ADDRESS_SELF:
			break;
			
		case DATA_TYPE_ALL_ZERO:
		default:			
			memset(write_Buf, 0x00, sizeof(write_Buf));
			break;
	}
	
	cmdType.Instruction = QUAD_INPUT_PAGE_PROG_CMD;
	cmdType.BusMode = QSPI_BUSMODE_114;	     
	cmdType.CmdFormat = QSPI_CMDFORMAT_CMD8_ADDR24_PDAT;		
	
	printf("Flash Programe data: \n");
	for (i = 0; i < pagNum; i++)
	{
		if (DATA_TYPE_ADDRESS_SELF == dataType)
		{
			for (j = 0; j < QSPI_PAGE_SIZE/4; j++)
			{
				write_Buf[j] = program_addr + i * QSPI_PAGE_SIZE + j;
			}
		}

		if (!useDMAFlag)
		{
			FLASH_ProgramPage(&cmdType, NULL, program_addr + i * QSPI_PAGE_SIZE, sizeof(write_Buf), (uint8_t*)(write_Buf));			
		}
		else
		{
			FLASH_ProgramPage(&cmdType, DMA2_Stream4, program_addr + i * QSPI_PAGE_SIZE, sizeof(write_Buf), (uint8_t*)(write_Buf));		
		}	
	}

	printf("Flash Read Data Start\n");
	for (i = 0; i < pagNum; i++)
	{
		if (DATA_TYPE_ADDRESS_SELF == dataType)
		{
			for (j = 0; j < QSPI_PAGE_SIZE/4; j++)
			{
				write_Buf[j] = program_addr + i * QSPI_PAGE_SIZE + j;
			}
		}
		
		CACHE_CleanAll(DCACHE);
		if (-1 == DataCheck(write_Buf, (uint8_t *)(program_addr + i * QSPI_PAGE_SIZE), sizeof(write_Buf)))
		{
			printf("Addr %#x data check error!\n", program_addr + i * QSPI_PAGE_SIZE);
            while(1);
			return -1;
		}
	}	
		
	printf("Flash Read Data Check OK \n");	
	return 0;
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
	{
	}
}
#endif
