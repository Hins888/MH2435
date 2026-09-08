#include <string.h>
#include <stdio.h>
#include "mh2435.h"
#include "mh_ecc.h"
#include "mh_bignum_tool.h"
#include "mh_misc.h"
#include "mh_rand.h"
//#include "crypt_debug.h"


//SECP256k1
#define ECC_P_256k1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"
#define ECC_A_256k1        "0000000000000000000000000000000000000000000000000000000000000000"
#define ECC_B_256k1        "0000000000000000000000000000000000000000000000000000000000000007"
#define ECC_GX_256k1       "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"
#define ECC_GY_256k1       "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"
#define ECC_N_256k1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141"

//#define ECC_PRIA      "F15EFC405AE3FBF3EACAA65C349E842844D83D2523B4E31DD7C2786E197831AB"
//#define ECC_PUBXA    "16A55AD53686237FF48273C44BCEC9848718A06D045315D1650750387C8B6699"
//#define ECC_PUBYA     "79A5A4D042A51C7850FB90FF2517E0BC0C9346A7BF0C8828A79540161EE713FD"

//#define ECC_PRIB      "39793653AE93FA86A5939A938D5AD5261E017BC72B905843E6E7AF43E4373EBC"
//#define ECC_PUBXB     "C03EB9E358D13152B4332122E4E30D68C89773D463815F729FD95F1FAECE2105"
//#define ECC_PUBYB     "E15CAD23CAA3805562BF50041F9F70AF86F8457FF16484FE5BBDE86E312319B9"


//SECP256r1
#define ECC_P_256r1         "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF"
#define ECC_A_256r1         "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC"
#define ECC_B_256r1         "5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B"
#define ECC_GX_256r1        "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296"
#define ECC_GY_256r1        "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5"
#define ECC_N_256r1         "FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551"

#define ECC_PRIA_256r1       "3165F1A988A5AF56F2D1383FEBA7DF3C1742CD2A46DC027760C9CE9EB808F6C1"
#define ECC_PUBXA_256r1      "CE745FAA9E2EFA71404B62FBFFDA16C98300B1ECD816C5EE5D6BBC3A7604F7EE"
#define ECC_PUBYA_256r1      "FDE33B2CE1AB3C8ABABC673EF07913304EE74998827E9A93D15261E35698A844"

#define ECC_PRIB_256r1      "E4E8118F5ED6BF84D3188391F147A25ADF3993C9AB59A4713968F73F066905AE"
#define ECC_PUBXB_256r1     "A932895FC829657D0156F7A05829354DF18C74B66AD23985D8F2B5A4DD2AB817"
#define ECC_PUBYB_256r1     "25BB6882ABB9C880172568047E0A85D392B75A73BDE2487C4AC525DADF763593"

//#define SINFO1        "115960C5598A2797C2D02D76D468CDC91234567890ABCDEF1234567890ABCDEF"
//#define SINFO2        "1234567890ABCDEF1234567890ABCDEF115960C5598A2797C2D02D76D468CDC9"


//SECP192k1
#define ECC_P_192k1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFEE37"
#define ECC_A_192k1         "000000000000000000000000000000000000000000000000"
#define ECC_B_192k1         "000000000000000000000000000000000000000000000003"
#define ECC_GX_192k1        "DB4FF10EC057E9AE26B07D0280B7F4341DA5D1B1EAE06C7D"
#define ECC_GY_192k1        "9B2F2F6D9C5628A7844163D015BE86344082AA88D95E2F9D"
#define ECC_N_192k1         "FFFFFFFFFFFFFFFFFFFFFFFE26F2FC170F69466A74DEFD8D"

//#define ECC_PRIA      "47D3C946E44A8F0A0981CB5F8E4DD8B2C094972C566C4489"
//#define ECC_PUBXA    "31AFDF1979FBC63902B69878216BAB0514BEBAF382384493"
//#define ECC_PUBYA     "DF90130CBBB8FACA6CB4846A2D11500F218CDCB818B5B8F4"

//#define ECC_PRIB      "8D7D4F0A65742CC4ED6CD827CCC0B7C0917DF3FFE56CD0DD"
//#define ECC_PUBXB     "C79BAF9D3E1C65F1A4B46FDBCE72E21389E73C935FCB2BF7"
//#define ECC_PUBYB     "D5C0E0FA0FC3F3D4059DC9A54791B3BDF19D71422AEAB7E2"


//SECP192r1
#define ECC_P_192r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF"
#define ECC_A_192r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFC"
#define ECC_B_192r1        "64210519E59C80E70FA7E9AB72243049FEB8DEECC146B9B1"
#define ECC_GX_192r1       "188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012"
#define ECC_GY_192r1       "07192B95FFC8DA78631011ED6B24CDD573F977A11E794811"
#define ECC_N_192r1        "FFFFFFFFFFFFFFFFFFFFFFFF99DEF836146BC9B1B4D22831"

#define ECC_PRIA_192r1     "8E9A397EF37F9710E0A7352C88EDBA9F92D54E884D119AAF"
#define ECC_PUBXA_192r1     "5E0E1E4CE6D39B72A263264A19D41CBAACA1E8AFAECDE706"
#define ECC_PUBYA_192r1     "6C2090B6871CB7356D850A815014B07F8C01FE8A15A1A0E2"

//#define ECC_PRIB      "C620640977E28A217DBD102FDFF7D11F3CC168184978C885"
//#define ECC_PUBXB     "6158DCC322D6DFFF9D86E6B2E1E7692A20992FD20A399658"
//#define ECC_PUBYB     "69C89F577503853C5327EE4FB7E16EBB983AC22E4A39D067"

//#define SINFO1        "115960C5598A2797C2D02D76D468CDC91234567890ABCDEF1234567890ABCDEF"
//#define SINFO2        "1234567890ABCDEF1234567890ABCDEF115960C5598A2797C2D02D76D468CDC9"

//SECP384r1
#define ECC_P_384r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFF"
#define ECC_A_384r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFC"
#define ECC_B_384r1        "B3312FA7E23EE7E4988E056BE3F82D19181D9C6EFE8141120314088F5013875AC656398D8A2ED19D2A85C8EDD3EC2AEF"
#define ECC_GX_384r1       "AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7"
#define ECC_GY_384r1       "3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F"
#define ECC_N_384r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC7634D81F4372DDF581A0DB248B0A77AECEC196ACCC52973"

#define ECC_PRIA_384r1       "6CCC9B1E2AF84D1C3C8704223F1E96BCEA6E4BB91724D0ABDA13CA587B124779063D13434BFD41587609E046C14A922E"
#define ECC_PUBXA_384r1      "420D47AC4ECF4E49264A6CA1818008A87F28E4788C8CF0511AD821A4BD1DE0BEAC2A25AB4D205BD723A63E000B5A9C83"
#define ECC_PUBYA_384r1      "8B141268999BFF1477217C8724C89AEC18C96BC88800AFC44BED6D55AF1D98A3E0CF2DBC045C3AF7E4399259FEB35685"

