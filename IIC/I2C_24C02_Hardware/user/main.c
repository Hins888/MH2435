#include <string.h>
#include "./usart/bsp_debug_usart.h"
#include "./i2c/bsp_i2c_ee.h"

#define  EEP_Firstpage      0x00
uint8_t I2c_Buf_Write[256];
uint8_t I2c_Buf_Read[256];

uint8_t I2C_Test(void);
uint8_t EEPROM_Erase(void);	


uint8_t I2c_Buf_Write_Default[256];
int main(void)
{
	/*初始化USART1*/
    Debug_USART_Config();

	printf("I2C Test V1.0 \r\n");
	
	/* I2C 外设初(AT24C02)始化 */
	I2C_EE_Init();

	EEPROM_Erase();
	if(I2C_Test() ==1)
	{
		printf("Test ok \r\n");	
	}
	else
	{
		printf("Test error \r\n");	
	}
  
    while (1);
}

uint8_t EEPROM_Erase(void){
	uint16_t i;
	memset(I2c_Buf_Write_Default,0xFF,256);
	 //将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
	I2C_EE_BufferWrite( I2c_Buf_Write_Default, EEP_Firstpage, 256);
	
	 EEPROM_INFO("读出的数据");
  //将EEPROM读出数据顺序保持到I2c_Buf_Read中
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256); 
   
  //将I2c_Buf_Read中的数据通过串口打印
	for (i=0; i<256; i++)
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write_Default[i])
		{
			printf("0x%02X ", I2c_Buf_Read[i]);
			EEPROM_ERROR("错误:I2C EEPROM写入与读出的数据不一致");
			return 0;
		}
		printf("0x%02X ", I2c_Buf_Read[i]);
		if(i%16 == 15)    
			printf("\n\r");
	}
	return 1;
}
/**
  * @brief  I2C(AT24C02)读写测试
  * @param  无
  * @retval 正常返回1 ，不正常返回0
  */
uint8_t I2C_Test(void)
{
	u16 i;

	EEPROM_INFO("写入的数据");
    
	for ( i=0; i<=255; i++ ) //填充缓冲
	{   
		I2c_Buf_Write[i] = i;

		printf("0x%02X ", I2c_Buf_Write[i]);
		if(i%16 == 15)    
			printf("\n\r");    
	}

	//将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
	I2C_EE_BufferWrite( I2c_Buf_Write, EEP_Firstpage, 256);

	EEPROM_INFO("写成功");

	EEPROM_INFO("读出的数据");
	//将EEPROM读出数据顺序保持到I2c_Buf_Read中
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256); 

	//将I2c_Buf_Read中的数据通过串口打印
	for (i=0; i<256; i++)
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			printf("0x%02X ", I2c_Buf_Read[i]);
			EEPROM_ERROR("错误:I2C EEPROM写入与读出的数据不一致");
			return 0;
		}
		printf("0x%02X ", I2c_Buf_Read[i]);
		if(i%16 == 15)    
			printf("\n\r");

	}
	EEPROM_INFO("I2C(AT24C02)读写测试成功");
	return 1;
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
