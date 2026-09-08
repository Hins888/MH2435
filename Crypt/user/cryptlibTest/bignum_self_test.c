#include "stdio.h"
#include "stdint.h"
#ifndef USE_CRYPT_LIB
#include "mh_crypt_ip.h"
#endif
#include "mh_bignum.h"
#include "mh_bignum_tool.h"
#include "mh_misc.h"
#include "mh_rand.h"
#include "crypt_debug.h"
#include "mh_crypt_bignum.h"
#include "mh_rsa.h"
#include "mh2435.h"

#define MAX_SIZE   32
#define GEN_DIV_PARA_COUNT 1000
#define DIV_TEST_COUNT     5


#define PARA_BYTE  32
#define PARA_A    "C46841E769D7F6A7599046C029900CB6D47BBD54BE83AF50284584BC1E390BF9"
#define PARA_B    "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93"
//#define PARA_A  "0FC9EEFF11BA052C407E7ECC4B6F25E210155DBA7DEE604C2B047D7DC9261DFC29EC7CD491188E68E04EA31CC7A14F6C856CE91E9A379CABDF45DD66C44ADE801B487D61B474F2DF6ACCB480BFD1C27E8BE2070773B193B3A6775A38FE5F045727940D83DCAD2D187A20D68BEFA0D21B8768E3D60EDE1A03F3A319FFA3CBE0C7DDCB1F94B2D151E74421B2402DAC59CE2CD733B7FE5BBF21D9A20BCD0EABCA582C1A3097AADED41664F128055B7C0868A38A27CE4E81B63BB8B35C2295F9C37304C16A7F75D103BC8280A318DF8BD60337A6E84001218BD5B9B6E105F22563FD590E61D02BB9515A503B106E711A992508EB2530B0ECB6A9A1455A871BCC87E9"
//#define PARA_B  "0BD34E32CFBAB12D49293926CE039A5E6A31389649A66A698A967F5B07D534FC7EFC356266FE7FCB4D6058AF452FFD17E728C3855F9A0E8CF33BA287C82178B0328B1E18D9309B0CC0436E48B8459B9268AECF72CF5B008505FA4CDD70FAA30119E799151E377B42EFBA0DFBAF2E8B4BAEED42F96620EDD43DEF46AA22C678BC"
#define MUL_A   "C923E91244FE7E411581E3E8CCFC6651271D1EC0A135B0A63AEF5705F8AEC7D0D93DBE779176C1BA28F6E4C439395EF7D7A7FDC48A3D532CBDA75751BAC568A67560D65A585FB22663F3BA2BE2CDA97FF5E7D58171322FD41146B8FF8CAA9F1938E38795B75FBB5C"
#define MUL_B   "16F5A714D7B2EB6C"


#define PARA_N  "A25AC26D47E943DF81742D6C5DCFB1AC6BD6F9592C441DA4A437CC395C0D2A2473FFDE2B71F889216CABCEC7DD1121F615172F16B0C4D7A9931AE93C581CE6C2AA4CF4F4236A6353"
#define PARA_NC "2527CADF7EC2D097"


#define MOD_P        "75A18418B8EB30CD990303626314215772406FB53C90BC34F8963643CB7C86655174E82AC581EA9D93FBCD8B9B9623E9FE8AD5E2F9311C8FEEFB709D78CC961DC49A3CA5F55733E68A10563695782DE1C8BE5F031E84540E14AA369FA8E03D03CCC7E02B2A030C44AF4B989DBC99640BB4925B9C3AAE287C241C1AF133502AD61593C62FAB42604680FBE1EC77C98B68008C15E52C24035A8F220C1B944C547F1E0FA4C6475395839536FD286F23063E802D657C6D176B46E3B1862616D32EADB0DC1C1AF83EAF33640224658BB46052ACD8D8876A7F35A9232ED4AD2E15FD958F294A5BFFBA11E2E18F7D3377136F6C91B93C9656E289E19FE3E928FDF7FB7F"
#define MOD_B        "65B37280142A48FCED5C720FC0A17DB2A6B6C1901CD1FECA252FEB61E5CF58E4068800EE4F54D2D9E35566BC5CB976B497E65FFA529F398E562BB38AE891A03B212D88E98073DE85A5A00D9B2CA11D0B14AF22CD2C832FD8DCD5E4ADC6BA95FFC7FBEBC80FE9D32ACB4EB6F52C742B11F11D83655E30B5FC786E15CC8C8D1C9F9EF66284D337B8A697E4F8485064D28823FD0DB4E75F5618437D776D6910405AB88B467F2E5DD4C206C670807E25227E61D02EE32B37C731DA1DF141D36CC48B636867C37C6A08C17EE8416A0A33A0791EE2233C3915EC38B2DA1984DE95F00BEAA1575DBD1ACCF4F8DB37500656BE4CD7A874F6247A127C2F32A046197D4D2A"
#define MOD_A        "1AF0D18F357B883EDC58A9FBEE1DA7209BCE173BDA631D69D04836776CE6EDAA8613288505A31501F033B22ACCE8068A987F90CC6469F35201A9E8E4810C1DD812BF96365833E44B82915C0AF25683A02F204DAE8DE93D9EB836B6EADC0E41BA66380EECF797B43EDFFF9714EE314C74F6475E33727768ED85871E1304EB11D505475398875DBE383CEE96C32804444E258547A7BE969185AAA0DBB4E654926B36C77D8AE2D429CDBA4A88A52823526AFD3AFDF16F328304348A528C719BB0CEC0A05ABF4825A184F28B77C304CB456E8C216AEA282FC533D84BC56D61D24DBEFDCAE91A418FD6B8300B47E2E25C66591B149C03BC6CCDC325B55D85AFD2C772"
#define MOD_PC       "813BECFA66F1F46C"

#ifndef USE_CRYPT_LIB

#define     BN_TEST_LEN     (512+64)

void Add_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8R1[BN_TEST_LEN], au8R2[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigB, BigR;
    uint8_t u8A, u8B, u8R, u8Word, u8DWord, u8Item, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t u32Ret = 0;
//    do
//    {
//        MHRAND_Prand(&u16SLenA, 2);
//        u16SLenA = (u16SLenA & 0x1ff) + 1;
//        MHRAND_Prand(au8A, u16SLenA);
//        MHRAND_Prand(&u16SLenB, 2);
//        u16SLenB = (u16SLenB & 0x1ff) + 1;
//        MHRAND_Prand(au8B, u16SLenB);
//        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
//    }while(u16SLen < 16);
//    
//    BigA.pu8Data = au8A;
//    BigA.u32Len = u16SLenA;
//    BigB.pu8Data = au8B;
//    BigB.u32Len = u16SLenB;
//    BigR.pu8Data = au8R1;
//    BigR.u32Len = u16SLen;
//    u32Ret = MHBN_BnAdd(&BigR, &BigA, &BigB);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
//    
//    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
//    u8DWord = MH_ROUND_UP(u8Word, 2);
//    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
//    u8A  = u8Spare; u8Spare += u8Item;
//    u8B  = u8Spare; u8Spare += u8Item;
//    u8R  = u8Spare; u8Spare += u8Item;
//    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
//    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
//    u32Ret = MHBN_Add(u8R,u8A,u8B,u8Word);
//    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWord<<2);
////    DBG_PRINT("u32Ret = %x\n",u32Ret);
////    ouputRes("A.\n", au8A, u16SLenA);
////    ouputRes("B.\n", au8B, u16SLenB);
////    ouputRes("R1.\n", au8R1, u16SLen);
////    ouputRes("R2.\n", au8R2, u16SLen);
//    r_printf((0 == memcmp(au8R1, au8R2, u16SLen)), "bignum add test \n");
//		
		u16SLen = 512+32;
		u16SLenA = u16SLenB = u16SLen;
    MHRAND_Prand(au8A, u16SLenA);
    MHRAND_Prand(au8B, u16SLenB);
//		BigA.pu8Data = au8A;
//    BigA.u32Len = u16SLenA;
//    BigB.pu8Data = au8B;
//    BigB.u32Len = u16SLenB;
//    BigR.pu8Data = au8R1;
//    BigR.u32Len = u16SLen;
//    u32Ret = MHBN_BnAdd(&BigR, &BigA, &BigB);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
        ouputRes("A.\n", au8A, u16SLenA);
    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("R1.\n", au8R1, u16SLen);
//    ouputRes("R2.\n", au8R2, u16SLen);
    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = u8Spare; u8Spare += u8Item;
    u8B  = u8Spare; u8Spare += u8Item;
    u8R  = u8Spare; u8Spare += u8Item;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
    u32Ret = MHBN_Add(u8R,u8A,u8B,u8Word);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWord<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
		r_printf((u32Ret == MH_RET_CRYPT_SUCCESS), "bignum add test \n");
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("R1.\n", au8R1, u16SLen);
//    ouputRes("R2.\n", au8R2, u16SLen);
//    r_printf((0 == memcmp(au8R1, au8R2, u16SLen)), "bignum add test \n");
		
		
}

void Sub_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8R1[BN_TEST_LEN], au8R2[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigB, BigR;
    uint8_t u8A, u8B, u8R, u8Word, u8DWord, u8Item, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t u32Ret = 0;
    do
    {
        MHRAND_Prand(&u16SLenA, 2);
        u16SLenA = (u16SLenA & 0x1ff) + 1;
        MHRAND_Prand(au8A, u16SLenA);
        MHRAND_Prand(&u16SLenB, 2);
        u16SLenB = (u16SLenB & 0x1ff) + 1;
        MHRAND_Prand(au8B, u16SLenB);
        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
    }while(u16SLen < 16);
    BigA.pu8Data = au8A;
    BigA.u32Len = u16SLenA;
    BigB.pu8Data = au8B;
    BigB.u32Len = u16SLenB;
    BigR.pu8Data = au8R1;
    BigR.u32Len = u16SLen;
    u32Ret = MHBN_BnSub(&BigR, &BigA, &BigB);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    
    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = u8Spare; u8Spare += u8Item;
    u8B  = u8Spare; u8Spare += u8Item;
    u8R  = u8Spare; u8Spare += u8Item;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
    u32Ret = MHBN_Sub(u8R,u8A,u8B,u8Word);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWord<<2);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("R1.\n", au8R1, u16SLen);
