/*
 * uart.h
 *
 *  Created on: 2020-4-27
 */

#ifndef __BSP_USART_H_
#define __BSP_USART_H_

#include "bsp_api.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    volatile int inIndex;   //中断发送索引
    volatile int outIndex;  //写缓冲区索引
    volatile int bufSize;   //缓冲区中的字节数
    uint8_t*     buf;       //发送缓冲区
    int          Rfu;       //发送缓冲区大小
} ST_UART_RING_BUFF;


int bsp_uart_init(uint32_t bound);
int bsp_uart_close(void);
int bsp_uart_write(int8_t *data, uint32_t dataLen);
int bsp_uart_read(int8_t *data, uint32_t dataLen, uint32_t timeOut);
int bsp_uart_fluse(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_USART_H_ */
