#include "touch_goodix.h"

#ifndef USE_SIMULATION

#include "bsp_usart/bsp_usart.h"
#include "bsp_touch/ctiic.h"

static const uint8_t GoodixAddrs[] = {GOODIX_SCCB_ADDRS};
uint8_t GT_Cmd = 0;

u8 GT9147_WR_Reg(u16 reg,u8 *buf,u8 len)
{
    return I2C_Write(GT_Cmd, buf, reg, len);
}

u8 GT9147_RD_Reg(u16 reg,u8 *buf,u8 len)
{
    return I2C_Read(GT_Cmd, buf, reg, len);
}

bool Touch_IsPressed(void)
{
    uint8_t status;
    
    GT9147_RD_Reg(GOODIX_READ_COORD_ADDR,&status,1);    //读取触摸点的状态
   
	 if ((status & BIT7) == 0)
        return false;

    if (status & BITS(3, 0))
        return true;
	
	status = 0;
	GT9147_WR_Reg(GOODIX_READ_COORD_ADDR,&status,1);//清标志
    return false;
}

bool Touch_GetXY(uint16_t *x, uint16_t *y)
{
    u8 buf[4];
    
    if(GT9147_RD_Reg(GOODIX_POINT_X_ADDR,buf,4))    //读取XY坐标值
    {
		return false;
	}
    *x=(((u16)buf[1]<<8)+buf[0]);
    *y=(((u16)buf[3]<<8)+buf[2]);
	
	buf[0] = 0;
	GT9147_WR_Reg(GOODIX_READ_COORD_ADDR,buf,1);//清标志
    return true;
}


bool Touch_Init(void)
{
	uint32_t Touch_ID;
    u8 temp[5]; 
    GPIO_InitTypeDef GPIO_Initure;
	
	PeripheralEnable(PeripheralGPIOA, true);
    PeripheralEnable(PeripheralGPIOF, true);
	
	GPIO_Initure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Initure.GPIO_OType = GPIO_OType_PP;
    GPIO_Initure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Initure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	
    GPIO_Init(GPIOA, &GPIO_Initure);
	
	// INT pin config
    // INT -- PF7
    GPIO_Initure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Initure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Initure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOF, &GPIO_Initure);
	
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	SystemDelay(10);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
    SystemDelay(50);
	
	
	CT_IIC_Init();
	
	for (int i = 0; i < sizeof(GoodixAddrs); i++) {
		GT_Cmd = GoodixAddrs[i];
		if(0 == GT9147_RD_Reg(GOODIX_REG_ID,temp,4))//读取产品ID
		{
			break;
		}
	}
	
	Touch_ID = 0;
    for (int i = 0; i < 4; i++) {
        if (temp[i] < 0x30)
            break;
        Touch_ID = (Touch_ID << 4) | (temp[i] & 0x0F);
    }
	LOG_INFO("Touch_ID is %#x",Touch_ID);
	
	switch (Touch_ID) {
        case 0x9271: // GT9271
        case 0x0911: // GT911
            break;

        case 0x1158: // GT1151Q
            break;
        // unknown id
        default:
            return false;
	}
	return true;
		
}
#endif