//    ouputRes("R2.\n", au8R2, u16SLen);
    r_printf((0 == memcmp(au8R1, au8R2, u16SLen)), "bignum sub test \n");
}

#if 0
void ModAdd_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8P[BN_TEST_LEN], au8R1[BN_TEST_LEN], au8R2[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigB, BigP, BigR;
    uint8_t u8A, u8B, u8P, u8R, u8Word, u8DWord, u8Item, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t u32Ret = 0;
    do
    {
        MHRAND_Prand(&u16SLenA, 2);
        u16SLenA = (u16SLenA & 0x1ff) + 1;
        MHRAND_Prand(au8A, u16SLenA);
        MHRAND_Prand(&u16SLenB, 2);
        u16SLenB = (u16SLenB & 0x1ff) + 1;
        MHRAND_Prand(au8B, u16SLenB);
        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
        MHRAND_Prand(au8P, u16SLen);
    }while(u16SLen < 16);
    //make sure P is bigger than A B
    au8P[0] = 0xff;
    BigA.pu8Data = au8A;
    BigA.u32Len = u16SLenA;
    BigB.pu8Data = au8B;
    BigB.u32Len = u16SLenB;
    BigP.pu8Data = au8P;
    BigP.u32Len = u16SLen;
    BigR.pu8Data = au8R1;
    BigR.u32Len = u16SLen;
    u32Ret = MHBN_BnModAdd(&BigR, &BigA, &BigB, &BigP);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = u8Spare; u8Spare += u8Item;
    u8B  = u8Spare; u8Spare += u8Item;
    u8P  = u8Spare; u8Spare += u8Item;
    u8R  = u8Spare; u8Spare += u8Item;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8P), au8P, u8DWord<<2, u16SLen);
    u32Ret = MHBN_ModAdd(u8R,u8A,u8B,u8P,u8Word);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWord<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au8A, u16SLenA);
    ouputRes("B.\n", au8B, u16SLenB);
    ouputRes("P.\n", au8P, u16SLen);
    ouputRes("R1.\n", au8R1, u16SLen);
    ouputRes("R2.\n", au8R2, u16SLen);
    r_printf((0 == memcmp(au8R1, au8R2, u16SLen)), "bignum mod add test \n");
}
#else
#define MOD_ADD_P "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF"
#define MOD_ADD_A "C839E174EC0EE51ACBB5E43ACBC67B5A10726CF515C8D6650D48B3117AAC96E3"
#define MOD_ADD_B "9622A64E03A9CEFC4DA7F7255BA300BB6607336ABC8F35790AF82F1ABD8A754D"
#define MOD_ADD_R "5E5C87C3EFB8B417195DDB6027697C157679A060D2580BDD1840E22C38370C31"
void ModAdd_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8P[BN_TEST_LEN], au8R[BN_TEST_LEN];
    uint8_t u8A, u8B, u8P, u8R, u8Word, u8DWord, u8Item, u8Spare = 0; 
	uint16_t u16Len;
	uint32_t u32Ret;
    //make sure P is bigger than A B
	
    DBG_PRINT("u32Ret = %x\n",u32Ret);
	u16Len = bn_read_string_from_head((uint8_t *)au8P, sizeof(au8P), MOD_ADD_P);
	bn_read_string_from_head((uint8_t *)au8A, sizeof(au8A), MOD_ADD_A);
	bn_read_string_from_head((uint8_t *)au8B, sizeof(au8B), MOD_ADD_B);
    u8Word = MH_ROUND_UP(u16Len, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = 4;
    u8B  = 7;
    u8P  = 2;
    u8R  = 4;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16Len);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16Len);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8P), au8P, u8DWord<<2, u16Len);
    u32Ret = MHBN_ModAdd(u8R,u8A,u8B,u8P,u8Word);
    MHMEM_CopyBig(au8R, MH_ITEM_TO_MEM(u8R), u16Len, u8DWord<<2);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au8A, u16Len);
    ouputRes("B.\n", au8B, u16Len);
    ouputRes("P.\n", au8P, u16Len);
    ouputRes("R.\n", au8R, u16Len);
	while(1);
//    r_printf((0 == memcmp(au8R1, au8R2, u16SLen)), "bignum mod add test \n");
}
#endif

void ModSub_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8P[BN_TEST_LEN], au8R1[BN_TEST_LEN], au8R2[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigB, BigP, BigR;
    uint8_t u8A, u8B, u8P, u8R, u8Word, u8DWord, u8Item, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t u32Ret = 0;
    do
    {
        MHRAND_Prand(&u16SLenA, 2);
        u16SLenA = (u16SLenA & 0x1ff) + 1;
        MHRAND_Prand(au8A, u16SLenA);
        MHRAND_Prand(&u16SLenB, 2);
        u16SLenB = (u16SLenB & 0x1ff) + 1;
        MHRAND_Prand(au8B, u16SLenB);
        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
        MHRAND_Prand(au8P, u16SLen);
    }while(u16SLen < 16);
    //make sure P is bigger than A B
    au8P[0] = 0xff;
    BigA.pu8Data = au8A;
    BigA.u32Len = u16SLenA;
    BigB.pu8Data = au8B;
    BigB.u32Len = u16SLenB;
    BigP.pu8Data = au8P;
    BigP.u32Len = u16SLen;
    BigR.pu8Data = au8R1;
    BigR.u32Len = u16SLen;
    u32Ret = MHBN_BnModSub(&BigR, &BigA, &BigB, &BigP);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = u8Spare; u8Spare += u8Item;
    u8B  = u8Spare; u8Spare += u8Item;
    u8P  = u8Spare; u8Spare += u8Item;
    u8R  = u8Spare; u8Spare += u8Item;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8P), au8P, u8DWord<<2, u16SLen);
    u32Ret = MHBN_ModSub(u8R,u8A,u8B,u8P,u8Word);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWord<<2);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("P.\n", au8P, u16SLen);
//    ouputRes("R1.\n", au8R1, u16SLen);
//    ouputRes("R2.\n", au8R2, u16SLen);
    r_printf((0 == memcmp(au8R1, au8R2, u16SLen)), "bignum mod sub test \n");
}

void Mul_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8R1[BN_TEST_LEN*2], au8R2[BN_TEST_LEN*2];
    MHBN_BIGNUM BigA, BigB, BigR;
    uint8_t u8A, u8B, u8R, u8Word, u8DWord, u8Item, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t u32Ret = 0;
    do
    {
        MHRAND_Prand(&u16SLenA, 2);
        u16SLenA = (u16SLenA & 0xff) + 1;
        MHRAND_Prand(au8A, u16SLenA);
        MHRAND_Prand(&u16SLenB, 2);
        u16SLenB = (u16SLenB & 0xff) + 1;
        MHRAND_Prand(au8B, u16SLenB);
        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
    }while(u16SLen < 16);
    BigA.pu8Data = au8A;
    BigA.u32Len = u16SLenA;
    BigB.pu8Data = au8B;
    BigB.u32Len = u16SLenB;
    BigR.pu8Data = au8R1;
    BigR.u32Len = u16SLenA + u16SLenB;
    u32Ret = MHBN_BnMul(&BigR, &BigA, &BigB);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = u8Spare; u8Spare += u8Item;
    u8B  = u8Spare; u8Spare += u8Item;
    u8R  = u8Spare; u8Spare += u8Item*2;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
    u32Ret = MHBN_Mul(u8R,u8A,u8B,u8Word);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLenA + u16SLenB, u8DWord<<3);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("R1.\n", au8R1, u16SLenA + u16SLenB);
//    ouputRes("R2.\n", au8R2, u16SLenA + u16SLenB);
    r_printf((0 == memcmp(au8R1, au8R2, u16SLen)), "bignum mul test \n");
}

void ModMul_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8P[BN_TEST_LEN], au8R1[BN_TEST_LEN], au8R2[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigB, BigP, BigR;
    MHBN_MOD BnMod;
    uint8_t u8A, u8B, u8P, u8PD, u8R, u8Word, u8DWord, u8Item, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t au32PC[2], u32Ret = 0;
    do
    {
        MHRAND_Prand(&u16SLenA, 2);
        u16SLenA = (u16SLenA & 0x1ff) + 1;
        MHRAND_Prand(au8A, u16SLenA);
        MHRAND_Prand(&u16SLenB, 2);
        u16SLenB = (u16SLenB & 0x1ff) + 1;
        MHRAND_Prand(au8B, u16SLenB);
        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
        MHRAND_Prand(au8P, u16SLen);
    }while(u16SLen < 16);
    //make sure P is bigger than A B    and P is odd
    au8P[0] = 0xff;
    au8P[u16SLen - 1] |= 0x01;
    BigA.pu8Data = au8A;
    BigA.u32Len = u16SLenA;
    BigB.pu8Data = au8B;
    BigB.u32Len = u16SLenB;
    BigP.pu8Data = au8P;
    BigP.u32Len = u16SLen;
    BigR.pu8Data = au8R1;
    BigR.u32Len = u16SLen;
    u32Ret = MHBN_BnModMul(&BigR, &BigA, &BigB, &BigP);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = u8Spare; u8Spare += u8Item;
    u8B  = u8Spare; u8Spare += u8Item;
    u8P  = u8Spare; u8Spare += u8Item;
    u8PD = u8Spare;  u8Spare += u8Item;
    u8R  = u8Spare; u8Spare += u8Item;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8P), au8P, u8DWord<<2, u16SLen);
    
    MHBN_GetModMulConst(au32PC, u8P, u8DWord);
    MHBN_GetDomainConst(u8PD, u8P, au32PC, u8DWord, u8Spare);
    BnMod.u8P = u8P;    BnMod.u8PD = u8PD;      BnMod.pu32PC = au32PC;
    u32Ret = MHBN_ModMul(u8R,u8A,u8B,&BnMod,u8Word);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWord<<2);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("P.\n", au8P, u16SLen);
//    ouputRes("R1.\n", au8R1, u16SLen);
//    ouputRes("R2.\n", au8R2, u16SLen);
    r_printf((0 == memcmp(au8R1, au8R2, u16SLen)), "bignum mod mul test \n");
}

