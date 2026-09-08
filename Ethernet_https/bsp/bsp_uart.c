#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp_api.h"
#include "bsp_uart.h"

#include "ring_buf.h"


//*****  UASRT DEMO USART ID CONFIG *********/
#define USART_TEST                  USART2

#define USE_USART_RX_DMAIDLE        1
#define USE_USART_TX_DMA            1
#define USE_USART_MODE_INT          2
#define USE_USART_MODE_LOOP         3


#define USE_USART_RX_MODE       USE_USART_RX_DMAIDLE
#define USE_USART_TX_MODE       USE_USART_MODE_LOOP


//*****  UASRT DEMO USART DMA CONFIG *********/
//测试的串口为串口2， 对应的的TX RX的使用DAM分别为5 6通道
#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE)
#define USART_RX_DMA_STREAM    DMA1_Stream5
#define RX_DMA_SIZE             128
#define RX_DMA_HALFSIZE         (RX_DMA_SIZE/2)
uint8_t RX_DMA_Buff[RX_DMA_SIZE];
#endif

#if (USE_USART_TX_MODE == USE_USART_TX_DMA)
#define USART_TX_DMA_STREAM    DMA1_Stream6
#define TX_DMA_SIZE             128
uint8_t TX_DMA_Buff[TX_DMA_SIZE];
#endif
/***********************************************/

#define UART_RBUF_SIZE   1204
uint8_t                  UART_rBuff[UART_RBUF_SIZE];
uint8_t                  UART_tBuff[UART_RBUF_SIZE];
static ST_UART_RING_BUFF uRxRingbuf = {0};
static ST_UART_RING_BUFF uTxRingbuf = {0};

void UART_DMA_RxConfiguration(void);
void UART_DMA_TxConfiguration(void * buff, u32 len);
void UART_DMA_TxStart(void * buff, u32 len);
void UART_NVIC_Configuration(void);

int bsp_uart_write(int8_t* data, uint32_t dataLen)
{
    int t_count = 0;

    if ((!data) || (dataLen == 0))
        return 0;

#if (USE_USART_TX_MODE == USE_USART_MODE_INT)
    int in_offset, out_offset;
    int last_len, space;

    in_offset  = uTxRingbuf.inIndex;
    out_offset = uTxRingbuf.outIndex;

    if (RINGBUF_IS_FULL(in_offset, out_offset, uTxRingbuf.bufSize)) return 0;

    space   = RINGBUF_SPACE(in_offset, out_offset, uTxRingbuf.bufSize);
    t_count = dataLen > space ? space : dataLen;

    last_len = RINGBUF_SPACE_TO_END(in_offset, out_offset, uTxRingbuf.bufSize);
    if (t_count <= last_len)
    {
        memcpy((u8*)(uTxRingbuf.buf + in_offset), data, t_count);
    }
    else
    {
        memcpy((u8*)(uTxRingbuf.buf + in_offset), data, last_len);
        memcpy(uTxRingbuf.buf, data + last_len, t_count - last_len);
    }
    uTxRingbuf.inIndex = (uTxRingbuf.inIndex + t_count) % uTxRingbuf.bufSize;

    // Enable the USART Transmoit interrupt when the  USARTx transmit data
    // register is empty
    if (RINGBUF_CNT(uTxRingbuf.inIndex, uTxRingbuf.outIndex, uTxRingbuf.bufSize) > 0)
    {
        uTxRingbuf.Rfu = 0;
        USART_ITConfig(USART_TEST, USART_IT_TXE, ENABLE);
    }
#elif (USE_USART_TX_MODE == USE_USART_TX_DMA )
    /*********************************************/
#if 0 //synchronous
    //DMA_Cmd(USART_TX_DMA_STREAM, DISABLE); 
    UART_DMA_TxStart(data, dataLen);
    while(DMA_GetFlagStatus(USART_TX_DMA_STREAM, DMA_FLAG_TCIF) == RESET) { }

    DMA_ClearITPendingBit(USART_TX_DMA_STREAM, DMA_FLAG_TCIF);
    DMA_ClearITPendingBit(USART_TX_DMA_STREAM, DMA_FLAG_HTIF);    
    DMA_Cmd(USART_TX_DMA_STREAM, DISABLE); 
#else  //asynchronous
    if(USART_TX_DMA_STREAM->CR & DMA_SxCR_EN)
        while(DMA_GetFlagStatus(USART_TX_DMA_STREAM, DMA_FLAG_TCIF) == RESET) { }
    DMA_ClearITPendingBit(USART_TX_DMA_STREAM, DMA_FLAG_TCIF);
    DMA_ClearITPendingBit(USART_TX_DMA_STREAM, DMA_FLAG_HTIF); 
    DMA_Cmd(USART_TX_DMA_STREAM, DISABLE);

    t_count = (dataLen > uTxRingbuf.bufSize) ? uTxRingbuf.bufSize : dataLen;
    memcpy(uTxRingbuf.buf, data, t_count);
    UART_DMA_TxStart(uTxRingbuf.buf, t_count);
#endif //
#else
    for (t_count = 0; t_count < dataLen; t_count++)
    {
        while (USART_GetFlagStatus(USART_TEST, USART_FLAG_TXE) == RESET) {}
        USART_SendData(USART_TEST, data[t_count]);
    }
#endif

     //if(t_count > 0)
     //   printf ("tx %d been n:%u %u\r\n", t_count, uTxRingbuf.outIndex, uTxRingbuf.inIndex);

    return t_count;
}

