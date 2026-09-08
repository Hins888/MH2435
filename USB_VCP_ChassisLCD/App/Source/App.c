#include "App.h"
#include "bsp_rgb_lcd.h"
#include "bsp_tim.h"
#include "bsp_sdram.h"
#include "jpegdecode.h"

#if 0
#define VCP_DBG(format,...) printf(format, ##__VA_ARGS__)
#else
#define VCP_DBG(format,...)
#endif

#define JPEG_OUTPUT_DATA_BUFFER0      	(0x20140000 - DPI_HACT*DPI_VACT* 3 / 2)	


#if DPI_USE_SDRAM
#define JPEG_RECV_DATA_BUFFER0  		(0x60000000)
#define JPEG_RECV_DATA_BUFFER1  		(JPEG_RECV_DATA_BUFFER0 + DPI_HACT * DPI_VACT * 3 / 2 )
#define DISPLAY_DATA_BUFFER0  			(JPEG_RECV_DATA_BUFFER1 + DPI_HACT * DPI_VACT * 3 / 2)	
#else

#define JPEG_RECV_DATA_BUFFER0  		(JPEG_OUTPUT_DATA_BUFFER0 - DPI_HACT * DPI_VACT * 3 / 2)
#define DISPLAY_DATA_BUFFER0  			(JPEG_RECV_DATA_BUFFER0 - DPI_HACT * DPI_VACT*DPI_DISPLAY_COLOR)	
#endif


typedef enum
{
	JPEG_IDLE = 0,
	JPEG_START,
	JPEG2YUV,
	WAIT_LTDC,
	YUV2RGB,
}Jpeg_Status;

typedef struct{
    uint32_t buf_addr;
    uint32_t buf_len;
	bool	 busy_flag;
}VCP_JPG_DATA;

extern uint8_t IMAGE_map[];

JPEG_InfoTypeDef JpegInfo;

VCP_JPG_DATA vcp_data0 = {((uint32_t)JPEG_RECV_DATA_BUFFER0),0,false};
#if DPI_USE_SDRAM
VCP_JPG_DATA vcp_data1 = {((uint32_t)JPEG_RECV_DATA_BUFFER1),0,false};
#endif

VCP_JPG_DATA *recv_data,*jepg_data;

__IO Jpeg_Status NewState = JPEG_IDLE;
__IO uint16_t frame_rate_time = 0xFFFF;//默认0xFFFF无时间间隔
__IO uint16_t time_count = 0;
__IO bool recv_flag	= false;
uint8_t display_picture(uint8_t * jpeg_data,uint32_t length);

