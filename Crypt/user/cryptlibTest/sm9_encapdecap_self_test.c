#include "stdio.h"
#include "stdint.h"
#include "mh_sm9.h"
#include "mh_bignum_tool.h"
#include "mh_bignum.h"
#include "debug.h"
#include "mh2435.h"
#include "mh_misc.h"
#include "mh_sm9_encapdecap.h"
#include "mh_rand.h"

#define MH_SM9_KE           "0001EDEE3778F441F8DEA3D9FA0ACC4E07EE36C93F9A08618AF4AD85CEDE1C22"
//#define MH_SM9_KE         "10585F4D0DD7A7ABBA36C114B7D3922C322088B6EF6A1334D8E0DD6DAE058150"
#define  MH_SM9_PPUB        "787ED7B8A51F3AB84E0A66003F32DA5C720B17ECA7137D39ABC66E3C80A892FF769DE61791E5ADC4B9FF85A31354900B202871279A8C49DC3F220F644C57A7B1"
#define  MH_SM9_DE          "94736ACD2C8C8796CC4785E938301A139A059D3537B6414140B2D31EECF41683115BAE85F5D8BC6C3DBD9E5342979ACCCF3C2F4F28420B1CB4F8C0B59A19B158\
7AA5E47570DA7600CD760A0CF7BEAF71C447F3844753FE74FA7BA92CA7D3B55F27538A62E7F7BFB51DCE08704796D94C9D56734F119EA44732B50E31CDEB75C1"
#define  MH_SM9_ENCAP_CIPHER  "A2FE4F66A6E6C74EE26FC42195DA0F155E5C1ADBE100323A871AB61FF6E621CF11B4FB15544D37814705B348A5F32EECA75260F43D7A4A16326624F15A66751F"
#define  MH_SM9_ENCAP_KEY     "A978E36D80C49B051228153E7721B8948AA8CA21335D0B258899ED001D5B791B"

#define MH_SM9_IDB          "426F62"
#define ENCAP_KEY_LEN       (32)

//SM9 encap Test
static void SM9_Encap_Test()
{
    uint8_t au8Pri[32],au8Ppub[64],au8Key[ENCAP_KEY_LEN],au8Id[32],au8De[128],au8Cipher[64], u8Hid, u8Base = 0;
    uint16_t u16IDlen;
    MH_SM9_INNER_SPACE  InnerCall;
    MHBN_BIGNUM BigID,BigK;
    MH_SM9_ENCAP_SPACE EcapSpace;
    MH_SM9_ENCAP_DECAP_CALL    EncapCall;
    
    memset(&InnerCall, 0, sizeof(InnerCall));
    memset(&EcapSpace, 0, sizeof(EcapSpace));
    memset(&EncapCall, 0, sizeof(EncapCall));
    //set param
    MHSM9_SetParam(&u8Base, &InnerCall);
    //Gen pubkey
    bn_read_string_from_head(au8Pri, 32, MH_SM9_KE);
    MHSM9_GenEncapPubKey(au8Ppub, au8Pri, &InnerCall);
    ouputRes("au8Pri.\n", au8Pri, sizeof(au8Pri));
    ouputRes("au8Pub.\n", au8Ppub, sizeof(au8Ppub));
    //set sign
    MHSM9_SetEncap(&EcapSpace, au8Ppub, &InnerCall);
    ouputRes("EcapSpace.\n", &EcapSpace, sizeof(EcapSpace));
    //GenSignSecKey
    u16IDlen = bn_read_string_from_head(au8Id, sizeof(au8Id), MH_SM9_IDB);
    BigID.pu8Data = au8Id;
    BigID.u32Len = u16IDlen;

    MHSM9_GenEncapSecKey(au8De, au8Pri, &BigID, &InnerCall); // no need in Encap
    ouputRes("au8De.\n", au8De, sizeof(au8De));
    //config
    BigK.pu8Data = au8Key;
    BigK.u32Len = ENCAP_KEY_LEN;
    EncapCall.pu8Cipher = au8Cipher;
    EncapCall.pIDb = &BigID;
    EncapCall.pSpace = &EcapSpace;
    EncapCall.pOutKey = &BigK;
    EncapCall.pInBase = &InnerCall;  
    EncapCall.u32Crc = MHCRC_CalcBuff(0xffff, &EncapCall, sizeof(MH_SM9_ENCAP_DECAP_CALL)-4);
    if(MH_RET_SM9_SUCCESS != MHSM9_Encap(&EncapCall))
    {
        r_printf(0, "sm9 encap test\n");
    }
    else
    {
        ouputRes("au8Key.\n", au8Key, ENCAP_KEY_LEN);
        ouputRes("au8Cipher.\n", au8Cipher, 64);
        r_printf(1, "sm9 encap test\n");
    }
}

