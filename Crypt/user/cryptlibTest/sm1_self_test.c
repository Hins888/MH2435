#include <string.h>
#include <stdio.h>
#include "mh2435.h"
#include "mh_sm1.h"
#include "mh_rand.h"
#include "debug.h"


static const uint8_t cau8Plain[16] = {0x20,0xc4,0xbf,0x20,0xc2,0xbc,0x20,0x0a,0xc7,0xb0,0x20,0xd1,0xd4,0x09,0x36,0x0a};

static const uint8_t cu8Skey128_8[16] = {0xa2,0x91,0xb3,0xeb,0xa4,0xed,0x22,0x5f,0x1c,0xea,0xa6,0x07,0x62,0x8f,0xb2,0x36};
static const uint8_t cu8Ekey128_8[16] = {0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef};
static const uint8_t cu8Akey128_8[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static const uint8_t cu8Skey192_10[16] = {0xa2,0x91,0xb3,0xeb,0xa4,0xed,0x22,0x5f,0x1c,0xea,0xa6,0x07,0x62,0x8f,0xb2,0x36};
static const uint8_t cu8Ekey192_10[16] = {0x40,0xBB,0x12,0xDD,0x6A,0x82,0x73,0x86,0x7F,0x35,0x29,0xD3,0x54,0xB4,0xA0,0x26};
static const uint8_t cu8Akey192_10[16] = {0x0C,0x90,0xE1,0x5A,0x4F,0x92,0x36,0xB2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static const uint8_t cu8Skey256_12[16] = {0xa2,0x91,0xb3,0xeb,0xa4,0xed,0x22,0x5f,0x1c,0xea,0xa6,0x07,0x62,0x8f,0xb2,0x36};
static const uint8_t cu8Ekey256_12[16] = {0x40,0xBB,0x12,0xDD,0x6A,0x82,0x73,0x86,0x7F,0x35,0x29,0xD3,0x54,0xB4,0xA0,0x26};
static const uint8_t cu8Akey256_12[16] = {0x0C,0x90,0xE1,0x5A,0x4F,0x92,0x36,0xB2,0x12,0x34,0x56,0x78,0x12,0x34,0x56,0x78};

static void SM1Data128_Test()
{
    uint32_t u32KeyLen, t;
    uint8_t au8Key[32 + 16 + 16], au8IV[16];//, au8Out[16];
//    uint8_t u8Option;
    MH_SYM_CRYPT_CALL callSm1;
    uint32_t u32Ret = 0;
//    uint8_t cau8Plain[1024] = {0};
    uint8_t cu8Cipher128_8[16] = {0};
    uint8_t au8Out[16] = {0};
    
//    memset(cau8Plain, 'd', sizeof(cau8Plain));
    memset(au8Key, 0, sizeof(au8Key));
    u32KeyLen = 0;
    memcpy(au8Key, cu8Skey128_8, 16);
    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Ekey128_8, 16);

    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Akey128_8, 16);

    //128bit round8 encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher128_8;
    callSm1.u32OutLen = sizeof(cu8Cipher128_8);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    u32Ret = MHSM1_EncDec(&callSm1);
		
		if (MH_RET_SM1_SUCCESS != u32Ret)
		{
			printf("errRet = %08x\n", u32Ret);
      r_printf(0, "MHSM1_EncDec test\n");
		}
		
    t = (memcmp(cu8Cipher128_8, cau8Plain, sizeof(cu8Cipher128_8)));
    r_printf(t, "SM1 128_8 ECB Encrypt Test\n");

    //128bit round8 decrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher128_8;
    callSm1.u32InLen = sizeof(cu8Cipher128_8);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    //GPIO_ResetBits(GPIO,GPIO_Pin_5); 
    MHSM1_EncDec(&callSm1);
    //GPIO_SetBits(GPIO,GPIO_Pin_5); 
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher128_8)));
    r_printf(t, "SM1 128_8 ECB Decrypt Test\n");

    //128bit round10 encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher128_8;
    callSm1.u32OutLen = sizeof(cu8Cipher128_8);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    //GPIO_ResetBits(GPIO,GPIO_Pin_5); 
    MHSM1_EncDec(&callSm1);
    //GPIO_SetBits(GPIO,GPIO_Pin_5);