int bsp_uart_read(int8_t* data, uint32_t dataLen, uint32_t timeOut)
{
    int recv_byte = 0;
#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE) || (USE_USART_RX_MODE == USE_USART_MODE_INT)
    unsigned int count, last_len;
    int          in_offset, out_offset;
#endif //(defined(UART_RECV_INT) || (USE_USART_MODE == USE_USART_RX_DMAIDLE) )

    if ((!data) || (dataLen == 0)) return 0;

    recv_byte = 0;
    do
    {
#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE) || (USE_USART_RX_MODE == USE_USART_MODE_INT)
        in_offset  = uRxRingbuf.inIndex;
        out_offset = uRxRingbuf.outIndex;

        // printf("read offset %d in %d out %d\r\n", recv_byte, uartRecvBuf.inIndex, uartRecvBuf.outIndex);
        if (RINGBUF_IS_EMPTY(in_offset, out_offset, uRxRingbuf.bufSize))
            return 0;

        count     = RINGBUF_CNT(in_offset, out_offset, uRxRingbuf.bufSize);
        recv_byte = count > dataLen ? dataLen : count;

        last_len = RINGBUF_CNT_TO_END(in_offset, out_offset, uRxRingbuf.bufSize);
        if (recv_byte <= last_len)
        {
            memcpy(data, (u8*)(uRxRingbuf.buf + out_offset), recv_byte);
        }
        else
        {
            memcpy(data, (u8*)(uRxRingbuf.buf + out_offset), last_len);
            memcpy(data + last_len, uRxRingbuf.buf, recv_byte - last_len);
        }
        uRxRingbuf.outIndex = (uRxRingbuf.outIndex + recv_byte) % uRxRingbuf.bufSize;

         //if ( recv_byte )
         //    printf ("R: %d o:%d-%d n:%d-%d\r\n", recv_byte, in_offset, out_offset, uRxRingbuf.inIndex, uRxRingbuf.outIndex);
        if (timeOut == 0)
            break;
#else // RX MODE = loop
        while (USART_GetFlagStatus(USART_TEST, USART_FLAG_RXNE) == RESET) {}
        data[recv_byte++] = USART_ReceiveData(USART_TEST);
#endif //(USE_USART_RX_MODE == USE_USART_RX_DMAIDLE) || (USE_USART_RX_MODE == USE_USART_RX_INT)
    }
    while (recv_byte < dataLen);

    return recv_byte;
}

int bsp_uart_init(uint32_t bound)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    //开启对应IO，串口的时钟
    //Method  1
    PeripheralEnable(PeripheralUSART1, true);
    PeripheralEnable(PeripheralGPIOE, true);
    //Method 二
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    /* 连接 PXx 到 USARTx_Tx*/
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF8_USART2);
    /*  连接 PXx 到 USARTx__Rx*/
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF8_USART2);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = bound;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART_TEST, &USART_InitStructure);

    /* Enable the USAR1 Interrupt */
    UART_NVIC_Configuration();

#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE) || (USE_USART_RX_MODE == USE_USART_MODE_INT)
    memset(&uRxRingbuf, 0x00, sizeof(uRxRingbuf));
    uRxRingbuf.bufSize = UART_RBUF_SIZE;
    uRxRingbuf.buf     = UART_rBuff;
    //USART_ITConfig(USART_TEST, USART_IT_RXNE, ENABLE);
#endif

