#include "lcd_rgb_2_1inch_ZJY_480x480.h"


#if (USE_480_480_565 == USE_LCD_TYPE)

#define LCD_SPI_DELAY	2

static void DEBUG_SPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd (DEBUG_SPI_SCK_GPIO_CLK |DEBUG_SPI_MOSI_GPIO_CLK|DEBUG_CS_GPIO_CLK, ENABLE);
	
	/*SCK */
	GPIO_InitStructure.GPIO_Pin = DEBUG_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_Init(DEBUG_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	/* MOSI */
	GPIO_InitStructure.GPIO_Pin = DEBUG_SPI_MOSI_PIN;
	GPIO_Init(DEBUG_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/* CS */
	GPIO_InitStructure.GPIO_Pin = DEBUG_CS_PIN;
	GPIO_Init(DEBUG_CS_GPIO_PORT, &GPIO_InitStructure);
	

	SPI_CS_HIGH();
	SPI_SCK_HIGH();
	SPI_MOSI_HIGH();

}

static void write_lcd_datas(uint32_t data,uint8_t len)
{
	int i;
	for(int8_t i =len-1; i >= 0; i--)
	{
		SPI_SCK_LOW();
		if(data & (1<<i))
		{
			SPI_MOSI_HIGH();
		}
		else
		{
			SPI_MOSI_LOW();
		}
		SystemDelayUs(LCD_SPI_DELAY);
		SPI_SCK_HIGH();
		SystemDelayUs(LCD_SPI_DELAY);
		
	}
	SPI_SCK_LOW();
}
void BSP_SPI_WR_REG(uint8_t cmd)
{
	uint32_t x;
	x = 0x0000|cmd; 
	write_lcd_datas(x,9);
	SystemDelayUs(1);	
}
void BSP_SPI_WR_DATA8(uint8_t cmd)
{
	uint32_t x;
	x = 0x0100|cmd; 
	write_lcd_datas(x,9);
	SystemDelayUs(1);	
	
}

void Write_LCD_REG(uint16_t cmd,uint16_t data)
{
	write_lcd_datas(data,9);
	SystemDelayUs(1);
		
}

static void Init_parameter(void)
{
	SPI_CS_LOW();
	
#if 1   ////ZJY 2.1" 480*480
	BSP_SPI_WR_REG(0x11);
    SystemDelay(120);

    BSP_SPI_WR_REG(0xFF);
    BSP_SPI_WR_DATA8(0x77);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x10);

    BSP_SPI_WR_REG(0xC0);
    BSP_SPI_WR_DATA8(0x3B);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xC1);
    BSP_SPI_WR_DATA8(0x0B); // VBP
    BSP_SPI_WR_DATA8(0x02);

    BSP_SPI_WR_REG(0xC2);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x02);

    BSP_SPI_WR_REG(0xCC);
    BSP_SPI_WR_DATA8(0x10);

    BSP_SPI_WR_REG(0xCD);
    BSP_SPI_WR_DATA8(0x08);

    BSP_SPI_WR_REG(0xB0); // Positive Voltage Gamma Control
    BSP_SPI_WR_DATA8(0x02);
    BSP_SPI_WR_DATA8(0x13);
    BSP_SPI_WR_DATA8(0x1B);
    BSP_SPI_WR_DATA8(0x0D);
    BSP_SPI_WR_DATA8(0x10);
    BSP_SPI_WR_DATA8(0x05);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0x07);
    BSP_SPI_WR_DATA8(0x07);
    BSP_SPI_WR_DATA8(0x24);
    BSP_SPI_WR_DATA8(0x04);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x0E);
    BSP_SPI_WR_DATA8(0x2C);
    BSP_SPI_WR_DATA8(0x33);
    BSP_SPI_WR_DATA8(0x1D);

    BSP_SPI_WR_REG(0xB1); // Negative Voltage Gamma Control
    BSP_SPI_WR_DATA8(0x05);
    BSP_SPI_WR_DATA8(0x13);
    BSP_SPI_WR_DATA8(0x1B);
    BSP_SPI_WR_DATA8(0x0D);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x05);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0x07);
    BSP_SPI_WR_DATA8(0x07);
    BSP_SPI_WR_DATA8(0x24);
    BSP_SPI_WR_DATA8(0x04);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x0E);
    BSP_SPI_WR_DATA8(0x2C);
    BSP_SPI_WR_DATA8(0x33);
    BSP_SPI_WR_DATA8(0x1D);

    BSP_SPI_WR_REG(0xFF);
    BSP_SPI_WR_DATA8(0x77);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x11);

    BSP_SPI_WR_REG(0xB0);
    BSP_SPI_WR_DATA8(0x5d); // 5d

    BSP_SPI_WR_REG(0xB1);  // VCOM amplitude setting
    BSP_SPI_WR_DATA8(0x43); // 43

    BSP_SPI_WR_REG(0xB2);  // VGH Voltage setting
    BSP_SPI_WR_DATA8(0x81); // 12V

    BSP_SPI_WR_REG(0xB3);
    BSP_SPI_WR_DATA8(0x80);

    BSP_SPI_WR_REG(0xB5);  // VGL Voltage setting
    BSP_SPI_WR_DATA8(0x43); //-8.3V

    BSP_SPI_WR_REG(0xB7);
    BSP_SPI_WR_DATA8(0x85);

    BSP_SPI_WR_REG(0xB8);
    BSP_SPI_WR_DATA8(0x20);

    BSP_SPI_WR_REG(0xC1);
    BSP_SPI_WR_DATA8(0x78);

    BSP_SPI_WR_REG(0xC2);
    BSP_SPI_WR_DATA8(0x78);

    BSP_SPI_WR_REG(0xD0);
    BSP_SPI_WR_DATA8(0x88);

    BSP_SPI_WR_REG(0xE0);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x02);

    BSP_SPI_WR_REG(0xE1);
    BSP_SPI_WR_DATA8(0x03);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x04);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x20);
    BSP_SPI_WR_DATA8(0x20);

    BSP_SPI_WR_REG(0xE2);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xE3);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xE4);
    BSP_SPI_WR_DATA8(0x22);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xE5);
    BSP_SPI_WR_DATA8(0x05);
    BSP_SPI_WR_DATA8(0xEC);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0x07);
    BSP_SPI_WR_DATA8(0xEE);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xE6);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xE7);
    BSP_SPI_WR_DATA8(0x22);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xE8);
    BSP_SPI_WR_DATA8(0x06);
    BSP_SPI_WR_DATA8(0xED);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0xEF);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xEB);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x40);
    BSP_SPI_WR_DATA8(0x40);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xED);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0xBA);
    BSP_SPI_WR_DATA8(0x0A);
    BSP_SPI_WR_DATA8(0xBF);
    BSP_SPI_WR_DATA8(0x45);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0x54);
    BSP_SPI_WR_DATA8(0xFB);
    BSP_SPI_WR_DATA8(0xA0);
    BSP_SPI_WR_DATA8(0xAB);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0xFF);

    BSP_SPI_WR_REG(0xEF);
    BSP_SPI_WR_DATA8(0x10);
    BSP_SPI_WR_DATA8(0x0D);
    BSP_SPI_WR_DATA8(0x04);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0x3F);
    BSP_SPI_WR_DATA8(0x1F);

    BSP_SPI_WR_REG(0xFF);
    BSP_SPI_WR_DATA8(0x77);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x13);

    BSP_SPI_WR_REG(0xEF);
    BSP_SPI_WR_DATA8(0x08);

    BSP_SPI_WR_REG(0xFF);
    BSP_SPI_WR_DATA8(0x77);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0x36);
    BSP_SPI_WR_DATA8(0x08);

    BSP_SPI_WR_REG(0x3A);
    BSP_SPI_WR_DATA8(0x60);
    
    BSP_SPI_WR_REG(0x11);
    SystemDelay(120);
    BSP_SPI_WR_REG(0x29);
	
