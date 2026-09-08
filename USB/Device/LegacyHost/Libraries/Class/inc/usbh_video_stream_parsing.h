#ifndef _USBH_VIDEO_STREAM_PARSING_H
#define _USBH_VIDEO_STREAM_PARSING_H

#include "usbh_video_core.h"

#define UVC_HEADER_SIZE_POS             0
#define UVC_HEADER_BIT_FIELD_POS        1

#define UVC_HEADER_FID_BIT              (1 << 0)
#define UVC_HEADER_EOF_BIT              (1 << 1)
#define UVC_HEADER_ERR_BIT              (1 << 6)

#define UVC_HEADER_SIZE                 12

typedef struct _uvc_data_struct
{
  uint32_t packet_cnt;
  uint32_t data_packet_cnt;
  uint32_t header_packet_cnt;
  uint32_t frame_cnt;
  
  uint32_t use_frame_len;
  uint8_t *use_buffer;
    
  uint32_t ready_frame_len;
  uint8_t *ready_buffer;
    
  uint8_t *buffer0;
  uint8_t *buffer1;
  
  uint8_t prev_fid;
  
  uint8_t ready_frame_flag;
}uvc_data_struct;

extern uvc_data_struct uvc_data;

void uvc_stream_data_process(uint16_t size);
void uvc_stream_init(uint8_t* buffer0, uint8_t* buffer1);

#endif