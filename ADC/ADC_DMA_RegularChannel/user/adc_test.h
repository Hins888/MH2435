#include "mh2435.h"



/***************************Configuration**************************************************/

#define WINDOWN_WIDTH   	80  			//Ƶ�ʷֲ���������ڿ���
#define ADC_BUFFER_SIZE     300            	//��������
#define TABLE_SIZE          80              //Ƶ�ʷֲ���С
#define PRINT_ALL_DATA      0               //1�����ÿ�����ԭʼֵ����ѹ��0�������
#define CHANNEL_NUM		2
/******************************************************************************************/



typedef struct
{
    uint16_t X_Axis;
    uint16_t Y_Axis;
}Type_TableDataDef;

typedef struct
{
    Type_TableDataDef *Table;
    uint16_t TableSize;
    uint16_t Frequency_MAX;
    uint16_t Frequency_MIN;
}Type_HistogramDef;


void AnalyzeData(Type_HistogramDef *Histogram, uint16_t *buf, uint16_t BufLen,uint8_t Channels, uint8_t Channel_number);
void HistogramStructInit(Type_HistogramDef *Histogram, uint32_t *buf, uint16_t bufLen);