#define DIV_P "DB17D6351D7540BC7917BFE70B846FE04FBD740BB334048E69916099433ADBF2"
#define DIV_D "4560A6EAF7A7EB797662ACE5FBF84DC98EC9FF7EE925025AC4FEBA66FB3DA3E5F89655E53ED288DA38398C1FE99CB43B4366650C177B7EB53F65164E87A6B175"
#define DIV_Q "51107C9E01CEC2A8735F67D44B1786B77330EFA8EC6F025F645381A8C6D9E623"
void Div_fixed_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8Q1[BN_TEST_LEN], au8Q2[BN_TEST_LEN], au8R1[BN_TEST_LEN], au8R2[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigB, BigR, BigQ;
    uint8_t u8A, u8B, u8R, u8Q, u8Word, u8DWord, u8Item, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t u32Ret = 0;

    
		GPIO_SetBits(GPIOB, GPIO_Pin_3);
	
    bn_read_string_from_head(au8A, 64, DIV_D); 
    bn_read_string_from_head(au8B, 32, DIV_P); 
	
    bn_read_string_from_head(au8R1, 32, DIV_Q); 
	
//MH_RSA_SCHK_RET(MHBN_Div(u8TmpQ, u8TmpR, pInBase->u8D, u8Tmp, u8NWord),MH_RET_CRYPT_SUCCESS); 
		u16SLen = 64;
		u16SLenA = 64;
		u16SLenB = 32;
    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = 0xF8;
    u8B  = 0x00;
    u8Q  = 0x02;
    u8R  = 0x04;
	
		ASYMC_CRYPT_CFG->RESCR = 0x000;
		
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, 64, 64);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, 64, 32);
		
    ouputRes("A.\n", au8A, u16SLenA);
    ouputRes("B.\n", au8B, u16SLenB);
		ouputRes("CRAM A.\n", MH_ITEM_TO_MEM(u8A), u16SLenA);
    ouputRes("CRAM B.\n", MH_ITEM_TO_MEM(u8B), u16SLenA);
		
    u32Ret = MHBN_Div(u8Q,u8R,u8A,u8B,0x10);
		
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), 32, u8DWord<<2);
    MHMEM_CopyBig(au8Q2, MH_ITEM_TO_MEM(u8Q), 32, u8DWord<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au8A, u16SLenA);
    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("Q1.\n", au8Q1, u16SLen);
    ouputRes("Q2.\n", au8Q2, u16SLenB);
//    ouputRes("R1.\n", au8R1, u16SLenB);
    ouputRes("R2.\n", au8R2, u16SLenB);
//    r_printf((0 == memcmp(au8R1, au8R2, u16SLenB)), "bignum Div test \n");
//    r_printf((0 == memcmp(au8R1, au8Q2, 32)), "bignum Div test \n");
		if(0 == memcmp(au8R1, au8Q2, 32))
		{
			DBG_PRINT("First OK.\n");
			GPIO_SetBits(GPIOB, GPIO_Pin_4);
		}
		
		
		
		GPIO_ResetBits(GPIOB, GPIO_Pin_4);

		ASYMC_CRYPT_CFG->RESCR = 0x200;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, 64, 64);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, 64, 32);
		
		ouputRes("CRAM A.\n", MH_ITEM_TO_MEM(u8A), u16SLenA);
    ouputRes("CRAM B.\n", MH_ITEM_TO_MEM(u8B), u16SLenA);
		
    u32Ret = MHBN_Div(u8Q,u8R,u8A,u8B,0x10);
		
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), 32, u8DWord<<2);
    MHMEM_CopyBig(au8Q2, MH_ITEM_TO_MEM(u8Q), 32, u8DWord<<2);
		
		
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au8A, u16SLenA);
    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("Q1.\n", au8Q1, u16SLen);
    ouputRes("Q2.\n", au8Q2, u16SLenB);
//    ouputRes("R1.\n", au8R1, u16SLenB);
    ouputRes("R2.\n", au8R2, u16SLenB);
		
		if(0 == memcmp(au8R1, au8Q2, 32))
		{
			
			DBG_PRINT("Sec OK.\n");
			GPIO_SetBits(GPIOB, GPIO_Pin_4);
		}
		
		
//		GPIO_ResetBits(GPIOB, GPIO_Pin_4);
//		
//		ASYMC_CRYPT_CFG->RESCR = 0x00E;
//    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, 64, 64);
//    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, 64, 32);
//		
//    u32Ret = MHBN_Div(u8Q,u8R,u8A,u8B,0x10);
//		
//    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), 32, u8DWord<<2);
//    MHMEM_CopyBig(au8Q2, MH_ITEM_TO_MEM(u8Q), 32, u8DWord<<2);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("B.\n", au8B, u16SLenB);
////    ouputRes("Q1.\n", au8Q1, u16SLen);
//    ouputRes("Q2.\n", au8Q2, u16SLenB);
////    ouputRes("R1.\n", au8R1, u16SLenB);
//    ouputRes("R2.\n", au8R2, u16SLenB);
////    r_printf((0 == memcmp(au8R1, au8R2, u16SLenB)), "bignum Div test \n");
////    r_printf((0 == memcmp(au8R1, au8Q2, 32)), "bignum Div test \n");
//		if(0 == memcmp(au8R1, au8Q2, 32))
//		{
//			DBG_PRINT("Thr OK.\n");
//			GPIO_SetBits(GPIOB, GPIO_Pin_4);
//		}
		
		GPIO_ResetBits(GPIOB, GPIO_Pin_4);
		GPIO_ResetBits(GPIOB, GPIO_Pin_3);
}

void Div_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8Q1[BN_TEST_LEN], au8Q2[BN_TEST_LEN], au8R1[BN_TEST_LEN], au8R2[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigB, BigR, BigQ;
    uint8_t u8A, u8B, u8R, u8Q, u8Word, u8DWord, u8Item, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t u32Ret = 0;
    do
    {
        MHRAND_Prand(&u16SLenA, 2);
        u16SLenA = (u16SLenA & 0x1ff) + 1;
        MHRAND_Prand(au8A, u16SLenA);
        MHRAND_Prand(&u16SLenB, 2);
        u16SLenB = (u16SLenB & 0x1ff) + 1;
        MHRAND_Prand(au8B, u16SLenB);
        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
    }while((u16SLen < 16) || (u16SLenA < u16SLenB));
    BigA.pu8Data = au8A;
    BigA.u32Len = u16SLenA;
    BigB.pu8Data = au8B;
    BigB.u32Len = u16SLenB;
    BigQ.pu8Data = au8Q1;
    BigQ.u32Len = u16SLen;
    BigR.pu8Data = au8R1;
    BigR.u32Len = u16SLenB;
    u32Ret = MHBN_BnDiv(&BigQ, &BigR, &BigA, &BigB);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    
    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = u8Spare; u8Spare += u8Item;
    u8B  = u8Spare; u8Spare += u8Item;
    u8Q  = u8Spare; u8Spare += u8Item;
    u8R  = u8Spare; u8Spare += u8Item;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
    u32Ret = MHBN_Div(u8Q,u8R,u8A,u8B,u8Word);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLenB, u8DWord<<2);
    MHMEM_CopyBig(au8Q2, MH_ITEM_TO_MEM(u8Q), u16SLen, u8DWord<<2);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("Q1.\n", au8Q1, u16SLen);
//    ouputRes("Q2.\n", au8Q2, u16SLen);
//    ouputRes("R1.\n", au8R1, u16SLenB);
//    ouputRes("R2.\n", au8R2, u16SLenB);
    r_printf((0 == memcmp(au8R1, au8R2, u16SLenB)), "bignum Div test \n");
    r_printf((0 == memcmp(au8Q1, au8Q2, u16SLen)), "bignum Div test \n");
}


#define EXP_A		"30E0D814E6851782B58CD3D3F999BB898F2DE174F5AC2B6456F55412D5C3B847A2C6A16FBF81EDCC3ACEBC439337E45DF83A8104E849E46424851AF5C74117FC"
#define EXP_B		"7EF35D23CF3C9E2007B1F0E76521266CFF7E6AA35670C37A28BF8F5312D4C0352657425684B681AA7D9730C5058593E751D22576505690147186EC67046D65F0"
#define EXP_P		"FFBB0179026579D5DABEE68E926EFD9FEBB93CE06D97E55D2FAD250D58BB7DD280A3F37E373F36FF71D0B4DC9A39F32CDD0378EE736E2C65ED27B849655F2463"