#if (USE_USART_TX_MODE == USE_USART_TX_DMA) || (USE_USART_TX_MODE == USE_USART_MODE_INT )
    memset(&uTxRingbuf, 0x00, sizeof(uTxRingbuf));
    uTxRingbuf.bufSize = UART_RBUF_SIZE;
    uTxRingbuf.buf     = UART_tBuff;
#endif

#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE)
    UART_DMA_RxConfiguration();

    USART_ITConfig(USART_TEST, USART_IT_IDLE, ENABLE);
    
    USART_ClearFlag(USART_TEST, USART_FLAG_IDLE); /*   *//* 解决第一个字节发不出去的问题 */

#elif (USE_USART_RX_MODE == USE_USART_MODE_INT) 
    USART_ITConfig(USART_TEST, USART_IT_IDLE, DISABLE);
    USART_ITConfig(USART_TEST, USART_IT_RXNE, ENABLE); 
    USART_ClearFlag(USART_TEST, USART_FLAG_IDLE); /*   *//* 解决第一个字节发不出去的问题 */
#endif


#if (USE_USART_TX_MODE == USE_USART_TX_DMA)
    UART_DMA_TxConfiguration(TX_DMA_Buff, TX_DMA_SIZE);
#endif

    USART_Cmd(USART_TEST, ENABLE);

    return 0;
}

int bsp_uart_close(void)
{
    memset(&uTxRingbuf, 0x00, sizeof(uTxRingbuf));
    return 0;
}


#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE)
void UART_DMA_RxConfiguration(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    //开启DMA通道的时钟
    //Method 1
    PeripheralEnable(PeripheralDMA1, ENABLE);
    //Method 2
    RCC_AHB4PeriphClockCmd(RCC_AHB4Periph_DMA1, ENABLE);
    
    // USART_RX DMA Config
    DMA_DeInit(USART_RX_DMA_STREAM);
    DMA_InitStructure.DMA_Channel = DMA_Channel_4; 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART_TEST->DR;
    /*内存地址(要传输的变量的指针)*/
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)RX_DMA_Buff;
    /*方向：从外设到内存*/        
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;    
    /*传输大小DMA_BufferSize=DEBUG_DATA_SIZE*/
    DMA_InitStructure.DMA_BufferSize = RX_DMA_SIZE;
    /*外设地址不增*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    /*内存地址自增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
    /*外设数据单位*/    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    /*内存数据单位 8bit*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    
    /*DMA模式：循环模式*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;     
    /*优先级：中*/    
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
    /*禁用FIFO*/
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
    /*存储器突发传输 1个节拍*/
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
    /*外设突发传输 1个节拍*/
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
    /*配置DMA的数据流7*/
    DMA_Init(USART_RX_DMA_STREAM, &DMA_InitStructure);
    
    //
    DMA_ClearFlag(USART_RX_DMA_STREAM, DMA_FLAG_TCIF);
    DMA_ITConfig(USART_RX_DMA_STREAM, DMA_IT_TE, ENABLE);
    DMA_ITConfig(USART_RX_DMA_STREAM, DMA_IT_TC, ENABLE);
    DMA_ITConfig(USART_RX_DMA_STREAM, DMA_IT_HT, ENABLE);

    USART_DMACmd(USART_TEST, USART_DMAReq_Rx, ENABLE);
    
    DMA_Cmd(USART_RX_DMA_STREAM, ENABLE);
}
#endif

#if (USE_USART_TX_MODE == USE_USART_TX_DMA)
void UART_DMA_TxStart(void * buff, u32 len)
{
    USART_TX_DMA_STREAM->M0AR = (u32)buff;
    USART_TX_DMA_STREAM->NDTR = len;
    
    DMA_Cmd(USART_TX_DMA_STREAM, ENABLE);
}