//#define ECC_PRIB      "F38FD9DFA7ABE1BB975C366BAC9E261578ED12F3CE28B2EE5662A565B9E2BC2ED93A41B70570FE1E4A872B1FA1B034F3"
//#define ECC_PUBXB     "AEEF1480EA443BB2C10054F006030BA683576D2377448401C9F48DCCCBF4F4D2333FB00BE4C01B0C21B5ED9C7D0710C2"
//#define ECC_PUBYB     "436C0ADB2C724F53AE9FC00C2DFD21AB2C0FC27952BC7EC930C6649C9CCD6208044A62CEFCD69B1F49090F9BFDA9BAAD"

//#define SINFO1        "115960C5598A2797C2D02D76D468CDC91234567890ABCDEF1234567890ABCDEF"
//#define SINFO2        "1234567890ABCDEF1234567890ABCDEF115960C5598A2797C2D02D76D468CDC9"

//SECP224r1
#define ECC_P_224r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000001"
#define ECC_A_224r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFE"
#define ECC_B_224r1        "B4050A850C04B3ABF54132565044B0B7D7BFD8BA270B39432355FFB4"
#define ECC_GX_224r1       "B70E0CBD6BB4BF7F321390B94A03C1D356C21122343280D6115C1D21"
#define ECC_GY_224r1       "BD376388B5F723FB4C22DFE6CD4375A05A07476444D5819985007E34"
#define ECC_N_224r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFF16A2E0B8F03E13DD29455C5C2A3D"

#define ECC_PRIA_224r1      "61780BFCF120232972F3993F9229A72F5F4508C6BF3CFE8A06DB33D6"
#define ECC_PUBXA_224r1     "BE810752B78F84CA2BEDAB92D0E56293F1A691F3B77765FD86D4729A"
#define ECC_PUBYA_224r1     "5AF6C5ECE0F14DB75D878FBFA83651943E6B90E29F97A802703440DD"

//#define ECC_PRIB      "961282A8697C0105B818512477D00AA821A5391C132592AA67D2C84B"
//#define ECC_PUBXB     "4A671D5CC20BB8FAAB5026BBDC3D20AD104039533550D785CB20C532"
//#define ECC_PUBYB     "C65025286D9207C17BCF4F23D507145F3AFB226FFF0E78C696275DB1"

//#define SINFO1        "115960C5598A2797C2D02D76D468CDC91234567890ABCDEF1234567890ABCDEF"
//#define SINFO2        "1234567890ABCDEF1234567890ABCDEF115960C5598A2797C2D02D76D468CDC9"

//secp521r1

#define ECC_P_521r1   "01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
#define ECC_A_521r1   "01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC"
#define ECC_B_521r1   "0051953EB9618E1C9A1F929A21A0B68540EEA2DA725B99B315F3B8B489918EF109E156193951EC7E937B1652C0BD3BB1BF073573DF883D2C34F1EF451FD46B503F00"
#define ECC_GX_521r1  "00C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66"
#define ECC_GY_521r1  "011839296A789A3BC0045C8A5FB42C7D1BD998F54449579B446817AFBD17273E662C97EE72995EF42640C550B9013FAD0761353C7086A272C24088BE94769FD16650"
#define ECC_N_521r1   "01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA51868783BF2F966B7FCC0148F709A5D03BB5C9B8899C47AEBB6FB71E91386409"

#define ECC_PRIA_521r1      "00FCA49A1D526A04E30CC0BD25846005D27506532BB00CFEC811CDBEBF371A2A9DA9E5300EEF33777F5C33C861C7FD332882AE74F03612C78DAC3346039933A35EE4"
#define ECC_PUBXA_521r1     "019D0011C0B600ABDC911A420849925FA4D3474BAF8350483306B8FEB51B9C3B744A7807B58753EC57251E49ABE5C5CEAE10B3D5E60291A80920BE034F2058D140D2"
#define ECC_PUBYA_521r1      "0013706C32B03E833F3876C92EF8A552A622A6BCD1D17242DC2ACF3CDE74C957F120A8F2D0C0C6837419052324DEA323B113B04EF92875E2123041601A10B8D04811"

//#define ECC_PRIB      "008AA27A095168DD914A4CEA39409071541E40C80D808E0AD7EB48F41AECDF780734310A8DCC68796DE3963E8328784D7554DB7F8B31AE6439531FC408FE9AC474BC"
//#define ECC_PUBXB     "0092A600671151B7BF9AB75C8F60554F061F6417CA71C686687E34E7FBD2DCFBAC02400AC5CBFEEC89C755AB00D26D84A78041C8F603DAC5C5D9EDC66404E1C0ABC5"
//#define ECC_PUBYB     "018B4C1817C4A9EDCFF9A72B60B718FE90ED8B1FC392247E1A87792468A63FF2A72C94FA3AFAA3A8CD2D650CD21D78B89616C35710A5D43666981D8C3A059170DD0E"


//brainpoolP192r1
#define ECC_P_192r1_B           "C302F41D932A36CDA7A3463093D18DB78FCE476DE1A86297"
#define ECC_A_192r1_B           "6A91174076B1E0E19C39C031FE8685C1CAE040E5C69A28EF"
#define ECC_B_192r1_B           "469A28EF7C28CCA3DC721D044F4496BCCA7EF4146FBF25C9"
#define ECC_GX_192r1_B          "C0A0647EAAB6A48753B033C56CB0F0900A2F5C4853375FD6"
#define ECC_GY_192r1_B          "14B690866ABD5BB88B5F4828C1490002E6773FA2FA299B8F"
#define ECC_N_192r1_B           "C302F41D932A36CDA7A3462F9E9E916B5BE8F1029AC4ACC1"

//brainpoolP224r1
#define ECC_P_224r1_B           "D7C134AA264366862A18302575D1D787B09F075797DA89F57EC8C0FF"
#define ECC_A_224r1_B           "68A5E62CA9CE6C1C299803A6C1530B514E182AD8B0042A59CAD29F43"
#define ECC_B_224r1_B           "2580F63CCFE44138870713B1A92369E33E2135D266DBB372386C400B"
#define ECC_GX_224r1_B          "0D9029AD2C7E5CF4340823B2A87DC68C9E4CE3174C1E6EFDEE12C07D"
#define ECC_GY_224r1_B          "58AA56F772C0726F24C6B89E4ECDAC24354B9E99CAA3F6D3761402CD"
#define ECC_N_224r1_B           "D7C134AA264366862A18302575D0FB98D116BC4B6DDEBCA3A5A7939F"

//brainpoolP256r1
#define ECC_P_256r1_B          "A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"
#define ECC_A_256r1_B           "7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"
#define ECC_B_256r1_B           "26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"
#define ECC_GX_256r1_B          "8BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262"
#define ECC_GY_256r1_B          "547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"
#define ECC_N_256r1_B           "A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"