void ModExp_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8P[BN_TEST_LEN], au8R1[BN_TEST_LEN], au8R2[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigB, BigP, BigR;
    MHBN_MOD BnMod;
    uint8_t u8A, u8B, u8P, u8PD, u8R, u8WordB, u8WordP, u8DWordP, u8DWordB, u8ItemP, u8ItemB, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t au32PC[2], u32Ret = 0;
//    do
//    {
//        MHRAND_Prand(&u16SLenA, 2);
//        u16SLenA = (u16SLenA & 0x3f) + 1;
//        u16SLenA = 17;
//        MHRAND_Prand(au8A, u16SLenA);
//        MHRAND_Prand(&u16SLenB, 2);
//        u16SLenB = (u16SLenB & 0x3f) + 1;
//        u16SLenB = 5;
//        MHRAND_Prand(au8B, u16SLenB);
//        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
//        MHRAND_Prand(au8P, u16SLen);
//    }while(u16SLen < 16);
	u16SLen	 = 64;
	u16SLenB = u16SLenA = 64;
	bn_read_string_from_head((uint8_t *)au8A, sizeof(au8A), EXP_A);
	bn_read_string_from_head((uint8_t *)au8B, sizeof(au8B), EXP_B);
	bn_read_string_from_head((uint8_t *)au8P, sizeof(au8P), EXP_P);
//	MHRAND_Prand(au8P, u16SLen);
//	MHRAND_Prand(au8A, u16SLenA);
//	MHRAND_Prand(au8B, u16SLenB);
    //make sure P is bigger than A B    and P is odd
//    au8P[0] = 0xff;
//    au8P[u16SLen - 1] |= 0x01;
//    BigA.pu8Data = au8A;
//    BigA.u32Len = u16SLenA;
//    BigB.pu8Data = au8B;
//    BigB.u32Len = u16SLenB;
//    BigP.pu8Data = au8P;
//    BigP.u32Len = u16SLen;
//    BigR.pu8Data = au8R1;
//    BigR.u32Len = u16SLen;
//    u32Ret = MHBN_BnModExp(&BigR, &BigA, &BigB, &BigP);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);

    u8WordB = MH_ROUND_UP(u16SLenB, 16)>>2;
    u8DWordB = MH_ROUND_UP(u8WordB, 2);
    u8ItemB = MH_ROUND_UP(u8WordB, 8)>>3;
    u8WordP = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWordP = MH_ROUND_UP(u8WordP, 2);
    u8ItemP = MH_ROUND_UP(u8WordP, 8)>>3;
    u8A  = u8Spare; u8Spare += u8ItemP;
    u8B  = u8Spare; u8Spare += u8ItemB;
    u8P  = u8Spare; u8Spare += u8ItemP;
    u8PD = u8Spare;  u8Spare += u8ItemP;
    u8R  = u8Spare; u8Spare += u8ItemP;
		
		ASYMC_CRYPT_CFG->RESCR = 0;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWordP<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWordB<<2, u16SLenB);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8P), au8P, u8DWordP<<2, u16SLen);
    MHBN_GetModMulConst(au32PC, u8P, u8DWordP);
    MHBN_GetDomainConst(u8PD, u8P, au32PC, u8DWordP, u8Spare);
    BnMod.u8P = u8P;    BnMod.u8PD = u8PD;      BnMod.pu32PC = au32PC;
    u32Ret = MHBN_ModExp(u8R,u8A,u8B,&BnMod,u8WordB,u8WordP);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWordP<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au8A, u16SLenA);
    ouputRes("B.\n", au8B, u16SLenB);
    ouputRes("P.\n", au8P, u16SLen);
    ouputRes("R2.\n", au8R2, u16SLen);
		
		ASYMC_CRYPT_CFG->RESCR = 0x20A;
		MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWordP<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWordB<<2, u16SLenB);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8P), au8P, u8DWordP<<2, u16SLen);
		MHBN_GetModMulConst(au32PC, u8P, u8DWordP);
    MHBN_GetDomainConst(u8PD, u8P, au32PC, u8DWordP, u8Spare);
    BnMod.u8P = u8P;    BnMod.u8PD = u8PD;      BnMod.pu32PC = au32PC;
		u32Ret = MHBN_ModExp(u8R,u8A,u8B,&BnMod,u8WordB,u8WordP);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWordP<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("R2.\n", au8R2, u16SLen);
		
		ASYMC_CRYPT_CFG->RESCR = 0x20E;
		MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWordP<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWordB<<2, u16SLenB);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8P), au8P, u8DWordP<<2, u16SLen);
		MHBN_GetModMulConst(au32PC, u8P, u8DWordP);
    MHBN_GetDomainConst(u8PD, u8P, au32PC, u8DWordP, u8Spare);
    BnMod.u8P = u8P;    BnMod.u8PD = u8PD;      BnMod.pu32PC = au32PC;
		u32Ret = MHBN_ModExp(u8R,u8A,u8B,&BnMod,u8WordB,u8WordP);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWordP<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("R2.\n", au8R2, u16SLen);
		
//    r_printf((0 == memcmp(au8R1, au8R2, u16SLen)), "bignum mod Exp test \n");
}
// C5734F5287B23EA9
#define MODEXP_A "2EC4DDFBCF0768685C46CB6D56792AF6C9600D5F29A3068074F81D71198DB39733546E1ECB3FACB9E12F0B2C192D09AFE75978DECC74ECD68711F53CF7383913D4C0B6E56237A9A5FCD422FE40FDA250DD7C5FC3024BB0AC80330695F4F66460D532825368EDDEA249D1B1A5A8EACED387EDEDD5BE131701CFCC4D52C12BB837ACFCD270631733E3F1D6A3BAEE4FF2DB71C050EEB662E9D58B42684965E50676D5208CB98E241B6F31A84BA4B869F98E64BDDE3071518D6E2AFC5035DBB8A4B79F97C18F7B8920B305B100A5F67F3568A76ACC9CB563BAFBEA77FDEA1BD1813FDC92CEC6153B9635EDBFF812279171D3928DA39C469EFA6E5AFF2D2B33611C7025A1336BBD3C541E77C21B6B65EE0C645F06055D67886AECAEE8E4A131D57407A5D9D29F36833AE4F213B594ABAA938C17326BE5BDF4B3058067349FC9B269E8A308B5F9864791B5FF6088C1B10441910A575B9BFEACA1CFD07F2A341422AB68CC7D4F2A364FEE54E1DDB27BFF48DCF6D27533CC01C13BB58AF772D2032341D7A4F1C47E3947145AE8EA48CB55FECCE26EC3B35731F93A70BDED14EBEE08081E2DB83D29BC9BE752543EDE88080FA3E2854E6A15F7684F3CB5B855D6DAD089B9A122228AC70402D0C127448FC95BFF71ACAF299031D0265CA6B8CCB69DC2BCB1F1C5F03D6D07B6349275AFAD12E1AB97A33DFBEC81F49C39B69336293A872356"
#define MODEXP_B "00000000000000000000000000010001"
#define MODEXP_P "8D6B71E45E7ACE6B445F7F4793412678BE1F0FEBC407F1075521DFD09C326B9CDEC864694D7D11D146D974AF654B922962BE604B7382DAD7FE00A07482944C2328B4E28DE03B8CCA73DEA02D7A414A5FDBF428D5EBDF86C827175D82A17C01FE8434F99F000B14FDACCE8F4025E9E27D8C3FA71B6A23980814E7557B54B944039E0968FCF6C67614E4F3E1D20B6CCC19791FEFFE78A5148567A24884B602DB75E07252D7940B7813396AA9372AB136BCE9F9FD020D5E46EB3B1A871D690982635E4E3E75A21F21915D30A9965460CB8366C40B7FFA7CD21C2F7D3DBF00FAC4A5E86AEF38BE89975E86FF528ECEE0103FB0226548202DB0D36C2F2AD9CDFCFC24DD15DA5B0CC8BE90EAB7C616C396708157D43518739DBC4BD040E6A7F2CC5DCBCFA50D5BB3D9D9D0D9CBC3C6047B41C863269038588A44C4554ED37E244D78DB869EEBE44402E48377A32941AA9D6DB773A0F6F88AA03A149FFD914A195B0BD73214EF8F84311DDF7EDE3D3948C3620933A788548171A3F9BD9ABB49178F2F50E1EF73C62317E56396477DBD342D0FF55F04489D01612F731E82386F25086B09C335580C099BD8A274ED5741084EADE3B756ACF47A83C19530675712B0A0913B5271C2836CD2E52AE4F4B79694CA3B22BD73BD7CEB933559660774B857D3593EAE7F759066B843C8CA7BCA598D011668426A711C479C4D2F90275A9730816545"
void ModExp_TestT()
{
    uint8_t au8A[512], au8B[16], au8P[512], au8R1[512], au8R2[512];
	uint8_t u8A, u8B, u8P, u8PD, u8R, u8Spare;
	uint32_t au32PC[2];
	MHBN_MOD BnMod;
	
	u8A = 0x20; u8B = 0x30; u8P = 0x0; u8PD = 0x10; u8R = 0x40; u8Spare = 0x40;
	BnMod.u8P = u8P;    BnMod.u8PD = u8PD;      BnMod.pu32PC = au32PC;
	MHCRYPT_AsymcConfig();
	bn_read_string_from_head(PMEMITEM[u8P], sizeof(au8A), MODEXP_P);  
	MHBN_GetModMulConst(au32PC, u8P, 128);
    MHBN_GetDomainConst(u8PD, u8P, au32PC, 128, u8Spare);
	bn_read_string_from_head(PMEMITEM[u8A], sizeof(au8A), MODEXP_A);   
	bn_read_string_from_head(PMEMITEM[u8B], sizeof(au8B), MODEXP_B);   
	MHBN_ModExp(u8R,u8A,u8B,&BnMod,4,128);
	memcpy(au8R1, PMEMITEM[u8R], 512);
	ouputRes("A.\n", PMEMITEM[u8A], 512);
	ouputRes("B.\n", PMEMITEM[u8B], 16);
	ouputRes("P.\n", PMEMITEM[u8P], 512);
	ouputRes("PD.\n", PMEMITEM[u8PD], 512);
	ouputRes("PC.\n", au32PC, 8);
	ouputRes("R1.\n", au8R1, 512);
	MHCRYPT_AsymcUnconfig();
	while(1);
}


#define RSA_E   "00010001"

void ModInv_Test()
{
    uint8_t au8E[4],au8A[BN_TEST_LEN], au8P[BN_TEST_LEN], au8R1[BN_TEST_LEN], au8R2[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigP, BigR;
    MH_RSA_KEY_SPACE InBase;
    MH_RSA_KEY KeySrc;
    uint8_t u8A, u8P, u8R, u8WordP, u8DWordP,  u8ItemP, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenP;
    uint32_t u32Ret = 0;
    do
    {
        MHRAND_Prand(&u16SLenA, 2);
        u16SLenA = (u16SLenA & 0xff);
        MHRAND_Prand(au8A, u16SLenA);
        MHRAND_Prand(&u16SLenP, 2);
        u16SLenP = (u16SLenP & 0xff);
    }while((u16SLenA < 16) || (u16SLenP <= u16SLenA) || (u16SLenP < 0x20));
    //get Prime P
//    DBG_PRINT("%x\n", u16SLenP);
    bn_read_string_from_head((uint8_t *)au8E, sizeof(au8E), RSA_E);
    memset((uint8_t *)&KeySrc,0,sizeof(KeySrc));
    memset((uint8_t *)&InBase,0,sizeof(InBase));
    KeySrc.u16NBits = u16SLenP<<3;
    KeySrc.u16EBits = 32;
    KeySrc.pu8E = au8E;
    KeySrc.pu8N = au8P;
    KeySrc.u32Crc = MHCRC_CalcBuff(0xffff, &KeySrc, sizeof(MH_RSA_KEY)-4);
    MHRSA_GenKey(&KeySrc, &InBase, NULL, 1);
//    ouputRes("P.\n", au8P, u16SLenP);

    au8P[u16SLenP -1] &= ~0x01; 
    BigA.pu8Data = au8A;
    BigA.u32Len = u16SLenA;
    BigP.pu8Data = au8P;
    BigP.u32Len = u16SLenP;
    BigR.pu8Data = au8R1;
    BigR.u32Len = u16SLenP;
    u32Ret = MHBN_BnModInv(&BigR, &BigA, &BigP);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    u8WordP = MH_ROUND_UP(u16SLenP, 4)>>2;
    u8DWordP = MH_ROUND_UP(u8WordP, 2);
    u8ItemP = MH_ROUND_UP(u8WordP, 8)>>3;
    u8A  = u8Spare; u8Spare += u8ItemP;
    u8P  = u8Spare; u8Spare += u8ItemP;
    u8R  = u8Spare; u8Spare += u8ItemP;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWordP<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8P), au8P, u8DWordP<<2, u16SLenP);
    
    u32Ret = MHBN_ModInvSoft(u8R,u8A,u8P,u8WordP);
    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLenP, u8DWordP<<2);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("P.\n", au8P, u16SLenP);
