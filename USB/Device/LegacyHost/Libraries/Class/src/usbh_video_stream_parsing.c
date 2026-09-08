#include "usbh_video_stream_parsing.h"
#include "usbh_video_desc_parsing.h"
#include "usbh_video_core.h"

uvc_data_struct uvc_data;
extern volatile uint8_t tmp_packet_framebuffer[UVC_RX_FIFO_SIZE_LIMIT];
extern USBH_VIDEO_TargetFormat_t USBH_VIDEO_Target_Format;

static uint8_t uvc_is_sof(void);
static uint8_t uvc_is_eof(void);
static uint8_t uvc_is_right_len(uint16_t size);
static uint8_t uvc_is_data_packet(uint16_t size);
static void video_stream_add_packet_data(uint8_t* buf, uint16_t data_size);
static uint8_t uvc_stream_switch_buffers(void);

void uvc_stream_init(uint8_t* buffer0, uint8_t* buffer1)
{
  if ((buffer0 == NULL) || (buffer1 == NULL))
    return;
  
  uvc_data.buffer0 = buffer0;
  uvc_data.buffer1 = buffer1;
  uvc_data.use_buffer = uvc_data.buffer0;
  uvc_data.ready_buffer = uvc_data.buffer1;
  uvc_data.use_frame_len = 0;
  uvc_data.ready_frame_len = 0;
  uvc_data.ready_frame_flag = false;
  uvc_data.packet_cnt = 0;
  uvc_data.data_packet_cnt = 0;
  uvc_data.header_packet_cnt = 0;
  uvc_data.frame_cnt = 0;
}

void uvc_stream_data_process(uint16_t size)
{
    uint8_t m_fid;
    uint16_t data_size;

    /* add packet counter */
    uvc_data.packet_cnt++;

    if(!uvc_is_right_len(size))
        return;

    if(uvc_is_data_packet(size))
    {
        uvc_data.data_packet_cnt ++;
        data_size = size - UVC_HEADER_SIZE;
        video_stream_add_packet_data((uint8_t*)&tmp_packet_framebuffer[UVC_HEADER_SIZE], data_size);
    }
    else
    {
        uvc_data.header_packet_cnt++;
    }
    
    if(uvc_is_eof())
    {
        if(!uvc_data.ready_frame_flag)
        {
            uvc_data.ready_frame_len = uvc_data.use_frame_len;
            uvc_data.ready_buffer = uvc_data.use_buffer;
            uvc_data.ready_frame_flag = true;
            uvc_data.frame_cnt++;
            
            uvc_stream_switch_buffers();
            uvc_data.use_frame_len = 0;
        }
        #if USE_UVC_MJPEG
        else
        {
            uvc_data.use_frame_len = 0;
        }
        #endif
    }
}

static uint8_t uvc_stream_switch_buffers(void)
{
    if (uvc_data.use_buffer == uvc_data.buffer0)
        uvc_data.use_buffer = uvc_data.buffer1;
    else
        uvc_data.use_buffer = uvc_data.buffer0;
  return 0;
}

static void video_stream_add_packet_data(uint8_t* buf, uint16_t size)
{
  if ((uvc_data.use_frame_len + size) > UVC_UNCOMP_FRAME_SIZE)
  {
    size = UVC_UNCOMP_FRAME_SIZE - uvc_data.use_frame_len;
  }
  if(size > 0)memcpy((void*)&uvc_data.use_buffer[uvc_data.use_frame_len], buf, size);
  uvc_data.use_frame_len+= size;
}

static uint8_t uvc_is_sof(void)
{
    uint8_t m_fid;
    m_fid = (tmp_packet_framebuffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_FID_BIT);
    if (m_fid != uvc_data.prev_fid)
    {
        return 1;
    }
    return 0;
}

static uint8_t uvc_is_eof(void)
{
    if (tmp_packet_framebuffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_EOF_BIT)
    {
        return 1;
    }
    return 0;
}

static uint8_t uvc_is_right_len(uint16_t size)
{
  if ((size < 2) && (size > UVC_RX_FIFO_SIZE_LIMIT))
  {
    return 0;
  }
  return 1;
}

static uint8_t uvc_is_data_packet(uint16_t size)
{
  if(size <= UVC_HEADER_SIZE)
  {
      return 0;
  }
  return 1;
}
