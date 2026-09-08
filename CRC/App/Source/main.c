#include "bsp_crc.h"

int main(void) {
    ErrorStatus result = SUCCESS;
    uint32_t    step   = 0;

    RetargetIOSetup(CONFIG_RETARGETIO_DEFAULT_SERIAL);

    printf(" CRC Test V1.0 start......\r\n");
    while (1) {
        switch (step) {

            case 0:
                result = CRC_CalculationFunction();
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                step++;
                break;

            case 1:
                result = CRC_BytesNumFunction(CRC_Type_16);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                result = CRC_BytesNumFunction(CRC_Type_32);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                step++;
                break;

            case 2:
                result = CRC_XORFunction(CRC_Type_16);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                result = CRC_XORFunction(CRC_Type_32);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                step++;
                break;

            case 3:
                result = CRC_ResultReversionFunction(CRC_Type_16);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                result = CRC_ResultReversionFunction(CRC_Type_32);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                step++;
                break;

            case 4:
                result = CRC_InputBytesReversionFunction(CRC_Type_16);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                result = CRC_InputBytesReversionFunction(CRC_Type_32);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                step++;
                break;

            case 5:
                result = CRC_InputBitsReversionFunction(CRC_Type_16);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                result = CRC_InputBitsReversionFunction(CRC_Type_32);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                step++;
                break;

            case 6:
                result = CRC_InitValueFunction(CRC_Type_16);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                result = CRC_InitValueFunction(CRC_Type_32);
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                step++;
                break;

            case 7:
                result = CRC_PolyChangeCRC16Function();
                printf("case %d:%s\n", step, (result == SUCCESS) ? "ok" : "fail...");
                step++;
                break;

            case 8:
                printf("------\n");
                step++;
                break;

            default:
                break;
        }
    }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
     ex: DBG_PRINT("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {}
}
#endif
