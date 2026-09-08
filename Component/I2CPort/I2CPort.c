#include "I2CPort.h"

static void SetAddress(I2CPortStruct* self, uint8_t address) {
    self->Address = address;
}

static bool ByteLoad(I2CPortStruct* self, const I2CByteConfigStruct* configs) {
    const I2CByteConfigStruct* config = configs;
    do {
        if (config->Mask)
            self->ModifyValue(self, I2CTransferA1D1, config->Address, config->Mask, config->Value);
        else
            self->WriteValue(self, I2CTransferA1D1, config->Address, config->Value);

        if (config->Delay)
            SystemDelay(config->Delay);
        config++;
    }
    while (config->Address != 0 || config->Value != 0 || config->Delay != 0);

    return true;
}

static bool ShortLoad(I2CPortStruct* self, const I2CShortConfigStruct* configs) {
    const I2CShortConfigStruct* config;

    uint32_t index = 0;
    do {
        config = &configs[index++];
        if (config->Mask)
            self->ModifyValue(self, config->Mask > 0xFF ? I2CTransferA2D2 : I2CTransferA2D1, config->Address, config->Mask, config->Value);
        else
            self->WriteValue(self, I2CTransferA2D1, config->Address, config->Value);

        if (config->Delay)
            SystemDelay(config->Delay);
        SystemDelay(1);
    }
    while (config->Address != 0 || config->Value != 0 || config->Delay != 0);

    return true;
}

#define IO_DEFAULT_I2C_CONFIG MakeIOConfig(IOModeAlternateOD, alternate, IOPullUp, IOSpeedLow, IODriveLow)

static bool Init(I2CPortStruct* self) {
    PeripheralEnable(self->Peripheral, true);
    PeripheralReset(self->Peripheral);

    uint8_t alternate = 0;

    alternate = 4;

    IOSetup(self->SCL, IO_DEFAULT_I2C_CONFIG);
    if (self->SDA == PB3 || self->SDA == PB4)
        alternate = 9;
    IOSetup(self->SDA, IO_DEFAULT_I2C_CONFIG);

    I2C_InitTypeDef i2cInit;
    I2C_StructInit(&i2cInit);

    I2C_DeInit(self->I2CV2);
    i2cInit.I2C_Mode                = I2C_Mode_I2C;
    i2cInit.I2C_DutyCycle           = I2C_DutyCycle_2;
    i2cInit.I2C_OwnAddress1         = 0xFE;
    i2cInit.I2C_Ack                 = I2C_Ack_Enable;
    i2cInit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2cInit.I2C_ClockSpeed          = self->Speed * 1000;

    I2C_Init(self->I2CV2, &i2cInit);
    I2C_Cmd(self->I2CV2, ENABLE);
    return true;
}

static bool I2CWaitEvent(I2CPortStruct* self, uint32_t I2C_EVENT, __IO uint32_t timeout) {
    __IO uint32_t clockPeriod = 1000 / self->Speed + 1;
    timeout *= 5;
    while (!I2C_CheckEvent(self->I2CV2, I2C_EVENT)) {
        SystemDelayUs(clockPeriod);
        if ((timeout--) == 0)
            return false;
    }
    return true;
}

static bool RawWrite(I2CPortStruct* self, uint8_t* reg, uint16_t regWidth, uint8_t* data, uint16_t length, bool isStop) {
    uint8_t i2cPortAddress = self->Address << 1;
    bool    result         = false;

    if (regWidth == 0 && length == 0)
        return true;

    // step 0: generate start
    I2C_GenerateSTART(self->I2CV2, ENABLE);
    if (!I2CWaitEvent(self, I2C_EVENT_MASTER_MODE_SELECT, 2)) // Test on EV5 and clear it
        goto End;

    // step 1: send address
    I2C_Send7bitAddress(self->I2CV2, i2cPortAddress, I2C_Direction_Transmitter);
    if (!I2CWaitEvent(self, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 10)) // Test on EV6 and clear it
        goto End;

    // step 2: register phase
    for (int i = 0; i < regWidth; i++) {
        I2C_SendData(self->I2CV2, *reg++);
        if (!I2CWaitEvent(self, I2C_EVENT_MASTER_BYTE_TRANSMITTED, 10)) // Test on EV8 and clear it
            goto End;
    }

    // step 3: data phase
    for (int i = 0; i < length; i++) {
        I2C_SendData(self->I2CV2, *data++);
        if (!I2CWaitEvent(self, I2C_EVENT_MASTER_BYTE_TRANSMITTED, 10)) // Test on EV8 and clear it
            goto End;
    }

    result = true;

End:
    // end: clear nak and generate stop (if needed)
    self->I2CV2->SR1 &= ~BIT10;
    if (isStop || !result) {
        I2C_GenerateSTOP(self->I2CV2, ENABLE);
        I2C_GenerateSTOP(self->I2CV2, DISABLE);
    }

    return result;
}