//    t = (!memcmp(cu8Cipher128_8, au8Out, sizeof(cu8Cipher128_8)));
//    r_printf(t, "SM1 128_8 ECB Encrypt Test\n");

    //128bit round10 decrypt
    //u8Option = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher128_8;
    callSm1.u32InLen = sizeof(cu8Cipher128_8);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    //GPIO_ResetBits(GPIO,GPIO_Pin_5); 
    MHSM1_EncDec(&callSm1);
    //GPIO_SetBits(GPIO,GPIO_Pin_5);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher128_8)));
    r_printf(t, "SM1 128_10 ECB Decrypt Test\n");

    
    //128bit round12 encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher128_8;
    callSm1.u32OutLen = sizeof(cu8Cipher128_8);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    //GPIO_ResetBits(GPIO,GPIO_Pin_5); 
    MHSM1_EncDec(&callSm1);
    //GPIO_SetBits(GPIO,GPIO_Pin_5);
//    t = (!memcmp(cu8Cipher128_8, au8Out, sizeof(cu8Cipher128_8)));
//    r_printf(t, "SM1 128_8 ECB Encrypt Test\n");

    //128bit round12 decrypt
    //u8Option = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher128_8;
    callSm1.u32InLen = sizeof(cu8Cipher128_8);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    MHSM1_EncDec(&callSm1);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher128_8)));
    r_printf(t, "SM1 128_12 ECB Decrypt Test\n");
    
    
       //128bit round14 encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher128_8;
    callSm1.u32OutLen = sizeof(cu8Cipher128_8);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_14;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    //GPIO_ResetBits(GPIO,GPIO_Pin_5); 
    MHSM1_EncDec(&callSm1);
    //GPIO_SetBits(GPIO,GPIO_Pin_5);
//    t = (!memcmp(cu8Cipher128_8, au8Out, sizeof(cu8Cipher128_8)));
//    r_printf(t, "SM1 128_8 ECB Encrypt Test\n");

    //128bit round14 decrypt
    //u8Option = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher128_8;
    callSm1.u32InLen = sizeof(cu8Cipher128_8);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_14;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    //GPIO_ResetBits(GPIO,GPIO_Pin_5); 
    MHSM1_EncDec(&callSm1);
    //GPIO_SetBits(GPIO,GPIO_Pin_5);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher128_8)));
    r_printf(t, "SM1 128_14 ECB Decrypt Test\n");

}

static void SM1Data192_Test()
{
    uint32_t u32KeyLen, t;
    uint8_t au8Key[32 + 16 + 16], au8IV[16];//, au8Out[16];
//    uint8_t u8Option;
    MH_SYM_CRYPT_CALL callSm1;
    uint8_t cu8Cipher192_10[16] = {0};
    uint8_t au8Out[16] = {0};
//    memset(cau8Plain, 'e', sizeof(cau8Plain));
    u32KeyLen = 0;
    memcpy(au8Key, cu8Skey192_10, 16);
    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Ekey192_10, 16);

    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Akey192_10, 16);

    //192bit round8 encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher192_10;
    callSm1.u32OutLen = sizeof(cu8Cipher192_10);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
//    t = (!memcmp(cu8Cipher192_10, au8Out, sizeof(cu8Cipher192_10)));
//    r_printf(t, "SM1 192_10 ECB Encrypt Test\n");
    //192bit round8 decrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher192_10;
    callSm1.u32InLen = sizeof(cu8Cipher192_10);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher192_10)));
    r_printf(t, "SM1 192_8 ECB Decrypt Test\n");
    
        //192bit round10 encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher192_10;
    callSm1.u32OutLen = sizeof(cu8Cipher192_10);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
//    t = (!memcmp(cu8Cipher192_10, au8Out, sizeof(cu8Cipher192_10)));
//    r_printf(t, "SM1 192_10 ECB Encrypt Test\n");
    //192bit round10 decrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher192_10;
    callSm1.u32InLen = sizeof(cu8Cipher192_10);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher192_10)));
    r_printf(t, "SM1 192_10 ECB Decrypt Test\n");
    
        //192bit round12 encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher192_10;
    callSm1.u32OutLen = sizeof(cu8Cipher192_10);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