//    ouputRes("R1.\n", au8R1, u16SLenP);
//    ouputRes("R2.\n", au8R2, u16SLenP);
    r_printf((0 == memcmp(au8R1, au8R2, u16SLenP)), "bignum mod Inv test \n");
}

void Cmp_Test()
{
    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN];
    MHBN_BIGNUM BigA, BigB;
    uint8_t u8A, u8B, u8Word, u8DWord, u8Item, u8Spare = 0; 
    uint16_t u16SLenA, u16SLenB, u16SLen;
    uint32_t u32Ret1 = 0, u32Ret2 = 0;
    do
    {
        MHRAND_Prand(&u16SLenA, 2);
        u16SLenA = (u16SLenA & 0x1ff) + 1;
        MHRAND_Prand(au8A, u16SLenA);
        MHRAND_Prand(&u16SLenB, 2);
        u16SLenB = (u16SLenB & 0x1ff) + 1;
        u16SLenB = u16SLenA;
        MHRAND_Prand(au8B, u16SLenB);
        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
    }while(u16SLen < 16);
    BigA.pu8Data = au8A;
    BigA.u32Len = u16SLenA;
    BigB.pu8Data = au8B;
    BigB.u32Len = u16SLenB;
    u32Ret1 = MHBN_BnCmp(&BigA, &BigB);
//    DBG_PRINT("u32Ret1 = %x\n",u32Ret1);
    
    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
    u8DWord = MH_ROUND_UP(u8Word, 2);
    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
    u8A  = u8Spare; u8Spare += u8Item;
    u8B  = u8Spare; u8Spare += u8Item;
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
    u32Ret2 = MHBN_ItemCmp(u8A,u8B,u8Word);
//    DBG_PRINT("u32Ret2 = %x\n",u32Ret2);
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("B.\n", au8B, u16SLenB);
    r_printf((u32Ret1 == u32Ret2), "bignum cmp test \n");
}

void Xor_Test()
{
//    uint8_t au8A[BN_TEST_LEN], au8B[BN_TEST_LEN], au8P[BN_TEST_LEN], au8R2[BN_TEST_LEN];
////    MHBN_BIGNUM BigA, BigB, BigP, BigR;
//    uint8_t u8A, u8B, u8R, u8Word, u8DWord, u8Item, u8Spare = 0; 
//    uint16_t u16SLenA, u16SLenB, u16SLen;
//    uint32_t u32Ret = 0;
//    do
//    {
//        MHRAND_Prand(&u16SLenA, 2);
//        u16SLenA = (u16SLenA & 0x1ff) + 1;
//        MHRAND_Prand(au8A, u16SLenA);
//        MHRAND_Prand(&u16SLenB, 2);
//        u16SLenB = (u16SLenB & 0x1ff) + 1;
//        MHRAND_Prand(au8B, u16SLenB);
//        u16SLen = (u16SLenA > u16SLenB) ? u16SLenA : u16SLenB;
//        MHRAND_Prand(au8P, u16SLen);
//    }while(u16SLen < 16);
//    //make sure P is bigger than A B

//    u8Word = MH_ROUND_UP(u16SLen, 4)>>2;
//    u8DWord = MH_ROUND_UP(u8Word, 2);
//    u8Item = MH_ROUND_UP(u8Word, 8)>>3;
//    u8A  = u8Spare; u8Spare += u8Item;
//    u8B  = u8Spare; u8Spare += u8Item;
//    u8R  = u8Spare; u8Spare += u8Item;
//    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8A), au8A, u8DWord<<2, u16SLenA);
//    MHMEM_CopyBig(MH_ITEM_TO_MEM(u8B), au8B, u8DWord<<2, u16SLenB);
//    u32Ret = MHBN_Xor(u8R,u8A,u8B,u8Word);
//    MHMEM_CopyBig(au8R2, MH_ITEM_TO_MEM(u8R), u16SLen, u8DWord<<2);
//    DBG_PRINT("u32Ret = %x\n",u32Ret);
//    ouputRes("A.\n", au8A, u16SLenA);
//    ouputRes("B.\n", au8B, u16SLenB);
//    ouputRes("R2.\n", au8R2, u16SLen);
}

void CRAM_Test()
{
    uint8_t u8Test = 0;
    *(uint8_t *)PMEMITEM[40] = 0xaa;
    u8Test = *(uint8_t *)PMEMITEM[40];
    DBG_PRINT("u8Test = %x\n",u8Test);
}

void SRAM_Test()
{
    uint8_t u8Test = 0;
    *(uint8_t *)0x20003800 = 0xaa;
    u8Test = *(uint8_t *)0x20003800;
    DBG_PRINT("u8Test = %x\n",u8Test);
}

void ouputRes1(char *pcFmt, void *pvbuff, uint32_t u32Len)
{
    int32_t i;
    uint8_t *pu8Buff = pvbuff;
    
    printf("%s", pcFmt);
    for (i = 0; i < u32Len; i++)
    {
        printf("%02X", pu8Buff[i]);
    }
    printf("\n");
}

#define CRAM_BASE_ADDR  (0x40005000)
#define CRAM_ITEM_SIZE  (0x80)
#define CRAM_ITEM_TO_ADDR(a)   ((uint8_t *)(CRAM_BASE_ADDR + (a)*32))

const uint8_t cau8InvA_256[32] = "\x87\xA6\x90\x91\x6B\x6B\xEF\x58\xA6\x84\xDE\x1E\xD1\x24\xA3\x30\x0E\x41\x19\x42\x8F\xDC\x63\x19\xB1\x7E\x0F\xC9\xB6\xDA\xF5\xB3";
//const uint8_t cau8InvA_256[32] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF4\x6A\xB6\x37\x79\x41\x12\x85";
const uint8_t cau8InvA2_256[32] = "\x50\x18\x8E\x23\x7D\x04\xC3\xDD\xB2\x8F\xF1\x40\x65\x80\x32\xBE\x51\x36\xA6\xC1\xC8\x22\x14\xB1\x88\xD0\xDF\x4E\xDE\x35\xFF\xD4";
const uint8_t cau8InvP_256[32] = "\xD8\xC8\x6E\xF2\xD6\x09\xE5\x18\xA0\x56\xF8\xE3\xA8\xA0\x7D\xD8\x13\xB7\x92\xE8\x56\x03\xFD\x6E\x7D\x1E\xAD\x01\x9F\x96\x48\x47";
const uint8_t cau8InvR_256[32] = "\x49\xB4\xA1\x79\x41\x7E\x5E\xFD\xED\x5C\x84\x61\x63\xDE\xD6\x83\x14\x6B\x70\x6E\x66\x5F\x7D\xEF\xF5\x9D\x41\x8E\xDC\x90\xC1\xDD";
void ModInv_Test_Fix_256()
{
    uint32_t u32Ret = 0;
    uint8_t u8A, u8P, u8R;
    uint16_t u16AlignLen;
    u16AlignLen = sizeof(cau8InvA_256);
    u8A = 0x12;
    u8P = CRAM_ITEM_SIZE - 0x20;
    u8R = CRAM_ITEM_SIZE - 0x18;
    memcpy(CRAM_ITEM_TO_ADDR(u8A), cau8InvA_256, sizeof(cau8InvA_256));
    memcpy(CRAM_ITEM_TO_ADDR(u8P), cau8InvP_256, sizeof(cau8InvP_256));
    u32Ret = MHBN_ModInv(u8R, u8A, u8P, u16AlignLen>>2);
	ouputRes1("", PMEMITEM[u8R], u16AlignLen);
    if(u32Ret != MH_RET_CRYPT_SUCCESS)
    {
        DBG_PRINT("ModInv fix_256 test error\n");
    }
    r_printf((0 == memcmp(CRAM_ITEM_TO_ADDR(u8R), cau8InvR_256, u16AlignLen)), "ModInv fix_256 test\n");
    memcpy(CRAM_ITEM_TO_ADDR(u8A), cau8InvA2_256, sizeof(cau8InvA_256));
	DBG_PRINT("ModInv2....\n");

    u32Ret = MHBN_ModInv(u8R, u8A, u8P, u16AlignLen>>2);
	ouputRes1("", PMEMITEM[u8R], u16AlignLen);
	
	DBG_PRINT("ModInv2 End....\n");
    if(u32Ret != MH_RET_CRYPT_GCD_ISNOT_ONE)
    {
        DBG_PRINT("ModInv fix_256 test error\n");
		while(1);
    }
    else
    {
		
        DBG_PRINT("ModInv fix_256 test success\n");
    }

}