#else //ZJY 2.76" 480*480
    BSP_SPI_WR_REG(0x11);
    SystemDelay(120);

    BSP_SPI_WR_REG(0xFF);
    BSP_SPI_WR_DATA8(0x77);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x13);

    BSP_SPI_WR_REG(0xEF);
    BSP_SPI_WR_DATA8(0x08);

    BSP_SPI_WR_REG(0xFF);
    BSP_SPI_WR_DATA8(0x77);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x10);

    BSP_SPI_WR_REG(0xC0);
    BSP_SPI_WR_DATA8(0x3B);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xC1);
    BSP_SPI_WR_DATA8(0x10);
    BSP_SPI_WR_DATA8(0x0C);

    BSP_SPI_WR_REG(0xC2);
    BSP_SPI_WR_DATA8(0x07);
    BSP_SPI_WR_DATA8(0x0A);

    BSP_SPI_WR_REG(0xC7);
    BSP_SPI_WR_DATA8(0x00); /*0x04->0x00*/

    BSP_SPI_WR_REG(0xCC);
    BSP_SPI_WR_DATA8(0x10);

    BSP_SPI_WR_REG(0xCD);
    BSP_SPI_WR_DATA8(0x08);

    BSP_SPI_WR_REG(0xB0);
    BSP_SPI_WR_DATA8(0x05);
    BSP_SPI_WR_DATA8(0x12);
    BSP_SPI_WR_DATA8(0x98);
    BSP_SPI_WR_DATA8(0x0E);
    BSP_SPI_WR_DATA8(0x0F);
    BSP_SPI_WR_DATA8(0x07);
    BSP_SPI_WR_DATA8(0x07);
    BSP_SPI_WR_DATA8(0x09);
    BSP_SPI_WR_DATA8(0x09);
    BSP_SPI_WR_DATA8(0x23);
    BSP_SPI_WR_DATA8(0x05);
    BSP_SPI_WR_DATA8(0x52);
    BSP_SPI_WR_DATA8(0x0F);
    BSP_SPI_WR_DATA8(0x67);
    BSP_SPI_WR_DATA8(0x2C);
    BSP_SPI_WR_DATA8(0x11);

    BSP_SPI_WR_REG(0xB1);
    BSP_SPI_WR_DATA8(0x0B);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x97);
    BSP_SPI_WR_DATA8(0x0C);
    BSP_SPI_WR_DATA8(0x12);
    BSP_SPI_WR_DATA8(0x06);
    BSP_SPI_WR_DATA8(0x06);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0x22);
    BSP_SPI_WR_DATA8(0x03);
    BSP_SPI_WR_DATA8(0x51);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x66);
    BSP_SPI_WR_DATA8(0x2B);
    BSP_SPI_WR_DATA8(0x0F);

    BSP_SPI_WR_REG(0xFF);
    BSP_SPI_WR_DATA8(0x77);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x11);

    BSP_SPI_WR_REG(0xB0);
    BSP_SPI_WR_DATA8(0x5D);

    BSP_SPI_WR_REG(0xB1);
    BSP_SPI_WR_DATA8(0x2D);

    BSP_SPI_WR_REG(0xB2);
    BSP_SPI_WR_DATA8(0x81);

    BSP_SPI_WR_REG(0xB3);
    BSP_SPI_WR_DATA8(0x80);

    BSP_SPI_WR_REG(0xB5);
    BSP_SPI_WR_DATA8(0x4E);

    BSP_SPI_WR_REG(0xB7);
    BSP_SPI_WR_DATA8(0x85);

    BSP_SPI_WR_REG(0xB8);
    BSP_SPI_WR_DATA8(0x20);

    BSP_SPI_WR_REG(0xC1);
    BSP_SPI_WR_DATA8(0x78);

    BSP_SPI_WR_REG(0xC2);
    BSP_SPI_WR_DATA8(0x78);

    BSP_SPI_WR_REG(0xD0);
    BSP_SPI_WR_DATA8(0x88);

    BSP_SPI_WR_REG(0xE0);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x02);

    BSP_SPI_WR_REG(0xE1);
    BSP_SPI_WR_DATA8(0x06);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0x05);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0x07);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x33);
    BSP_SPI_WR_DATA8(0x33);

    BSP_SPI_WR_REG(0xE2);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x33);
    BSP_SPI_WR_DATA8(0x33);
    BSP_SPI_WR_DATA8(0xF4);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0xF4);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0xE3);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x11);

    BSP_SPI_WR_REG(0xE4);
    BSP_SPI_WR_DATA8(0x44);
    BSP_SPI_WR_DATA8(0x44);

    BSP_SPI_WR_REG(0xE5);
    BSP_SPI_WR_DATA8(0x0D);
    BSP_SPI_WR_DATA8(0xF5);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0xF0);
    BSP_SPI_WR_DATA8(0x0F);
    BSP_SPI_WR_DATA8(0xF7);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0xF0);
    BSP_SPI_WR_DATA8(0x09);
    BSP_SPI_WR_DATA8(0xF1);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0xF0);
    BSP_SPI_WR_DATA8(0x0B);
    BSP_SPI_WR_DATA8(0xF3);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0xF0);

    BSP_SPI_WR_REG(0xE6);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x11);
    BSP_SPI_WR_DATA8(0x11);

    BSP_SPI_WR_REG(0xE7);
    BSP_SPI_WR_DATA8(0x44);
    BSP_SPI_WR_DATA8(0x44);

    BSP_SPI_WR_REG(0xE8);
    BSP_SPI_WR_DATA8(0x0C);
    BSP_SPI_WR_DATA8(0xF4);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0xF0);
    BSP_SPI_WR_DATA8(0x0E);
    BSP_SPI_WR_DATA8(0xF6);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0xF0);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0xF0);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0xF0);
    BSP_SPI_WR_DATA8(0x0A);
    BSP_SPI_WR_DATA8(0xF2);
    BSP_SPI_WR_DATA8(0x30);
    BSP_SPI_WR_DATA8(0xF0);

    BSP_SPI_WR_REG(0xE9);
    BSP_SPI_WR_DATA8(0x36);
    BSP_SPI_WR_DATA8(0x01);

    BSP_SPI_WR_REG(0xEB);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0xE4);
    BSP_SPI_WR_DATA8(0xE4);
    BSP_SPI_WR_DATA8(0x44);
    BSP_SPI_WR_DATA8(0x88);
    BSP_SPI_WR_DATA8(0x40);

    BSP_SPI_WR_REG(0xED);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0x10);
    BSP_SPI_WR_DATA8(0xAF);
    BSP_SPI_WR_DATA8(0x76);
    BSP_SPI_WR_DATA8(0x54);
    BSP_SPI_WR_DATA8(0x2B);
    BSP_SPI_WR_DATA8(0xCF);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0xFF);
    BSP_SPI_WR_DATA8(0xFC);
    BSP_SPI_WR_DATA8(0xB2);
    BSP_SPI_WR_DATA8(0x45);
    BSP_SPI_WR_DATA8(0x67);
    BSP_SPI_WR_DATA8(0xFA);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0xFF);

    BSP_SPI_WR_REG(0xEF);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0x08);
    BSP_SPI_WR_DATA8(0x45);
    BSP_SPI_WR_DATA8(0x3F);
    BSP_SPI_WR_DATA8(0x54);

    BSP_SPI_WR_REG(0xFF);
    BSP_SPI_WR_DATA8(0x77);
    BSP_SPI_WR_DATA8(0x01);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_DATA8(0x00);

    BSP_SPI_WR_REG(0x3A);
    BSP_SPI_WR_DATA8(0x60);

    BSP_SPI_WR_REG(0x36);
    BSP_SPI_WR_DATA8(0x08);

    BSP_SPI_WR_REG(0x35);
    BSP_SPI_WR_DATA8(0x00);
    BSP_SPI_WR_REG(0x11);
    SystemDelay(120); // ms
    BSP_SPI_WR_REG(0x29);
