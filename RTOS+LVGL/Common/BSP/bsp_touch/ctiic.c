#include "ctiic.h"

#include "bsp_usart/bsp_usart.h"

#ifdef USE_HW_IIC
static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;  
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);
/**
  * @brief  I2C1 I/O配置
  * @param  无
  * @retval 无
  */
static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
   
 
  /*!< TOUCH_I2C_SCL_GPIO_CLK and TOUCH_I2C_SDA_GPIO_CLK Periph clock enable */
  PeripheralEnable(TOUCH_I2C_SCL_GPIO_CLK | TOUCH_I2C_SDA_GPIO_CLK, ENABLE);

  /*!< GPIO configuration */
  /* Connect PXx to I2C_SCL*/
  GPIO_PinAFConfig(TOUCH_I2C_SCL_GPIO_PORT, TOUCH_I2C_SCL_SOURCE, TOUCH_I2C_SCL_AF);
  /* Connect PXx to I2C_SDA*/
  GPIO_PinAFConfig(TOUCH_I2C_SDA_GPIO_PORT, TOUCH_I2C_SDA_SOURCE, TOUCH_I2C_SDA_AF);  
  
  /*!< Configure TOUCH_I2C pins: SCL */   
  GPIO_InitStructure.GPIO_Pin = TOUCH_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(TOUCH_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure TOUCH_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = TOUCH_I2C_SDA_PIN;
  GPIO_Init(TOUCH_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
 
}

/**
  * @brief  I2C 工作模式配置
  * @param  无
  * @retval 无
  */
void CT_IIC_Init(void)
{
	I2C_InitTypeDef  I2C_InitStructure; 
	
	I2C_GPIO_Config();
	
	 /*!< TOUCH_I2C Periph clock enable */
	PeripheralEnable(TOUCH_I2C_CLK, ENABLE);
	
	/* I2C 配置 */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;	
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		                    /* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
	I2C_InitStructure.I2C_OwnAddress1 =0xA0; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;	
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	/* I2C的寻址模式 */
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;	                            /* 通信速率 */
	I2C_Init(TOUCH_I2C, &I2C_InitStructure);	                                      /* I2C1 初始化 */
	I2C_Cmd(TOUCH_I2C, ENABLE);  	                                                /* 使能 I2C1 */

	I2C_AcknowledgeConfig(TOUCH_I2C, ENABLE);  
}

uint8_t I2C_Write(uint8_t DevAddr7bits, uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
    I2CTimeout = I2CT_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(TOUCH_I2C, I2C_FLAG_BUSY))  
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(4);
    } 
  
    /* Send START condition */
    I2C_GenerateSTART(TOUCH_I2C, ENABLE);
  
    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(5);
    } 
  
    /* Send Dev address for write */
    I2C_Send7bitAddress(TOUCH_I2C, DevAddr7bits, I2C_Direction_Transmitter);
  
    I2CTimeout = I2CT_FLAG_TIMEOUT;
  
    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(6);
    } 
    /* Send the DEV's internal address to write to */    
    I2C_SendData(TOUCH_I2C, (WriteAddr>>8)&0xFF);  

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while(! I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) 
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(7);
    }

    I2C_SendData(TOUCH_I2C, WriteAddr&0xFF);  

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while(! I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) 
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(7);
    } 
  
    /* While there is data to be written */
    while(NumByteToWrite--)  
    {
        /* Send the current byte */
        I2C_SendData(TOUCH_I2C, *pBuffer); 

        /* Point to the next byte to be written */
        pBuffer++; 
  
        I2CTimeout = I2CT_FLAG_TIMEOUT;

        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(8);
        } 
    }

    /* Send STOP condition */
    I2C_GenerateSTOP(TOUCH_I2C, ENABLE);
  
    return 0;
}

uint8_t I2C_Read(uint8_t DevAddr7bits, uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{  
    I2CTimeout = I2CT_LONG_TIMEOUT;

    while(I2C_GetFlagStatus(TOUCH_I2C, I2C_FLAG_BUSY))   
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
    }
    /* Send START condition */
    I2C_GenerateSTART(TOUCH_I2C, ENABLE);
  
    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
    }

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(TOUCH_I2C, DevAddr7bits, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
 
    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(11);
    }
    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(TOUCH_I2C, ENABLE);

    /* Send the  address to write to */
    I2C_SendData(TOUCH_I2C, (ReadAddr>>8)&0xFF);  

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(12);
    }
    
    I2C_SendData(TOUCH_I2C, ReadAddr&0xFF);  

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(12);
    }
    
    /* Send STRAT condition a second time */  
    I2C_GenerateSTART(TOUCH_I2C, ENABLE);
  
     I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(13);
    }
    /* Send EEPROM address for read */
    I2C_Send7bitAddress(TOUCH_I2C, DevAddr7bits, I2C_Direction_Receiver);
  
     I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(14);
    }
    /* While there is data to be read */
    while(NumByteToRead)  
    {
        if(NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(TOUCH_I2C, DISABLE);
      
            /* Send STOP Condition */
            I2C_GenerateSTOP(TOUCH_I2C, ENABLE);
        }

        I2CTimeout = I2CT_LONG_TIMEOUT;
        while(I2C_CheckEvent(TOUCH_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
        {
            if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
        }     
        
        /* Read a byte from the device */
        *pBuffer = I2C_ReceiveData(TOUCH_I2C);

        /* Point to the next location where the byte read will be saved */
        pBuffer++; 
  
        /* Decrement the read bytes counter */
        NumByteToRead--;
                 
    }
    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(TOUCH_I2C, ENABLE);
  
    return 0;
}


static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  LOG_ERROR("I2C Time Out! errorCode = %d\n",errorCode);
  I2C_GenerateSTOP(TOUCH_I2C, ENABLE);
  return errorCode;
}