//brainpoolP384r1
#define ECC_P_384r1_B           "8CB91E82A3386D280F5D6F7E50E641DF152F7109ED5456B412B1DA197FB71123ACD3A729901D1A71874700133107EC53"
#define ECC_A_384r1_B           "7BC382C63D8C150C3C72080ACE05AFA0C2BEA28E4FB22787139165EFBA91F90F8AA5814A503AD4EB04A8C7DD22CE2826"
#define ECC_B_384r1_B           "04A8C7DD22CE28268B39B55416F0447C2FB77DE107DCD2A62E880EA53EEB62D57CB4390295DBC9943AB78696FA504C11"
#define ECC_GX_384r1_B          "1D1C64F068CF45FFA2A63A81B7C13F6B8847A3E77EF14FE3DB7FCAFE0CBD10E8E826E03436D646AAEF87B2E247D4AF1E"
#define ECC_GY_384r1_B          "8ABE1D7520F9C2A45CB1EB8E95CFD55262B70B29FEEC5864E19C054FF99129280E4646217791811142820341263C5315"
#define ECC_N_384r1_B           "8CB91E82A3386D280F5D6F7E50E641DF152F7109ED5456B31F166E6CAC0425A7CF3AB6AF6B7FC3103B883202E9046565"

//brainpoolP521r1
#define ECC_P_521r1_B           "AADD9DB8DBE9C48B3FD4E6AE33C9FC07CB308DB3B3C9D20ED6639CCA703308717D4D9B009BC66842AECDA12AE6A380E62881FF2F2D82C68528AA6056583A48F3"
#define ECC_A_521r1_B           "7830A3318B603B89E2327145AC234CC594CBDD8D3DF91610A83441CAEA9863BC2DED5D5AA8253AA10A2EF1C98B9AC8B57F1117A72BF2C7B9E7C1AC4D77FC94CA"
#define ECC_B_521r1_B           "3DF91610A83441CAEA9863BC2DED5D5AA8253AA10A2EF1C98B9AC8B57F1117A72BF2C7B9E7C1AC4D77FC94CADC083E67984050B75EBAE5DD2809BD638016F723"
#define ECC_GX_521r1_B          "81AEE4BDD82ED9645A21322E9C4C6A9385ED9F70B5D916C1B43B62EEF4D0098EFF3B1F78E2D0D48D50D1687B93B97D5F7C6D5047406A5E688B352209BCB9F822"
#define ECC_GY_521r1_B          "7DDE385D566332ECC0EABFA9CF7822FDF209F70024A57B1AA000C55B881F8111B2DCDE494A5F485E5BCA4BD88A2763AED1CA2B2FA8F0540678CD1E0F3AD80892"
#define ECC_N_521r1_B           "AADD9DB8DBE9C48B3FD4E6AE33C9FC07CB308DB3B3C9D20ED6639CCA70330870553E5C414CA92619418661197FAC10471DB1D381085DDADDB58796829CA90069"


#define MSG         "1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF"
#define SINFO1        "115960C5598A2797C2D02D76D468CDC91234567890ABCDEF1234567890ABCDEF"
#define SINFO2        "1234567890ABCDEF1234567890ABCDEF115960C5598A2797C2D02D76D468CDC9"
#define ECC_CIPHER_256r1    "B4EC989B8D19CFDA06D407202D8758ED2477DFCAFBAA34E9F09EF8E3D76F9ABE7857DD0414B00407D54FFAB25782790C034DCFB9E32966FF796D16628438192ED275EFBFA99F3E012B65D639C5EF571D37A8CCF05E535B4DDF9CA08F8B3B743A2FC5D85447613EAF0E061D8E7C0D27971E3EBB14294A73377983F03CF3CA92D4"


#define     USE_CRYPT_LIB       (1) 



#if 1

#define KEYBITS (256)
#define KEYLEN  ((KEYBITS + 7)>>3)
#define MSG_LEN (256)

static void ECC_EcdhTest()
{
    uint32_t u32Ret = 0;
    uint8_t au8P[KEYLEN],au8A[KEYLEN],au8B[KEYLEN],au8N[KEYLEN],au8GX[KEYLEN],au8GY[KEYLEN];
    uint8_t au8PriA[KEYLEN],au8PubXA[KEYLEN],au8PubYA[KEYLEN],au8Space[MH_ECC256_SPACE_LEN];
    uint8_t au8PriB[KEYLEN],au8PubXB[KEYLEN],au8PubYB[KEYLEN],au8KA[KEYLEN],au8KB[KEYLEN];
    uint8_t u8Base = 0;
    MHECC_PARAM EccPara;
    MHECC_KEY   KeySrc;
    MH_ECC_INNER_BASE InBaseA;
    MH_ECC_INNER_BASE InBaseB;
    MHECC_ECDH_CALL EcdhCall;
    bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_256r1);  
    bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_256r1);
    bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_256r1);
    bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_256r1);  
    bn_read_string_from_head(au8GX, sizeof(au8GX), ECC_GX_256r1);
    bn_read_string_from_head(au8GY, sizeof(au8GY), ECC_GY_256r1);
    bn_read_string_from_head(au8PriA, sizeof(au8PriA), ECC_PRIA_256r1);
    bn_read_string_from_head(au8PubXA, sizeof(au8PubXA), ECC_PUBXA_256r1);
    bn_read_string_from_head(au8PubYA, sizeof(au8PubXA), ECC_PUBYA_256r1);
    bn_read_string_from_head(au8PriB, sizeof(au8PriB), ECC_PRIB_256r1);
    bn_read_string_from_head(au8PubXB, sizeof(au8PubXB), ECC_PUBXB_256r1);
    bn_read_string_from_head(au8PubYB, sizeof(au8PubXB), ECC_PUBYB_256r1);
    memset(&EccPara,0,sizeof(EccPara));
    memset(&KeySrc,0,sizeof(KeySrc));
    memset(&InBaseA,0,sizeof(InBaseA));
    memset(&InBaseB,0,sizeof(InBaseB));
    EccPara.pu8P = au8P; 
    EccPara.pu8A = au8A;
    EccPara.pu8B = au8B;
    EccPara.pu8N = au8N;
    EccPara.pu8Gx = au8GX;
    EccPara.pu8Gy = au8GY;
    EccPara.pu8Space = au8Space;
    EccPara.u32SpaceLen = MH_ECC256_SPACE_LEN;
    EccPara.u16BitLen = KEYBITS;
    EccPara.u32Crc = MHCRC_CalcBuff(0xffff, &EccPara, sizeof(MHECC_PARAM)-4); 
    MHECC_ParamInit(&EccPara);
	
    memset(&KeySrc,0,sizeof(KeySrc));
    KeySrc.pu8D = au8PriA;
    KeySrc.u16Bits = KEYBITS;
    KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
    MHECC_SetKey(&KeySrc,&InBaseA,&u8Base,&EccPara);
	
    memset(&KeySrc,0,sizeof(KeySrc));
    KeySrc.pu8PubX = au8PubXB;
    KeySrc.pu8PubY = au8PubYB;
    KeySrc.u16Bits = KEYBITS;
    KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
    MHECC_SetKey(&KeySrc,&InBaseB,&u8Base,&EccPara);
	
    EcdhCall.pParam = &EccPara;
    EcdhCall.pInBaseU = &InBaseA;
    EcdhCall.pInBaseV = &InBaseB;
    EcdhCall.pu8KeyOut = au8KA;
    EcdhCall.u32Crc = MHCRC_CalcBuff(0xffff, &EcdhCall, sizeof(MHECC_ECDH_CALL)-4); 
	
    MHECC_ECDH(&EcdhCall);
	
    memset(&KeySrc,0,sizeof(KeySrc));
    KeySrc.pu8D = au8PriB;
    KeySrc.u16Bits = KEYBITS;
    KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
    MHECC_SetKey(&KeySrc,&InBaseB,&u8Base,&EccPara);
	
    memset(&KeySrc,0,sizeof(KeySrc));
    KeySrc.pu8PubX = au8PubXA;
    KeySrc.pu8PubY = au8PubYA;
    KeySrc.u16Bits = KEYBITS;
    KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
    MHECC_SetKey(&KeySrc,&InBaseA,&u8Base,&EccPara);    
	
    EcdhCall.pInBaseU = &InBaseB;
    EcdhCall.pInBaseV = &InBaseA;
    EcdhCall.pu8KeyOut = au8KB;
    EcdhCall.u32Crc = MHCRC_CalcBuff(0xffff, &EcdhCall, sizeof(MHECC_ECDH_CALL)-4); 
	
	u32Ret = MHECC_ECDH(&EcdhCall);
    if(MH_RET_ECC_SUCCESS == u32Ret)
    {
        r_printf((0 == memcmp(au8KA, au8KB,  (KEYBITS + 7)>>3)), "mh_ecc_ecdh test\n");
    }
    else
    {
		printf("errRet = %08x\n", u32Ret);
        r_printf(0, "mh_ecc_ecdh test\n");
    }
}
#endif



