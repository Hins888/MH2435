#ifndef __RTL8201F_H
#define __RTL8201F_H


#define RTL8201x_PHY_ADDRESS     0x03                //RTL8201F PHYоƬ
 
u8 dev_rfl8201x_init(void);
u8 RTL8201x_Get_Speed(void);
u8 ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
u8 ETH_Tx_Packet(u16 FrameLength);
u32 ETH_GetCurrentTxBuffer(void);


#endif   //__RTL8201F_H

