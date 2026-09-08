#include "USBMSD.h"
#include "USBDevice.h"
#include "VirtualFatFS.h"

#define FLASH_FILESYSTEM_ADDR   (0x08081000)
#define FLASH_FILESYSTEM_SIZE   (KB(256))
#define FLASH_SECTOR_SIZE       (KB(4))
#define TEST_FILE_SIZE          (KB(64))

uint8_t sector_buf[FLASH_SECTOR_SIZE]  __attribute__((aligned(4))) = {0} ;
extern void send_char(uint8_t* data,uint32_t len);

InterfaceMSDStruct InterfaceMSD;

static bool MSDDiskStatus(InterfaceMSDStruct* self)
{
    return false;
}

static uint32_t MSDDiskRead(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t blockAddr, uint32_t blockCount)
{
    return vfs_read(blockAddr, buffer, blockCount);
}

static void MSDDiskWrite(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t blockAddr, uint32_t blockSize)
{
    vfs_write(blockAddr, buffer, blockSize);
}

#if (USBMSD_DEMO == USBMSD_DEMO_VFS_STREAM)
BlockStruct StreamBlock;

bool MSDStreamSend(uint32_t* buffer, uint32_t bytesToSend, void (*popCallback)(BlockStruct* block))
{
    __disable_irq();
    bool result = false;
    if (StreamBlock.Locked)
    {
        result = false;
    }
    else
    {
        StreamBlock.Buffer      = buffer;
        StreamBlock.Size        = bytesToSend;
        StreamBlock.PopCallback = popCallback;
        StreamBlock.Locked      = true;
        result                  = true;
    }
    __enable_irq();
    return result;
}

static uint32_t MSDStreamRead(uint32_t sector_offset, uint8_t* data, uint32_t num_sectors)
{
    printf_dbg("Read sector %d,%d.\n", sector_offset, num_sectors);

    if (!StreamBlock.Locked)
        return 0;
    
    int readSize = num_sectors * VFS_SECTOR_SIZE;
    int readableSize = StreamBlock.Size - sector_offset*VFS_SECTOR_SIZE;
    
    readSize     = MIN(readSize, readableSize);
    FLASH_ReadPage(NULL,NULL,data,((uint32_t)StreamBlock.Buffer) + sector_offset*VFS_SECTOR_SIZE,readSize);
    
    printf("readSize %d.\n", readSize);
    
    return num_sectors * VFS_SECTOR_SIZE;
}

static void Flash_FileSystem_area_init(uint32_t addr,uint32_t size)
{
    uint32_t sector_num = 0;
    
    sector_num = size/FLASH_SECTOR_SIZE;
    if(size%FLASH_SECTOR_SIZE)sector_num++;
    
    for(uint32_t i = 0; i < sector_num; i++)
    {
        FLASH_EraseSector(addr + (i*FLASH_SECTOR_SIZE));
    }
}


