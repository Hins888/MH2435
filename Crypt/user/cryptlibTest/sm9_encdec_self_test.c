#include "stdio.h"
#include "stdint.h"
#include "mh_sm9.h"
#include "mh_bignum_tool.h"
#include "mh_bignum.h"
#include "debug.h"
#include "mh2435.h"
#include "mh_misc.h"
#include "mh_rand.h"
#include "mh_sm9_encryptdecrypt.h"
#include "mh_rand.h"

#define MH_SM9_KE           "0001EDEE3778F441F8DEA3D9FA0ACC4E07EE36C93F9A08618AF4AD85CEDE1C22"
//#define MH_SM9_KE         "A3F456C5A3D62348E2BC771327C4C734318928097B37FAAC11B97B0E6F6A82BD"
#define  MH_SM9_PPUB        "787ED7B8A51F3AB84E0A66003F32DA5C720B17ECA7137D39ABC66E3C80A892FF769DE61791E5ADC4B9FF85A31354900B202871279A8C49DC3F220F644C57A7B1"
#define  MH_SM9_DE	        "94736ACD2C8C8796CC4785E938301A139A059D3537B6414140B2D31EECF41683115BAE85F5D8BC6C3DBD9E5342979ACCCF3C2F4F28420B1CB4F8C0B59A19B158\
7AA5E47570DA7600CD760A0CF7BEAF71C447F3844753FE74FA7BA92CA7D3B55F27538A62E7F7BFB51DCE08704796D94C9D56734F119EA44732B50E31CDEB75C1"
#define  MH_SM9_ENC_MSG     "4368696E65736520494245207374616E64617264"
#define  MH_SM9_RAND        "0000AAC0541779C8FC45E3E2CB25C12B5D2576B2129AE8BB5EE2CBE5EC9E785C"

#define  MH_SM9_SM4_CIPHER   "B0348D0F6E35DF63FC7C19644DD5D5FBA21E5B99C916B7DEF56823535EE81D8C9DA6380A6A0D4EE4BF9E428EF9C7D000CE1924A2DFC4A4C73B09EF4C6DF96624\
774CBBD0C9D1D0BAA880061B0DCB6743A619E5D1D811E1B80EFB8F1C80165D3E0D68713296167DB132957A5F2367DF9F9A0A3D46171A367CCD383FC1B0011303"

#define  MH_SM9_XOR_CIPHER   "2445471164490618E1EE20528FF1D545B0F14C8BCAA44544F03DAB5DAC07D8FF42FFCA97D57CDDC05EA405F2E586FEB3A6930715532B8000759F13059ED59AC0\
BA672387BCD6DE5016A158A52BB2E7FC429197BCAB70B25AFEE37A2B9DB9F3671B5F5B0E951489682F3E64E1378CDD5DA9513B1C"


#define MH_SM9_IDB           "426F62"


#define MH_SM9_MSG_LEN  (128)
#define USE_SM4     (1)

