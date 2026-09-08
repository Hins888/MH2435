/*--------------------- Flash Configuration ----------------------------------
; <h> Flash Configuration
;   <o0> Flash Base Address <0x0-0xFFFFFFFF:8>
;   <o1> Flash Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
 *----------------------------------------------------------------------------*/
#define __ROM_BASE      0x08001000
#define __ROM_SIZE      0x01FFF000

/*--------------------- Embedded RAM Configuration ---------------------------
; <h> RAM Configuration
;   <o0> RAM Base Address    <0x0-0xFFFFFFFF:8>
;   <o1> RAM Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
 *----------------------------------------------------------------------------*/
#define __RAM_BASE      0x20000000
#define __RAM_SIZE      0x00140000

/*--------------------- Stack / Heap Configuration ---------------------------
; <h> Stack / Heap Configuration
;   <o0> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;   <o1> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
 *----------------------------------------------------------------------------*/
#define __STACK_SIZE    0x2000
#define __HEAP_SIZE     0x2000

/*--------------------- CMSE Venner Configuration ---------------------------
; <h> CMSE Venner Configuration
;   <o0>  CMSE Venner Size (in Bytes) <0x0-0xFFFFFFFF:32>
; </h>
 *----------------------------------------------------------------------------*/
#define __CMSEVENEER_SIZE    0x200