void UART_DMA_TxConfiguration(void * buff, u32 len)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    //开启DMA通道的时钟
    //Method 1
    PeripheralEnable(PeripheralDMA1, ENABLE);
    //Method 2
    RCC_AHB4PeriphClockCmd(RCC_AHB4Periph_DMA1, ENABLE);
    
     DMA_DeInit(USART_TX_DMA_STREAM);
    // USART_tX DMA Config
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
    /*设置DMA源：SPI数据寄存器地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART_TEST->DR;;
    /*内存地址(要传输的变量的指针)*/
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buff;
    /*方向：从内存到外设*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    /*传输大小DMA_BufferSize=DEBUG_DATA_SIZE*/
    DMA_InitStructure.DMA_BufferSize = len;
    /*外设地址不增*/        
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    /*内存地址自增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /*外设数据单位*/    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    /*内存数据单位 8bit*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    /*DMA模式：非循环模式*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    /*优先级：中*/    
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    /*禁用FIFO*/
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    /*存储器突发传输 1个节拍*/
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    /*外设突发传输 1个节拍*/
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
    /*配置DMA的数据流7*/
    DMA_Init(USART_TX_DMA_STREAM, &DMA_InitStructure);
    
    //
    USART_DMACmd(USART_TEST, USART_DMAReq_Tx, ENABLE);
    DMA_ClearFlag(USART_TX_DMA_STREAM, DMA_FLAG_TCIF);
    
    //DMA_ITConfig(USART_TX_DMA_STREAM, DMA_IT_TC, ENABLE);

    DMA_Cmd(USART_TX_DMA_STREAM, DISABLE);
}
#endif

void UART_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE)
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
#endif

#if (USE_USART_TX_MODE == USE_USART_TX_DMA)
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
#endif

}

/************************************************************
 * @brief   Uart_DMA_Rx_Data
 * @param   NULL
 * @return  NULL
 * @author  mh
 * @date    2018-xx-xx
 * @version v1.0
 * @note    使用串口 DMA 接收时调用的函数
 ***********************************************************/
#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE)
void Receive_DataPack(void)
{
    int i, count;
    /* 接收的数据长度 */
    uint32_t buff_length = 0, offset = 0;

    /* 关闭DMA ，防止干扰 */
    DMA_Cmd(USART_RX_DMA_STREAM, DISABLE); /* 暂时关闭dma，数据尚未处理 */

    /* 清DMA标志位 */
    DMA_ClearFlag(USART_RX_DMA_STREAM, DMA_FLAG_TCIF);
    DMA_ClearFlag(USART_RX_DMA_STREAM, DMA_FLAG_HTIF);

    /* 获取接收到的数据长度 单位为字节*/
    offset = 0;
    buff_length = (RX_DMA_SIZE - USART_RX_DMA_STREAM->NDTR);
    if(buff_length == RX_DMA_SIZE) return ;
    
    if (buff_length >= RX_DMA_HALFSIZE)
    {
        offset = RX_DMA_HALFSIZE;
        buff_length -= RX_DMA_HALFSIZE;
    }

    //printf("buff_length = %u\n", buff_length);
    count = RINGBUF_SPACE(uRxRingbuf.inIndex, uRxRingbuf.outIndex, uRxRingbuf.bufSize);
    if (count < buff_length)
        uRxRingbuf.outIndex = (uRxRingbuf.outIndex + (buff_length - count)) % uRxRingbuf.bufSize;
    for (i = 0; i < buff_length; i++)
    {
        // printf("data= %x\n",DMA_Buff[i]);
        uRxRingbuf.buf[uRxRingbuf.inIndex++] = RX_DMA_Buff[offset + i];
        if (uRxRingbuf.inIndex == uRxRingbuf.bufSize) uRxRingbuf.inIndex = 0;
    }
    
    /* 重新赋值计数值，必须大于等于最大可能接收到的数据帧数目 */
    USART_RX_DMA_STREAM->NDTR = RX_DMA_SIZE;
    USART_RX_DMA_STREAM->M0AR = (u32)RX_DMA_Buff;

    /* 此处应该在处理完数据再打开，如在 DataPack_Process() 打开*/
    DMA_Cmd(USART_RX_DMA_STREAM, ENABLE);

    /*
    DMA 开启，等待数据。注意，如果中断发送数据帧的速率很快，MCU来不及处理此次接收到的数据，
    中断又发来数据的话，这里不能开启，否则数据会被覆盖。有2种方式解决：
    1. 在重新开启接收DMA通道之前，将Rx_Buf缓冲区里面的数据复制到另外一个数组中，
    然后再开启DMA，然后马上处理复制出来的数据。
    2. 建立双缓冲，重新配置DMA_MemoryBaseAddr的缓冲区地址，那么下次接收到的数据就会
    保存到新的缓冲区中，不至于被覆盖。
    */
}
#endif


