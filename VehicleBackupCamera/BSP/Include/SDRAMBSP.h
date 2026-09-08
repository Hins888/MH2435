#ifndef __SDRAM_BSP_H__
#define __SDRAM_BSP_H__

#include "mh2435.h"

#ifndef CONFIG_SDRAM_IO_LIST
#ifndef CONFIG_SDRAM_CTL_IO
/*!
    @brief SDRAM Control IO List

    @param LDQM: { PE0  }
    @param UDQM: { PE1  }
    @param CLK:  { PG8  }
    @param CKE:  { PH2  }
    @param CSN:  { PH3  }
    @param BA0:  { PG4  }
    @param BA1:  { PG5  }
    @param RAS:  { PF11 }
    @param CAS:  { PG15 }
    @param WE:   { PH5  }

                            LDQM UDQM CLK  CKE  CSN  BA0  BA1  RAS   CAS   WE  */
#define CONFIG_SDRAM_CTL_IO PE0, PE1, PG8, PH2, PH3, PG4, PG5, PF11, PG15, PH5,
#endif

#ifndef CONFIG_SDRAM_ADDR_IO
/*!
    @brief SDRAM Control IO List

    @param A0:   { PF0  }
    @param A1:   { PF1  }
    @param A2:   { PF2  }
    @param A3:   { PI13 }
    @param A4:   { PI14 }
    @param A5:   { PI15 }
    @param A6:   { PF12 }
    @param A7:   { PF13 }
    @param A8:   { PF14 }
    @param A9:   { PF15 }
    @param A10:  { PG0  }
    @param A11:  { PG1  }
                             A0   A1   A2   A3    A4    A5    A6    A7    A8    A9    A10  A11  */
#define CONFIG_SDRAM_ADDR_IO PF0, PF1, PF2, PI13, PI14, PI15, PF12, PF13, PF14, PF15, PG0, PG1,
#endif

#ifndef CONFIG_SDRAM_DATA_IO
/*!
    @brief SDRAM Data IO List

    @param D0:   { PD14 }
    @param D1:   { PD15 }
    @param D2:   { PD0  }
    @param D3:   { PD1  }
    @param D4:   { PE7  }
    @param D5:   { PE8  }
    @param D6:   { PE9  }
    @param D7:   { PE10 }
    @param D8:   { PI8  }
    @param D9:   { PC12 }
    @param D10:  { PG3  }
    @param D11:  { PI11 }
    @param D12:  { PI12 }
    @param D13:  { PD8  }
    @param D14:  { PD9  }
    @param D15:  { PD10 }

                             D0    D1    D2   D3   D4   D5   D6   D7    D8   D9    D10  D11   D12   D13  D14  D15   */
#define CONFIG_SDRAM_DATA_IO PD14, PD15, PD0, PD1, PE7, PE8, PE9, PE10, PI8, PC12, PG3, PI11, PI12, PD8, PD9, PD10,
#endif

#ifndef CONFIG_SDRAM_DEFAULT_TIMMING
/* Parameter Index: 0-divisor, 1-delay, 2-cas, 3-ras, 4-rcd, 5-rp, 6-wr, 7-rcar, 8-xsr, 9-rc, A-init, B-initref, C-ref.
                                                     0  1  2  3  4  5  6  7  8  9  A      B  C                           */
//#define CONFIG_SDRAM_DEFAULT_TIMMING MakeSDRAMConfig(2, 1, 3, 8, 3, 3, 2, 8, 8, 8, 30000, 8, 2292) // 150M
#define CONFIG_SDRAM_DEFAULT_TIMMING MakeSDRAMConfig(2, 2, 3, 4, 1, 1, 1, 6, 6, 6, 24009, 7, 1822) // 120M
#endif

typedef union {
    struct {
        uint8_t Divisor : 8;
        uint8_t Delay : 8;
        uint16_t : 16;
        uint32_t CASL : 2; // [1:0]
        uint32_t RAS : 4;  // [5:2]
        uint32_t RCD : 3;  // [8:6]
        uint32_t RP : 3;   // [11:9]
        uint32_t WR : 2;   // [13:12]
        uint32_t RCAR : 4; // [17:14]
        uint32_t XSRL : 4; // [21:18]
        uint32_t RC : 4;   // [25:22]
        uint32_t CASH : 1; // [26]
        uint32_t XSRH : 5; // [31:27]

        uint32_t INIT : 16;   // [15:0]
        uint32_t INITREF : 4; // [19:16]
        uint32_t : 12;        // [31:20]

        uint32_t REF : 16; // [15:0]
        uint32_t : 16;     // [31:16]
    };
    struct {
        uint32_t : 32;
        uint32_t TimmingReg[3];
    };
} SDRAMTimmingUnion;

#define MakeSDRAMConfig(divisor, delay, cas, ras, rcd, rp, wr, rcar, xsr, rc, init, initref, ref) \
    ((SDRAMTimmingUnion) {                                                                        \
        .Divisor = divisor,                                                                       \
        .Delay   = delay,                                                                         \
        .REF     = ref,                                                                           \
        .CASL    = (cas - 1) & 0x3,                                                               \
        .CASH    = (cas - 1) >> 2,                                                                \
        .RAS     = (ras - 1),                                                                     \
        .RCD     = (rcd - 1),                                                                     \
        .RP      = (rp - 1),                                                                      \
        .WR      = (wr - 1),                                                                      \
        .RCAR    = (rcar - 1),                                                                    \
        .XSRL    = (xsr - 1) & 0xF,                                                               \
        .XSRH    = (xsr - 1) >> 4,                                                                \
        .RC      = (rc - 1),                                                                      \
        .INIT    = (init - 1),                                                                    \
        .INITREF = (initref - 1),                                                                 \
    })

#define CONFIG_SDRAM_IO_LIST                                          \
    (IOEnum[]) {                                                      \
        CONFIG_SDRAM_CTL_IO CONFIG_SDRAM_ADDR_IO CONFIG_SDRAM_DATA_IO \
    }
#endif

#ifndef CONFIG_SDRAM_IO_COUNT
#define CONFIG_SDRAM_IO_COUNT (sizeof(CONFIG_SDRAM_IO_LIST) / sizeof(IOEnum))
#endif

extern void SDRAMSetup(void);

#endif
