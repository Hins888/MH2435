#include "jpeg_check.h"

//数据构成：无关数据0，start1, end 1，无关数据3，start2，end2，无关数据4, 数据交换为ABC->BCA
static char jpeg_segment_swap(uint8_t *buf, long bufLength, long start1, long end1, long start2, long end2)
{
    if(start1 > bufLength)return -1;
    if(end1 > bufLength)return -1;
    if(start2 > bufLength)return -1;
    if(end2 > bufLength)return -1;

    uint16_t len1 = end1 - start1;
    uint16_t len2 = end2 - start2;
    uint8_t *tempBuf = malloc(len1);
    if(tempBuf == NULL)return -1;

    memset(tempBuf, 0, len1);

    memcpy(tempBuf, buf + start1, len1);
    memcpy(buf + start1, buf + end1, end2 - end1);

    memcpy(buf + end2 - len1, tempBuf, len1);
    
    free(tempBuf);
    return 1;
}

bool jpeg_check_head_swap(uint8_t *buf, long length)
{
    long dqtStartAddress = 0, dqtEndAddress = 0;
    long sofStartAddress = 0, sofEndAddress = 0;
    
	JPEG_CHECK_DEBUG("%s %d \r\n",__func__,__LINE__);	
    for(uint32_t i = 0; i < JPEG_MAX_HEAD; i++)
    {
        if(*(uint16_t*)(&buf[i]) == BIG_LITTLE_SWAP16(DQT_MARKER))
        {
            JPEG_CHECK_DEBUG("find dqt start: %d\n",i);
            JPEG_CHECK_DEBUG("find dqt len: %d\n",BIG_LITTLE_SWAP16(*(uint16_t*)(&buf[i + 2])));
            //not need to swap segment ,dqt front of sof0
            if(!sofStartAddress)return true;
            
            if(!dqtStartAddress)dqtStartAddress = i;
            i += sizeof(DQT_MARKER) + BIG_LITTLE_SWAP16(*(uint16_t*)(&buf[i + 2]));
            dqtEndAddress = i;
            i -= 1;
            //if next segment is not dqt,can break;
            if(*(uint16_t*)(&buf[i + 1]) != BIG_LITTLE_SWAP16(DQT_MARKER))break;
        }
        
        if(*(uint16_t*)(&buf[i]) == BIG_LITTLE_SWAP16(SOF_MARKER))
        {
            JPEG_CHECK_DEBUG("find sof\n");
            JPEG_CHECK_DEBUG("find sof start: %d\n",i);
            JPEG_CHECK_DEBUG("find sof len: %d\n",BIG_LITTLE_SWAP16(*(uint16_t*)(&buf[i + 2])));
            
            if(!sofStartAddress)sofStartAddress = i;
            i += sizeof(SOF_MARKER) + BIG_LITTLE_SWAP16(*(uint16_t*)(&buf[i + 2]));
            sofEndAddress = i;
            i -= 1;
        }
        
        if(*(uint16_t*)(&buf[i]) == BIG_LITTLE_SWAP16(SOS_MARKER))
        {
            JPEG_CHECK_DEBUG("find sos\n");
            JPEG_CHECK_DEBUG("find sos start: %d\n",i);
            break;
        }
    }
    
    if(!sofStartAddress || !dqtStartAddress)
    {
        return false;
    }
    
	JPEG_CHECK_DEBUG("%s %d \r\n",__func__,__LINE__);	
    //dqt is not front of sof0, now we need to swap segment
    jpeg_segment_swap(buf, length, sofStartAddress, sofEndAddress, dqtStartAddress, dqtEndAddress);
    
	JPEG_CHECK_DEBUG("%s %d \r\n",__func__,__LINE__);	
    return true;
}

bool jpeg_check_start(uint8_t* buf,uint32_t len)
{
    if(*((uint16_t*)buf) == BIG_LITTLE_SWAP16(SOI_MARKER))return true;
    return false;
}

bool jpeg_check_end(uint8_t* buf,uint32_t len)
{
    if(len < 10)return false;
    
    for(uint32_t i = len - 4;i < len; i++)
    {
        if(*((uint16_t*)&buf[i - 1]) == BIG_LITTLE_SWAP16(EOI_MARKER))return true;
    }
    
    return false;
}