static void MSDStreamWrite(uint32_t sector_offset, const uint8_t* data, uint32_t num_sectors)
{
    printf_dbg("Write sector %d,%d.\n", sector_offset, num_sectors);
    
    if (!StreamBlock.Locked)
        return;
    
    uint32_t writeSize = num_sectors * VFS_SECTOR_SIZE;
    uint32_t write_addr = 0;
    
    uint32_t flash_sector_num = 0;
    uint32_t flash_sector_addr = 0;
    uint32_t flash_sector_ablesize = 0;
    uint32_t flash_sector_headremainder = 0;
    uint32_t flash_sector_endremainder = 0;
    
    uint32_t sector_buf_write_addr = 0;
    uint32_t sector_buf_write_size = 0;
    
    memset(sector_buf,0,sizeof(sector_buf));
    
    write_addr = (uint32_t)(((uint8_t*)StreamBlock.Buffer) + sector_offset * VFS_SECTOR_SIZE);
    
    if((write_addr&0xFFFFF000) == ((write_addr + writeSize)&0xFFFFF000))
    {
        flash_sector_num = 1;
    }
    else
    {
        flash_sector_headremainder = FLASH_SECTOR_SIZE - (write_addr % FLASH_SECTOR_SIZE);
        flash_sector_endremainder = ((write_addr + writeSize) % FLASH_SECTOR_SIZE);
        flash_sector_num = (writeSize - flash_sector_headremainder - flash_sector_endremainder)/FLASH_SECTOR_SIZE;
        
        if(flash_sector_headremainder)flash_sector_num++;
        if(flash_sector_endremainder)flash_sector_num++;
    }
    
    printf_dbg("writeSize = %d\n",writeSize);
    printf_dbg("flash_sector_num = %d\n",flash_sector_num);
    
    for(uint32_t i = 0;i < flash_sector_num;i++)
    {
        //计算要写入到flash的地址
        write_addr = (uint32_t)(((uint8_t*)StreamBlock.Buffer) + sector_offset * VFS_SECTOR_SIZE + i*FLASH_SECTOR_SIZE);
        printf_dbg("write_adder = 0x%08x\n",write_addr);
        
        //计算要写入的扇区
        flash_sector_addr = write_addr;
        flash_sector_addr &= ~0x00000FFF;
        printf_dbg("flash_sector_adder = 0x%08x\n",flash_sector_addr);
        
        //计算当前扇区可写入数据大小
        flash_sector_ablesize = FLASH_SECTOR_SIZE - (write_addr - flash_sector_addr);
        printf_dbg("flash_sector_ablesize = %d\n",flash_sector_ablesize);
        
        //计算要写入到扇区缓冲的地址
        sector_buf_write_addr = (uint32_t)(sector_buf + (write_addr - flash_sector_addr));
        printf_dbg("sector_buf_write_adder = 0x%08x\n",sector_buf_write_addr);
        
        //计算本次要写入的数据大小
        sector_buf_write_size = writeSize;
        if(sector_buf_write_size > flash_sector_ablesize)sector_buf_write_size = flash_sector_ablesize;
        printf_dbg("sector_buf_write_size = %d\n",sector_buf_write_size);
        
        //读取扇区内容到缓冲
        for(uint32_t j = 0; j < FLASH_SECTOR_SIZE/QSPI_PAGE_SIZE;j++)
        {
            FLASH_ReadPage(NULL,NULL,sector_buf + j*QSPI_PAGE_SIZE,flash_sector_addr + j*QSPI_PAGE_SIZE,QSPI_PAGE_SIZE);
        }
        
        //写入数据到扇区缓冲
        memcpy((void*)sector_buf_write_addr,data,sector_buf_write_size);
        
        //擦除扇区
        FLASH_EraseSector(flash_sector_addr);
        
        //将扇区缓冲重新写入到指定扇区
        for(uint32_t j = 0; j < FLASH_SECTOR_SIZE/QSPI_PAGE_SIZE;j++)
        {
            FLASH_ProgramPage(NULL, NULL,flash_sector_addr + j*QSPI_PAGE_SIZE, QSPI_PAGE_SIZE, sector_buf + j*QSPI_PAGE_SIZE);
        }
        
        writeSize -= sector_buf_write_size;
    }
    printf_dbg("\n");
}

#elif (USBMSD_DEMO == USBMSD_DEMO_VFS_SWKEY)

static const uint8_t USBKeySWInquiry[36] = {
    0x00, // Direct Access Device
    0x80, // RMB 80
    0x02, // ISO/ECMA/ANSI
    0x02, // Response Data Format: SCIS-2
    0x1f, // Additional Length
    0x00, 0x00,
    0x00, // Reserved

    'S',  'W',  'U', 'K', 'E', 'Y', ' ', ' ',                                         //
    ' ',  ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //
    ' ',  ' ',  ' ', ' '                                                              //
};

static bool SWKeyCBWDecode(InterfaceMSDStruct* self)
{
    switch (self->CBW.CB[0])
    {
        // override default SCSI Command or deal Custom SCSI Command decode here
        // case EXAMPLE_CMD:
        //     ExampleDecode(...);
        //     return true;
    }
    return false;
}

static bool SWKeyInProcess(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t bytesWritten)
{
    switch (self->CBW.CB[0])
    {
        // override default SCSI Command or deal Custom SCSI Command in process here
        // case EXAMPLE_CMD:
        //     ExampleProcess(...);
        //     return true;
    }
    return false;
}

static bool SWKeyOutProcess(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t bytesRead)
{
    switch (self->CBW.CB[0])
    {
        // override default SCSI Command or deal Custom SCSI Command out process here
        // case EXAMPLE_CMD:
        //     ExampleProcess(...);
        //     return true;
    }
    return false;
}

#elif (USBMSD_DEMO == USBMSD_DEMO_VFS_CDROM)