#define     KEY_LEN_MAX     ((521+7)>>3)
#define     ECC_MSG_LEN_MAX     (128)
#define     PARA_NUM    (5)


static void ECC_EciesTest()
{
	uint32_t u32Ret = 0;
    uint32_t au32Para[PARA_NUM] = {192,224,256,384,521};
    uint8_t au8P[KEY_LEN_MAX],au8A[KEY_LEN_MAX],au8B[KEY_LEN_MAX],au8N[KEY_LEN_MAX],au8GX[KEY_LEN_MAX],au8GY[KEY_LEN_MAX];
    uint8_t au8Pri[KEY_LEN_MAX],au8PubX[KEY_LEN_MAX],au8PubY[KEY_LEN_MAX],au8Msg[ECC_MSG_LEN_MAX], au8Space[MH_ECC521_SPACE_LEN];
    uint8_t au8SINFO1[32], au8SINFO2[32], au8Cipher[ECC_MSG_LEN_MAX + KEY_LEN_MAX*2 + 32], au8Plain[ECC_MSG_LEN_MAX];
    uint8_t i, u8Base = 0;
    uint16_t u16MsgLen,u16Ilen1,u16Ilen2;
    MHECC_PARAM EccPara;
    MHECC_KEY   KeySrc;
    MH_ECC_INNER_BASE InBase;
    MHECC_ENC_DEC_CALL EciesCall;
                               
    for(i = 0; i < PARA_NUM; i++)
    {
        printf("%d bits ecc enc_dec test\n",au32Para[i]);
        memset(&EccPara,0,sizeof(EccPara));
         // curve para init
        if(au32Para[i] == 192)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_192r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_192r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_192r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_192r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_192r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_192r1); 
        }
        else if(au32Para[i] == 224)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_224r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_224r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_224r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_224r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_224r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_224r1); 
        }
        else if(au32Para[i] == 256)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_256k1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_256k1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_256k1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_256k1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_256k1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_256k1); 
        }
        else if(au32Para[i] == 384)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_384r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_384r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_384r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_384r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_384r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_384r1); 
        }
        else if(au32Para[i] == 521)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_521r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_521r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_521r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_521r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_521r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_521r1);      
        }
        u16Ilen1 = bn_read_string_from_head(au8SINFO1, sizeof(au8SINFO1), SINFO1);
        u16Ilen2 = bn_read_string_from_head(au8SINFO2, sizeof(au8SINFO2), SINFO2);
        
        memset(&KeySrc,0,sizeof(KeySrc));
        memset(&InBase,0,sizeof(InBase));
        memset(au8Cipher,0,sizeof(au8Cipher));
        u8Base = 0;
        
        // init curve para
        EccPara.pu8P = au8P; 
        EccPara.pu8A = au8A;
        EccPara.pu8B = au8B;
        EccPara.pu8N = au8N;
        EccPara.pu8Gx = au8GX;
        EccPara.pu8Gy = au8GY;
        EccPara.pu8Space = au8Space;
        EccPara.u32SpaceLen = MH_ECC521_SPACE_LEN;
        EccPara.u16BitLen = au32Para[i];
        EccPara.u32Crc = MHCRC_CalcBuff(0xffff, &EccPara, sizeof(MHECC_PARAM)-4); 
        MHECC_ParamInit(&EccPara);

        //genkey
        KeySrc.pu8D = au8Pri;
        KeySrc.pu8PubX = au8PubX;
        KeySrc.pu8PubY = au8PubY;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        u32Ret = MHECC_GenKey(&KeySrc, &EccPara);
        if(MH_RET_ECC_SUCCESS != u32Ret)
		{
			printf("errRet = %08x\n", u32Ret);
            r_printf(0, "MHECC_GenKey test\n");
        }
