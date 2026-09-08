#ifndef __JPEG_CHECK_H__
#define __JPEG_CHECK_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define JPEG_MAX_HEAD   1024
#define USE_JPEGCHECK_DEBUG 0

#if USE_JPEGCHECK_DEBUG
#define  JPEG_CHECK_DEBUG(...)    printf(__VA_ARGS__);
#else
#define  JPEG_CHECK_DEBUG(...)
#endif

// JPEG 标记
#define SOI_MARKER  ((uint16_t)0xFFD8)  // 起始标记
#define DQT_MARKER  ((uint16_t)0xFFDB)  // 定义量化表
#define SOF_MARKER  ((uint16_t)0xFFC0)  // 起始帧 (Baseline DCT)

#define SOS_MARKER  ((uint16_t)0xFFDA)  // 数据开始标记
#define EOI_MARKER  ((uint16_t)0xFFD9)  // 结束标记


#define BIG_LITTLE_SWAP16(x)        ( (((uint16_t)x & 0xff00) >> 8) | \
                                      (((uint16_t)x & 0x00ff) << 8) )

bool jpeg_check_head_swap(uint8_t *buf, long length);
bool jpeg_check_start(uint8_t* buf,uint32_t len);
bool jpeg_check_end(uint8_t* buf,uint32_t len);
#endif