//SM9 decap Test
static void SM9_Decap_Test()
{
    uint8_t au8Pri[32],au8Ppub[64],au8Cipher[64],au8Id[32],au8De[128],au8Key[ENCAP_KEY_LEN], au8Key1[ENCAP_KEY_LEN],u8Hid, u8Base = 0;
    uint16_t u16IDlen;
    MH_SM9_INNER_SPACE  InnerCall;
    MHBN_BIGNUM BigID,BigK;
    MH_SM9_ENCAP_SPACE DcapSpace;
    MH_SM9_ENCAP_DECAP_CALL    DecapCall;
    
    memset(&InnerCall, 0, sizeof(InnerCall));
    memset(&DcapSpace, 0, sizeof(DcapSpace));
    memset(&DecapCall, 0, sizeof(DecapCall));
    //set param
    MHSM9_SetParam(&u8Base, &InnerCall);
    //Gen pubkey
    bn_read_string_from_head(au8Pri, 32, MH_SM9_KE);
    MHSM9_GenEncapPubKey(au8Ppub, au8Pri, &InnerCall);
//    ouputRes("au8Pri.\n", au8Pri, sizeof(au8Pri));
//    ouputRes("au8Pub.\n", au8Ppub, sizeof(au8Ppub));
    //set sign
    MHSM9_SetEncap(&DcapSpace, au8Ppub, &InnerCall);
//    ouputRes("DcapSpace.\n", &DcapSpace, sizeof(DcapSpace));
    //GenSignSecKey
    u16IDlen = bn_read_string_from_head(au8Id, sizeof(au8Id), MH_SM9_IDB);
    BigID.pu8Data = au8Id;
    BigID.u32Len = u16IDlen;

    MHSM9_GenEncapSecKey(au8De, au8Pri, &BigID, &InnerCall);
//    ouputRes("au8De.\n", au8De, sizeof(au8De));
    bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), MH_SM9_ENCAP_CIPHER);
    bn_read_string_from_head(au8Key, sizeof(au8Key), MH_SM9_ENCAP_KEY);
    BigK.pu8Data = au8Key1;
    BigK.u32Len = ENCAP_KEY_LEN;
    DecapCall.pu8Cipher = au8Cipher;
    DecapCall.pu8De = au8De;
    DecapCall.pIDb = &BigID;
    DecapCall.pOutKey = &BigK;
    DecapCall.pSpace = &DcapSpace;
    DecapCall.pInBase = &InnerCall; 
    DecapCall.u32Crc = MHCRC_CalcBuff(0xffff, &DecapCall, sizeof(MH_SM9_ENCAP_DECAP_CALL)-4);
    if(MH_RET_SM9_SUCCESS != MHSM9_Decap(&DecapCall))
    {
        r_printf(0, "sm9 decap test\n");
    }
    else
    {
        r_printf(0 == memcmp(au8Key, au8Key1, ENCAP_KEY_LEN), "sm9 decap test\n");
    }
}

