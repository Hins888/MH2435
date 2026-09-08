#ifndef __BSP_KEYBOARD_H__
#define __BSP_KEYBOARD_H__

#include "mh2435.h"

#define KEY_S1	   0x01
#define KEY_S2	   0x02
#define KEY_S3	   0x03
#define KEY_S4	   0x04

#define KBD_ROWS    2
#define KBD_COLS    2

extern volatile uint8_t key_code;

extern GPIO_TypeDef *row_ports[KBD_ROWS];	
extern uint16_t row_pins[KBD_ROWS];

extern GPIO_TypeDef * col_ports[KBD_COLS];
extern uint16_t col_pins[KBD_COLS];


void KBD_NvicConfig(void);
void KBD_ExtiConfig(void);

#endif
