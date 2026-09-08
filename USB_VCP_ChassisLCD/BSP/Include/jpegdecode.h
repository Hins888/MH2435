#ifndef __JPEGDECODE_H__
#define __JPEGDECODE_H__


#include "mh2435.h"
#include "jpeg_check.h"

void JpegInit(void);
void JpegDecodeYuv(uint8_t* jpegSrc, uint32_t jpegSize, uint8_t* yuv, JPEG_InfoTypeDef *jpegInfo);
void Yuv2Rgb(uint8_t* yuv, uint8_t*rgb);
void Yuv2RgbInit(JPEG_InfoTypeDef *jpegInfo);

void JpegDecodeYuv_interrupt(uint8_t* jpegSrc, uint32_t jpegSize, uint8_t* yuv);


#endif


