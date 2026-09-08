#include "RetargetIO.h"

#if defined(__CC_ARM)
#pragma import(__use_no_semihosting);
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__ASM(".global __use_no_semihosting\n");
#endif

#define SerialRegisterType USART_TypeDef

#define SerialTxEmpty(uart)    USART_GetFlagStatus(uart, USART_FLAG_TXE)
#define SerialSend(uart, data) USART_SendData(uart, data)

#define SerialRxNotEmpty(uart) USART_GetFlagStatus(uart, USART_FLAG_RXNE)
#define SerialReceive(uart)    USART_ReceiveData(uart)

PeripheralStruct* SerialPeripheral = NULL;

void RetargetIOSetup(PeripheralEnum peripheral, IOEnum txIO, IOEnum rxIO) {
    SerialPeripheral = PeripheralMap + peripheral;

    SerialRegisterType* uart = (SerialRegisterType*)SerialPeripheral->Base;

    PeripheralEnable(peripheral, true);
    PeripheralReset(peripheral);

    USART_InitTypeDef serialConfig = {0};

    serialConfig.USART_BaudRate            = 115200;
    serialConfig.USART_WordLength          = USART_WordLength_8b;
    serialConfig.USART_StopBits            = USART_StopBits_1;
    serialConfig.USART_Parity              = USART_Parity_No;
    serialConfig.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    serialConfig.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(uart, &serialConfig);
    USART_Cmd(uart, ENABLE);

    uint8_t alternate = 0;

    alternate = peripheral >= PeripheralUART4 ? 8 : 7;

    IOConfigStruct uartIOConfig = MakeIOConfig(IOModeAlternate, alternate, IOPullNone, IOSpeedLow, IODriveLow);
    IOSetup(rxIO, uartIOConfig);
    IOSetup(txIO, uartIOConfig);
}

#if __GNUC__
int _write(int file, char* ptr, int len) {
    if (SerialPeripheral == NULL)
        return -1;

    SerialRegisterType* uart = (SerialRegisterType*)SerialPeripheral->Base;

    for (int i = 0; i < len; i++) {
        while (!SerialTxEmpty(uart));
        SerialSend(uart, (uint8_t)*ptr++);
    }
    return len;
}
#endif

int fputc(int c, FILE* stream) {
    if (SerialPeripheral == NULL)
        return -1;

    SerialRegisterType* uart = (SerialRegisterType*)SerialPeripheral->Base;

    if (c == '\n') {
        while (!SerialTxEmpty(uart));
        SerialSend(uart, (uint16_t)'\r');
    }
    while (!SerialTxEmpty(uart));
    SerialSend(uart, (uint8_t)c);
    return c;
}

int fgetc(FILE* f) {
    if (SerialPeripheral == NULL)
        return -1;

    SerialRegisterType* uart = (SerialRegisterType*)SerialPeripheral->Base;

    while (!SerialRxNotEmpty(uart));
    return (int)SerialReceive(uart);
}

#if USE_COMPONENT_NEWLIB_HEADER

#if defined(__ARMCC_VERSION)
struct __FILE {
    int handle;
};

FILE __stdout;
FILE __stdin;
FILE __stderr;
#endif

void _ttywrch(int ch) {}

void _sys_exit(int return_code) {
label:
    goto label; /* endless loop */
}

#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include "ff.h"

#define MAX_FILES 16

FIL file_table[MAX_FILES];
int file_in_use[MAX_FILES];

void init_file_table() {
    for (int i = 0; i < MAX_FILES; i++) {
        file_in_use[i] = 0;
    }
}

int allocate_fd() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_in_use[i] == 0) {
            file_in_use[i] = 1;
            return i;
        }
    }
    return -1;
}

void release_fd(int fd) {
    if (fd >= 0 && fd < MAX_FILES) {
        file_in_use[fd] = 0;
    }
}

int open(const char* path, int flags, ...) {
    BYTE fatfs_mode = FA_READ;
    FIL* file;
    int  fd;

    if (flags & O_RDONLY) {
        fatfs_mode |= FA_READ;
    }
    if (flags & O_WRONLY) {
        fatfs_mode |= FA_WRITE;
    }
    if (flags & O_RDWR) {
        fatfs_mode |= (FA_READ | FA_WRITE);
    }

    fd = allocate_fd();
    if (fd == -1) {
        errno = EMFILE;
        return -1;
    }

    file = &file_table[fd];

    FRESULT res = f_open(file, path, fatfs_mode);
    if (res != FR_OK) {
        release_fd(fd);
        errno = EIO;
        return -1;
    }

    return fd;
}

int read(int fd, void* buf, size_t bytes_read) {
    FRESULT res;

    if (fd < 0 || fd >= MAX_FILES || file_in_use[fd] == 0) {
        errno = EBADF;
        return -1;
    }

    res = f_read(&file_table[fd], buf, bytes_read, &bytes_read);
    if (res != FR_OK) {
        errno = EIO;
        return -1;
    }

    return bytes_read;
}

int write(int fd, const void* buf, size_t bytes_written) {
    FRESULT res;

    if (fd < 0 || fd >= MAX_FILES || file_in_use[fd] == 0) {
        errno = EBADF;
        return -1;
    }

    res = f_write(&file_table[fd], buf, bytes_written, &bytes_written);
    if (res != FR_OK) {
        errno = EIO;
        return -1;
    }

    return bytes_written;
}

off_t lseek(int fd, off_t offset, int whence) {
    FRESULT res;
    FSIZE_t pos;

    if (fd < 0 || fd >= MAX_FILES || file_in_use[fd] == 0) {
        errno = EBADF;
        return -1;
    }

    if (whence == SEEK_SET) {
        pos = offset;
    }
    else if (whence == SEEK_CUR) {
        pos = f_tell(&file_table[fd]) + offset;
    }
    else if (whence == SEEK_END) {
        pos = f_size(&file_table[fd]) + offset;
    }
    else {
        errno = EINVAL;
        return -1;
    }

    res = f_lseek(&file_table[fd], pos);
    if (res != FR_OK) {
        errno = EIO;
        return -1;
    }

    return pos;
}

int close(int fd) {
    FRESULT res;

    if (fd < 0 || fd >= MAX_FILES || file_in_use[fd] == 0) {
        errno = EBADF;
        return -1;
    }

    res = f_close(&file_table[fd]);
    if (res != FR_OK) {
        errno = EIO;
        return -1;
    }

    release_fd(fd);
    return 0;
}

int mkdir(const char* pathname, mode_t mode) {
    FRESULT res = f_mkdir(pathname);

    if (res != FR_OK) {
        errno = ENOSPC;
        return -1;
    }

    return 0;
}

int fstat(int fd, struct stat* buf) {
    FILINFO fno;
    FRESULT res;

    FIL* file = &file_table[fd];

    res = f_stat(NULL, &fno);
    if (res != FR_OK) {
        errno = ENOENT;
        return -1;
    }

    buf->st_size = fno.fsize;
    buf->st_mode = S_IFREG;

    return 0;
}

// TODO
char* tempnam(const char* dir, const char* pfx) {
    return NULL;
}

FILE* fdopen(int fd, const char* mode) {
    return NULL;
}

int rename(const char* oldName, const char* newName) {
    return NULL;
}

clock_t clock(void) {
    return 0;
}

#endif