#else

//控制I2C速度的延时
void CT_Delay(void)
{
    SystemDelayUs(4);
}

//电容触摸芯片IIC接口初始化
void CT_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    // PB6 -- SCL
    // PB7 -- SDA
    PeripheralEnable(PeripheralGPIOB, true);
    
    GPIO_Initure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Initure.GPIO_OType = GPIO_OType_OD;
    GPIO_Initure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Initure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Initure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_Init(GPIOB, &GPIO_Initure);
    
    CT_IIC_SDA_H;
    CT_IIC_SCL_H;
}

//产生IIC起始信号
void CT_IIC_Start(void)
{
    CT_IIC_SDA_H;
    CT_IIC_SCL_H;
    CT_Delay();
    CT_IIC_SDA_L; //START:when CLK is high,DATA change form high to low
    CT_Delay();
    CT_IIC_SCL_L; //钳住I2C总线，准备发送或接收数据
    CT_Delay();
}
//产生IIC停止信号
void CT_IIC_Stop(void)
{
    CT_IIC_SDA_L; //STOP:when CLK is high DATA change form low to high
    CT_Delay();
    CT_IIC_SCL_H;
    CT_Delay();
    CT_IIC_SDA_H; //发送I2C总线结束信号
    CT_Delay();
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t CT_IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    uint8_t rack = 0;
    
    CT_IIC_SDA_H;
    CT_Delay();
    CT_IIC_SCL_H;
    CT_Delay();

    while (CT_READ_SDA)
    {
        ucErrTime++;

        if (ucErrTime > 250)
        {
            CT_IIC_Stop();
            rack = 1;
            break;
        }

        CT_Delay();
    }

    CT_IIC_SCL_L; //时钟输出0
    CT_Delay();

    return rack;
}
//产生ACK应答
void CT_IIC_Ack(void)
{
    CT_IIC_SDA_L;
    CT_Delay();
    CT_IIC_SCL_H;
    CT_Delay();
    CT_IIC_SCL_L;
    CT_Delay();
    CT_IIC_SDA_H;
    CT_Delay();
}
//不产生ACK应答
void CT_IIC_NAck(void)
{
    CT_IIC_SDA_H;
    CT_Delay();
    CT_IIC_SCL_H;
    CT_Delay();
    CT_IIC_SCL_L;
    CT_Delay();
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void CT_IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;

    for (t = 0; t < 8; t++)
    {
        if ((txd & 0x80) >> 7)
        {
            CT_IIC_SDA_H;
        }
        else 
        {
            CT_IIC_SDA_L;
        }
        
        CT_Delay();
        CT_IIC_SCL_H;
        CT_Delay();
        CT_IIC_SCL_L;
        txd <<= 1;
    }

    CT_IIC_SDA_H;
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t CT_IIC_Read_Byte(unsigned char ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++ )
    {
        receive <<= 1;
        CT_IIC_SCL_H;
        CT_Delay();

        if (CT_READ_SDA)receive++;

        CT_IIC_SCL_L;
        CT_Delay();
    }

    if (!ack)CT_IIC_NAck();//发送nACK
    else CT_IIC_Ack(); //发送ACK

    return receive;
}

uint8_t I2C_Write(uint8_t DevAddr7bits, uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
	uint8_t i;
	uint8_t ret=0;
	CT_IIC_Start();    
	
	CT_IIC_Send_Byte(DevAddr7bits);       //发送写命令      
	if(CT_IIC_Wait_Ack())
	{
		CT_IIC_Stop();                    //产生一个停止条件 
		return 1;
	}
	
	CT_IIC_Send_Byte(WriteAddr>>8);       //发送高8位地址
	if(CT_IIC_Wait_Ack())
	{
		CT_IIC_Stop();                    //产生一个停止条件 
		return 1;
	}                               
	
	CT_IIC_Send_Byte(WriteAddr&0XFF);       //发送低8位地址
	if(CT_IIC_Wait_Ack())
	{
		CT_IIC_Stop();                    //产生一个停止条件 
		return 1;
	}
	
	for(i=0;i<NumByteToWrite;i++)
	{       
		CT_IIC_Send_Byte(pBuffer[i]);      //发数据
		ret=CT_IIC_Wait_Ack();
		if(ret)break;  
	}
	CT_IIC_Stop();                    //产生一个停止条件        
	return ret; 
}

uint8_t I2C_Read(uint8_t DevAddr7bits,  uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	uint8_t i; 
	uint8_t ret = 0;
	CT_IIC_Start();    
	CT_IIC_Send_Byte(DevAddr7bits);   //发送写命令      
	if(CT_IIC_Wait_Ack())
	{
		CT_IIC_Stop();                    //产生一个停止条件 
		return 1;
	}
	
	CT_IIC_Send_Byte(ReadAddr>>8);       //发送高8位地址
	if(CT_IIC_Wait_Ack())
	{
		CT_IIC_Stop();                    //产生一个停止条件 
		return 1;
	}
	
	CT_IIC_Send_Byte(ReadAddr&0XFF);       //发送低8位地址
	if(CT_IIC_Wait_Ack())
	{
		CT_IIC_Stop();                    //产生一个停止条件 
		return 1;
	}
	
	CT_IIC_Start();              
	CT_IIC_Send_Byte(DevAddr7bits+1);   //发送读命令           
	if(CT_IIC_Wait_Ack())
	{
		CT_IIC_Stop();                    //产生一个停止条件 
		return 1;
	}
	
	for(i=0;i<NumByteToRead;i++)
	{       
		pBuffer[i]=CT_IIC_Read_Byte(i==(NumByteToRead-1)?0:1); //发数据      
	} 
	CT_IIC_Stop();//产生一个停止条件 
	return ret;
} 

#endif
