//#define modExp_A_1024		"9F0F8256C0583B63DD072F9BF62CF6F011D48E1533DCF809EB3C331F99D0669DFBEC9D2AF0F490A518039820E9751C5B1724127FCA61C32FC7F5D1B76FCAE29A6CDA81824495DF6CBA0E2AE2F153476CFF9FF46DAC7DFDBE393CC5C1779A961CBC5E051EFAD3932DE7352BA1BFCDEF8ADCAD2159C866DE68D857EDD35DFDE847"
//#define modExp_B_1024		"9B0C42D363FA5D3857056A5895C50A05E3D07E1B14B260C769A959C703678B14A9A8BC778ABDC7B93A0D021348E10605302E3CF6B15C23385BA063819DE6DE4297EBACC93718C2DD8DC1C6F3630C5227FA3758868A388FE3495714056DDDD244AC029D9319A8175356C8488F72A9C94C54F709B0154D293BECB8D64968B51129"
//#define modExp_P_1024		"F5A9D27559B5A6C83E6BFBCDC51A25CF0E37FFA0426E365D22EADF84009C2776883B1B539DB9A526706B9DF10D931A202294C7E39E6726B746ECF3649028DFB73BBCC9A91524DE212A7BC9A917CB22CAFEAC72D5CF89AC954EEEE6181FE84410113BD14084F2B7AB7B6CED3B986D3A0FE6FF164690D2FFC4F27308C06DCF0BBF"
//#define modExp_R_1024		"B52EAA3369D2314037A930D5ADF352EEBD405B81F3FA29D494F9F59CAA716482A00AD960234A05D968E49F1EB1FC7A6C3C49EBF932FE354136B963331B97A750F129C1712B5A30E3A8C9BB3CB171667BFF07278C93DE816BEBF6DFC42C08414A87E0F277B3984CD965169D3B407F1D4B37C1647E7232502FAF2E7ED3D20FC3D0"

void BigNum_Test()
{
//	uint8_t au8A[512], au8B[512], au8P[512], au8R1[512];

//	bn_read_string_from_head(au8A, sizeof(au8A), modExp_A_1024); 
//	bn_read_string_from_head(au8B, sizeof(au8B), modExp_B_1024); 
//	bn_read_string_from_head(au8P, sizeof(au8P), modExp_P_1024); 
//	bn_read_string_from_head(au8R1, sizeof(au8R1), modExp_R_1024); 
//	ouputRes("A:\n", au8A, 128);
//	ouputRes("B:\n", au8B, 128);
//	ouputRes("P:\n", au8P, 128);
//	ouputRes("R:\n", au8R1, 128);
	
//    Add_Test();
//    Sub_Test();
//    ModAdd_Test();
//    ModSub_Test();
//    Mul_Test();
//    ModMul_Test();
Div_fixed_Test();
//    Div_Test();
//    ModExp_Test();
//    ModInv_Test();
//    Cmp_Test();
//    Xor_Test();
}


#endif


#if 0
void mul_Test()
{
    uint32_t au32A[8];
    uint32_t au32B[8];
    uint32_t au32R[16];
    uint16_t u16LenByteA, u16LenWord;
    uint8_t u8ItemSize, u8A, u8B, u8R, u8Spare = 0;
    u16LenByteA = bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), PARA_A);
    bn_read_string_from_head((uint8_t *)au32B, sizeof(au32B), PARA_B);
    u16LenWord  =  MH_ROUND_UP(u16LenByteA,4) >> 2;
    
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A = u8Spare; u8Spare += u8ItemSize;
    u8B = u8Spare; u8Spare += u8ItemSize;
    u8R = u8Spare;
    
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16LenWord<<2);
    
    MHBN_Mul(u8R, u8A, u8B, u16LenWord);
    
    memcpy( au32R,MH_ITEM_TO_MEM(u8R), u16LenWord << 3);
    ouputRes("A.\n", au32A, u16LenWord << 2);
    ouputRes("B.\n", au32B, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 3);
//    while(1);
}

void div_Test()
{
    uint32_t au32A[64];
    uint32_t au32B[64];
    uint32_t au32R[64];
    uint32_t au32Q[64];
    uint16_t u16LenByteA, u16LenWord;
    uint8_t u8ItemSize, u8A, u8B, u8R, u8Q, u8Spare = 0;
    u16LenByteA = bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), PARA_A);
    bn_read_string_from_head((uint8_t *)au32B, sizeof(au32B), PARA_B);
    u16LenWord  =  MH_ROUND_UP(u16LenByteA,4) >> 2;
    
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A = u8Spare; u8Spare += u8ItemSize;
    u8B = u8Spare; u8Spare += u8ItemSize;
    u8Q = u8Spare; u8Spare += u8ItemSize;
    u8R = u8Spare + u8ItemSize*3;
   
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16LenWord<<2);
    MHBN_Div(u8Q, u8R, u8A, u8B, u16LenWord); 
    memcpy( au32R,MH_ITEM_TO_MEM(u8R), u16LenWord<<2);
    memcpy( au32Q,MH_ITEM_TO_MEM(u8Q), u16LenWord<<2);
    ouputRes("A.\n", au32A, u16LenWord << 2);
    ouputRes("B.\n", au32B, u16LenWord << 2);
    ouputRes("Q.\n", au32Q, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 2);
//    while(1);
}

void modinv_Test()
{
    uint32_t au32A[18];
    uint32_t au32B[18];
    uint32_t au32R[18];
    uint16_t u16LenByteA, u16LenWord;
    uint8_t u8ItemSize, u8A, u8B, u8R,  u8Spare = 0;
    u16LenByteA = bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), ECC_P);
    bn_read_string_from_head((uint8_t *)au32B, sizeof(au32B), ECC_GX);
    u16LenWord  =  MH_ROUND_UP(u16LenByteA,4) >> 2;
    
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A = u8Spare; u8Spare += u8ItemSize;
    u8B = u8Spare; u8Spare += u8ItemSize;
    u8R = u8Spare;
    
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16LenWord<<2);
    
    if(MHBN_ModInv(u8R, u8B, u8A, u16LenWord) != MH_RET_CRYPT_SUCCESS)
    {
        DBG_PRINT("The ModInv calc is error!\n");
        while(1);
    }
    
    memcpy( au32R,MH_ITEM_TO_MEM(u8R), u16LenWord<<2);
    ouputRes("A.\n", au32A, u16LenWord << 2);
    ouputRes("B.\n", au32B, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 2);
    while(1);
}

void modInvSoft_Test()
{
    uint32_t au32A[8];
    uint32_t au32B[8];
    uint32_t au32R[8];
    uint16_t u16LenByteA, u16LenWord;
    uint8_t u8ItemSize, u8A, u8B, u8R, u8Spare = 0;
    u16LenByteA = bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), PARA_A);
    bn_read_string_from_head((uint8_t *)au32B, sizeof(au32B), PARA_B);
    u16LenWord  =  MH_ROUND_UP(u16LenByteA,4) >> 2;
    
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A = u8Spare; u8Spare += u8ItemSize;
    u8B = u8Spare; u8Spare += u8ItemSize;
    u8R = u8Spare;
    
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16LenWord<<2);
    
    if(MHBN_ModInvSoft(u8R, u8B, u8A, u16LenWord) != MH_RET_CRYPT_SUCCESS)
    {
        DBG_PRINT("The ModInv calc is error!\n");
        while(1);
    }
    
    memcpy( au32R,MH_ITEM_TO_MEM(u8R), u16LenWord<<2);
    ouputRes("A.\n", au32A, u16LenWord << 2);
    ouputRes("B.\n", au32B, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 2);
//    while(1);
}
void getMontConst_Test()
{
    uint32_t au32P[18];
    uint32_t au32R[2] = {0};
    uint16_t u16LenByte, u16LenWord;
    uint8_t u8ItemSize, u8P, u8Spare = 0;
    u16LenByte = bn_read_string_from_head((uint8_t *)au32P, sizeof(au32P), ECC_P);
    u16LenWord  =  MH_ROUND_UP(u16LenByte,4) >> 2; 
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    u8P = u8Spare;
    
    memcpy(MH_ITEM_TO_MEM(u8P), au32P, u16LenWord<<2);
    MHBN_GetModMulConst(au32R, u8P, u16LenWord);
    ouputRes("R.\n", au32R, 8);
//    while(1);
}

void montModMul_Test()
{
    uint32_t au32A[12];
    uint32_t au32B[12];
    uint32_t au32P[12];
    uint32_t au32R[12];
    uint32_t au32PC[2];
    uint16_t u16LenByteP, u16LenWord;
    uint8_t u8ItemSize, u8A, u8B, u8P, u8R, u8Spare = 0;
    bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), MOD_A);
    bn_read_string_from_head((uint8_t *)au32B, sizeof(au32B), MOD_B);
    u16LenByteP = bn_read_string_from_head((uint8_t *)au32P, sizeof(au32P), MOD_P);
    bn_read_string_from_head((uint8_t *)au32PC, sizeof(au32PC), MOD_PC);
    u16LenWord  =  MH_ROUND_UP(u16LenByteP,4) >> 2;
    
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A = u8Spare; u8Spare += u8ItemSize;
    u8B = u8Spare; u8Spare += u8ItemSize;
    u8P = u8Spare; u8Spare += u8ItemSize;
    u8R = u8Spare; u8Spare += u8ItemSize;
    
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8P), au32P, u16LenWord<<2);
    
    if(MHBN_MontModMul(u8R, u8A, u8B, u8P, au32PC, u16LenWord, u8Spare) != MH_RET_CRYPT_SUCCESS)
    {
        DBG_PRINT("The MontModMul calc is error!\n");
        while(1);
    }
    
    memcpy(au32R,MH_ITEM_TO_MEM(u8R), u16LenWord<<2);
    ouputRes("A.\n", au32A, u16LenWord << 2);
    ouputRes("B.\n", au32B, u16LenWord << 2);
    ouputRes("P.\n", au32P, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 2);
//    while(1);
}

void getDomainConst_Test()
{
    uint32_t au32P[18];
    uint32_t au32R[18];
    uint32_t au32PC[2];
    uint16_t u16LenByte, u16LenWord;
    uint8_t  u8ItemSize, u8P, u8R, u8Spare = 0;
    u16LenByte = bn_read_string_from_head((uint8_t *)au32P, sizeof(au32P), ECC_P);
    bn_read_string_from_head((uint8_t *)au32PC, sizeof(au32PC), ECC_PC);
    u16LenWord  =  MH_ROUND_UP(u16LenByte,4) >> 2; 
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize >>= MH_ASYMC_MEM_ITEM_WORD_BIT;
    u8P = u8Spare; u8Spare += u8ItemSize;
    u8R = u8Spare; u8Spare += u8ItemSize;
    memcpy(MH_ITEM_TO_MEM(u8P), au32P, u16LenWord<<2);
    MHBN_GetDomainConst(u8R, u8P, au32PC, u16LenWord, u8Spare);
    memcpy(au32R,MH_ITEM_TO_MEM(u8R), u16LenWord<<2);
    ouputRes("P.\n", au32P, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 2);
    while(1);   
}

