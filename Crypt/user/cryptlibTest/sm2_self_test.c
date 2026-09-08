#include <string.h>
#include <stdio.h>
#include "mh2435.h"
#include "mh_sm2.h"
#include "mh_bignum_tool.h"
#include "debug.h"


#define KEY_WORDS           (256 / 32)
#define SM2_MSG_LEN_MAX     (1024)
#define ID_LEN_MAX          (128)
#define HASH_LEN            (32)

#define cp      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF"
#define ca      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC"
#define cb      "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93"
#define cGx     "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7"
#define cGy     "BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0"
#define cn      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123"

           
#define cex     "1CDFD6D0375D84164EA5CBCD929874F19384743E20407EF1CCFDC07D960643F7"             
#define cey     "F0053CCD7FE91230A12003A084964928DE12DC97E6F51F2B46C43935DB9F7857"         
#define ckeyd	"C46841E769D7F6A7599046C029900CB6D47BBD54BE83AF50284584BC1E390BF9"

#define cIDa    "414C4943453132333435363738394142434445464748405941484F4F2E434F4D"

#define cm      "115960C5598A2797C2D02D76D468CDC9"
#define rs      "81F83524B61A651C62E7C9F6FE9C8C8EA2D9CBFDB3A1E6672C26DA6502AE4DA216148EA9152D558DC51529AB7A1ECE0F730EF3335104CFD2097851C4F377328E"

#define ccipher         "0470342F90FA3213647483B18B6974187F9D16F73CBFBECCF88CF57388C0C663E5CD73A4460987F928ABCD184C0196B55B97E535A45E841BAA62C73B7B81CF31F464B7FCC88FFC327247A49B36872131A7A4EEE1AF44C46E32977A14BE98C2606BAFFA7A0BA0391A64BC549920357939E8"
#define std_ccipher     "70342F90FA3213647483B18B6974187F9D16F73CBFBECCF88CF57388C0C663E5CD73A4460987F928ABCD184C0196B55B97E535A45E841BAA62C73B7B81CF31F4A4EEE1AF44C46E32977A14BE98C2606BAFFA7A0BA0391A64BC549920357939E864B7FCC88FFC327247A49B36872131A7"

static uint32_t MHSM2_EncStd(MH_SM2_ENC_DEC_CALL *pCall)
{
    uint32_t u32Ret;
    uint8_t au8C2Tmp[SM2_MSG_LEN_MAX];
    u32Ret = MHSM2_Enc(pCall);
    if (MH_RET_SM2_SUCCESS != u32Ret)
    {
        return u32Ret;
    }
    memcpy(au8C2Tmp, pCall->pu8Output + 64, pCall->u32InLen);    
    memcpy(pCall->pu8Output + 64, pCall->pu8Output + 64 + pCall->u32InLen, 32);
    memcpy(pCall->pu8Output + 64 + 32, au8C2Tmp, pCall->u32InLen);
    return u32Ret;
}
static uint32_t MHSM2_DecStd(MH_SM2_ENC_DEC_CALL *pCall)
{
    uint32_t u32Ret, u32Mlen;
    uint8_t au8C3Tmp[32];
    u32Mlen = pCall->u32InLen - 64 - 32;
    memcpy(au8C3Tmp, pCall->pu8Input + 64, sizeof(au8C3Tmp));
    memcpy(pCall->pu8Input + 64, pCall->pu8Input + 32 + 64, u32Mlen);
    memcpy((uint8_t *)(pCall->pu8Input + 64 + u32Mlen), au8C3Tmp, sizeof(au8C3Tmp));
    u32Ret = MHSM2_Dec(pCall);
    memcpy(pCall->pu8Input + 32 + 64, pCall->pu8Input + 64, pCall->u32OutLen);
    memcpy(pCall->pu8Input + 64, au8C3Tmp, sizeof(au8C3Tmp));
    return u32Ret;
}