//        ouputRes("au8Pri.\n", au8Pri, KeySrc.u16Bits>>3);
//        ouputRes("au8PubX.\n", au8PubX, KeySrc.u16Bits>>3);
//        ouputRes("au8PubY.\n", au8PubY, KeySrc.u16Bits>>3);
        
        //set ecc key
        KeySrc.pu8D = au8Pri;
        KeySrc.pu8PubX = au8PubX;
        KeySrc.pu8PubY = au8PubY;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        MHECC_SetKey(&KeySrc,&InBase,&u8Base,&EccPara);

        MHRAND_Prand(&u16MsgLen,sizeof(u16MsgLen));
        u16MsgLen = (u16MsgLen & 0x7f) + 1; 
        MHRAND_Prand(au8Msg,u16MsgLen);
        EciesCall.pParam = &EccPara;
        EciesCall.pInBase = &InBase;
        EciesCall.pu8Input = au8Msg;
        EciesCall.u16InLen = u16MsgLen;
        EciesCall.pu8SharedMsg1 = au8SINFO1;
        EciesCall.u16M1Len = u16Ilen1;
        EciesCall.pu8SharedMsg2 = au8SINFO2;
        EciesCall.u16M2Len = u16Ilen2;
        
        EciesCall.u16MackeyLen = 32;
        EciesCall.pu8Output = au8Cipher;  
        EciesCall.u16OutLen = u16MsgLen + 32 + (((au32Para[i]+7)>>3) << 1);
        EciesCall.u32Crc = MHCRC_CalcBuff(0xffff, &EciesCall, sizeof(MHECC_ENC_DEC_CALL)-4);    
		
		u32Ret = MHECC_EciesEnc(&EciesCall);
        if(MH_RET_ECC_SUCCESS != u32Ret)
        {
			printf("errRet = %08x\n", u32Ret);
            r_printf(0, "mh_ecc_enc test\n");
        }
//        ouputRes("au8Msg.\n", au8Msg, u16MsgLen);
//        ouputRes("au8Cipher.\n", au8Cipher, EciesCall.u16OutLen);
        memset(au8Plain,0,sizeof(au8Plain));
        EciesCall.pu8Input = au8Cipher;
        EciesCall.u16InLen = EciesCall.u16OutLen;
        EciesCall.pu8Output = au8Plain; 
        EciesCall.u32Crc = MHCRC_CalcBuff(0xffff, &EciesCall, sizeof(MHECC_ENC_DEC_CALL)-4);    
		
		u32Ret = MHECC_EciesDec(&EciesCall);
        if(MH_RET_ECC_SUCCESS != u32Ret)
        {
			printf("errRet = %08x\n", u32Ret);
            r_printf(0, "mh_ecc_dec test\n");
        }       
//        ouputRes("au8Plain.\n", au8Plain, u16MsgLen);
        r_printf((0 == memcmp(au8Plain, au8Msg, u16MsgLen)), "mh_ecc_enc/dec test\n");
    }        
}


static void ECC_SignTest()
{
    static uint32_t au32Kbits[5] = {521,224,256,384,521};
    uint32_t u32Ret,u32Klen, u32OffLen;
    uint32_t au32Para[PARA_NUM] = {521,224,256,384,521};
    uint8_t au8P[KEY_LEN_MAX],au8A[KEY_LEN_MAX],au8B[KEY_LEN_MAX],au8N[KEY_LEN_MAX],au8GX[KEY_LEN_MAX],au8GY[KEY_LEN_MAX];
    uint8_t au8Pri[KEY_LEN_MAX],au8PubX[KEY_LEN_MAX],au8PubY[KEY_LEN_MAX],au8Msg[ECC_MSG_LEN_MAX],au8Space[MH_ECC521_SPACE_LEN];
    uint8_t au8SigR[KEY_LEN_MAX],au8SigS[KEY_LEN_MAX],au8E[32];
    uint16_t u16MsgLen;
    uint8_t i,u8Base = 0;
    MHECC_PARAM EccPara;
    MHECC_KEY   KeySrc;
    MH_ECC_INNER_BASE InBase;
    MHECC_SIG_VER_CALL SignCall;
    MHECC_SIGN SignRs;

    for(i = 0; i < PARA_NUM; i++)
    {
        printf("\r\n%d bits ecc sign_verify  test\r\n",au32Para[i]);
        memset(&EccPara,0,sizeof(EccPara));
        if(au32Para[i] == 192)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_192r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_192r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_192r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_192r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_192r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_192r1); 
        }
        else if(au32Para[i] == 224)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_224r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_224r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_224r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_224r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_224r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_224r1); 
        }
        else if(au32Para[i] == 256)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_256k1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_256k1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_256k1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_256k1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_256k1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_256k1); 
        }
        else if(au32Para[i] == 384)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_384r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_384r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_384r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_384r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_384r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_384r1);
         
        }
        else if(au32Para[i] == 521)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_521r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_521r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_521r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_521r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_521r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_521r1); 
        }
        
        //traversal Klen
        au32Kbits[i] = (au32Kbits[i]-1)%au32Para[i];
        if(au32Kbits[i] < 64)
        {
            au32Kbits[i] = au32Para[i];
        }
        u32Klen = (au32Kbits[i] + 7) >> 3;
        u32OffLen = ((au32Para[i] + 7)>>3) - u32Klen;
        memset(au8Pri, 0 , sizeof(au8Pri));
        MHRAND_Prand(au8Pri + u32OffLen, u32Klen);
        if(au32Kbits[i] & 0x07)
        {
            au8Pri[u32OffLen] &= (1<<(au32Kbits[i] & 0x07)) - 1;
        }
        
        MHRAND_Prand(&u16MsgLen,sizeof(u16MsgLen));
        u16MsgLen = (u16MsgLen & 0xff) + 1;
        u16MsgLen = 16;
        MHRAND_Prand(au8Msg,u16MsgLen);
        memset(&KeySrc,0,sizeof(KeySrc));
        memset(&InBase,0,sizeof(InBase));
        memset(au8SigR,0,sizeof(au8SigR));
        memset(au8SigS,0,sizeof(au8SigS));
        u8Base = 0;
        
        EccPara.pu8P = au8P;
        EccPara.pu8A = au8A;
        EccPara.pu8B = au8B;
        EccPara.pu8N = au8N;
        EccPara.pu8Gx = au8GX;
        EccPara.pu8Gy = au8GY;
        EccPara.pu8Space = au8Space;
        EccPara.u32SpaceLen = MH_ECC521_SPACE_LEN;
        EccPara.u16BitLen = au32Para[i];
        EccPara.u32Crc = MHCRC_CalcBuff(0xffff, &EccPara, sizeof(MHECC_PARAM)-4); 
        MHECC_ParamInit(&EccPara);
        
		memset(au8PubX, 0, sizeof(au8PubX));
        memset(au8PubY, 0, sizeof(au8PubY));
		
        //genkey
        KeySrc.pu8D = au8Pri;
        KeySrc.pu8PubX = au8PubX;
        KeySrc.pu8PubY = au8PubY;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        u32Ret = MHECC_CompleteKey(&KeySrc, &EccPara);
        if(u32Ret != MH_RET_ECC_SUCCESS)
        {
			printf("errRet = %08x\n", u32Ret);
            ouputRes("au8Pri.\n", au8Pri, KeySrc.u16Bits>>3);
            while(1);
        }