int main(void) {
	uint32_t readsize = 0;
	
	uint8_t* recvBuffer;
	uint8_t recv_cmd = 0;
	uint8_t cmd_type = 0;
	uint32_t recv_buf_length = 0;
	uint32_t recv_buf_offset = 0;

    RetargetIOSetup(CONFIG_RETARGETIO_DEFAULT_SERIAL);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	TIMx_Configuration();
	
#if DPI_USE_SDRAM
	SDRAM_Config();
#endif

	//初始化显示的图片
	memcpy((void*)DISPLAY_DATA_BUFFER0,IMAGE_map,DPI_HACT * DPI_VACT * DPI_DISPLAY_COLOR);
	
	//初始化LCD
	disp_init(DISPLAY_DATA_BUFFER0);

    JpegInit();
	
	//USB初始化
    USBSetup();
	
//	IOSetup(PA0,IO_DEFAULT_OUTPUT_CONFIG);//测试IO
	
	recv_data = &vcp_data0;
	recvBuffer = (uint8_t*)recv_data->buf_addr;
	jepg_data = recv_data;

    while (1) {
		
		if(!recv_data->busy_flag)
		{
			switch(recv_cmd)
			{
				case 0:
				{
					
					readsize = VCPReadBytes((uint8_t*)recvBuffer, 1);
					if(readsize)
					{
						if('M' == recvBuffer[0])
						{					
							recv_cmd = 1;
						}
					}
					
					break;
				}
				case 1:
					if(VCPReadBytes((uint8_t*)recvBuffer, 1))
					{
						if('H' == recvBuffer[0])
						{
							recv_cmd = 2;
						}
						else
							recv_cmd = 0;
					}
					break;
				
				case 2:
					if(VCPReadBytes((uint8_t*)recvBuffer, 1))
					{
						if('J' == recvBuffer[0])
						{
							recv_cmd = 3;
						}
						else
							recv_cmd = 0;
					}
					break;	
					
				case 3:
					if(VCPReadBytes((uint8_t*)recvBuffer, 1))
					{
						if('P' == recvBuffer[0])
						{
							recv_cmd = 4;
						}
						else
							recv_cmd = 0;
					}
					break;
					
				case 4:
					if(VCPReadBytes((uint8_t*)recvBuffer, 1))
					{
						if('E' == recvBuffer[0])
						{
							recv_cmd = 5;
						}
						else
							recv_cmd = 0;
					}
					break;
					
				case 5:
					if(VCPReadBytes((uint8_t*)recvBuffer, 1))
					{
						if('G' == recvBuffer[0])
						{
							recv_cmd = 6;
						}
						else
							recv_cmd = 0;
					}
					break;
					
					
				case 6:
					if(VCPReadBytes((uint8_t*)recvBuffer, 1))
					{
						cmd_type = recvBuffer[0];
						recv_cmd = 7;
					}
					break;
					
					
				case 7:
					recv_buf_length = 4;
					while(recv_buf_length)
					{
						readsize = VCPReadBytes((uint8_t*)&recvBuffer[4-recv_buf_length], recv_buf_length);
						
						recv_buf_length = (recv_buf_length-readsize) < 0?0:(recv_buf_length-readsize);
					}
					recv_buf_length = recvBuffer[0] + (recvBuffer[1] << 8) + (recvBuffer[2] << 16) + (recvBuffer[3] << 24); 
					recv_data->buf_len = recv_buf_length;
					VCP_DBG("recv_buf_length is %#x \r\n",recv_buf_length);

					recv_cmd = 8;
					break;
					
				case 8:
					recv_buf_offset = 0;
					while(recv_buf_offset < recv_buf_length)
					{
						readsize = VCPReadBytes((uint8_t*)&recvBuffer[recv_buf_offset], recv_buf_length - recv_buf_offset);
						recv_buf_offset += readsize;
					}
					recvBuffer[recv_buf_length] = 0x00;
					recvBuffer[recv_buf_length+1] = 0x00;
					recvBuffer[recv_buf_length+2] = 0x00;
					recvBuffer[recv_buf_length+3] = 0x00;
					VCP_DBG("recv_buf_data is over *****\r\n");
					recv_cmd = 9;
					break;
				case 9:		
					if('D' == cmd_type)
					{
					#if(0 == DPI_USE_SDRAM)	
						jepg_data->busy_flag = true;
						recv_flag = true;
				
					#else	
						if(jepg_data->busy_flag)	
							break;
						jepg_data = recv_data;
						jepg_data->busy_flag = true;
						recv_data = (recv_data == &vcp_data0)?&vcp_data1:&vcp_data0;
						recvBuffer = (uint8_t*)recv_data->buf_addr;
						recv_flag = true;
					#endif
					}
					
					if('C' == cmd_type)
					{
						frame_rate_time = recvBuffer[0] + (recvBuffer[1] << 8);
						VCP_DBG("frame_rate_time = %d \r\n",frame_rate_time);
				
					}
					VCP_DBG("display_picture is ok\r\n");
				default:
					recv_cmd = 0;	
			}
		}

						
						
		if(recv_flag)
		{
			if(!(NewState || time_count))
			{
				time_count = frame_rate_time == 0xFFFF?0:frame_rate_time;//解码时间间隔
				
				NewState = JPEG2YUV;
				JpegDecodeYuv_interrupt((uint8_t *)jepg_data->buf_addr,(jepg_data->buf_len + 3)/4*4, (uint8_t*)JPEG_OUTPUT_DATA_BUFFER0);
				recv_flag = false;
	
			}
		}
	}
}




void BASIC_TIM_IRQHandler(void)
{
	if ( TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET ) 
	{	
		if(time_count)
			time_count--;
		
		TIM_ClearITPendingBit(BASIC_TIM , TIM_IT_Update);  	
	}		 	
}

//DMA Stream0
void JpegDecodeYuv_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF))
	{
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF);
		DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF);
		DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF);
		JPEG_GetInfo(&JpegInfo);

		VCP_DBG("%d %d\r\n",JpegInfo.ImageHeight,JpegInfo.ImageWidth);
		JPEG_Cmd(DISABLE);
		jepg_data->busy_flag = false;
		NewState = WAIT_LTDC;
	}
	
    NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
}

//DMA2D
void custom_DMA2D_IRQHandler(void)
{
	if(DMA2D_IsActiveFlag_TC(DMA2D))
	{
		DMA2D_ClearFlag_TC(DMA2D);
		NewState = JPEG_IDLE;
	}
    NVIC_ClearPendingIRQ(DMA2D_IRQn);
}

//LTDC
void custom_LTDC_IRQHandler(void)
{
	if(WAIT_LTDC == NewState)
	{
		Yuv2RgbInit(&JpegInfo);
		Yuv2Rgb((uint8_t *)JPEG_OUTPUT_DATA_BUFFER0, (uint8_t*)DISPLAY_DATA_BUFFER0);
		NewState = YUV2RGB;
	}
    LTDC_ClearFlag(LTDC_IT_LI);
    NVIC_ClearPendingIRQ(LTDC_IRQn);
}