#endif
	
	SPI_CS_HIGH();
}
static IOEnum dpiIOList[] = CONFIG_DPI_IO_LIST;
bool DPIPanelConstractor(DisplayStruct* display,uint32_t Pointer) {

    display->HorizontalSyncActive = DPI_HSA;
    display->HorizontalBackPorch  = DPI_HBP;
    display->HorizontalActive     = DPI_HACT;
    display->HorizontalFrontPorch = DPI_HFP;

    display->VerticalSyncActive = DPI_VSA;
    display->VerticalBackPorch  = DPI_VBP;
    display->VerticalActive     = DPI_VACT;
    display->VerticalFrontPorch = DPI_VFP;
	
	display->IsHsyncValidLow       = true;
    display->IsVsyncValidLow       = true;
    display->IsDataEnableValidLow  = false;
    display->IsPclkFallingSampling = false;
	
	#ifdef DPI_PIXEL_CLOCK
	display->PixelClock	= DPI_PIXEL_CLOCK;		//kHz
	#else
		display->PixelClock	= 25000;		//kHz
	#endif

	#ifdef DPI_DISPLAY_ROTATION
		display->rotation = DPI_DISPLAY_ROTATION;
	#else
		display->rotation = DISPLAT_ROTATION_0;
	#endif

    display->Color = DPI_DISPLAY_COLOR;
	
	
	display->Pointer = Pointer;
	
	display->ResetIO = DPI_RESET_IO;
	display->BacklightIO = DPI_BACKLIGHT_IO;
	
	display->IOCount = CONFIG_DPI_IO_COUNT;
    display->IOList  = dpiIOList;
	
    DisplayStartupSequence(display, true, true, 1, 1, 10);
	
	DEBUG_SPI_Init();
	Init_parameter();
	
    return true;
}


#endif