//        MHECC_GenKey(&KeySrc, &EccPara);
//        ouputRes("au8Pri.\n", au8Pri, KeySrc.u16Bits>>3);
//        ouputRes("au8PubX.\n", au8PubX, KeySrc.u16Bits>>3);
//        ouputRes("au8PubY.\n", au8PubY, KeySrc.u16Bits>>3);
        
        //set key
        KeySrc.pu8D = au8Pri;
        KeySrc.pu8PubX = au8PubX;
        KeySrc.pu8PubY = au8PubY;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        MHECC_SetKey(&KeySrc,&InBase,&u8Base,&EccPara);
        
        SignRs.pu8SignR = au8SigR;
        SignRs.pu8SignS = au8SigS;
        SignCall.pu8Sign = &SignRs;
        SignCall.pInBase = &InBase;
        SignCall.pParam = &EccPara;
        SignCall.pu8E = au8E;
        SignCall.pu8Msg = au8Msg;
        SignCall.u16ELen = sizeof(au8E);
        SignCall.u32MLen = u16MsgLen;

        MHECC_HashE(&SignCall);
//        ouputRes("au8E.\n", au8E, 32);
//        ouputRes("au8Msg.\n", au8Msg, 16);
        SignCall.u32Crc = MHCRC_CalcBuff(0xffff, &SignCall, sizeof(MHECC_SIG_VER_CALL)-4);     

		u32Ret = MHECC_EcdsaSign(&SignCall);
        if(u32Ret != MH_RET_ECC_SUCCESS)
        {
			printf("errRet = %08x\n", u32Ret);
            r_printf(0, "mh_ecc sign test\n");
        }
//        ouputRes("au8SigR.\n", au8SigR, (au32Para[i] + 7)>>3);
//        ouputRes("au8SigS.\n", au8SigS, (au32Para[i] + 7)>>3);
        SignCall.u32Crc = MHCRC_CalcBuff(0xffff, &SignCall, sizeof(MHECC_SIG_VER_CALL)-4);  
		
		u32Ret = MHECC_EcdsaVerify(&SignCall);
		if(MH_RET_ECC_SUCCESS != u32Ret)
		{
			printf("errRet = %08x\n", u32Ret);
            r_printf(0, "mh_ecc sign/verify test\n");
			
		}
		else
		{
			r_printf(1, "mh_ecc sign/verify test\n");
		}
    }
}

static void ECC_SignTest_Brain()
{
	uint32_t u32Ret = 0;
    uint16_t au32Para[PARA_NUM - 1] = {224,224,256,384};	//521 No support
    uint8_t au8P[KEY_LEN_MAX],au8A[KEY_LEN_MAX],au8B[KEY_LEN_MAX],au8N[KEY_LEN_MAX],au8GX[KEY_LEN_MAX],au8GY[KEY_LEN_MAX];
    uint8_t au8Pri[KEY_LEN_MAX],au8PubX[KEY_LEN_MAX],au8PubY[KEY_LEN_MAX],au8Msg[ECC_MSG_LEN_MAX],au8Space[MH_ECC521_SPACE_LEN];
    uint8_t au8SigR[KEY_LEN_MAX],au8SigS[KEY_LEN_MAX],au8E[32];
    uint16_t u16MsgLen;
    uint8_t i,u8Base = 0;
    MHECC_PARAM EccPara;
    MHECC_KEY   KeySrc;
    MH_ECC_INNER_BASE InBase;
    MHECC_SIG_VER_CALL SignCall;
    MHECC_SIGN SignRs;
    
    for(i = 0; i < PARA_NUM - 1; i++)
    {
        printf("%d bits ecc sign_verify_B  test\n",au32Para[i]);
        memset(&EccPara,0,sizeof(EccPara));
        if(au32Para[i] == 192)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_192r1_B);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_192r1_B);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_192r1_B);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_192r1_B);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_192r1_B);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_192r1_B); 
        }
        else if(au32Para[i] == 224)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_224r1_B);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_224r1_B);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_224r1_B);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_224r1_B);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_224r1_B);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_224r1_B); 
        }
        else if(au32Para[i] == 256)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_256r1_B);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_256r1_B);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_256r1_B);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_256r1_B);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_256r1_B);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_256r1_B); 
        }
        else if(au32Para[i] == 384)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_384r1_B);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_384r1_B);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_384r1_B);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_384r1_B);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_384r1_B);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_384r1_B);          
        }

        MHRAND_Prand(&u16MsgLen,sizeof(u16MsgLen));
        u16MsgLen = (u16MsgLen & 0xff) + 1;
        MHRAND_Prand(au8Msg,u16MsgLen);
        memset(&KeySrc,0,sizeof(KeySrc));
        memset(&InBase,0,sizeof(InBase));
        memset(au8SigR,0,sizeof(au8SigR));
        memset(au8SigS,0,sizeof(au8SigS));
        u8Base = 0;
        
        EccPara.pu8P = au8P;
        EccPara.pu8A = au8A;
        EccPara.pu8B = au8B;
        EccPara.pu8N = au8N;
        EccPara.pu8Gx = au8GX;
        EccPara.pu8Gy = au8GY;
        EccPara.pu8Space = au8Space;
        EccPara.u32SpaceLen = MH_ECC521_SPACE_LEN;
        EccPara.u16BitLen = au32Para[i];
        EccPara.u32Crc = MHCRC_CalcBuff(0xffff, &EccPara, sizeof(MHECC_PARAM)-4); 
        MHECC_ParamInit(&EccPara);
        
        //genkey
        KeySrc.pu8D = au8Pri;
        KeySrc.pu8PubX = au8PubX;
        KeySrc.pu8PubY = au8PubY;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        u32Ret = MHECC_GenKey(&KeySrc, &EccPara);
        if(MH_RET_ECC_SUCCESS != u32Ret)
        {
            printf("errRet = %08x\n", u32Ret);
            r_printf(0, "MHECC_GenKey test\n");
        }
        
        //set key
        KeySrc.pu8D = au8Pri;
        KeySrc.pu8PubX = au8PubX;
        KeySrc.pu8PubY = au8PubY;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        MHECC_SetKey(&KeySrc,&InBase,&u8Base,&EccPara);
        
        SignRs.pu8SignR = au8SigR;
        SignRs.pu8SignS = au8SigS;
        SignCall.pu8Sign = &SignRs;
        SignCall.pInBase = &InBase;
        SignCall.pParam = &EccPara;
        SignCall.pu8E = au8E;
        SignCall.pu8Msg = au8Msg;
        SignCall.u16ELen = sizeof(au8E);
        SignCall.u32MLen = u16MsgLen;

        MHECC_HashE(&SignCall);
        SignCall.u32Crc = MHCRC_CalcBuff(0xffff, &SignCall, sizeof(MHECC_SIG_VER_CALL)-4);         
        
        u32Ret = MHECC_EcdsaSign(&SignCall);
        if(u32Ret != MH_RET_ECC_SUCCESS)
        {
            printf("errRet = %08x\n", u32Ret);
            r_printf(0, "mh_ecc sign_B test\n");
        }
        SignCall.u32Crc = MHCRC_CalcBuff(0xffff, &SignCall, sizeof(MHECC_SIG_VER_CALL)-4);  
        
        u32Ret = MHECC_EcdsaVerify(&SignCall);
        if(MH_RET_ECC_SUCCESS!= u32Ret)
        {
            printf("errRet = %08x\n", u32Ret);
            r_printf(0, "mh_ecc sign/verify_B test\n");
        }
        r_printf(1, "mh_ecc sign/verify_B test\n");
    }
}