static void SM2_EncDecFuncTest(void)
{
    
    uint8_t test_cplain[40];
    uint8_t test_ccipher[40 + 64 + 32];
    uint8_t message[40];
    uint8_t cipher[40 + 64 + 32];
    uint8_t au8Sm2KeyD[MH_SM2_KEY_BYTES];
    uint8_t au8Sm2KeyPx[MH_SM2_KEY_BYTES];
    uint8_t au8Sm2KeyPy[MH_SM2_KEY_BYTES];
    uint32_t mlen;
    MH_SM2_KEY Sm2Key;
    MH_SM2_ENC_DEC_CALL sm2Call;
    MH_SM2_INNER_BASE sm2InBase;
    uint8_t u8Base = 0;
    Sm2Key.pu8D = au8Sm2KeyD;
    Sm2Key.pu8PubX = au8Sm2KeyPx;
    Sm2Key.pu8PubY = au8Sm2KeyPy;
    
    bn_read_string_to_bytes(Sm2Key.pu8D, MH_SM2_KEY_BYTES, ckeyd);
    bn_read_string_to_bytes(Sm2Key.pu8PubX, MH_SM2_KEY_BYTES, cex);
    bn_read_string_to_bytes(Sm2Key.pu8PubY, MH_SM2_KEY_BYTES, cey);
    bn_read_string_from_head(cipher, sizeof(cipher), std_ccipher);
    mlen = bn_read_string_from_head(message, sizeof(message), cm);
    memset(test_ccipher, 0, sizeof(test_ccipher));
    
    printf("\nSM2 crypt Test In\n");
    
    memset(&sm2InBase, 0, sizeof(sm2InBase));
    Sm2Key.u32Crc = MHCRC_CalcBuff(0xffff, &Sm2Key, sizeof(MH_SM2_KEY)-4); 
    MHSM2_SetKey(&Sm2Key, &sm2InBase, &u8Base);
    
    memset(test_cplain, 0, sizeof(test_cplain));
    sm2Call.pInBase = &sm2InBase;
    sm2Call.pu8Input = cipher;
    sm2Call.pu8Output = test_cplain;
    sm2Call.u32InLen = mlen + 64 + 32;
    sm2Call.u32OutLen = 0;
    sm2Call.u32Crc = MHCRC_CalcBuff(0xffff, &sm2Call, sizeof(MH_SM2_ENC_DEC_CALL)-4);
    MHSM2_DecStd(&sm2Call);
    r_printf((0 == memcmp(test_cplain, message,  sm2Call.u32OutLen)), "mh_sm2_dec test\n");
    
    sm2Call.pInBase = &sm2InBase;
    sm2Call.pu8Input = message;
    sm2Call.pu8Output = test_ccipher;
    sm2Call.u32InLen = mlen;
    sm2Call.u32OutLen = 0;
    sm2Call.u32Crc = MHCRC_CalcBuff(0xffff, &sm2Call, sizeof(MH_SM2_ENC_DEC_CALL)-4);
    MHSM2_EncStd(&sm2Call);
//    ouputRes("test_ccipher.\n", test_ccipher, mlen + 64 + 32);
    memset(test_cplain, 0, sizeof(test_cplain));
    sm2Call.pu8Input = test_ccipher;
    sm2Call.pu8Output = test_cplain;
    sm2Call.u32InLen = mlen + 64 + 32;
    sm2Call.u32OutLen = 0;
    sm2Call.u32Crc = MHCRC_CalcBuff(0xffff, &sm2Call, sizeof(MH_SM2_ENC_DEC_CALL)-4);
    MHSM2_DecStd(&sm2Call);
    r_printf((0 == memcmp(test_cplain, message,  sm2Call.u32OutLen)), "mh_sm2_enc/dec test\n");
}