static bool RawRead(I2CPortStruct* self, uint8_t* data, uint16_t length) {
    uint8_t i2cPortAddress = self->Address << 1;
    bool    result         = false;

    // step 0: generate start
    I2C_GenerateSTART(self->I2CV2, ENABLE);
    if (!I2CWaitEvent(self, I2C_EVENT_MASTER_MODE_SELECT, 2)) // Test on EV5 and clear it
        goto End;

    // step 1: send address
    I2C_Send7bitAddress(self->I2CV2, i2cPortAddress, I2C_Direction_Receiver);
    if (!I2CWaitEvent(self, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, 10)) // Test on EV6 and clear it
        goto End;

    // step 2: data phase
    for (int i = 0; i < length; i++) {
        I2C_AcknowledgeConfig(self->I2CV2, i == length - 1 ? DISABLE : ENABLE); // Prepare an ACK/NACK for the next data received
        if (!I2CWaitEvent(self, I2C_EVENT_MASTER_BYTE_RECEIVED, 10))            // Test on EV7 and clear it
            goto End;

        *data++ = I2C_ReceiveData(self->I2CV2); // Receive the Data
    }

    result = true;

End:
    // end: generate stop (if needed)
    I2C_GenerateSTOP(self->I2CV2, ENABLE);
    I2C_GenerateSTOP(self->I2CV2, DISABLE);

    return result;
}

static bool Write(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, uint8_t* data, uint32_t length) {
    bool    isAddressLE  = type >> 7;
    uint8_t addressWidth = (type & 0x70) >> 4;

    if (!isAddressLE)
        address = __REV(address) >> ((4 - addressWidth) * 8);
    return RawWrite(self, ((uint8_t*)&address), addressWidth, data, length, true);
}

static bool Read(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, uint8_t* data, uint32_t length) {
    bool    isAddressLE  = type >> 7;
    uint8_t addressWidth = (type & 0x70) >> 4;

    if (!isAddressLE)
        address = __REV(address) >> ((4 - addressWidth) * 8);
    if (!RawWrite(self, ((uint8_t*)&address), addressWidth, NULL, 0, false))
        return false;

    return RawRead(self, data, length);
}

static bool WriteValue(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, uint32_t value) {
    bool    isAddressLE  = type >> 7;
    uint8_t addressWidth = (type & 0x70) >> 4;
    bool    isDataLE     = (type >> 3) & 1;
    uint8_t dataWidth    = type & 0x7;

    if (!isAddressLE)
        address = __REV(address) >> ((4 - addressWidth) * 8);

    if (!isDataLE)
        value = __REV(value) >> ((4 - dataWidth) * 8);

    return RawWrite(self, ((uint8_t*)&address), addressWidth, (uint8_t*)&value, dataWidth, true);
}

static bool ReadValue(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, void* value) {
    bool    isAddressLE  = type >> 7;
    uint8_t addressWidth = (type & 0x70) >> 4;
    bool    isDataLE     = (type >> 3) & 1;
    uint8_t dataWidth    = type & 0x7;

    if (!isAddressLE)
        address = __REV(address) >> ((4 - addressWidth) * 8);
    if (!RawWrite(self, ((uint8_t*)&address), addressWidth, NULL, 0, false))
        return false;

    uint32_t readValue = 0;
    if (!RawRead(self, (uint8_t*)&readValue, dataWidth))
        return false;

    if (!isAddressLE)
        readValue = __REV(readValue) >> ((4 - dataWidth) * 8);

    for (int i = 0; i < dataWidth; i++) {
        ((uint8_t*)value)[i] = ((uint8_t*)&readValue)[i];
    }
    return true;
}

static bool ModifyValue(I2CPortStruct* self, I2CTransferTypeEnum type, uint32_t address, uint32_t mask, uint32_t value) {

    uint32_t readValue = 0;
    if (!self->ReadValue(self, type, address, &readValue))
        return false;

    readValue &= ~mask;
    value &= mask;
    value |= readValue;

    return self->WriteValue(self, type, address, value);
}

bool I2CPortConstractor(I2CPortStruct* self, PeripheralEnum peripheralI2C, IOEnum scl, IOEnum sda) {
    memset(self, 0, sizeof(I2CPortStruct));

    self->Peripheral = peripheralI2C;
    self->SCL        = scl;
    self->SDA        = sda;
    self->Speed      = 400;

    self->Init       = Init;
    self->SetAddress = SetAddress;

    PeripheralStruct* peripheral = PeripheralMap + peripheralI2C;

    self->Read  = Read;
    self->Write = Write;

    self->ReadValue   = ReadValue;
    self->WriteValue  = WriteValue;
    self->ModifyValue = ModifyValue;

    self->ByteLoad  = ByteLoad;
    self->ShortLoad = ShortLoad;

    self->I2CBase = peripheral->Base;

    return true;
}
