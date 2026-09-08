#ifndef __BSP_CPU_H
#define __BSP_CPU_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include "mh2435.h"


#define UART_DBUG_ID    1


#define VECTORS_SECTION             __attribute__((section(".VECTORSLIST")))
#define SRAM_FUNCTION               __attribute__((section(".SRAM_FUNCTION")))

#define reg_bit_read(a, n)              (((*(u32 *)(a))&(1<<(n)))>>(n))
#define reg_bit_clr(a, n)               do { *(u32 *)(a) &= ~(1<<(n)); } while(0)
#define reg_bit_set(a, n)               do { *(u32 *)(a) |= (1<<(n)); } while(0)

#define reg_bits_read(a, off, nbits)    (((*(u32 *)(a))&(((1<<(nbits))-1)<<(off)))>>(off))
#define reg_bits_clr(a, off, nbits)     do { *(u32 *)(a) &= ~(((1<<(nbits))-1)<<(off)); } while(0)
#define reg_bits_set(a, off, nbits, v)  do { reg_bits_clr(a,off,nbits); *(u32 *)(a) |= (((1<<(nbits))-1)&(v))<<(off); } while(0)


#ifndef bool
typedef unsigned char bool;      /* 8-bit*/
#endif

#ifndef true
    #define true 1
#endif

#ifndef false
    #define false 0
#endif
/*common Error Code*/
#ifndef OS_OK
#define OS_OK               (0)//succes
#endif

#ifndef OS_ERR
#define OS_ERR             (-1) //error
#endif

#ifndef OS_ENODEV
#define OS_ENODEV      -1000    /* No such device */
#endif

#ifndef OS_ENOENT
#define OS_ENOENT      -1001    /* No such file or directory */
#endif

#ifndef OS_EIO
#define OS_EIO         -1002    /* I/O error */
#endif

#ifndef OS_ENXIO
#define OS_ENXIO       -1003    /* No such device or address */
#endif

#ifndef OS_EACCES
#define OS_EACCES      -1004    /* Permission denied */
#endif

#ifndef OS_EINVAL
#define OS_EINVAL      -1005    /* Invalid argument */
#endif

#ifndef OS_ENOMEM
#define OS_ENOMEM      -1006    /* Out of memory */
#endif

#ifndef OS_EBUSY
#define OS_EBUSY       -1007    /* Device or resource busy */
#endif

#ifndef OS_ETIMEOUT
#define OS_ETIMEOUT     -1008  /* timeout */
#endif

#ifndef OS_EDATA
#define OS_EDATA         -1009  /* data error */
#endif


#ifdef __cplusplus
}
#endif

#endif   //__BSP_CPU_H