static void USART_IRQHandler(int index)
{
    int j;

    // printf("T%x\r\n", USART_TEST->CR1);
    if (USART_GetITStatus(USART_TEST, USART_IT_IDLE) != RESET)
    {
        //printf("T%x-%d\r\n", USART_TEST->CR1, USART_RX_DMA_CHANNEL->CNDTR);
#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE)
        if ((RX_DMA_SIZE - USART_RX_DMA_STREAM->NDTR) > 0)
            Receive_DataPack();
#endif
        USART_ReceiveData(USART_TEST);
        USART_ClearFlag(USART_TEST, USART_FLAG_IDLE);
    }

    if (USART_GetFlagStatus(USART_TEST, USART_FLAG_RXNE) == SET)
    {
        USART_ClearFlag(USART_TEST, USART_FLAG_RXNE);
        j = RINGBUF_SPACE(uRxRingbuf.inIndex, uRxRingbuf.outIndex, uRxRingbuf.bufSize);
        if (j < 1)
            uRxRingbuf.outIndex = (uRxRingbuf.outIndex + 1) % uRxRingbuf.bufSize;
        // printf("R%x\r\n", UART_rRingbuf.inIndex);
        uRxRingbuf.buf[uRxRingbuf.inIndex++] = USART_ReceiveData(USART_TEST);
        if (uRxRingbuf.inIndex == uRxRingbuf.bufSize)
            uRxRingbuf.inIndex = 0;
    }

    if (USART_GetITStatus(USART_TEST, USART_IT_TXE) != RESET)
    {
        USART_ClearFlag(USART_TEST, USART_IT_TXE);
        j = RINGBUF_CNT(uTxRingbuf.inIndex, uTxRingbuf.outIndex, uTxRingbuf.bufSize);

        // printf("R:%x-%x\r\n", j, UART_tRingbuf.outIndex);
        if (j < 1)
            USART_ITConfig(USART_TEST, USART_IT_TXE, DISABLE);
        else
        {
            USART_SendData(USART_TEST, uTxRingbuf.buf[uTxRingbuf.outIndex++] & 0x00ff);
            if (uTxRingbuf.outIndex == uRxRingbuf.bufSize)
                uTxRingbuf.outIndex = 0;
        }
    }
}

void USART2_IRQHandler(void)
{
    USART_IRQHandler(2);
}

void USART3_IRQHandler(void)
{
    USART_IRQHandler(3);
}

void DMA1_Stream5_IRQHandler(void)
{
    int i, count = 0;
#if (USE_USART_RX_MODE == USE_USART_RX_DMAIDLE)
    // printf("D-INT %x\r\n", DMA1->ISR);
    if (DMA_GetITStatus(DMA1_Stream5, DMA_IT_HTIF) == SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_FLAG_HTIF);
        count = RINGBUF_SPACE(uRxRingbuf.inIndex, uRxRingbuf.outIndex, uRxRingbuf.bufSize);
        if (count < RX_DMA_HALFSIZE)
            uRxRingbuf.inIndex = (uRxRingbuf.inIndex + (RX_DMA_HALFSIZE - count)) % uRxRingbuf.bufSize;
        for (i = 0; i < RX_DMA_HALFSIZE; i++)
        {
            uRxRingbuf.buf[uRxRingbuf.inIndex++] = RX_DMA_Buff[i];
            if (uRxRingbuf.inIndex == uRxRingbuf.bufSize)
                uRxRingbuf.inIndex = 0;
        }
    }

    if (DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF) == SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_FLAG_TCIF);
        count = RINGBUF_SPACE(uRxRingbuf.inIndex, uRxRingbuf.outIndex, uRxRingbuf.bufSize);
        if (count < RX_DMA_HALFSIZE)
            uRxRingbuf.inIndex = (uRxRingbuf.inIndex + (RX_DMA_HALFSIZE - count)) % uRxRingbuf.bufSize;
        for (i = RX_DMA_HALFSIZE; i < RX_DMA_SIZE; i++)
        {
            uRxRingbuf.buf[uRxRingbuf.inIndex++] = RX_DMA_Buff[i];
            if (uRxRingbuf.inIndex == uRxRingbuf.bufSize)
                uRxRingbuf.inIndex = 0;
        }
    }

    if (DMA_GetITStatus(DMA1_Stream5, DMA_IT_TEIF) == SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_FLAG_HTIF);
        // DMA_Flag = 1;
    }
#endif
}

void DMA1_Stream6_IRQHandler(void)
{
     printf("D-INT %x\r\n", DMA1->LISR);
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_HTIF) == SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_FLAG_HTIF);
    }

    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF) == SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_FLAG_TCIF);
    }

    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TEIF) == SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_FLAG_HTIF);
        // DMA_Flag = 1;
    }
}