//    t = (!memcmp(cu8Cipher192_10, au8Out, sizeof(cu8Cipher192_10)));
//    r_printf(t, "SM1 192_10 ECB Encrypt Test\n");
    //192bit round10 decrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher192_10;
    callSm1.u32InLen = sizeof(cu8Cipher192_10);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher192_10)));
    r_printf(t, "SM1 192_12 ECB Decrypt Test\n");
    
        //192bit round14 encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher192_10;
    callSm1.u32OutLen = sizeof(cu8Cipher192_10);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_14;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
//    t = (!memcmp(cu8Cipher192_10, au8Out, sizeof(cu8Cipher192_10)));
//    r_printf(t, "SM1 192_10 ECB Encrypt Test\n");
    //192bit round14 decrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher192_10;
    callSm1.u32InLen = sizeof(cu8Cipher192_10);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_14;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher192_10)));
    r_printf(t, "SM1 192_14 ECB Decrypt Test\n");
    
}

static void SM1Data256_Test()
{
    uint32_t u32KeyLen, t;
    uint8_t au8Key[32 + 16 + 16], au8IV[16];//, au8Out[16];
//    uint8_t u8Option;
    MH_SYM_CRYPT_CALL callSm1;
//    uint8_t cau8Plain[1024] = {0};
    uint8_t cu8Cipher256_12[16] = {0};
    uint8_t au8Out[16] = {0};
//    memset(cau8Plain, 'f', sizeof(cau8Plain));
    u32KeyLen = 0;
    memcpy(au8Key, cu8Skey256_12, 16);
    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Ekey256_12, 16);

    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Akey256_12, 16);

    //256bit round8 encrypt
    //u8Option = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    memset(au8IV, 0, sizeof(au8IV));

    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher256_12;
    callSm1.u32OutLen = sizeof(cu8Cipher256_12);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    //mh_Sm1(au8Key, au8IV, (void *)cau8Plain, au8Out, 16, u8Option);
    MHSM1_EncDec(&callSm1);
//    t = (!memcmp(cu8Cipher256_12, au8Out, sizeof(cu8Cipher256_12)));
//    r_printf(t, "SM1 256_12 ECB Encrypt Test\n");
    //256bit round12 decrypt
    //u8Option = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    memset(au8IV, 0, sizeof(au8IV));

    callSm1.pu8In = (uint8_t *)cu8Cipher256_12;
    callSm1.u32InLen = sizeof(cu8Cipher256_12);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
    //mh_Sm1(au8Key, au8IV, (void *)cu8Cipher256_12, au8Out, 16, u8Option);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher256_12)));
    r_printf(t, "SM1 256_8 ECB Decrypt Test\n");
    
        //256bit round10 encrypt
    //u8Option = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    memset(au8IV, 0, sizeof(au8IV));

    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher256_12;
    callSm1.u32OutLen = sizeof(cu8Cipher256_12);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    //mh_Sm1(au8Key, au8IV, (void *)cau8Plain, au8Out, 16, u8Option);
    MHSM1_EncDec(&callSm1);
//    t = (!memcmp(cu8Cipher256_12, au8Out, sizeof(cu8Cipher256_12)));
//    r_printf(t, "SM1 256_12 ECB Encrypt Test\n");
    //256bit round12 decrypt
    //u8Option = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    memset(au8IV, 0, sizeof(au8IV));

    callSm1.pu8In = (uint8_t *)cu8Cipher256_12;
    callSm1.u32InLen = sizeof(cu8Cipher256_12);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
    //mh_Sm1(au8Key, au8IV, (void *)cu8Cipher256_12, au8Out, 16, u8Option);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher256_12)));
    r_printf(t, "SM1 256_10 ECB Decrypt Test\n");


        //256bit round12 encrypt
    //u8Option = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher256_12;
    callSm1.u32OutLen = sizeof(cu8Cipher256_12);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    //mh_Sm1(au8Key, au8IV, (void *)cau8Plain, au8Out, 16, u8Option);
    MHSM1_EncDec(&callSm1);
