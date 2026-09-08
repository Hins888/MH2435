/*
 * ring_buf.h
 *
 *  Created on: 2022年5月7日
 *      Author: sz_fae
 */

#ifndef INCLUDE_RING_BUF_H_
#define INCLUDE_RING_BUF_H_

#include "bsp_cpu.h"

typedef struct circ_buf {
    unsigned char *buf;
    uint32_t head;
    uint32_t tail;
    uint32_t bufsize;
    uint32_t stattus;
} ST_CIRC_BUF;


static __inline int RINGBUF_IS_EMPTY(uint32_t head, uint32_t tail, uint32_t size)
{
    return head == tail;
 }

static __inline int RINGBUF_IS_FULL(uint32_t head, uint32_t tail, uint32_t size)
{
    return tail == ((head + 1) % size);
}

static __inline int RINGBUF_CNT(uint32_t head, uint32_t tail, uint32_t size)
{
    // ((tail +size- head)&(size-1)) //size is 2^n
    return (head >= tail) ? (head - tail) : (size + head - tail); // Not support / cmd
    //return ((head + size - tail)%(size));
}

static __inline int RINGBUF_SPACE(uint32_t head, uint32_t tail, uint32_t size)
{
    // ((head +size- tail)&(size-1))  // size is 2^n
    return (head >= tail) ? (size + tail - head - 1) :  (tail - head - 1);  // Not support / cmd
    //return ((tail +size- head -1)%(size));  //CIRC_CNT(tail, head+1, size);
}

static __inline int RINGBUF_CNT_TO_END(uint32_t head, uint32_t tail, uint32_t size)
{
    uint32_t end = size - tail;
    uint32_t n = ((head + size - tail)%(size));
    return (n < end ? n : end);
}

static __inline int RINGBUF_SPACE_TO_END(uint32_t head, uint32_t tail, uint32_t size)
{
    uint32_t end = size - 1 - head;
    uint32_t n = (tail +size- head -1)%(size);
    return (n <= end ? n : end);
}

#endif /* INCLUDE_RING_BUF_H_ */
