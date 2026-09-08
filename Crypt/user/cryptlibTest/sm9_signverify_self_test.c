#include "stdio.h"
#include "stdint.h"
#include "mh_sm9.h"
#include "mh_bignum_tool.h"
#include "mh_bignum.h"
#include "debug.h"
#include "mh2435.h"
#include "mh_misc.h"
#include "mh_sm9_signverify.h"
#include "mh_rand.h"

#define MH_SM9_KS               "000130E78459D78545CB54C587E02CF480CE0B66340F319F348A1D5B1F2DC5F4"
#define  MH_SM9_PPUB            "9F64080B3084F733E48AFF4B41B565011CE0711C5E392CFB0AB1B6791B94C40829DBA116152D1F786CE843ED24A3B573414D2177386A92DD8F14D65696EA5E32\
69850938ABEA0112B57329F447E3A0CBAD3E2FDB1A77F335E89E1408D0EF1C2541E00A53DDA532DA1A7CE027B7A46F741006E85F5CDFF0730E75C05FB4E3216D"
#define  MH_SM9_KEY             "A5702F05CF1315305E2D6EB64B0DEB923DB1A0BCF0CAFF90523AC8754AA6982078559A844411F9825C109F5EE3F52D720DD01785392A727BB1556952B2B013D3"
#define  MH_SM9_SIGN_MSG        "4368696E65736520494253207374616E64617264"
#define MH_SM9_SIGN_H           "84480EE1C5B2E7CF6535DC70AAF9984B530D0FA1595B7917BB37D108F2040A80"
#define MH_SM9_SIGN_S           "AF70FE8528AE9132C2E9130595F5CBFEB8A72A5880D78112EC610C9F0E0A01A4A52CB5582FCE5B9D93DA18C0A438DA07388B18CA8E9AA5956F4E76917901F6D7"
#define MH_SM9_IDA              "416C696365"

static void SM9_Sign_Test()
{
    uint8_t au8Pri[32],au8Pub[128],au8Msg[32],au8KeyD[64], au8Id[8];
    uint16_t u16Mlen, u16IdLen;
    uint8_t u8Hid, u8Base = 0;
    MHBN_BIGNUM BigID, BigMsg;
    MH_SM9_INNER_SPACE  InnerCall;
    MH_SM9_SIGN_SPACE SignSpace;
    MH_SM9_SIGN  Sm9Sign;
    MH_SM9_SIGN_CALL    SignCall;
   
    memset(&InnerCall, 0, sizeof(InnerCall));
    memset(&SignSpace, 0, sizeof(SignSpace));
    memset(&SignCall, 0, sizeof(SignCall));
    //set param
    MHSM9_SetParam(&u8Base, &InnerCall);
    //Gen pubkey
    bn_read_string_from_head(au8Pri, 32, MH_SM9_KS);
    MHSM9_GenSignPubKey(au8Pub, au8Pri, &InnerCall);
    ouputRes("au8Pri.\n", au8Pri, sizeof(au8Pri));
    ouputRes("au8Pub.\n", au8Pub, sizeof(au8Pub));
    //set sign
    MHSM9_SetSign(&SignSpace, au8Pub, &InnerCall);
    ouputRes("SignSpace.\n", &SignSpace, sizeof(SignSpace));
    //GenSignSecKey
    u16IdLen = bn_read_string_from_head(au8Id, sizeof(au8Id), MH_SM9_IDA);
    BigID.pu8Data = au8Id;
    BigID.u32Len = u16IdLen;
    MHSM9_GenSignSecKey(au8KeyD, au8Pri, &BigID, &InnerCall);
    ouputRes("au8KeyD.\n", au8KeyD, sizeof(au8KeyD));
    //do sign
    u16Mlen = bn_read_string_from_head(au8Msg, sizeof(au8Msg), MH_SM9_SIGN_MSG);
    BigMsg.pu8Data = au8Msg;
    BigMsg.u32Len = u16Mlen;
    SignCall.pIDa = &BigID;
    SignCall.pMsg = &BigMsg;
    SignCall.pSpace = &SignSpace;
    SignCall.pInBase = &InnerCall;
    SignCall.pSign = &Sm9Sign;
    SignCall.pu8KeyD = au8KeyD;
    MHSM9_Sign(&SignCall);
    ouputRes("H.\n", Sm9Sign.au8H, 32);
    ouputRes("S.\n", Sm9Sign.au8S, 64);
}

