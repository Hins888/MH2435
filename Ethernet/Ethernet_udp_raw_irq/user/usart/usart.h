#ifndef __USART_H
#define __USART_H

#include <stdio.h>
#include "mh2435.h"

void    USART_Configuration(uint32_t BaudRate);
uint8_t GetCmd(void);
#endif /* __USART_H */
