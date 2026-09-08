#include "adc_test.h"
#include "stdio.h"

void Sequence(Type_HistogramDef *Histogram);
void PrintHistogram(Type_HistogramDef *Histogram);
uint8_t AddX_AxisData(Type_HistogramDef *Histogram, uint16_t X_data);
void HistogramStructInit(Type_HistogramDef *Histogram, uint32_t *buf, uint16_t bufLen);

//频率统计表结构初始化
void HistogramStructInit(Type_HistogramDef *Histogram, uint32_t *buf, uint16_t bufLen)
{
    memset(buf, 0, bufLen);
    memset(Histogram, 0, sizeof(Type_HistogramDef));
    
    Histogram->Table = (Type_TableDataDef *)buf;
    Histogram->TableSize = bufLen / 4;
    Histogram->Frequency_MAX = 0;
    Histogram->Frequency_MIN = ~0;
}
//添加频率统计数据
uint8_t AddX_AxisData(Type_HistogramDef *Histogram, uint16_t X_data)
{
    uint16_t i, isFindX_Data = 0;

    for (i = 0; i < Histogram->TableSize && (0 != Histogram->Table[i].Y_Axis); i++)
    {
        if (X_data == Histogram->Table[i].X_Axis)
        {
            Histogram->Table[i].Y_Axis ++;
            isFindX_Data = 1;
            break;
        }
    }
    if (!isFindX_Data)
    {
        Histogram->Table[i].X_Axis = X_data;
        Histogram->Table[i].Y_Axis ++;
    }
    if (Histogram->Table[i].Y_Axis > Histogram->Frequency_MAX)  Histogram->Frequency_MAX = Histogram->Table[i].Y_Axis;
    if (Histogram->Table[i].Y_Axis < Histogram->Frequency_MIN)  Histogram->Frequency_MIN = Histogram->Table[i].Y_Axis;
    
    return Histogram->TableSize - i - 1;    //返回剩余空间
}
//输出频率分布表
void PrintHistogram(Type_HistogramDef *Histogram)
{
    uint16_t i, j;
	float UnitLength;
	if(Histogram->Frequency_MAX - Histogram->Frequency_MIN)
		UnitLength = (float)WINDOWN_WIDTH / (float)(Histogram->Frequency_MAX - Histogram->Frequency_MIN); //缩放至窗口范围
	else
		UnitLength = (float)WINDOWN_WIDTH;

    Sequence(Histogram);    //对频率进行排序
    printf("\r\n+");
    for (i = 0; i < WINDOWN_WIDTH + 15; i++)
    {
        printf("-");
    }
    printf("\r\n");
    for (i = 0; i < Histogram->TableSize; i++)
    {
        if (0UL == Histogram->Table[i].Y_Axis)  //数据打印结束
        {
            break;
        }
        else
        {
            printf("*%d\t| ", Histogram->Table[i].X_Axis);
            for (j = 0; j < (UnitLength * Histogram->Table[i].Y_Axis + 0.5); j++)
            {
                printf("o");
            }
            printf("  %d\r\n", Histogram->Table[i].Y_Axis);
        }
    }
    printf("+");
    for (i = 0; i < WINDOWN_WIDTH + 15; i++)
    {
        printf("-");
    }
    printf("\r\n");
}
//计算方差,均值,最大最小值,统计频率分布
void AnalyzeData(Type_HistogramDef *Histogram, uint16_t *buf, uint16_t BufLen,uint8_t Channels, uint8_t Channel_number)
{
    uint16_t i, max, min;
    double Sum ,f16Average;
    uint16_t RemainTableSize = 0xFFFF, ret;

    Sum = 0;
    max = 0;
    min = 4096;
    
    for (i = 0; i < BufLen; i++)
    {
        Sum += buf[i*Channels + Channel_number];
        if (buf[i*Channels + Channel_number] > max)   max = buf[i*Channels + Channel_number];
        if (buf[i*Channels + Channel_number] < min)   min = buf[i*Channels + Channel_number];
#if PRINT_ALL_DATA
		printf("%4d:%04d\r\n",i,buf[i*Channels + Channel_number]);
#endif
        if (RemainTableSize > (ret = AddX_AxisData(Histogram, buf[i*Channels + Channel_number])))       //添加数据到频率统计表
        {
            RemainTableSize = ret;
        }
    }
    f16Average = Sum / BufLen;

    Sum = 0;
    for (i = 0; i < BufLen; i++)            //计算方差
    {
        Sum += ( (double)buf[i*Channels + Channel_number] - f16Average) * ( (double)buf[i*Channels + Channel_number] - f16Average);
    }
    
    printf("+-----------------------+\r\n");
    printf("* 采样数:\t  %6d\r\n", BufLen);
    printf("* 最大值:\t  %6d\r\n", max);
    printf("* 最小值:\t  %6d\r\n", min);
    printf("* 电压浮动:\t%6.0fmV\r\n", (float)(max - min) * 3300 / 4096);
    printf("* 电压:\t\t  %6.0fmV\r\n", f16Average * 3300 / 4096);
    printf("* 均值:\t\t  %6.2f\r\n", f16Average);
    printf("* 方差:\t\t  %6.4f\r\n", Sum / BufLen);
    printf("+-----------------------+\r\n");
    printf("*频率分布\t 数据表剩余容量 %d/%d %s", RemainTableSize, Histogram->TableSize, (RemainTableSize > 0)?(" "):("数据表容量不足,部分数据未显示!"));
    
    PrintHistogram(Histogram);
}
//直接插入排序,对频率分布进行排序显示
void Sequence(Type_HistogramDef *Histogram)
{
    int16_t i,j;
    Type_TableDataDef TableTmp;
    
    for(j = 1; j < Histogram->TableSize && 0 != Histogram->Table[j].Y_Axis ; j++) //直接插入排序
    {
        TableTmp = Histogram->Table[j];
        i = j - 1;
        //注:两个条件顺序不可调换
        while(i >= 0 && Histogram->Table[i].X_Axis > TableTmp.X_Axis)     //从大到小: < ;从小到大: >
        {                                                                 //Histogram->Table[i].Y_Axis > TableTmp.Y_Axis以频率值进行排序
            Histogram->Table[i + 1] = Histogram->Table[i];
            i--;
        }
        Histogram->Table[i + 1] = TableTmp;
    }
}