static const uint8_t USBCDROMInquiry[] = {
    0x05, 0x80, 0x02, 0x02, 0x1F, 0x00, 0x00, 0x00,      //
    'M',  'e',  'g',  'a',  'H',  'u',  'n',  't',       //
    'V',  'F',  'S',  '-',  'C',  'D',  'R',  'O',  'M', //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,            //
    '1',  '.',  '0',  0x00,
};

#endif

uint32_t MSDReadBuffer[ToUintSize(VFS_SECTOR_SIZE)];
uint32_t MSDWriteBuffer[ToUintSize(VFS_SECTOR_SIZE)];

void MSDSetup(USBDeviceStruct* device)
{
    InterfaceMSDStruct* msd = &InterfaceMSD;
    InterfaceMSDConstractor(msd);

    msd->DiskStatusCallback = MSDDiskStatus;
    msd->DiskReadCallback   = MSDDiskRead;
    msd->DiskWriteCallback  = MSDDiskWrite;

#if (USBMSD_DEMO == USBMSD_DEMO_VFS_STREAM)
    #if 0
    // Visual FileSystem Init
    vfs_init((vfs_filename_t) {"ESL-Disk"}, MB(64));
    FatDirectoryEntry_t* streamFile = vfs_create_file((vfs_filename_t) {"DISK    RAW"}, MSDStreamRead, MSDStreamWrite, KB(512));
    streamFile->attributes          = VFS_FILE_ATTR_HIDDEN;

    msd->base.Name = L"USB-ESL Mass Storage Device";

    StreamBlock.Locked      = false;
    StreamBlock.Size        = 0;
    StreamBlock.Buffer      = NULL;
    StreamBlock.PopCallback = NULL;
    #else
    //Flash_FileSystem_area_init(FLASH_FILESYSTEM_ADDR,FLASH_FILESYSTEM_SIZE);
    
    vfs_init((vfs_filename_t) {"ESL-Disk"}, FLASH_FILESYSTEM_SIZE);
    FatDirectoryEntry_t* streamFile = vfs_create_file((vfs_filename_t) {"DISK    TXT"}, MSDStreamRead, MSDStreamWrite, TEST_FILE_SIZE);
    streamFile->attributes          = VFS_FILE_ATTR_SYSTEM;

    msd->base.Name = L"USB-ESL Mass Storage Device";

    StreamBlock.Locked      = true;
    StreamBlock.Size        = FLASH_FILESYSTEM_SIZE;
    StreamBlock.Buffer      = (uint32_t *)FLASH_FILESYSTEM_ADDR;
    
    StreamBlock.PopCallback = NULL;
    #endif
#elif (USBMSD_DEMO == USBMSD_DEMO_VFS_SWKEY)
    // Visual FileSystem Init
    vfs_init((vfs_filename_t) {"ESL-Key"}, MB(64));

    msd->base.Name = L"USB-ESL Key";

    msd->InquiryData        = (uint32_t*)USBKeySWInquiry;
    msd->InquirySize        = sizeof(USBKeySWInquiry);
    msd->CBWDecodeCallback  = SWKeyCBWDecode;
    msd->InProcessCallback  = SWKeyInProcess;
    msd->OutProcessCallback = SWKeyOutProcess;
#elif (USBMSD_DEMO == USBMSD_DEMO_VFS_CDROM)
    vfs_init((vfs_filename_t) {"ESL-CD"}, MB(64));

    msd->base.Name = L"USB-ESL CDROM";

    msd->InquiryData = (uint32_t*)USBCDROMInquiry;
    msd->InquirySize = sizeof(USBCDROMInquiry);
#endif

    // Setup MSD Properties
    msd->BlockSize       = VFS_SECTOR_SIZE;
    msd->BlockCount      = vfs_get_total_size() / VFS_SECTOR_SIZE;
    msd->ReadBuffer      = MSDReadBuffer;
    msd->ReadBufferSize  = sizeof(MSDReadBuffer);
    msd->WriteBuffer     = MSDWriteBuffer;
    msd->WriteBufferSize = sizeof(MSDWriteBuffer);

    // Setup MSD Endpoint
    msd->RxEPConfig.Index = 1;
    msd->TxEPConfig.Index = 1;

    // Improve the transfer performance of USB
    msd->RxEPConfig.MaxPacket        = 512;
    msd->RxEPConfig.AdditionalBuffer = 1;
    msd->TxEPConfig.MaxPacket        = 512;
    msd->TxEPConfig.AdditionalBuffer = 1;

    device->RegisterInterface(device, &msd->base);
}