//SM9 encrypt Test
static void SM9_Enc_Test()
{
    uint8_t au8Pri[32],au8Ppub[64],au8Msg[32],au8Id[32],au8De[128],au8Out[MH_SM9_MSG_LEN + 96 + 16], u8Hid, u8Base = 0;
    uint16_t u16Mlen, u16IDlen;
    MH_SM9_INNER_SPACE  InnerCall;
    MHBN_BIGNUM BigID = {0},BigMsg = {0},BigOut = {0};
    MH_SM9_ENC_SPACE EncSpace;
    MH_SM9_ENC_DEC_CALL    EncCall;
    
    memset(&InnerCall, 0, sizeof(InnerCall));
    memset(&EncSpace, 0, sizeof(EncSpace));
    memset(&EncCall, 0, sizeof(EncCall));
    //set param
    MHSM9_SetParam(&u8Base, &InnerCall);
    //Gen pubkey
    bn_read_string_from_head(au8Pri, 32, MH_SM9_KE);
    MHSM9_GenEncPubKey(au8Ppub, au8Pri, &InnerCall);
    ouputRes("au8Pri.\n", au8Pri, sizeof(au8Pri));
    ouputRes("au8Pub.\n", au8Ppub, sizeof(au8Ppub));
    //set sign
    MHSM9_SetEnc(&EncSpace, au8Ppub, &InnerCall);
    ouputRes("EncSpace.\n", &EncSpace, sizeof(EncSpace));
    //GenSignSecKey
    u16IDlen = bn_read_string_from_head(au8Id, sizeof(au8Id), MH_SM9_IDB);
    BigID.pu8Data = au8Id;
    BigID.u32Len = u16IDlen;

    MHSM9_GenEncSecKey(au8De, au8Pri, &BigID, &InnerCall); // no need in Enc
    ouputRes("au8De.\n", au8De, sizeof(au8De));
    //do enc
    u16Mlen = bn_read_string_from_head(au8Msg, sizeof(au8Msg), MH_SM9_ENC_MSG);
    BigMsg.pu8Data = au8Msg;
    BigMsg.u32Len = u16Mlen;
    BigOut.pu8Data = au8Out;
    EncCall.pIDb = &BigID;
    EncCall.pInput = &BigMsg;
    EncCall.pOutput = &BigOut;
    EncCall.pInBase = &InnerCall;
    EncCall.pSpace = &EncSpace;
#if USE_SM4
    EncCall.u32KdfType = SM9_ENC_KDF_SM4;
#else
    EncCall.u32KdfType = SM9_ENC_KDF_XOR;
#endif
    MHSM9_Encrypt(&EncCall);
    ouputRes("Output.\n", au8Out, EncCall.pOutput->u32Len);
}

static void SM9_Dec_Test()
{
    uint8_t au8Pri[32],au8Ppub[64],au8Cipher[MH_SM9_MSG_LEN + 96 + 16],au8Id[32],au8De[128],au8Msg[MH_SM9_MSG_LEN], u8Hid, u8Base = 0;
    uint16_t u16Mlen, u16IDlen;
    MH_SM9_INNER_SPACE  InnerCall;
    MHBN_BIGNUM BigID = {0},BigMsg = {0},BigOut = {0};
    MH_SM9_ENC_DEC_CALL    DecCall;
    
    memset(&InnerCall, 0, sizeof(InnerCall));
    memset(&DecCall, 0, sizeof(DecCall));
    //set param
    MHSM9_SetParam(&u8Base, &InnerCall);
    //Gen pubkey
    bn_read_string_from_head(au8Pri, 32, MH_SM9_KE);
    MHSM9_GenEncPubKey(au8Ppub, au8Pri, &InnerCall);
//    ouputRes("au8Pri.\n", au8Pri, sizeof(au8Pri));
//    ouputRes("au8Pub.\n", au8Ppub, sizeof(au8Ppub));
    
    //GenSignSecKey
    u16IDlen = bn_read_string_from_head(au8Id, sizeof(au8Id), MH_SM9_IDB);
    BigID.pu8Data = au8Id;
    BigID.u32Len = u16IDlen;
    MHSM9_GenEncSecKey(au8De, au8Pri, &BigID, &InnerCall);
//    ouputRes("au8De.\n", au8De, sizeof(au8De));
#if USE_SM4
    u16Mlen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), MH_SM9_SM4_CIPHER);
#else
    u16Mlen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), MH_SM9_XOR_CIPHER);
#endif
    BigMsg.pu8Data = au8Cipher;
    BigMsg.u32Len = u16Mlen;
    BigOut.pu8Data = au8Msg;
    DecCall.pIDb = &BigID;
    DecCall.pInput = &BigMsg;
    DecCall.pOutput = &BigOut;
    DecCall.pInBase = &InnerCall;
    DecCall.pu8De = au8De;
#if USE_SM4
    DecCall.u32KdfType = SM9_ENC_KDF_SM4;
#else
    EncCall.u32KdfType = SM9_ENC_KDF_XOR;
#endif
    
    DecCall.u32Crc = MHCRC_CalcBuff(0xffff, &DecCall, sizeof(MH_SM9_ENC_DEC_CALL)-4);
    if(MH_RET_SM9_SUCCESS != MHSM9_Decrypt(&DecCall))
    {
        r_printf(0, "sm9 dec test\n");
    }
    else
    {
        r_printf(1, "sm9 dec test\n");
//        ouputRes("Output.\n", au8Msg, DecCall.pOutput->u32Len);
    }
}