void modMul_Test()
{
    uint32_t au32A[18];
    uint32_t au32B[18];
    uint32_t au32P[18];
    uint32_t au32PD[18];
    uint32_t au32R[18];
    uint32_t au32PC[2];
    MHBN_MOD BnMod;
    uint16_t u16LenByteP, u16LenWord;
    uint8_t u8ItemSize, u8A, u8B, u8P,  u8PD, u8R, u8Spare = 0;
    bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), ECC_GX);
    bn_read_string_from_head((uint8_t *)au32B, sizeof(au32B), ECC_GY);
    u16LenByteP = bn_read_string_from_head((uint8_t *)au32P, sizeof(au32P), ECC_P);
    bn_read_string_from_head((uint8_t *)au32PD, sizeof(au32PD), ECC_PD);
    bn_read_string_from_head((uint8_t *)au32PC, sizeof(au32PC), ECC_PC);
    u16LenWord  =  MH_ROUND_UP(u16LenByteP,4) >> 2;
    
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A  = u8Spare; u8Spare += u8ItemSize;
    u8B  = u8Spare; u8Spare += u8ItemSize;
    u8P  = u8Spare; u8Spare += u8ItemSize;
    u8PD = u8Spare; u8Spare += u8ItemSize;
    u8R  = u8Spare;
    
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8P), au32P, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8PD), au32PD, u16LenWord<<2);
    BnMod.u8P = u8P;
    BnMod.u8PD = u8PD;
    BnMod.pu32PC = au32PC; 
    
    if(MHBN_ModMul(u8R, u8A, u8B, &BnMod, u16LenWord) != MH_RET_CRYPT_SUCCESS)
    {
        DBG_PRINT("The ModMul calc is error!\n");
        while(1);
    }
    
    memcpy( au32R,MH_ITEM_TO_MEM(u8R), u16LenWord<<2);
    ouputRes("A.\n", au32A, u16LenWord << 2);
    ouputRes("B.\n", au32B, u16LenWord << 2);
    ouputRes("P.\n", au32P, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 2);
//    while(1);
}

void MontModExp_Test()
{
    uint32_t au32A[64];
    uint32_t au32B[64];
    uint32_t au32P[64];
    uint32_t au32R[64];
    uint32_t au32PC[2];
    uint16_t u16LenByteB, u16LenByteP, u16LenWord, u16LenB;
    uint8_t u8ItemSize, u8A, u8B, u8P, u8R, u8Spare = 0;
    bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), MOD_A);
    u16LenByteB = bn_read_string_from_head((uint8_t *)au32B, sizeof(au32B), MOD_B);
    u16LenByteP = bn_read_string_from_head((uint8_t *)au32P, sizeof(au32P), MOD_P);
    bn_read_string_from_head((uint8_t *)au32PC, sizeof(au32PC), MOD_PC);
    u16LenWord  =  MH_ROUND_UP(u16LenByteP,4) >> 2;
    u16LenB     =  MH_ROUND_UP(u16LenByteB,4) >> 2;
    
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A = u8Spare; u8Spare += u8ItemSize;
    u8B = u8Spare; u8Spare += u8ItemSize;
    u8P = u8Spare; u8Spare += u8ItemSize;
    u8R = u8Spare;
    
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8P), au32P, u16LenWord<<2);
    
    if(MHBN_MontModExp(u8R, u8A, u8B, u8P, au32PC, u16LenB, u16LenWord) != MH_RET_CRYPT_SUCCESS)
    {
        DBG_PRINT("The MontModExp calc is error!\n");
        while(1);
    }
    
    memcpy( au32R,MH_ITEM_TO_MEM(u8R), u16LenWord<<2);
    ouputRes("A.\n", au32A, u16LenWord << 2);
    ouputRes("B.\n", au32B, u16LenB << 2);
    ouputRes("P.\n", au32P, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 2);
    
//    while(1);
}

void ModExp_Test()
{
    uint32_t au32A[64];
    uint32_t au32B[64];
    uint32_t au32P[64];
    uint32_t au32PD[64];
    uint32_t au32R[64];
    uint32_t au32PC[2];
    MHBN_MOD BnMod;
    uint16_t u16LenByteB, u16LenByteP, u16LenWord, u16LenB ;
    uint8_t u8ItemSize, u8A, u8B, u8P,  u8PD, u8R, u8Spare = 0;
    bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), MOD_A);
    u16LenByteB = bn_read_string_from_head((uint8_t *)au32B, sizeof(au32B), MOD_B);
    u16LenByteP = bn_read_string_from_head((uint8_t *)au32P, sizeof(au32P), MOD_P);
//    bn_read_string_from_head((uint8_t *)au32PD, sizeof(au32PD), MOD_PD);
    bn_read_string_from_head((uint8_t *)au32PC, sizeof(au32PC), MOD_PC);
    u16LenWord  =  MH_ROUND_UP(u16LenByteP,4) >> 2;
    u16LenB     =  MH_ROUND_UP(u16LenByteB,4) >> 2;
//    u16LenB     =  MH_ROUND_UP(u16LenByteB,16) >> 2;
    
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A  = u8Spare; u8Spare += u8ItemSize;
    u8B  = u8Spare; u8Spare += u8ItemSize;
    u8P  = u8Spare; u8Spare += u8ItemSize;
    u8PD = u8Spare; u8Spare += u8ItemSize;
    u8R  = u8Spare;
    
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8P), au32P, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8PD), au32PD, u16LenWord<<2);
    BnMod.u8P = u8P;
    BnMod.u8PD = u8PD;
    BnMod.pu32PC = au32PC; 
    
    if(MHBN_ModExp(u8R, u8A, u8B, &BnMod, u16LenB, u16LenWord) != MH_RET_CRYPT_SUCCESS)
    {
        DBG_PRINT("The ModExp calc is error!\n");
        while(1);
    }
    
    ouputRes("A.\n", au32A, u16LenWord << 2);
    ouputRes("B.\n", au32B, u16LenB << 2);
    ouputRes("P.\n", au32P, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 2);
//    while(1);
}

void NonMul_Test()
{
    uint32_t au32A[32];
    uint32_t au32B[2];
    uint32_t au32R[32 + 2];
    uint16_t u16LenByteA, u16LenByteB, u16LenWordA, u16LenWordB, u16LenWordR;
    uint8_t u8ASize, u8A, u8B, u8R, u8Spare = 0;
    u16LenByteA = bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), MUL_A);
    u16LenByteB = bn_read_string_from_head((uint8_t *)au32B, sizeof(au32B), MUL_B);
    u16LenWordA  =  MH_ROUND_UP(u16LenByteA, 8) >> 2; 
    u16LenWordB  =  MH_ROUND_UP(u16LenByteB, 8) >> 2; 
    u16LenWordR = u16LenWordA + u16LenWordB;
    u8ASize = MH_ROUND_UP(u16LenWordA, 8);
    u8ASize = u8ASize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A  = u8Spare; u8Spare += u8ASize;
    u8B  = u8Spare; u8Spare += 1;
    u8R  = u8Spare; u8Spare += u8ASize + 1;
    
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenWordA<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16LenWordB<<2);
    
    if(MHBN_HalfLenMul(u8R, u8A, u8B, u16LenWordA, u16LenWordB, u8Spare) != MH_RET_CRYPT_SUCCESS)
    {
        DBG_PRINT("The NonMul calc is error!\n");
        while(1);
    }
    
    memcpy( au32R,MH_ITEM_TO_MEM(u8R), u16LenWordR<<2);
    ouputRes("A.\n", au32A, u16LenWordA << 2);
    ouputRes("B.\n", au32B, u16LenWordB << 2);
    ouputRes("R.\n", au32R, u16LenWordR << 2);
    MHRAND_Prand(au32B, u16LenWordA << 2);
    ouputRes("B.\n", au32A, u16LenWordA << 2);
}

void Shift_Test()
{
    uint32_t au32A[32];
    uint32_t au32R[32];
    uint32_t u32Bits = 255;
    uint16_t u16AByte,u16AWord;
    uint8_t u8ASize, u8A, u8R, u8Spare = 0;
    u16AByte = bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), PARA_A);
    u16AWord = MH_ROUND_UP(u16AByte, 4)>>2;
    u8ASize = MH_ROUND_UP(u16AWord, 8)>>3;

    u8A  = u8Spare; u8Spare += u8ASize;
    u8R  = u8Spare; u8Spare += u8ASize;
    memset(MH_ITEM_TO_MEM(u8R), 0, u16AWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16AWord<<2);
    MHBN_ShiftLH(u8R,u8A,u16AWord,u32Bits);
    memcpy(au32R, MH_ITEM_TO_MEM(u8R), u16AWord<<2);
    ouputRes("A.\n", au32A, u16AWord << 2);
    ouputRes("R.\n", au32R, u16AWord << 2);
    memset(MH_ITEM_TO_MEM(u8R), 0, u16AWord<<2);
    MHBN_ShiftRH(u8R,u8A,u16AWord,u32Bits);
    memcpy(au32R, MH_ITEM_TO_MEM(u8R), u16AWord<<2);
    ouputRes("A.\n", au32A, u16AWord << 2);
    ouputRes("R.\n", au32R, u16AWord << 2);
}