static void SM9_EncapDecapFunc_Test()
{
    uint8_t au8Pri[32],au8Ppub[64],au8Cipher[64],au8Id[32],au8De[128],au8Key[ENCAP_KEY_LEN], au8Key1[ENCAP_KEY_LEN],u8Hid, u8Base = 0;
    uint16_t u16IDlen;
    MH_SM9_INNER_SPACE  InnerCall;
    MHBN_BIGNUM BigID,BigK;
    MH_SM9_ENCAP_SPACE DcapSpace;
    MH_SM9_ENCAP_DECAP_CALL   EncapCall, DecapCall;
    
    memset(&InnerCall, 0, sizeof(InnerCall));
    memset(&DcapSpace, 0, sizeof(DcapSpace));
    memset(&EncapCall, 0, sizeof(EncapCall));
    memset(&DecapCall, 0, sizeof(DecapCall));
    //set param
    MHSM9_SetParam(&u8Base, &InnerCall);
    //Gen pubkey
//    bn_read_string_from_head(au8Pri, 32, MH_SM9_KE);
    MHRAND_Prand(au8Pri,32);
    au8Pri[0] &= 0x07;
    MHSM9_GenEncapPubKey(au8Ppub, au8Pri, &InnerCall);
//    ouputRes("au8Pri.\n", au8Pri, sizeof(au8Pri));
//    ouputRes("au8Pub.\n", au8Ppub, sizeof(au8Ppub));
    //set sign
    MHSM9_SetEncap(&DcapSpace, au8Ppub, &InnerCall);
//    ouputRes("DcapSpace.\n", &DcapSpace, sizeof(DcapSpace));
    //GenSignSecKey
    u16IDlen = bn_read_string_from_head(au8Id, sizeof(au8Id), MH_SM9_IDB);
    BigID.pu8Data = au8Id;
    BigID.u32Len = u16IDlen;

    MHSM9_GenEncapSecKey(au8De, au8Pri, &BigID, &InnerCall);
//    ouputRes("au8De.\n", au8De, sizeof(au8De));
     //config
    BigK.pu8Data = au8Key;
    BigK.u32Len = ENCAP_KEY_LEN;
    EncapCall.pu8Cipher = au8Cipher;
    EncapCall.pIDb = &BigID;
    EncapCall.pSpace = &DcapSpace;
    EncapCall.pOutKey = &BigK;
    EncapCall.pInBase = &InnerCall; 
    EncapCall.u32Crc = MHCRC_CalcBuff(0xffff, &EncapCall, sizeof(MH_SM9_ENCAP_DECAP_CALL)-4);
    if(MH_RET_SM9_SUCCESS != MHSM9_Encap(&EncapCall))
    {
        r_printf(0, "sm9 encap test\n");
    }
//    else
//    {
//        ouputRes("au8Key.\n", au8Key, ENCAP_KEY_LEN);
//        ouputRes("au8Cipher.\n", au8Cipher, 64);
//    }
    
    BigK.pu8Data = au8Key1;
    BigK.u32Len = ENCAP_KEY_LEN;
    DecapCall.pu8Cipher = au8Cipher;
    DecapCall.pu8De = au8De;
    DecapCall.pIDb = &BigID;
    DecapCall.pOutKey = &BigK;
    DecapCall.pSpace = &DcapSpace;
    DecapCall.pInBase = &InnerCall;  
    DecapCall.u32Crc = MHCRC_CalcBuff(0xffff, &DecapCall, sizeof(MH_SM9_ENCAP_DECAP_CALL)-4);
    if(MH_RET_SM9_SUCCESS != MHSM9_Decap(&DecapCall))
    {
        r_printf(0, "sm9 decap test\n");
    }
    else
    {
        r_printf(0 == memcmp(au8Key, au8Key1, ENCAP_KEY_LEN), "sm9 encap/decap test\n");
    }
}


void SM9_EncapDecapTest()
{
    SM9_Encap_Test();
    SM9_Decap_Test();
    SM9_EncapDecapFunc_Test();
}