static void SM9_Verify_Test()
{
    uint8_t au8Pri[32],au8Pub[128],au8Msg[32],au8KeyD[64], au8Id[8];
    uint16_t u16Mlen, u16IdLen;
    uint8_t u8Base = 0;
    MHBN_BIGNUM BigID, BigMsg;
    MH_SM9_INNER_SPACE  InnerCall;
    MH_SM9_SIGN_SPACE SignSpace;
    MH_SM9_SIGN  Sm9Sign;
    MH_SM9_SIGN_CALL    SignCall;
   
    memset(&InnerCall, 0, sizeof(InnerCall));
    memset(&SignSpace, 0, sizeof(SignSpace));
    memset(&SignCall, 0, sizeof(SignCall));
    //set param
    MHSM9_SetParam(&u8Base, &InnerCall);
    //Gen pubkey
    bn_read_string_from_head(au8Pri, 32, MH_SM9_KS);
    MHSM9_GenSignPubKey(au8Pub, au8Pri, &InnerCall);
    ouputRes("au8Pri.\n", au8Pri, sizeof(au8Pri));
    ouputRes("au8Pub.\n", au8Pub, sizeof(au8Pub));
    //set sign
    MHSM9_SetSign(&SignSpace, au8Pub, &InnerCall);
    ouputRes("SignSpace.\n", &SignSpace, sizeof(SignSpace));
    //GenSignSecKey
    u16IdLen = bn_read_string_from_head(au8Id, sizeof(au8Id), MH_SM9_IDA);
    BigID.pu8Data = au8Id;
    BigID.u32Len = u16IdLen;
    MHSM9_GenSignSecKey(au8KeyD, au8Pri, &BigID, &InnerCall);
    ouputRes("au8KeyD.\n", au8KeyD, sizeof(au8KeyD));
    //do sign
    u16Mlen = bn_read_string_from_head(au8Msg, sizeof(au8Msg), MH_SM9_SIGN_MSG);
    bn_read_string_from_head(Sm9Sign.au8H, sizeof(Sm9Sign.au8H), MH_SM9_SIGN_H);
    bn_read_string_from_head(Sm9Sign.au8S, sizeof(Sm9Sign.au8S), MH_SM9_SIGN_S);
    BigMsg.pu8Data = au8Msg;
    BigMsg.u32Len = u16Mlen;
    SignCall.pIDa = &BigID;
    SignCall.pMsg = &BigMsg;
    SignCall.pSpace = &SignSpace;
    SignCall.pInBase = &InnerCall;
    SignCall.pSign = &Sm9Sign;
    SignCall.pu8KeyD = au8KeyD;
    SignCall.u32Crc = MHCRC_CalcBuff(0xffff, &SignCall, sizeof(MH_SM9_SIGN_CALL)-4);
    r_printf((MH_RET_SM9_SUCCESS==MHSM9_Verify(&SignCall)), "mh sm9 verify test\n");
//    MHSM9_Sign(&SignCall);
//    ouputRes("H.\n", Sm9Sign.au8H, 32);
//    ouputRes("S.\n", Sm9Sign.au8S, 64);
}

static void SM9_SignVeriFunc_Test()
{
    uint8_t au8Pri[32],au8Pub[128],au8Msg[32],au8KeyD[64], au8Id[8];
    uint16_t u16Mlen, u16IdLen;
    uint8_t u8Hid, u8Base = 0;
    MHBN_BIGNUM BigID, BigMsg;
    MH_SM9_INNER_SPACE  InnerCall;
    MH_SM9_SIGN_SPACE SignSpace;
    MH_SM9_SIGN  Sm9Sign;
    MH_SM9_SIGN_CALL    SignCall;

    memset(&InnerCall, 0, sizeof(InnerCall));
    memset(&SignSpace, 0, sizeof(SignSpace));
    memset(&SignCall, 0, sizeof(SignCall));
    
    //set param
    MHSM9_SetParam(&u8Base, &InnerCall);
    //Gen pubkey
    bn_read_string_from_head(au8Pri, 32, MH_SM9_KS);
//    MHRAND_Prand(au8Pri, 32);
//    au8Pri[0] &= 0x07;
    MHSM9_GenSignPubKey(au8Pub, au8Pri, &InnerCall);
//    ouputRes("au8Pri.\n", au8Pri, sizeof(au8Pri));
//    ouputRes("au8Pub.\n", au8Pub, sizeof(au8Pub));
    //set sign
    MHSM9_SetSign(&SignSpace, au8Pub, &InnerCall);
//    ouputRes("pu8Pub.\n", PreCalc.pu8Pub, sizeof(PreCalc.pu8Pub));
//    ouputRes("pu8G.\n", PreCalc.pu8G, sizeof(PreCalc.pu8G));

    //GenSignSecKey
    u16IdLen = bn_read_string_from_head(au8Id, sizeof(au8Id), MH_SM9_IDA);
    BigID.pu8Data = au8Id;
    BigID.u32Len = u16IdLen;
    MHSM9_GenSignSecKey(au8KeyD, au8Pri, &BigID, &InnerCall);
//    ouputRes("au8KeyD.\n", au8KeyD, sizeof(au8KeyD));
    //do sign
    u16Mlen = bn_read_string_from_head(au8Msg, sizeof(au8Msg), MH_SM9_SIGN_MSG);
//    bn_read_string_from_head(Sm9Sign.au8H, sizeof(Sm9Sign.au8H), MH_SM9_SIGN_H);
//    bn_read_string_from_head(Sm9Sign.au8S, sizeof(Sm9Sign.au8S), MH_SM9_SIGN_S);
    BigMsg.pu8Data = au8Msg;
    BigMsg.u32Len = u16Mlen;
    SignCall.pIDa = &BigID;
    SignCall.pMsg = &BigMsg;
    SignCall.pSpace = &SignSpace;
    SignCall.pInBase = &InnerCall;
    SignCall.pSign = &Sm9Sign;
    SignCall.pu8KeyD = au8KeyD;
    SignCall.u32Crc = MHCRC_CalcBuff(0xffff, &SignCall, sizeof(MH_SM9_SIGN_CALL)-4);
    memset(&Sm9Sign, 0, sizeof(Sm9Sign));
    MHSM9_Sign(&SignCall);   
    ouputRes("H.\n", Sm9Sign.au8H, 32);
    ouputRes("S.\n", Sm9Sign.au8S, 64);
    r_printf((MH_RET_SM9_SUCCESS==MHSM9_Verify(&SignCall)), "mh sm9 sign/verify test\n");
}

void SM9_SignVeriTest()
{
    SM9_Sign_Test();
    SM9_Verify_Test();
    SM9_SignVeriFunc_Test();
}