#define    ADD_SRC_A     "00000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
#define    ADD_SRC_B     "00000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF1"
void Add_Test()
{
    uint8_t u8ASize, u8A, u8R, u8B, u8Spare = 0;
    uint16_t u16AByte,u16AWord;
    uint32_t au32A[32];
    uint32_t au32B[32];
    uint32_t au32R[32];
    uint32_t u32Ret = 0;
    u16AByte = bn_read_string_from_head((uint8_t *)au32A, sizeof(ADD_SRC_A), ADD_SRC_A);
    bn_read_string_from_head((uint8_t *)au32B, sizeof(ADD_SRC_B), ADD_SRC_B);
    u16AWord = MH_ROUND_UP(u16AByte, 4)>>2;
    
    u8ASize = MH_ROUND_UP(u16AWord, 8)>>3;
    u8A  = u8Spare; u8Spare += u8ASize;
    u8B  = u8Spare; u8Spare += u8ASize;
    u8R  = u8Spare; u8Spare += u8ASize;
    memset(MH_ITEM_TO_MEM(u8R), 0, u16AWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16AWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8B), au32B, u16AWord<<2);
    u32Ret = MHBN_Add(u8R,u8A,u8B,u16AWord-1);
    memcpy(au32R, MH_ITEM_TO_MEM(u8R), u16AWord<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au32A, u16AWord << 2);
    ouputRes("B.\n", au32B, u16AWord << 2);
    ouputRes("R.\n", au32R, u16AWord << 2);
}
#define    ADDONE_SRC_DATA     "00000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
void AddOne_Test()
{
    uint8_t u8ASize, u8A, u8R, u8Spare = 0;
    uint16_t u16AByte,u16AWord;
    uint32_t au32A[32];
    uint32_t au32R[32];
    uint32_t u32Ret = 0;
    u16AByte = bn_read_string_from_head((uint8_t *)au32A, sizeof(ADDONE_SRC_DATA), ADDONE_SRC_DATA);
    u16AWord = MH_ROUND_UP(u16AByte, 4)>>2;
    
    u8ASize = MH_ROUND_UP(u16AWord, 8)>>3;
    u8A  = u8Spare; u8Spare += u8ASize;
    u8R  = u8Spare; u8Spare += u8ASize;
    memset(MH_ITEM_TO_MEM(u8R), 0, u16AWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16AWord<<2);
    u32Ret = MHBN_AddOne(u8R,u8A,u16AWord - 1);
    memcpy(au32R, MH_ITEM_TO_MEM(u8R), u16AWord<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au32A, u16AWord << 2);
    ouputRes("R.\n", au32R, u16AWord << 2);
}


#define    ADDU32_SRC_DATA     "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

void Addu32_Test()
{
    uint8_t u8ASize, u8A, u8R, u8Spare = 0;
    uint16_t u16AByte,u16AWord;
    uint32_t au32A[32];
    uint32_t au32R[32];
    uint32_t u32B, u32Ret = 0;
    u16AByte = bn_read_string_from_head((uint8_t *)au32A, sizeof(ADDU32_SRC_DATA), ADDU32_SRC_DATA);
    u16AWord = MH_ROUND_UP(u16AByte, 4)>>2;
    u32B = 0x12345678;
    u8ASize = MH_ROUND_UP(u16AWord, 8)>>3;
    u8A  = u8Spare; u8Spare += u8ASize;
    u8R  = u8Spare; u8Spare += u8ASize;
    memset(MH_ITEM_TO_MEM(u8R), 0, u16AWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16AWord<<2);
    u32Ret = MHBN_AddU32(u8R,u8A,u32B,u16AWord);
    memcpy(au32R, MH_ITEM_TO_MEM(u8R), u16AWord<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au32A, u16AWord << 2);
    ouputRes("R.\n", au32R, u16AWord << 2);
}

#define    SUBONE_SRC_DATA     "000000000100000000000000000000000000000000000000"
void SubOne_Test()
{
    uint8_t u8ASize, u8A, u8R, u8Spare = 0;
    uint16_t u16AByte,u16AWord;
    uint32_t au32A[32];
    uint32_t au32R[32];
    uint32_t u32Ret = 0;
    u16AByte = bn_read_string_from_head((uint8_t *)au32A, sizeof(SUBONE_SRC_DATA), SUBONE_SRC_DATA);
    u16AWord = MH_ROUND_UP(u16AByte, 4)>>2;
    
    u8ASize = MH_ROUND_UP(u16AWord, 8)>>3;
    u8A  = u8Spare; u8Spare += u8ASize;
    u8R  = u8Spare; u8Spare += u8ASize;
    memset(MH_ITEM_TO_MEM(u8R), 0, u16AWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16AWord<<2);
    u32Ret = MHBN_SubOne(u8R,u8A,u16AWord - 1);
    memcpy(au32R, MH_ITEM_TO_MEM(u8R), u16AWord<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au32A, u16AWord << 2);
    ouputRes("R.\n", au32R, u16AWord << 2);
}

#define   SUBU32_SRC_DATA     "000000000000000000000000000000000000000012345678"

void Subu32_Test()
{
    uint8_t u8ASize, u8A, u8R, u8Spare = 0;
    uint16_t u16AByte,u16AWord;
    uint32_t au32A[32];
    uint32_t au32R[32];
    uint32_t u32B, u32Ret = 0;
    u16AByte = bn_read_string_from_head((uint8_t *)au32A, sizeof(SUBU32_SRC_DATA), SUBU32_SRC_DATA);
    u16AWord = MH_ROUND_UP(u16AByte, 4)>>2;
    u32B = 0x21234567;
    u8ASize = MH_ROUND_UP(u16AWord, 8)>>3;
    u8A  = u8Spare; u8Spare += u8ASize;
    u8R  = u8Spare; u8Spare += u8ASize;
    memset(MH_ITEM_TO_MEM(u8R), 0, u16AWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16AWord<<2);
    u32Ret = MHBN_SubU32(u8R,u8A,u32B,u16AWord-1);
    memcpy(au32R, MH_ITEM_TO_MEM(u8R), u16AWord<<2);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
    ouputRes("A.\n", au32A, u16AWord << 2);
    ouputRes("R.\n", au32R, u16AWord << 2);
}

#define    CMP_SRC_DATA     "000000000000000000000000543210AB"
void CmpInt_Test()
{
    uint8_t u8ASize, u8A, u8Spare = 0;
    uint16_t u16AByte,u16AWord;
    uint32_t au32A[32];
    uint32_t u32B, u32Ret = 0;
    u16AByte = bn_read_string_from_head((uint8_t *)au32A, sizeof(CMP_SRC_DATA), CMP_SRC_DATA);
    u16AWord = MH_ROUND_UP(u16AByte, 4)>>2;
    u32B = 0x543210FB;
    u8ASize = MH_ROUND_UP(u16AWord, 8)>>3;
    u8A  = u8Spare; u8Spare += u8ASize;
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16AWord<<2);
    u32Ret = MHBN_ItemCmpIntEfc(u8A,u32B,u16AWord - 1);
    DBG_PRINT("u32Ret = %x\n",u32Ret);
}

//#define MONT_SRC_A   "B290D62F2D6B64831D47750C1DDAAA4D254FA3F14A069263FD4C4DB2CEE16430FF20BC04997ED7368B7BEFAC157CF5D5"
//#define MONT_SRC_P   "00000000CEE16430FF20BC04997ED7368B7BEFAC157CF5D5"
//#define MONT_SRC_PC  "83646B2D2FD690B2" //B290D62F2D6B6483
//#define MONT_SRC_PD  "000000000A54645BDADB6AA669ADCDA31A635F0DAFC8DFDA"

#define MONT_SRC_A   "39E86CB53BEDFDAAB290D62F2D6B64831D47750C1DDAAA4D254FA3F14A069263FD4C4DB2CEE16430FF20BC04997ED7368B7BEFAC157CF5D5"
#define MONT_SRC_P   "027789F4F8954AF8B5F43A6EC17179F7"
#define MONT_SRC_PC  "39E86CB53BEDFDAA" //AAFDED3BB56CE839
#define MONT_SRC_PD  "026376E6AF46A04C17028F89ABE12FDC"

void MontMod_Test()
{
    uint32_t au32A[32];
    uint32_t au32P[32];
    uint32_t au32PD[32];
    uint32_t au32R[32];
    uint32_t au32PC[2];
    MHBN_MOD BnMod;
    uint16_t u16LenByteA, u16LenByteP, u16LenWord, u16LenA ;
    uint8_t u8ItemSize, u8ItemSizeA, u8A, u8P, u8PD, u8R, u8Spare = 0;
    u16LenByteA = bn_read_string_from_head((uint8_t *)au32A, sizeof(au32A), MONT_SRC_A);
    u16LenByteP = bn_read_string_from_head((uint8_t *)au32P, sizeof(au32P), MONT_SRC_P);
    bn_read_string_from_head((uint8_t *)au32PD, sizeof(au32PD), MONT_SRC_PD);
    bn_read_string_from_head((uint8_t *)au32PC, sizeof(au32PC), MONT_SRC_PC);
    u16LenWord  =  MH_ROUND_UP(u16LenByteP,4) >> 2;
    u16LenA     =  MH_ROUND_UP(u16LenByteA,4) >> 2;
    
    u8ItemSize = MH_ROUND_UP(u16LenWord, 8);
    u8ItemSize = u8ItemSize >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    u8ItemSizeA = MH_ROUND_UP(u16LenA, 8);
    u8ItemSizeA = u8ItemSizeA >> MH_ASYMC_MEM_ITEM_WORD_BIT;
    
    u8A  = u8Spare; u8Spare += u8ItemSizeA;
    u8P  = u8Spare; u8Spare += u8ItemSize;
    u8PD = u8Spare; u8Spare += u8ItemSize;
    u8R  = u8Spare; u8Spare += u8ItemSize;
    
    memcpy(MH_ITEM_TO_MEM(u8A), au32A, u16LenA<<2);
    memcpy(MH_ITEM_TO_MEM(u8P), au32P, u16LenWord<<2);
    memcpy(MH_ITEM_TO_MEM(u8PD), au32PD, u16LenWord<<2);
    BnMod.u8P = u8P;
    BnMod.u8PD = u8PD;
    BnMod.pu32PC = au32PC; 
    
    if(MHBN_MontMod(u8R, u8A, &BnMod, u16LenA, u16LenWord, u8Spare) != MH_RET_CRYPT_SUCCESS)
    {
        DBG_PRINT("The MontMod calc is error!\n");
        while(1);
    }
    memcpy(au32R, MH_ITEM_TO_MEM(u8R), u16LenWord<<2);
    ouputRes("A.\n", au32A, u16LenA << 2);
    ouputRes("P.\n", au32P, u16LenWord << 2);
    ouputRes("R.\n", au32R, u16LenWord << 2);
//    while(1);
}


void BIGNUM_Test()
{
    Add_Test();
    AddOne_Test();
    SubOne_Test();
    Addu32_Test();
    Subu32_Test();
    CmpInt_Test();
    MontMod_Test();
    while(1);
}
#endif
