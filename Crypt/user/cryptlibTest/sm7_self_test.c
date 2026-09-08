#include "mh2435.h"
#include <stdio.h>
#include "mh_sm7.h"
#include "debug.h"
#include "mh_rand.h"


static void SM7_Func_Test(void)
{
    uint32_t t;
    uint8_t au8Cipher1[8] = {0xCE,0x3C,0x08,0xD4, 0x02,0xAE,0x24,0x7C};
    uint8_t  au8Key1[16] = {0x1F,0xD3,0x84,0xD8, 0x6B,0x50,0xBE,0x01, 0x21,0x43,0xD6,0x16, 0x18,0x15,0x19,0x83};
    uint8_t  au8Plain1[8] = {0xE2,0x73,0x2F,0xB8, 0x1D,0x7D,0x7E,0x51};
    
    uint8_t au8Cipher2[8] = {0x5F,0xE3,0x7C,0xA1, 0x77,0xA1,0xC5,0xCC};
    uint8_t au8Cipher3[8] = {0xE2,0x67,0x38,0x03, 0x61,0xFC,0x32,0x79};
    
    uint8_t  au8Key2[16] = {0x11,0x22,0x33,0x44, 0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00};
    uint8_t  au8Plain2[8] = {0x46,0x14,0x76,0xC9, 0x45,0x01,0x20,0x01};
    uint8_t  au8Iv[8] = {0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17};
    uint8_t au8mCipher[8] = {0};
    uint8_t au8mPlain[8] = {0};
    uint8_t  au8IvTmp[8] = {0};
    uint8_t  au8Tmp[8] = {0};
    uint8_t  au8Tmp1[8] = {0};
    uint8_t  au8IvOut[8] = {0};
    uint32_t u32Ret = 0;
    MH_SYM_CRYPT_CALL callSm7;
    
//ECB    
    memset((uint8_t *)&callSm7, 0, sizeof(callSm7));
    callSm7.pu8In = au8Plain1;
    callSm7.pu8IV = au8Iv;
    callSm7.pu8Key = au8Key1;
    callSm7.pu8Out = au8mCipher;
    callSm7.u32InLen = sizeof(au8Plain1);
    callSm7.u32OutLen = sizeof(au8mCipher);
    callSm7.u16Opt = MH_SM7_OPT_BLK_ECB | MH_SM7_OPT_MODE_ENCRYPT;
    callSm7.u32Crc = MHCRC_CalcBuff(0xffff, &callSm7, sizeof(MH_SYM_CRYPT_CALL)-4);      
    MHSM7_EncDec(&callSm7);
    t = (!memcmp(au8Cipher1, au8mCipher, sizeof(au8Cipher1)));
    r_printf(t, "SM7 ECB ENC Test\n");   
    
    
    memset((uint8_t *)&callSm7, 0, sizeof(callSm7));
    callSm7.pu8In = au8mCipher;
    callSm7.pu8IV = au8Iv;
    callSm7.pu8Key = au8Key1;
    callSm7.pu8Out = au8mPlain;
    callSm7.u32InLen = sizeof(au8mCipher);
    callSm7.u32OutLen = sizeof(au8mPlain);
    callSm7.u16Opt = MH_SM7_OPT_BLK_ECB | MH_SM7_OPT_MODE_DECRYPT;
    callSm7.u32Crc = MHCRC_CalcBuff(0xffff, &callSm7, sizeof(MH_SYM_CRYPT_CALL)-4);    
    MHSM7_EncDec(&callSm7);    
    t = (!memcmp(au8Plain1, au8mPlain, sizeof(au8Plain1)));
    r_printf(t, "SM7 ECB ENC/DEC Test\n");   
    
//CBC  
    memset((uint8_t *)&callSm7, 0, sizeof(callSm7));
    memcpy(au8IvTmp,au8Iv,sizeof(au8Iv));
    callSm7.pu8In = au8Plain2;
    callSm7.pu8IV = au8IvTmp;
    callSm7.pu8Key = au8Key2;
    callSm7.pu8Out = au8mCipher;
    callSm7.u32InLen = sizeof(au8Plain2);
    callSm7.u32OutLen = sizeof(au8mCipher);
    callSm7.pu8IVout = au8IvOut;
    callSm7.u16Opt = MH_SM7_OPT_BLK_CBC | MH_SM7_OPT_MODE_ENCRYPT;
    callSm7.u32Crc = MHCRC_CalcBuff(0xffff, &callSm7, sizeof(MH_SYM_CRYPT_CALL)-4);       
    MHSM7_EncDec(&callSm7);
    t = (!memcmp(au8Cipher3, au8mCipher, sizeof(au8mCipher)));
    r_printf(t, "SM7 CBC ENC Test\n"); 
    
    memset((uint8_t *)&callSm7, 0, sizeof(callSm7));
    memcpy(au8IvTmp,au8Iv,sizeof(au8Iv));
    callSm7.pu8In = au8mCipher;
    callSm7.pu8IV = au8IvTmp;
    callSm7.pu8Key = au8Key2;
    callSm7.pu8Out = au8mPlain;
    callSm7.u32InLen = sizeof(au8mCipher);
    callSm7.u32OutLen = sizeof(au8mPlain);
    callSm7.pu8IVout = au8IvOut;
    callSm7.u16Opt = MH_SM7_OPT_BLK_CBC | MH_SM7_OPT_MODE_DECRYPT;
    callSm7.u32Crc = MHCRC_CalcBuff(0xffff, &callSm7, sizeof(MH_SYM_CRYPT_CALL)-4);    
    u32Ret = MHSM7_EncDec(&callSm7);  
    
    t = (!memcmp(au8Plain2, au8mPlain, sizeof(au8Plain2)));
    r_printf(t, "SM7 CBC ENC/DEC Test\n"); 
    
}

void SM7_Test(void)
{
    SM7_Func_Test();
}