//    t = (!memcmp(cu8Cipher256_12, au8Out, sizeof(cu8Cipher256_12)));
//    r_printf(t, "SM1 256_12 ECB Encrypt Test\n");
    //256bit round12 decrypt
    //u8Option = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    memset(au8IV, 0, sizeof(au8IV));

    callSm1.pu8In = (uint8_t *)cu8Cipher256_12;
    callSm1.u32InLen = sizeof(cu8Cipher256_12);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
    //mh_Sm1(au8Key, au8IV, (void *)cu8Cipher256_12, au8Out, 16, u8Option);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher256_12)));
    r_printf(t, "SM1 256_12 ECB Decrypt Test\n");
    
        //256bit round14 encrypt
    //u8Option = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    memset(au8IV, 0, sizeof(au8IV));

    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher256_12;
    callSm1.u32OutLen = sizeof(cu8Cipher256_12);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_14;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    //mh_Sm1(au8Key, au8IV, (void *)cau8Plain, au8Out, 16, u8Option);
    MHSM1_EncDec(&callSm1);
//    t = (!memcmp(cu8Cipher256_12, au8Out, sizeof(cu8Cipher256_12)));
//    r_printf(t, "SM1 256_12 ECB Encrypt Test\n");
    //256bit round12 decrypt
    //u8Option = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    memset(au8IV, 0, sizeof(au8IV));

    callSm1.pu8In = (uint8_t *)cu8Cipher256_12;
    callSm1.u32InLen = sizeof(cu8Cipher256_12);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_14;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    MHSM1_EncDec(&callSm1);
    //mh_Sm1(au8Key, au8IV, (void *)cu8Cipher256_12, au8Out, 16, u8Option);
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher256_12)));
    r_printf(t, "SM1 256_14 ECB Decrypt Test\n");
}
 
static void SM1_FixedTest(void)
{
    SM1Data128_Test();
    SM1Data192_Test();
    SM1Data256_Test();
}

#if MH_SM1_SUPPORT_OFB_CFB

#define SM1_CFB_OFB_LEN     (0x20)