static void ECC_PointMul_Test()
{
    static uint32_t au32Kbits[5] = {192,224,256,384,521};
    uint32_t u32Ret,u32Klen, u32OffLen;
    uint32_t au32Para[PARA_NUM] = {192,224,256,384,521};
    uint8_t au8P[KEY_LEN_MAX],au8A[KEY_LEN_MAX],au8B[KEY_LEN_MAX],au8N[KEY_LEN_MAX],au8GX[KEY_LEN_MAX],au8GY[KEY_LEN_MAX],au8P2X[KEY_LEN_MAX];
    uint8_t au8Pri[KEY_LEN_MAX],au8P1X[KEY_LEN_MAX],au8P1Y[KEY_LEN_MAX],au8Space[MH_ECC521_SPACE_LEN],au8P2Y[KEY_LEN_MAX];
    uint8_t au8K[KEY_LEN_MAX], au8T[KEY_LEN_MAX];
    MHECC_POINT PointR, PointA;
    MHBN_BIGNUM BigK, BigD, BigT, BigN;
    uint8_t i = 0;
    uint16_t u16Byte;
    MHECC_PARAM EccPara;
    MHECC_KEY   KeySrc;


    for(i = 0; i < PARA_NUM; i++)
    {
        printf("\r\n%d bits ecc point mul  test\r\n",au32Para[i]);
        memset(&EccPara,0,sizeof(EccPara));
        if(au32Para[i] == 192)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_192r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_192r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_192r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_192r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_192r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_192r1); 
        }
        else if(au32Para[i] == 224)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_224r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_224r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_224r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_224r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_224r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_224r1); 
        }
        else if(au32Para[i] == 256)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_256k1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_256k1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_256k1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_256k1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_256k1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_256k1); 
        }
        else if(au32Para[i] == 384)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_384r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_384r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_384r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_384r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_384r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_384r1);
         
        }
        else if(au32Para[i] == 521)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_521r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_521r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_521r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_521r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_521r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_521r1); 
        }
        
        //traversal Klen
//        au32Kbits[i] = (au32Kbits[i]-1)%au32Para[i];
        au32Kbits[i] = au32Para[i];
        if(au32Kbits[i] < 64)
        {
            au32Kbits[i] = au32Para[i];
        }
		
        u32Klen = (au32Kbits[i] + 7) >> 3;
        u32OffLen = ((au32Para[i] + 7)>>3) - u32Klen;
        memset(au8Pri, 0 , sizeof(au8Pri));
        MHRAND_Prand(au8Pri + u32OffLen, u32Klen);
        if(au32Kbits[i] & 0x07)
        {
            au8Pri[u32OffLen] &= (1<<(au32Kbits[i] & 0x07)) - 1;
        }
        
        memset(&KeySrc,0,sizeof(KeySrc));
        EccPara.pu8P = au8P;
        EccPara.pu8A = au8A;
        EccPara.pu8B = au8B;
        EccPara.pu8N = au8N;
        EccPara.pu8Gx = au8GX;
        EccPara.pu8Gy = au8GY;
        EccPara.pu8Space = au8Space;
        EccPara.u32SpaceLen = MH_ECC521_SPACE_LEN;
        EccPara.u16BitLen = au32Para[i];
        EccPara.u32Crc = MHCRC_CalcBuff(0xffff, &EccPara, sizeof(MHECC_PARAM)-4); 
        MHECC_ParamInit(&EccPara);

        memset(au8P1X, 0, sizeof(au8P1X));
        memset(au8P1Y, 0, sizeof(au8P1Y));
        
        u16Byte = (au32Para[i] + 7)>>3;
        //genkey
        KeySrc.pu8D = au8Pri;
        KeySrc.pu8PubX = au8P1X;
        KeySrc.pu8PubY = au8P1Y;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        u32Ret = MHECC_CompleteKey(&KeySrc, &EccPara);
        if(u32Ret != MH_RET_ECC_SUCCESS)
        {
            printf("errRet = %08x\n", u32Ret);
            ouputRes("au8Pri.\n", au8Pri, u16Byte);
            while(1);
        }
        
        memset(au8K, 0 , sizeof(au8K));
        MHRAND_Prand(au8K + u32OffLen, u32Klen);
        if(au32Kbits[i] & 0x07)
        {
            au8K[u32OffLen] &= (1<<(au32Kbits[i] & 0x07)) - 1;
        }
        
        PointA.pu8X = au8P1X;
        PointA.pu8Y = au8P1Y;
        PointR.pu8X = au8P2X;
        PointR.pu8Y = au8P2Y;
        BigK.pu8Data = au8K;
        BigK.u32Len = u16Byte;
        u32Ret = MHECC_PointMul(&PointR, &PointA, &BigK, &EccPara);
        if(u32Ret != MH_RET_ECC_POINT_SUCCESS)
        {
            printf("errRet = %08x\n", u32Ret);
            ouputRes("au8Pri.\n", au8K, u16Byte);
            while(1);
        }
        
        BigT.pu8Data = au8T;
        BigT.u32Len = u16Byte;
        BigD.pu8Data = au8Pri;
        BigD.u32Len = u16Byte;
        BigN.pu8Data = au8N;
        BigN.u32Len = u16Byte;
        MHBN_BnModMul(&BigT, &BigD, &BigK, &BigN);
//        ouputRes("au8T.\n", au8T, u16Byte);
//        ouputRes("BigD.\n", au8Pri, u16Byte);
//        ouputRes("BigK.\n", au8K, u16Byte);
//        ouputRes("au8N.\n", au8N, u16Byte);
        
        //genkey
        KeySrc.pu8D = au8T;
        KeySrc.pu8PubX = au8P1X;
        KeySrc.pu8PubY = au8P1Y;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        u32Ret = MHECC_CompleteKey(&KeySrc, &EccPara);
        if(u32Ret != MH_RET_ECC_SUCCESS)
        {
            printf("errRet = %08x\n", u32Ret);
            ouputRes("au8Pri.\n", au8Pri, u16Byte);
            while(1);
        }
        r_printf(0 == memcmp(au8P1X, au8P2X, u16Byte) && 0 == (memcmp(au8P1Y, au8P2Y, u16Byte)),"ecc point mul test\n");
    }
}