static void SM2_SignVeriFuncTest(void)
{
    uint16_t u32MsgLen, u32IDlen;
    uint8_t au8E[HASH_LEN];
    uint8_t au8IDa[ID_LEN_MAX];
    uint8_t au8Msg[MH_SM2_KEY_BYTES];
    uint8_t au8Pri[MH_SM2_KEY_BYTES];
    uint8_t au8PubX[MH_SM2_KEY_BYTES];
    uint8_t au8PubY[MH_SM2_KEY_BYTES];
    uint8_t au8Sign[MH_SM2_KEY_BYTES<<1];
    MH_SM2_KEY Sm2Key;
    MH_SM2_INNER_BASE sm2InBase;
    MH_SM2_SIG_VER_CALL Sm2Call;
    uint8_t u8Base = 0;

    Sm2Key.pu8D = au8Pri;
    Sm2Key.pu8PubX = au8PubX;
    Sm2Key.pu8PubY = au8PubY;
    
    bn_read_string_from_head(Sm2Key.pu8D, MH_SM2_KEY_BYTES, ckeyd);
    bn_read_string_from_head(Sm2Key.pu8PubX, MH_SM2_KEY_BYTES, cex);
    bn_read_string_from_head(Sm2Key.pu8PubY, MH_SM2_KEY_BYTES, cey);
    u32IDlen = bn_read_string_from_head(au8IDa, MH_SM2_KEY_BYTES, cIDa);
    u32MsgLen = bn_read_string_from_head(au8Msg, MH_SM2_KEY_BYTES, cm);
    bn_read_string_from_head(au8Sign, MH_SM2_KEY_BYTES<<1, rs);
    
    u8Base = 0;
    memset(&sm2InBase, 0, sizeof(sm2InBase));
    Sm2Key.u32Crc = MHCRC_CalcBuff(0xffff, &Sm2Key, sizeof(MH_SM2_KEY)-4); 
    MHSM2_SetKey(&Sm2Key, &sm2InBase, &u8Base);
    
    Sm2Call.pInBase = &sm2InBase;
    Sm2Call.pSm2Key = &Sm2Key;
    Sm2Call.pu8IDa = au8IDa;
    Sm2Call.u32IDaLen = u32IDlen;
    Sm2Call.pu8Msg = au8Msg;
    Sm2Call.u32MLen = u32MsgLen;
    Sm2Call.pu8E = au8E;
    Sm2Call.pu8Sign = au8Sign; 
    MHSM2_HashE(&Sm2Call);
//    ouputRes("au8E.\n", au8E, MH_SM2_KEY_BYTES);
    Sm2Call.u32Crc = MHCRC_CalcBuff(0xffff, &Sm2Call, sizeof(MH_SM2_SIG_VER_CALL)-4);  
    r_printf((MH_RET_SM2_SUCCESS == MHSM2_VeriE(&Sm2Call)), "mh_sm2 verify test\n");
    
    Sm2Call.pInBase = &sm2InBase;
    Sm2Call.pSm2Key = &Sm2Key;
    Sm2Call.pu8IDa = au8IDa;
    Sm2Call.u32IDaLen = u32IDlen;
    Sm2Call.pu8Msg = au8Msg;
    Sm2Call.u32MLen = u32MsgLen;
    Sm2Call.pu8E = au8E;
    Sm2Call.pu8Sign = au8Sign; 
    MHSM2_HashE(&Sm2Call);
    Sm2Call.u32Crc = MHCRC_CalcBuff(0xffff, &Sm2Call, sizeof(MH_SM2_SIG_VER_CALL)-4);  
    if(MH_RET_SM2_SUCCESS != MHSM2_SignE(&Sm2Call))
    {
        r_printf(0, "mh_sm2 sign test\n");
    }
//    ouputRes("au8E.\n", au8E, MH_SM2_KEY_BYTES);
//    ouputRes("au8Sign.\n", au8Sign, MH_SM2_KEY_BYTES<<1);
    Sm2Call.u32Crc = MHCRC_CalcBuff(0xffff, &Sm2Call, sizeof(MH_SM2_SIG_VER_CALL)-4);  
    r_printf((MH_RET_SM2_SUCCESS == MHSM2_VeriE(&Sm2Call)), "mh_sm2 sign/verify test\n");
}

static void SM2GenKey_Test()
{
    uint32_t u32Ret = 0;
    uint8_t au8Pri[MH_SM2_KEY_BYTES];
    uint8_t au8PubX[MH_SM2_KEY_BYTES];
    uint8_t au8PubY[MH_SM2_KEY_BYTES];
    
    uint8_t au8PubXx[MH_SM2_KEY_BYTES];
    uint8_t au8PubYy[MH_SM2_KEY_BYTES];
    
    MH_SM2_KEY Sm2Key;
    Sm2Key.pu8D = au8Pri;
    Sm2Key.pu8PubX = au8PubX;
    Sm2Key.pu8PubY = au8PubY;
    Sm2Key.u32Crc = MHCRC_CalcBuff(0xffff, &Sm2Key, sizeof(MH_SM2_KEY)-4); 
    u32Ret = MHSM2_GenKey(&Sm2Key);
    r_printf((MH_RET_SM2_SUCCESS == u32Ret), "mh_sm2 gen key test\n");
    
//    ouputRes("au8Pri.\n", au8Pri, MH_SM2_KEY_BYTES);
//    ouputRes("au8PubX.\n", au8PubX, MH_SM2_KEY_BYTES);
//    ouputRes("au8PubY.\n", au8PubY, MH_SM2_KEY_BYTES);
    
    memcpy(au8PubXx, au8PubX, MH_SM2_KEY_BYTES);
    memcpy(au8PubYy, au8PubY, MH_SM2_KEY_BYTES);
    
    memset(au8PubX, 0, sizeof(au8PubX));
    memset(au8PubY, 0, sizeof(au8PubY)); 
    Sm2Key.u32Crc = MHCRC_CalcBuff(0xffff, &Sm2Key, sizeof(MH_SM2_KEY)-4); 
    u32Ret = MHSM2_CompleteKey(&Sm2Key);
    r_printf((MH_RET_SM2_SUCCESS == u32Ret), "mh_sm2 complete key test\n");
    
    r_printf((!(memcmp(au8PubX, au8PubXx, MH_SM2_KEY_BYTES))&&!(memcmp(au8PubY, au8PubYy, MH_SM2_KEY_BYTES))), "SM2GenKey_Test\n");
}

void SM2_Test()
{
    SM2GenKey_Test();
    SM2_EncDecFuncTest();
    SM2_SignVeriFuncTest();
}