static void SM9_EncDecFunc_Test()
{
    uint8_t au8Pri[32],au8Ppub[64],au8Cipher[MH_SM9_MSG_LEN + 96 + 16],au8Id[32],au8De[128],au8Plain[MH_SM9_MSG_LEN],\
    au8Plain1[MH_SM9_MSG_LEN], u8Hid, u8Base = 0;
    uint16_t u16Mlen, u16IDlen;
    MH_SM9_INNER_SPACE  InnerCall;
    MHBN_BIGNUM BigID,BigMsg,BigOut;
    MH_SM9_ENC_SPACE EncSpace;
    MH_SM9_ENC_DEC_CALL   EncCall, DecCall;
    
    memset(&InnerCall, 0, sizeof(InnerCall));
    memset(&EncSpace, 0, sizeof(EncSpace));
    memset(&DecCall, 0, sizeof(DecCall));
    memset(&EncCall, 0, sizeof(EncCall));
    //set param
    MHSM9_SetParam(&u8Base, &InnerCall);
    //Gen pubkey
//    bn_read_string_from_head(au8Pri, 32, MH_SM9_KE);
    MHRAND_Prand(au8Pri, 32);
    au8Pri[0] &= 0x07;
    MHSM9_GenEncPubKey(au8Ppub, au8Pri, &InnerCall);
//    ouputRes("au8Pri.\n", au8Pri, sizeof(au8Pri));
//    ouputRes("au8Pub.\n", au8Ppub, sizeof(au8Ppub));
    //set sign
    MHSM9_SetEnc(&EncSpace, au8Ppub, &InnerCall);
//    ouputRes("EncSpace.\n", &EncSpace, sizeof(EncSpace));
    //GenSignSecKey
    u16IDlen = bn_read_string_from_head(au8Id, sizeof(au8Id), MH_SM9_IDB);
    BigID.pu8Data = au8Id;
    BigID.u32Len = u16IDlen;
    MHSM9_GenEncSecKey(au8De, au8Pri, &BigID, &InnerCall);
    MHRAND_Prand(&u16Mlen,2);
    u16Mlen &= 0x7f;
    MHRAND_Prand(au8Plain, u16Mlen);
    //do Enc
    BigMsg.pu8Data = au8Plain;
    BigMsg.u32Len = u16Mlen;
    BigOut.pu8Data = au8Cipher;
    EncCall.pIDb = &BigID;
    EncCall.pInput = &BigMsg;
    EncCall.pOutput = &BigOut;
    EncCall.pInBase = &InnerCall;
    EncCall.pSpace = &EncSpace;
#if USE_SM4
    EncCall.u32KdfType = SM9_ENC_KDF_SM4;
#else
    EncCall.u32KdfType = SM9_ENC_KDF_XOR;
#endif  
    EncCall.u32Crc = MHCRC_CalcBuff(0xffff, &EncCall, sizeof(MH_SM9_ENC_DEC_CALL)-4);
    MHSM9_Encrypt(&EncCall);
//    ouputRes("Output.\n", au8Cipher, EncCall.pOutput->u32Len);
    //do Dec
    BigMsg.pu8Data = au8Cipher;
    BigMsg.u32Len = EncCall.pOutput->u32Len;
    BigOut.pu8Data = au8Plain1;
    DecCall.pIDb = &BigID;
    DecCall.pInput = &BigMsg;
    DecCall.pOutput = &BigOut;
    DecCall.pInBase = &InnerCall;
    DecCall.pSpace = &EncSpace;
    DecCall.pu8De = au8De;
#if USE_SM4
    DecCall.u32KdfType = SM9_ENC_KDF_SM4;
#else
    DecCall.u32KdfType = SM9_ENC_KDF_XOR;
#endif
    DecCall.u32Crc = MHCRC_CalcBuff(0xffff, &DecCall, sizeof(MH_SM9_ENC_DEC_CALL)-4);
    if(MH_RET_SM9_SUCCESS != MHSM9_Decrypt(&DecCall))
    {
        r_printf(0, "sm9 dec test\n");
    }
    else
    {
        r_printf((0 == memcmp(au8Plain, au8Plain1, BigOut.u32Len)),  "sm9 enc/dec test\n");
    }
}

void SM9_EncDecTest()
{
    SM9_Enc_Test();
    SM9_Dec_Test();
    SM9_EncDecFunc_Test();
}