static void ECC_PointAdd_Test()
{
    static uint32_t au32Kbits[5] = {192,224,256,384,521};
    uint32_t u32Ret,u32Klen, u32OffLen;
    uint32_t au32Para[PARA_NUM] = {192,224,256,384,521};
    uint8_t au8P[KEY_LEN_MAX],au8A[KEY_LEN_MAX],au8B[KEY_LEN_MAX],au8N[KEY_LEN_MAX],au8GX[KEY_LEN_MAX],au8GY[KEY_LEN_MAX];
    uint8_t au8P1X[KEY_LEN_MAX],au8P1Y[KEY_LEN_MAX],au8P2X[KEY_LEN_MAX],au8P2Y[KEY_LEN_MAX],au8RX[KEY_LEN_MAX],au8RY[KEY_LEN_MAX];
    uint8_t au8Space[MH_ECC521_SPACE_LEN],au8K1[KEY_LEN_MAX],au8K2[KEY_LEN_MAX], au8T[KEY_LEN_MAX];
    uint16_t i, u16Byte;
    MHECC_POINT PointR, PointA, PointB;
    MHBN_BIGNUM BigK1, BigK2, BigN, BigT;
    MHECC_PARAM EccPara;
    MHECC_KEY   KeySrc;

    for(i = 0; i < PARA_NUM; i++)
    {
        printf("\r\n%d bits ecc point add  test\r\n",au32Para[i]);
        memset(&EccPara,0,sizeof(EccPara));
        if(au32Para[i] == 192)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_192r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_192r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_192r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_192r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_192r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_192r1); 
        }
        else if(au32Para[i] == 224)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_224r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_224r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_224r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_224r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_224r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_224r1); 
        }
        else if(au32Para[i] == 256)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_256k1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_256k1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_256k1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_256k1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_256k1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_256k1); 
        }
        else if(au32Para[i] == 384)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_384r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_384r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_384r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_384r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_384r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_384r1);
         
        }
        else if(au32Para[i] == 521)
        {
            bn_read_string_from_head(au8P, sizeof(au8P), ECC_P_521r1);  
            bn_read_string_from_head(au8A, sizeof(au8A), ECC_A_521r1);
            bn_read_string_from_head(au8B, sizeof(au8B), ECC_B_521r1);
            bn_read_string_from_head(au8N, sizeof(au8N), ECC_N_521r1);  
            bn_read_string_from_head(au8GX, sizeof(au8GX),ECC_GX_521r1);
            bn_read_string_from_head(au8GY, sizeof(au8GY),ECC_GY_521r1); 
        }
        
        //traversal Klen
        au32Kbits[i] = (au32Kbits[i]-1)%au32Para[i];
        if(au32Kbits[i] < 64)
        {
            au32Kbits[i] = au32Para[i];
        }
    
        u32Klen = (au32Kbits[i] + 7) >> 3;
        u32OffLen = ((au32Para[i] + 7)>>3) - u32Klen;
        memset(au8K1, 0 , sizeof(au8K1));
        MHRAND_Prand(au8K1 + u32OffLen, u32Klen);
        if(au32Kbits[i] & 0x07)
        {
            au8K1[u32OffLen] &= (1<<(au32Kbits[i] & 0x07)) - 1;
        }
        
        memset(&KeySrc,0,sizeof(KeySrc));

        EccPara.pu8P = au8P;
        EccPara.pu8A = au8A;
        EccPara.pu8B = au8B;
        EccPara.pu8N = au8N;
        EccPara.pu8Gx = au8GX;
        EccPara.pu8Gy = au8GY;
        EccPara.pu8Space = au8Space;
        EccPara.u32SpaceLen = MH_ECC521_SPACE_LEN;
        EccPara.u16BitLen = au32Para[i];
        EccPara.u32Crc = MHCRC_CalcBuff(0xffff, &EccPara, sizeof(MHECC_PARAM)-4); 
        MHECC_ParamInit(&EccPara);
        
        u16Byte = (au32Para[i] + 7)>>3;

        //genkey
        KeySrc.pu8D = au8K1;
        KeySrc.pu8PubX = au8P1X;
        KeySrc.pu8PubY = au8P1Y;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        u32Ret = MHECC_CompleteKey(&KeySrc, &EccPara);
        if(u32Ret != MH_RET_ECC_SUCCESS)
        {
            printf("errRet = %08x\n", u32Ret);
            ouputRes("au8Pri.\n", au8K1, u16Byte);
            while(1);
        }

        memset(au8K2, 0 , sizeof(au8K2));
        MHRAND_Prand(au8K2 + u32OffLen, u32Klen);
        if(au32Kbits[i] & 0x07)
        {
            au8K2[u32OffLen] &= (1<<(au32Kbits[i] & 0x07)) - 1;
        }
        //genkey
        KeySrc.pu8D = au8K2;
        KeySrc.pu8PubX = au8P2X;
        KeySrc.pu8PubY = au8P2Y;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        u32Ret = MHECC_CompleteKey(&KeySrc, &EccPara);
        if(u32Ret != MH_RET_ECC_SUCCESS)
        {
            printf("errRet = %08x\n", u32Ret);
            ouputRes("au8Pri.\n", au8K2, u16Byte);
            while(1);
        }
  
        PointA.pu8X = au8P1X;
        PointA.pu8Y = au8P1Y;
        PointB.pu8X = au8P2X;
        PointB.pu8Y = au8P2Y;
        PointR.pu8X = au8RX;
        PointR.pu8Y = au8RY;
        MHECC_PointAdd(&PointR, &PointA, &PointB, &EccPara);
        
        
        BigT.pu8Data = au8T;
        BigT.u32Len = u16Byte;
        BigK1.pu8Data = au8K1;
        BigK1.u32Len = u16Byte;
        BigK2.pu8Data = au8K2;
        BigK2.u32Len = u16Byte;
        BigN.pu8Data = au8N;
        BigN.u32Len = u16Byte;
        MHBN_BnModAdd(&BigT, &BigK1, &BigK2, &BigN);
        
        //genkey
        KeySrc.pu8D = au8T;
        KeySrc.pu8PubX = au8P1X;
        KeySrc.pu8PubY = au8P1Y;
        KeySrc.u16Bits = au32Para[i];
        KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MHECC_KEY)-4); 
        u32Ret = MHECC_CompleteKey(&KeySrc, &EccPara);
        if(u32Ret != MH_RET_ECC_SUCCESS)
        {
            printf("errRet = %08x\n", u32Ret);
            ouputRes("au8Pri.\n", au8T, u16Byte);
            while(1);
        }
        r_printf(0 == memcmp(au8P1X, au8RX, u16Byte) && 0 == (memcmp(au8P1Y, au8RY, u16Byte)),"ecc point add test\n");    
    }
}

void ECC_Test()
{
    ECC_EcdhTest();
    ECC_SignTest();
    ECC_EciesTest();
    ECC_SignTest_Brain();
    ECC_PointMul_Test();
    ECC_PointAdd_Test();
}