void SM1_Extra_Func_Test()
{
    uint8_t au8Plain_CFB[SM1_CFB_OFB_LEN] = {0x39, 0x69, 0x1C, 0x9A, 0x5B, 0x2D, 0x61, 0xA0, 0x9A, 0x21, 0xC6, 0x11, 0x2C, 0x5B, 0x52, 0x81, \
                            0x62, 0x73, 0xC5, 0x07, 0x71, 0x11, 0x66, 0x13, 0x47, 0xF1, 0xCA, 0x9F, 0xA1, 0xF3, 0x97, 0x81};
    uint8_t au8Plain_OFB[SM1_CFB_OFB_LEN] = {0xEC, 0x20, 0x8A, 0x2A, 0xE1, 0x64, 0x94, 0xCB, 0x06, 0x23, 0x12, 0x5E, 0x60, 0x19, 0x60, 0x25, \
                            0x3B, 0x37, 0x3C, 0x30, 0x79, 0xA3, 0xC3, 0x7D, 0x2B, 0xFF, 0x9D, 0x64, 0x5E, 0xEE, 0x74, 0x32};
    uint8_t au8Cipher_CFB[SM1_CFB_OFB_LEN] = {0xb6, 0x71, 0xf6, 0xac, 0xe2, 0x78, 0x50, 0x47, 0x92, 0xf2, 0xf1, 0x27, 0xd4, 0x01, 0xb9, 0xd3, \
                            0xd3, 0xd8, 0x24, 0x82, 0xf6, 0xcd, 0x3e, 0x4c, 0x9d, 0x99, 0xf6, 0x17, 0x5f, 0xd8, 0x7f, 0xe6};
    uint8_t au8Cipher_OFB[SM1_CFB_OFB_LEN] = {0x39, 0xB9, 0x44, 0xEF, 0x90, 0xEF, 0x2A, 0xDE, 0x9B, 0x93, 0x37, 0xE5, 0x65, 0xC0, 0x68, 0x71, \
                            0x77, 0x4F, 0x05, 0x0C, 0xE7, 0xA5, 0xD1, 0xD0, 0xBA, 0x91, 0x94, 0x7D, 0xF1, 0x51, 0xA7, 0x42};
    uint8_t au8Key_CFB[16] = {0x8B, 0x93, 0xF2, 0xD5, 0x3C, 0x26, 0x3A, 0x4C, 0xDF, 0x7F, 0xFC, 0x65, 0xC9, 0x24, 0x26, 0xF3};
    uint8_t au8Key_OFB[16] = {0x03, 0x61, 0xAF, 0x08, 0x10, 0xD5, 0xDA, 0x3F, 0x66, 0x85, 0x05, 0x9F, 0x94, 0x74, 0x50, 0x4D};
    uint8_t au8IV_CFB[16] = {0xDC, 0x15, 0x68, 0x76, 0xE5, 0x3E, 0x7F, 0xB2, 0xEA, 0x32, 0x6B, 0xB7, 0x83, 0xDB, 0xDC, 0xCC};
    uint8_t au8IV_OFB[16] = {0x84, 0x9B, 0x9C, 0x1A, 0x1E, 0xFB, 0xBD, 0x24, 0x40, 0x2C, 0x82, 0x78, 0xBE, 0xA3, 0x09, 0x23};
    uint8_t au8Result[SM1_CFB_OFB_LEN] = {0};
    MH_SYM_CRYPT_CALL callSm1;
    //CFB Test
    // sm1_128_8 cfb enc
    memset(&callSm1, 0, sizeof(callSm1));
    callSm1.pu8In = (uint8_t *)au8Plain_CFB;
    callSm1.u32InLen = SM1_CFB_OFB_LEN;
    callSm1.pu8Out = au8Result;
    callSm1.u32OutLen = SM1_CFB_OFB_LEN;
    callSm1.pu8IV = au8IV_CFB;
    callSm1.pu8Key = au8Key_CFB;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_CFB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    if(MH_RET_SM1_SUCCESS != MHSM1_EncDecExtra(&callSm1))
    {
        r_printf(0, "SM1 CFB enc test\n");
    }
    r_printf(!memcmp(au8Cipher_CFB, au8Result, SM1_CFB_OFB_LEN), "SM1 128_8 CFB Encrypt Test\n");
    // sm1_128_8 cfb dec
    callSm1.pu8In = au8Cipher_CFB;
    callSm1.u32InLen = SM1_CFB_OFB_LEN;
    callSm1.pu8Out = au8Result;
    callSm1.u32OutLen = SM1_CFB_OFB_LEN;
    callSm1.pu8IV = au8IV_CFB;
    callSm1.pu8Key = au8Key_CFB;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_CFB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    if(MH_RET_SM1_SUCCESS != MHSM1_EncDecExtra(&callSm1))
    {
        r_printf(0, "SM1 CFB dec test\n");
    }
    r_printf(!memcmp(au8Plain_CFB, au8Result, SM1_CFB_OFB_LEN), "SM1 128_8 CFB Decrypt  Test\n");
    
    //OFB TEST
    // sm1_128_8 ofb enc
    memset(&callSm1, 0, sizeof(callSm1));
    callSm1.pu8In = (uint8_t *)au8Plain_OFB;
    callSm1.u32InLen = SM1_CFB_OFB_LEN;
    callSm1.pu8Out = au8Result;
    callSm1.u32OutLen = SM1_CFB_OFB_LEN;
    callSm1.pu8IV = au8IV_OFB;
    callSm1.pu8Key = au8Key_OFB;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_OFB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    if(MH_RET_SM1_SUCCESS != MHSM1_EncDecExtra(&callSm1))
    {
        r_printf(0, "SM1 OFB enc test\n");
    }
    r_printf(!memcmp(au8Cipher_OFB, au8Result, SM1_CFB_OFB_LEN), "SM1 128_8 OFB Encrypt Test\n");
    // sm1_128_8 ofb dec
    callSm1.pu8In = au8Cipher_OFB;
    callSm1.u32InLen = SM1_CFB_OFB_LEN;
    callSm1.pu8Out = au8Result;
    callSm1.u32OutLen = SM1_CFB_OFB_LEN;
    callSm1.pu8IV = au8IV_OFB;
    callSm1.pu8Key = au8Key_OFB;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_OFB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    if(MH_RET_SM1_SUCCESS != MHSM1_EncDecExtra(&callSm1))
    {
        r_printf(0, "SM1 OFB dec test\n");
    }
    r_printf(!memcmp(au8Plain_OFB, au8Result, SM1_CFB_OFB_LEN), "SM1 128_8 OFB Decrypt  Test\n");
}

