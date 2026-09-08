#include "USBMSD.h"
#include "USBDevice.h"
#include "VirtualFatFS.h"

InterfaceMSDStruct InterfaceMSD;

static bool MSDDiskStatus(InterfaceMSDStruct* self) {
    return false;
}

static uint32_t MSDDiskRead(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t blockAddr, uint32_t blockCount) {
    return vfs_read(blockAddr, buffer, blockCount);
}

static void MSDDiskWrite(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t blockAddr, uint32_t blockSize) {
    vfs_write(blockAddr, buffer, blockSize);
}

#if (USBMSD_DEMO == USBMSD_DEMO_VFS_STREAM)
BlockStruct StreamBlock;

bool MSDStreamSend(uint32_t* buffer, uint32_t bytesToSend, void (*popCallback)(BlockStruct* block)) {
    __disable_irq();
    bool result = false;
    if (StreamBlock.Locked) {
        result = false;
    }
    else {
        StreamBlock.Buffer      = buffer;
        StreamBlock.Size        = bytesToSend;
        StreamBlock.PopCallback = popCallback;
        StreamBlock.Locked      = true;
        result                  = true;
    }
    __enable_irq();
    return result;
}

static uint32_t MSDStreamRead(uint32_t sector_offset, uint8_t* data, uint32_t num_sectors) {
    printf_dbg("Read sector %d,%d.\n", sector_offset, num_sectors);
    if (!StreamBlock.Locked)
        return 0;

    int readSize = num_sectors * VFS_SECTOR_SIZE;
    readSize     = MIN(readSize, StreamBlock.Size);

    memcpy(data, StreamBlock.Buffer, readSize);
    memset(data, 0x00, num_sectors * VFS_SECTOR_SIZE - readSize);

    StreamBlock.Size -= readSize;
    StreamBlock.Buffer = (uint32_t*)((uint32_t)StreamBlock.Buffer + readSize);

    if (StreamBlock.Size == 0) {
        StreamBlock.Locked = false;
        if (StreamBlock.PopCallback) {
            StreamBlock.PopCallback(&StreamBlock);
        }
        StreamBlock.Size        = 0;
        StreamBlock.Buffer      = NULL;
        StreamBlock.PopCallback = NULL;
    }

    return num_sectors * VFS_SECTOR_SIZE;
}

static void MSDStreamWrite(uint32_t sector_offset, const uint8_t* data, uint32_t num_sectors) {
    printf_dbg("Write sector %d,%d.\n", sector_offset, num_sectors);
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

static bool SWKeyCBWDecode(InterfaceMSDStruct* self) {
    switch (self->CBW.CB[0]) {
        // override default SCSI Command or deal Custom SCSI Command decode here
        // case EXAMPLE_CMD:
        //     ExampleDecode(...);
        //     return true;
    }
    return false;
}

static bool SWKeyInProcess(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t bytesWritten) {
    switch (self->CBW.CB[0]) {
        // override default SCSI Command or deal Custom SCSI Command in process here
        // case EXAMPLE_CMD:
        //     ExampleProcess(...);
        //     return true;
    }
    return false;
}

static bool SWKeyOutProcess(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t bytesRead) {
    switch (self->CBW.CB[0]) {
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

void MSDSetup(USBDeviceStruct* device) {
    InterfaceMSDStruct* msd = &InterfaceMSD;
    InterfaceMSDConstractor(msd);

    msd->DiskStatusCallback = MSDDiskStatus;
    msd->DiskReadCallback   = MSDDiskRead;
    msd->DiskWriteCallback  = MSDDiskWrite;

#if (USBMSD_DEMO == USBMSD_DEMO_VFS_STREAM)
    printf("MSD: Virtual Fat FS USB Disk.\n");

    // Visual FileSystem Init
    vfs_init((vfs_filename_t) {"ESL-Disk"}, MB(64));
    FatDirectoryEntry_t* streamFile = vfs_create_file((vfs_filename_t) {"DISK    RAW"}, MSDStreamRead, MSDStreamWrite, KB(512));
    streamFile->attributes          = VFS_FILE_ATTR_HIDDEN;

    msd->base.Name = L"USB-ESL Mass Storage Device";

    StreamBlock.Locked      = false;
    StreamBlock.Size        = 0;
    StreamBlock.Buffer      = NULL;
    StreamBlock.PopCallback = NULL;
#elif (USBMSD_DEMO == USBMSD_DEMO_VFS_SWKEY)
    printf("MSD: USB SW Key.\n");

    // Visual FileSystem Init
    vfs_init((vfs_filename_t) {"ESL-Key"}, MB(64));

    msd->base.Name = L"USB-ESL Key";

    msd->InquiryData        = (uint32_t*)USBKeySWInquiry;
    msd->InquirySize        = sizeof(USBKeySWInquiry);
    msd->CBWDecodeCallback  = SWKeyCBWDecode;
    msd->InProcessCallback  = SWKeyInProcess;
    msd->OutProcessCallback = SWKeyOutProcess;
#elif (USBMSD_DEMO == USBMSD_DEMO_VFS_CDROM)
    printf("MSD: USB CDDROM.\n");

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