void SM1_Extra_Rand_Test(void)
{
    uint32_t i, j, k, t;
    uint32_t u32MsgLen = 0;
    MH_SYM_CRYPT_CALL callSm1;  
    SM1_MODE_TYP ModeType[2] = {{MH_SM1_OPT_BLK_CFB, "CFB"},
                             {MH_SM1_OPT_BLK_OFB, "OFB"}};
    SM1_KEY_TYP KeyType[3] = {{16, NULL, MH_SM1_OPT_KEY_128, "KEY_128"},
                           {24, NULL, MH_SM1_OPT_KEY_192, "KEY_192"},
                           {32, NULL, MH_SM1_OPT_KEY_256, "KEY_256"}};
    SM1_ROUND_NUM RoundNum[4] = {{MH_SM1_OPT_ROUND_8,"8_round"},
                                {MH_SM1_OPT_ROUND_10,"10_round"},
                                {MH_SM1_OPT_ROUND_12,"12_round"},
                                {MH_SM1_OPT_ROUND_14,"14_round"}};
    uint8_t au8Plain[2][SM1_TIME_TEST_DATA];
    uint8_t au8Cipher[SM1_TIME_TEST_DATA];
    uint8_t au8Iv[16] = {0};
    uint8_t au8IvTmp[16] = {0};
    uint8_t au8Key[32] = {0}; 
    KeyType[0].pu8Key = au8Key;
    KeyType[1].pu8Key = au8Key;
    KeyType[2].pu8Key = au8Key;    
    printf("\n SM1 Test Start\n");
    memset(&callSm1, 0, sizeof(callSm1));
    //key 128 192 256
    for (i = 0; i < 3; i++)
    {
        //pack mode CFB OFB
        for (j = 0; j < 2; j++)
        {
            //round num 8 10 12 14
            for(k = 0; k < 4; k++)
            {
                memset(au8Plain, 0, sizeof(au8Plain));
                memset(au8Cipher, 0, sizeof(au8Cipher));
                memset(au8Plain[1], 0, sizeof(au8Plain[1]));
                memset(au8Iv,0,sizeof(au8Iv));
                memset(KeyType[i].pu8Key,0,sizeof(au8Key));
                MHRAND_Prand(&u32MsgLen,sizeof(u32MsgLen));
                u32MsgLen = (u32MsgLen & 0x7f) + 1;
                MHRAND_Prand(au8Plain,u32MsgLen);
                MHRAND_Prand(au8Iv,sizeof(au8Iv));
                MHRAND_Prand(KeyType[i].pu8Key,KeyType[i].u8KLen);
                memcpy(au8IvTmp, au8Iv, sizeof(au8Iv));
                
                memset(&callSm1, 0, sizeof(callSm1));
                //SM1 enc
                callSm1.pu8In = au8Plain[0];
                callSm1.u32InLen = u32MsgLen;
                callSm1.pu8Out = au8Cipher;
                callSm1.u32OutLen = u32MsgLen;                
                callSm1.pu8IV = au8IvTmp;
                callSm1.pu8Key = KeyType[i].pu8Key;
                callSm1.u16Opt = ModeType[j].u32Val | MH_SM1_OPT_MODE_ENCRYPT | KeyType[i].u32Size | RoundNum[k].u32Round;
                callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);               
                if(MH_RET_SM1_SUCCESS != MHSM1_EncDecExtra(&callSm1))
                {
                    r_printf(0, "SM1 enc test\n");
                }

                //SM1 dec   
                memcpy(au8IvTmp, au8Iv, sizeof(au8Iv));           
                callSm1.pu8In = au8Cipher;
                callSm1.u32InLen = u32MsgLen;
                callSm1.pu8Out = au8Plain[1];
                callSm1.u32OutLen = u32MsgLen;
                callSm1.pu8IV = au8IvTmp;
                callSm1.pu8Key = KeyType[i].pu8Key;
                callSm1.u16Opt = ModeType[j].u32Val | MH_SM1_OPT_MODE_DECRYPT | KeyType[i].u32Size | RoundNum[k].u32Round;
                callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
                if(MH_RET_SM1_SUCCESS != MHSM1_EncDecExtra(&callSm1))
                {
                    r_printf(0, "SM1 dec test\n");
                }                    
                t = (!memcmp(au8Plain[0], au8Plain[1], u32MsgLen));
                printf(" %s %s %s SM1 Test ", ModeType[j].pu8Name, KeyType[i].pu8Name, RoundNum[k].pu8Name);
                r_printf(t, "\n");
            }
        }
        
    }
}

#endif


void SM1_Test(void)
{
	SM1_FixedTest();
	
#if MH_SM1_SUPPORT_OFB_CFB
    SM1_Extra_Func_Test();
#endif

}

