#include "XS9950.h"

// ------- Static Definitions --------

#define XS9950_I2C_ADDRS 0x30, 0x33
#define XS9950_ID        (0x9950)

#define XS9950_WIDTH  1280
#define XS9950_HEIGHT 720

// NI SDK definitions
#define NiSdkWriteByte(address, value)                     i2cPort->WriteValue(i2cPort, I2CTransferA2D1, address, value)
#define NiSdkReadByte(address, value)                      i2cPort->ReadValue(i2cPort, I2CTransferA2D1, address, value)
#define NiSdkWriteRegBit(address, offsetL, offsetH, value) i2cPort->ModifyValue(i2cPort, I2CTransferA2D1, address, BITS(offsetH, offsetL), value << offsetL)
#define NiSdkMipiWriteReg(address, value)            \
    NiSdkWriteByte(address + 0x5000, (value >> 24)); \
    NiSdkWriteByte(address + 0x5001, (value >> 16)); \
    NiSdkWriteByte(address + 0x5002, (value >> 8));  \
    NiSdkWriteByte(address + 0x5003, (value >> 0));

typedef enum tagNiVideoFormat {
    NI_CVI_1280x720_25HZ = 0,
    NI_CVI_1280x720_30HZ,
    NI_CVI_1280x720_50HZ,
    NI_CVI_1280x720_60HZ,
    NI_CVI_1920x1080_25HZ,
    NI_CVI_1920x1080_30HZ,

    NI_CVI_1280x720_30HZ_V20 = 26,
    NI_CVI_1280x720_60HZ_V20,
    NI_CVI_1920x1080_30HZ_V20,
    NI_CVI_1920x1080_15HZ,
    NI_CVI_1280x960_25HZ,
    NI_CVI_1280x960_30HZ,

    NI_AHD_1280x720_25HZ = 64,
    NI_AHD_1280x720_30HZ,
    NI_AHD_1280x720_50HZ,
    NI_AHD_1280x720_60HZ,
    NI_AHD_1920x1080_25HZ,
    NI_AHD_1920x1080_30HZ,

    NI_TVI_1280x720_25HZ = 128,
    NI_TVI_1280x720_30HZ,
    NI_TVI_1280x720_50HZ,
    NI_TVI_1280x720_60HZ,
    NI_TVI_1920x1080_25HZ,
    NI_TVI_1920x1080_30HZ,

    NI_TVI3_1280x720_25HZ,
    NI_TVI3_1280x720_30HZ,

    NI_TVI_1920x1080_15HZ = 141,

    NI_SD_NTSC_JM = 200,
    NI_SD_NTSC_443,
    NI_SD_PAL_M,
    NI_SD_PAL_60,
    NI_SD_PAL_CN,
    NI_SD_PAL_BGHID,

    NI_VIDEO_FMT_BUTT = 255
} NI_VIDEO_FORMAT_E;

static bool ExcuteCommand(CameraStruct* camera, CameraCommandEnum command, uint32_t parameter) {
    I2CPortStruct* i2cPort = camera->I2CPort;

    if (i2cPort->Address != camera->I2CAddress)
        i2cPort->SetAddress(i2cPort, camera->I2CAddress);

    switch (command) {
        case CameraCommandResume:
            NiSdkMipiWriteReg(0x04, 0x01); // DPHY Enable
            NiSdkWriteByte(0xE08, 0x01);   // MIPI Enable
            return true;
        case CameraCommandSuspend:
            NiSdkMipiWriteReg(0xE08, 0x00); // MIPI Disable
            NiSdkMipiWriteReg(0x04, 0x00);  // DPHY Disable
            SystemDelay(1);
            return true;
        default:
            return false;
    }
}

static I2CShortConfigStruct xs9950_960H_N[] = {
    {0x0e08, 0x00, 0x00},
    {0x0102, 0x40, 0x00},
    {0x0105, 0xe1, 0x00},
    {0x0108, 0x80, 0x00},
    {0x080d, 0x00, 0x00},
    {0x0158, 0x01, 0x00},
    {0x0a60, 0x00, 0x00},
    {0x0a88, 0x20, 0x00},
    {0x0121, 0x5a, 0x00},
    {0x0122, 0x4b, 0x00},
    {0x0125, 0x73, 0x00},
    {0x010c, 0x00, 0x00},
    {0x420b, 0x2f, 0x00}, //clamp
    {0x0100, 0x38, 0x00},
    {0x0a60, 0x00, 0x00},
    {0x0803, 0x1f, 0x00},
    {0x080e, 0x1f, 0x00},
    {0x0803, 0x1f, 0x00},
    {0x080e, 0x3f, 0x00},
    {0x080e, 0x3f, 0x00},
    {0x0e08, 0x01, 0x00},
    {0x0800, 0x04, 0x00},
    {0x0805, 0x07, 0x00},
    {0x0800, 0x04, 0x00},
    {0x0800, 0x06, 0x00},
    {0x0805, 0x0e, 0x00},
    {0x0b50, 0x08, 0x00},
    {0x0e08, 0x00, 0x00},
    {0x010c, 0x00, 0x00},
    {0x0305, 0xe1, 0x00},
    {0x033b, 0x02, 0x00},
    {0x0511, 0x00, 0x00},
    {0x0158, 0x03, 0x00},
    {0x0a60, 0x00, 0x00},
    {0x0a88, 0x20, 0x00},
    {0x0121, 0x5a, 0x00},
    {0x0122, 0x4b, 0x00},
    {0x0125, 0x73, 0x00},
    {0x0126, 0x4c, 0x00},
    {0x0505, 0x00, 0x00},
    {0x0506, 0x00, 0x00},
    {0x0106, 0x80, 0x00},
    {0x0107, 0x00, 0x00},
    {0x0108, 0x80, 0x00},
    {0x0109, 0x00, 0x00},
    {0x010a, 0x12, 0x00},
    {0x010a, 0x12, 0x00},
    {0x010b, 0x02, 0x00},
    {0x010b, 0x02, 0x00},
    {0x033a, 0x00, 0x00},
    {0x0e08, 0x01, 0x00},
    {0x0102, 0x40, 0x00},
    {0x0105, 0xe1, 0x00},
    {0x0108, 0x80, 0x00},
    {0x0156, 0x00, 0x00},
    {0x0157, 0x00, 0x00},
    {0x0156, 0x00, 0x00},
    {0x0157, 0x00, 0x00},
    {0x0507, 0x03, 0x00}, // bit4, 0:720H; 1:960H;
    {0x0503, 0x3c, 0x00},
    {0x015a, 0xc0, 0x00},
    {0x015b, 0x03, 0x00},
    {0x015c, 0x49, 0x00},
    {0x015d, 0x00, 0x00},
    {0x015e, 0xf0, 0x00},
    {0x015f, 0x00, 0x00},
    {0x0160, 0x39, 0x00},
    {0x0161, 0x01, 0x00},
    {0x0165, 0xff, 0x00},
    {0x0316, 0x48, 0x00},
    {0x0336, 0xde, 0x00},
    //{0x033B, 0x02, 0x00},
    {0x0337, 0x01, 0x00},

    {},
};

static void              NiSdkDevInit(I2CPortStruct* i2cPort);
static void              NiSdkDevSysInit(I2CPortStruct* i2cPort);
static NI_VIDEO_FORMAT_E NiSdkDetect(I2CPortStruct* i2cPort);

static bool Init(CameraStruct* camera) {
    // I2C setup
    I2CPortStruct* i2cPort = camera->I2CPort;

    NiSdkDevSysInit(i2cPort);
    NiSdkDevInit(i2cPort);
    NI_VIDEO_FORMAT_E format = NiSdkDetect(i2cPort);

    if (format == NI_SD_NTSC_JM) {
        i2cPort->ShortLoad(i2cPort, xs9950_960H_N);
        camera->Window     = (CameraWindowStruct) {0, 0, 720, 300};
        camera->CropWindow = (CameraWindowStruct) {0, 0, 720, 300};
    }

    return CameraInit(camera);
}

bool XS9950Constractor(CameraStruct* camera) {
    // power not used
    if (!CameraStartupSequence(camera, true, true, true, 1, 1, 10))
        return false;

    // I2C setup
    I2CPortStruct* i2cPort = camera->I2CPort;

    uint16_t sensorID = 0;

    // reduce I2C Port Speed
    i2cPort->Speed = 100;
    i2cPort->Init(i2cPort);

    uint8_t addrs[] = {XS9950_I2C_ADDRS};
    for (int i = 0; i < sizeof(addrs); i++) {
        // I2C setup
        i2cPort->SetAddress(i2cPort, addrs[i]);

        if (                                                                               //
            !i2cPort->ReadValue(i2cPort, I2CTransferA2D1, 0x40F1, &sensorID) ||            //
            !i2cPort->ReadValue(i2cPort, I2CTransferA2D1, 0x40F0, (void*)&sensorID + 1) || //
            sensorID != XS9950_ID                                                          //
        ) {
            sensorID = 0;
            continue;
        }
        break;
    }

    if (sensorID == 0)
        return false;

    camera->I2CAddress = i2cPort->Address;

    camera->Window = (CameraWindowStruct) {0, 0, XS9950_WIDTH, XS9950_HEIGHT};
    //camera->CropWindow  = (CameraWindowStruct) {0, 0, XS9950_WIDTH, XS9950_HEIGHT};
    camera->Color       = CameraColorYUV422;
    camera->MemoryColor = camera->Color;

    camera->Init = Init;

    camera->ExecuteCommand = ExcuteCommand;

    CSIStruct* csiPort = camera->CSIHandle;

    csiPort->LaneClock = CONFIG_XS9950_LANE_RATE * 1000;

    csiPort->Lanes = CSILanes1;

    return true;
}

/****************  chn reg   ************************/
#define NI_0000_VIDEO_STATUS_REGISTER_1                                  (0x000)
#define NI_0001_HD_VIDEO_STANDARD_READBACK                               (0x001)
#define NI_0002_SD_VIDEO_STANDARD_READBACK                               (0x002)
#define NI_0003_VSYNC_STATUS_REGISTER_2                                  (0x003)
#define NI_0004_SIGNAL_LOSS_FACT_LSB                                     (0x004)
#define NI_0005_SIGNAL_LOSS_FACT_MSB                                     (0x005)
#define NI_0006_SYNC_DEPTH_LSB                                           (0x006)
#define NI_0007_SYNC_DEPTH_MSB                                           (0x007)
#define NI_0008_HD_STATUS                                                (0x008)
#define NI_0009_SD_STATUS                                                (0x009)
#define NI_000A_HD_STD                                                   (0x00A)
#define NI_000B_SD_STD                                                   (0x00B)
#define NI_000C_HD_SYNC_DEPTH_LSB                                        (0x00C)
#define NI_000D_SD_SYNC_DEPTH_LSB                                        (0x00D)
#define NI_000E_HD_SYNC_POWER_BYTE0                                      (0x00E)
#define NI_000F_HD_SYNC_POWER_BYTE1                                      (0x00F)
#define NI_0010_HD_SYNC_POWER_BYTE2                                      (0x010)
#define NI_0011_HD_SYNC_POWER_BYTE3                                      (0x011)
#define NI_0012_HD_BURST_POWER_BYTE0                                     (0x012)
#define NI_0013_HD_BURST_POWER_BYTE1                                     (0x013)
#define NI_0014_HD_BURST_POWER_BYTE2                                     (0x014)
#define NI_0015_HD_BURST_POWER_BYTE3                                     (0x015)
#define NI_0016_HD_FSC_PHASE_ERR_ABS_LSB                                 (0x016)
#define NI_0017_HD_FSC_PHASE_ERR_ABS_MSB                                 (0x017)
#define NI_0018_HD_CVI_LE_00_Y                                           (0x018)
#define NI_0019_HD_CVI_LE_00_U                                           (0x019)
#define NI_001A_HD_CVI_LE_00_V                                           (0x01A)
#define NI_001B_HD_CVI_LE_01_Y                                           (0x01B)
#define NI_001C_HD_CVI_LE_01_U                                           (0x01C)
#define NI_001D_HD_CVI_LE_01_V                                           (0x01D)
#define NI_0021_HD_LAST_MIN_VALUE_LSB                                    (0x021)
#define NI_0022_HD_LAST_MIN_VALUE_MSB                                    (0x022)
#define NI_0023_HD_SHOOT_WIN_MIN_VAL_LSB                                 (0x023)
#define NI_0024_HD_SHOOT_WIN_MIN_VAL_MSB                                 (0x024)
#define NI_0025_HD_0_LE_LSB                                              (0x025)
#define NI_0026_HD_1_LE_LSB                                              (0x026)
#define NI_0027_HD_LE_MSB                                                (0x027)
#define NI_0028_HD_MARK_STD_MODE                                         (0x028)
#define NI_0029_HD_DIFF_MAX_LSB                                          (0x029)
#define NI_002A_HD_DIFF_MAX_MSB                                          (0x02A)
#define NI_002B_HD_SYNC_WIDTH_LSB                                        (0x02B)
#define NI_002C_HD_SYNC_WIDTH_MSB                                        (0x02C)
#define NI_0100_HD_AFE_CLAMP_TARGET                                      (0x100)
#define NI_0101_HD_LUMINANCE_GAIN_MANUAL_CONTROL_REGISTER                (0x101)
#define NI_0102_HD_COLOR_GAIN_MANUAL_CONTROL_REGISTER                    (0x102)
#define NI_0103_HD_WHITE_PEAK_OFFSET                                     (0x103)
#define NI_0104_HD_MANUAL_CLAMP_OFFSET_SETTING                           (0x104)
#define NI_0105_HD_AGC_CONTROL_REGISTER                                  (0x105)
#define NI_0106_HD_CONTRAST_CONTROL_REGISTER                             (0x106)
#define NI_0107_HD_BRIGHTNESS_CONTROL_REGISTER                           (0x107)
#define NI_0108_HD_SATURATION_CONTROL_REGISTER                           (0x108)
#define NI_0109_HD_HUE_CONTROL_REGISTER                                  (0x109)
#define NI_010A_HD_HBLANK_CONTROL_REGISTER                               (0x10A)
#define NI_010B_HD_VBLANK_CONTROL_REGISTER                               (0x10B)
#define NI_010C_HD_VIDEO_STD_CONFIG1                                     (0x10C)
#define NI_010D_HD_VIDEO_STD_CONFIG2                                     (0x10D)
#define NI_010E_HD_YOUT_GAIN_CTRL                                        (0x10E)
#define NI_010F_HD_UOUT_GAIN_CTRL                                        (0x10F)
#define NI_0110_HD_VOUT_GAIN_CTRL                                        (0x110)
#define NI_0111_HD_Y_BLANK_LEVEL                                         (0x111)
#define NI_0112_HD_C_BLANK_LEVEL                                         (0x112)
#define NI_0113_ANALOGUE_EQ                                              (0x113)
#define NI_0114_HD_COLOR_KILL_CONTROL_REGISTER_1                         (0x114)
#define NI_0115_HD_COLOR_KILL_CONTROL_REGISTER_2                         (0x115)
#define NI_0116_HD_SYNC_DEPTH_CONTROL_REGISTER                           (0x116)
#define NI_0117_HD_BURST_POWER_CONTROL_REGISTER                          (0x117)
#define NI_011D_HD_Y_PEAK_CONTROL_REGISTER_2                             (0x11D)
#define NI_011E_Y_DELAY                                                  (0x11E)
#define NI_011F_SYNC_LOCK_CONTROL_REGISTER                               (0x11F)
#define NI_0120_SYNC_CONTROL_REGISTER                                    (0x120)
#define NI_0121_PAL_SYNC_CONTROL_REGISTER_1                              (0x121)
#define NI_0122_NTSC_SYNC_CONTROL_REGISTER_1                             (0x122)
#define NI_0123_PAL_SYNC_CONTROL_REGISTER_2                              (0x123)
#define NI_0124_NTSC_SYNC_CONTROL_REGISTER_2                             (0x124)
#define NI_0125_PAL_SYNC_CONTROL_REGISTER_3                              (0x125)
#define NI_0126_NTSC_SYNC_CONTROL_REGISTER_3                             (0x126)
#define NI_0127_SHORT_LOCK_TERM_CONTROL_REGISTER                         (0x127)
#define NI_0128_AV_AND_BK_FLOOR_THRESHOLD                                (0x128)
#define NI_0129_HSYNC_SHAPE_THRESHOLD                                    (0x129)
#define NI_012A_LONG_TERM_LOCK_EXIT_THRESHOLD                            (0x12A)
#define NI_012C_LOCK_FLOOR_CONTROL_REGISTER                              (0x12C)
#define NI_012D_FRONT_END_HSYNC_LOOP_CONTROL_REGISTER                    (0x12D)
#define NI_012E_BACK_END_HSYNC_STEP_CONTROL_REGISTER                     (0x12E)
#define NI_012F_BACK_END_HSYNC_LOOP_CONTROL_REGISTER                     (0x12F)
#define NI_0130_TBC_AND_BACK_END_HSYNC_LOOP_CONTROL_REGISTER             (0x130)
#define NI_0131_FRONT_AND_BACK_END_COMMON_HSYNC_LOOP_CONTROL_REGISTER    (0x131)
#define NI_0132_SYNC_HEAD_CONTROL_REGISTER                               (0x132)
#define NI_0133_HD_THE_LSB_OF_Y_PEAK_THRESHOLD                           (0x133)
#define NI_0134_HD_THE_MSB_OF_Y_PEAK_THRESHOLD                           (0x134)
#define NI_0135_ANALOG_EQ_THRESHOLD1_LSB                                 (0x135)
#define NI_0136_ANALOG_EQ_THRESHOLD1_MSB                                 (0x136)
#define NI_0137_ANALOG_EQ_THRESHOLD2_LSB                                 (0x137)
#define NI_0138_ANALOG_EQ_THRESHOLD2_MSB                                 (0x138)
#define NI_0139_ANALOG_EQ_THRESHOLD3_LSB                                 (0x139)
#define NI_013A_ANALOG_EQ_THRESHOLD3_MSB                                 (0x13A)
#define NI_013B_ANALOG_EQ_THRESHOLD4_LSB                                 (0x13B)
#define NI_013C_ANALOG_EQ_THRESHOLD4_MSB                                 (0x13C)
#define NI_013D_ANALOG_EQ_THRESHOLD5_LSB                                 (0x13D)
#define NI_013E_ANALOG_EQ_THRESHOLD5_MSB                                 (0x13E)
#define NI_013F_ANALOG_EQ_THRESHOLD6_LSB                                 (0x13F)
#define NI_0140_ANALOG_EQ_THRESHOLD6_MSB                                 (0x140)
#define NI_0141_ANALOG_EQ_THRESHOLD7_LSB                                 (0x141)
#define NI_0142_ANALOG_EQ_THRESHOLD7_MSB                                 (0x142)
#define NI_0143_PARAMETER_BP_NUM_MAN_LOW                                 (0x143)
#define NI_0144_PARAMETER_BP_NUM_MAN_HIGH                                (0x144)
#define NI_0145_PARAMETER_HV_WIN_MAN_LOW                                 (0x145)
#define NI_0146_PARAMETER_HV_WIN_MAN_HIGH                                (0x146)
#define NI_0147_BURST_POWER_SEL                                          (0x147)
#define NI_014A_COMB_FILT_KH_LSB                                         (0x14A)
#define NI_014B_COMB_FILT_KH_MSB                                         (0x14B)
#define NI_014C_COMB_FILT_KV_LSB                                         (0x14C)
#define NI_014D_COMB_FILT_KV_MSB                                         (0x14D)
#define NI_014E_AHD_DNSAMP_RATE_BYTE0                                    (0x14E)
#define NI_014F_AHD_DNSAMP_RATE_BYTE1                                    (0x14F)
#define NI_0150_AHD_DNSAMP_RATE_BYTE2                                    (0x150)
#define NI_0151_AHD_DNSAMP_RATE_BYTE3                                    (0x151)
#define NI_0152_COMB_FILT_FSC_LM_DLY                                     (0x152)
#define NI_0153_COMB_FILT_BL_SPL_ADJ                                     (0x153)
#define NI_0154_COMB_FILT_RESET_CNT_MAX_ADJ                              (0x154)
#define NI_0156_HD_PHASE_INITIAL_LSB                                     (0x156)
#define NI_0157_HD_PHASE_INITIAL_MSB                                     (0x157)
#define NI_0158_COMB_FILT_CTRL                                           (0x158)
#define NI_0159_TEST_PATTERN_CTRL                                        (0x159)
#define NI_015A_TEST_PATTERN_ACT_PIX_LSB                                 (0x15A)
#define NI_015B_TEST_PATTERN_ACT_PIX_MSB                                 (0x15B)
#define NI_015C_TEST_PATTERN_TOCAL_PIX_LSB                               (0x15C)
#define NI_015D_TEST_PATTERN_TOCAL_PIX_MSB                               (0x15D)
#define NI_015E_TEST_PATTERN_ACT_LINE_LSB                                (0x15E)
#define NI_015F_TEST_PATTERN_ACT_LINE_MSB                                (0x15F)
#define NI_0160_TEST_PATTERN_TOCAL_LINE_LSB                              (0x160)
#define NI_0161_TEST_PATTERN_TOCAL_LINE_MSB                              (0x161)
#define NI_0165_TEST_PATTERN_VSTD_LSB                                    (0x165)
#define NI_0166_TEST_PATTERN_VSTD_MSB                                    (0x166)
#define NI_0167_VB_WIN_MAN_LSB                                           (0x167)
#define NI_0168_VB_WIN_MAN_MSB                                           (0x168)
#define NI_0169_HD_CVI_LE_00_CFG                                         (0x169)
#define NI_016A_HD_CVI_LE_00_LINE_LSB                                    (0x16A)
#define NI_016B_HD_CVI_LE_00_PIX_LSB                                     (0x16B)
#define NI_016C_HD_CVI_LE_00_PIX_MSB                                     (0x16C)
#define NI_016D_HD_CVI_LE_01_CFG                                         (0x16D)
#define NI_016E_HD_CVI_LE_01_LINE_LSB                                    (0x16E)
#define NI_016F_HD_CVI_LE_01_PIX_LSB                                     (0x16F)
#define NI_0170_HD_CVI_LE_01_PIX_MSB                                     (0x170)
#define NI_0171_HD_CVI_LE_00_LINE_MSB                                    (0x171)
#define NI_0172_HD_CVI_LE_01_LINE_MSB                                    (0x172)
#define NI_01A9_HD_CVI_PHASE_INITIAL_LSB                                 (0x1A9)
#define NI_01AA_HD_CVI_PHASE_INITIAL_MSB                                 (0x1AA)
#define NI_01B0_HD_LE_EN                                                 (0x1B0)
#define NI_01B1_HD_0_ACC_NUM                                             (0x1B1)
#define NI_01B2_HD_0_LE_LINE_LSB                                         (0x1B2)
#define NI_01B3_HD_0_LE_LINE_MSB                                         (0x1B3)
#define NI_01B4_HD_0_LE_PIX_LSB                                          (0x1B4)
#define NI_01B5_HD_0_LE_PIX_MSB                                          (0x1B5)
#define NI_01B6_HD_0_LE_THR                                              (0x1B6)
#define NI_01B7_HD_1_ACC_NUM                                             (0x1B7)
#define NI_01B8_HD_1_LE_LINE_LSB                                         (0x1B8)
#define NI_01B9_HD_1_LE_LINE_MSB                                         (0x1B9)
#define NI_01BA_HD_1_LE_PIX_LSB                                          (0x1BA)
#define NI_01BB_HD_1_LE_PIX_MSB                                          (0x1BB)
#define NI_01BC_HD_1_LE_THR                                              (0x1BC)
#define NI_01BD_HD_MIN_VALUE_LCOUNT_LSB                                  (0x1BD)
#define NI_01BE_HD_MIN_VALUE_LCOUNT_MSB                                  (0x1BE)
#define NI_01BF_HD_NEW_SEL                                               (0x1BF)
#define NI_01C0_HD_PID_SPEED                                             (0x1C0)
#define NI_01C1_HD_PID_GAIN                                              (0x1C1)
#define NI_01C2_HD_Y_DCLP_SPEED                                          (0x1C2)
#define NI_01C3_HD_Y_DCLP_GAIN                                           (0x1C3)
#define NI_01C4_HD_Y_DGAIN_SPEED                                         (0x1C4)
#define NI_01C5_HD_Y_DGAIN_GAIN                                          (0x1C5)
#define NI_01C6_HD_C_DGAIN_SPEED                                         (0x1C6)
#define NI_01C7_HD_C_DGAIN_GAIN                                          (0x1C7)
#define NI_01C8_HD_C2_DGAIN_SPEED                                        (0x1C8)
#define NI_01C9_HD_C2_DGAIN_GAIN                                         (0x1C9)
#define NI_01CA_HD_C_THR_LSB                                             (0x1CA)
#define NI_01CB_HD_C_THR_MSB                                             (0x1CB)
#define NI_01CC_HD_C_LOW_THR_LSB                                         (0x1CC)
#define NI_01CD_HD_C_LOW_THR_MSB                                         (0x1CD)
#define NI_01CE_HD_PW_CFG                                                (0x1CE)
#define NI_01DB_HD_HACC4_THR_LSB                                         (0x1DB)
#define NI_01DC_HD_HACC4_THR_MSB                                         (0x1DC)
#define NI_01DD_HD_SLICE_THR1_LSB                                        (0x1DD)
#define NI_01DE_HD_SLICE_THR1_MSB                                        (0x1DE)
#define NI_01DF_HD_SLICE_THR2_LSB                                        (0x1DF)
#define NI_01E0_HD_SLICE_THR2_MSB                                        (0x1E0)
#define NI_01E1_HD_SLICE_THR3_LSB                                        (0x1E1)
#define NI_01E2_HD_SLICE_THR3_MSB                                        (0x1E2)
#define NI_01E3_HD_GET_MIN_EN                                            (0x1E3)
#define NI_01E4_HD_SHOOT_LINE_BEG_LSB                                    (0x1E4)
#define NI_01E5_HD_SHOOT_LINE_BEG_MSB                                    (0x1E5)
#define NI_01E6_HD_SHOOT_LINE_END_LSB                                    (0x1E6)
#define NI_01E7_HD_SHOOT_LINE_END_MSB                                    (0x1E7)
#define NI_01E8_HD_SHOOT_VAL_LSB                                         (0x1E8)
#define NI_01E9_HD_SHOOT_VAL_MSB                                         (0x1E9)
#define NI_01EA_HD_RD_VAL_UPDATE                                         (0x1EA)
#define NI_0300_SD_AFE_CLAMP_TARGET                                      (0x300)
#define NI_0301_SD_LUMINANCE_GAIN_MANUAL_CONTROL_REGISTER                (0x301)
#define NI_0302_SD_COLOR_GAIN_MANUAL_CONTROL_REGISTER                    (0x302)
#define NI_0303_SD_WHITE_PEAK_OFFSET                                     (0x303)
#define NI_0304_SD_MANUAL_CLAMP_OFFSET_SETTING                           (0x304)
#define NI_0305_SD_AGC_CONTROL_REGISTER                                  (0x305)
#define NI_030C_SD_VIDEO_STANDARD_MANUAL_SET                             (0x30C)
#define NI_0312_SD_BURST_LOCK_1                                          (0x312)
#define NI_0313_SD_BURST_LOCK_2                                          (0x313)
#define NI_0314_SD_SYNC_LOCK_CONTROL0                                    (0x314)
#define NI_0315_SD_SYNC_LOCK_CONTROL1                                    (0x315)
#define NI_0316_SD_BURST_GATE_START_POSITION_FOR_PAL                     (0x316)
#define NI_0317_SD_BURST_GATE_START_POSITION_FOR_NTSC                    (0x317)
#define NI_0318_SD_SYNC_TIP_START_POSITION_FOR_PAL                       (0x318)
#define NI_0319_SD_SYNC_TIP_START_POSITION_FOR_NTSC                      (0x319)
#define NI_031A_SD_BACK_PORCH_START_POSITION_FOR_PAL                     (0x31A)
#define NI_031B_SD_BACK_PORCH_START_POSITION_FOR_NTSC                    (0x31B)
#define NI_031C_SD_SHORT_TERM_LOCK_ENTRY_AND_EXIT_THRESHOLD              (0x31C)
#define NI_031D_SD_AV_AND_BK_FLOOR_THRESHOLD                             (0x31D)
#define NI_031E_SD_HSYNC_SHAPE_THRESHOLD                                 (0x31E)
#define NI_031F_SD_LONG_TERM_LOCK_EXIT_THRESHOLD                         (0x31F)
#define NI_0320_SD_NOISE_MEASUREMENT_CONTROL                             (0x320)
#define NI_0321_SD_AV_AND_BK_MINIMAL_CONTROL                             (0x321)
#define NI_0322_SD_FRONT_END_HSYNC_LOOP_CONTROL_REGISTER                 (0x322)
#define NI_0323_SD_TBC_CONTROL                                           (0x323)
#define NI_0325_SD_TBC_AND_BACK_END_HSYNC_LOOP_CONTROL_REGISTER          (0x325)
#define NI_0326_SD_FRONT_AND_BACK_END_COMMON_HSYNC_LOOP_CONTROL_REGISTER (0x326)
#define NI_0327_SD_SYNC_HEAD_CONTROL_REGISTER                            (0x327)
#define NI_0328_SD_YC_SEP_CONTROL_REGISTER                               (0x328)
#define NI_0329_SD_ADAPTIVE_COMB_CONTROL_REGISTER_1                      (0x329)
#define NI_032A_SD_ADAPTIVE_COMB_CONTROL_REGISTER_2                      (0x32A)
#define NI_032B_SD_ADAPTIVE_COMB_CONTROL_REGISTER_3                      (0x32B)
#define NI_032C_SD_ADAPTIVE_COMB_CONTROL_REGISTER_4                      (0x32C)
#define NI_032D_SD_ADAPTIVE_COMB_CONTROL_REGISTER_5                      (0x32D)
#define NI_032E_SD_ADAPTIVE_COMB_CONTROL_REGISTER_6                      (0x32E)
#define NI_032F_SD_ADAPTIVE_COMB_CONTROL_REGISTER_7                      (0x32F)
#define NI_0330_SD_COLOR_KILL_CONTROL_REGISTER_1                         (0x330)
#define NI_0331_SD_COLOR_KILL_CONTROL_REGISTER_2                         (0x331)
#define NI_0332_SD_SYNC_DEPTH_CONTROL_REGISTER                           (0x332)
#define NI_0333_SD_BURST_POWER_CONTROL_REGISTER                          (0x333)
#define NI_0334_SD_Y_PEAK_THRESHOLD_LSB                                  (0x334)
#define NI_0335_SD_Y_PEAK_THRESHOLD_MSB                                  (0x335)
#define NI_0336_SD_BURST_LOCK_COEFF                                      (0x336)
#define NI_0337_SD_CTI_CONTROL_REGISTER_1                                (0x337)
#define NI_0338_SD_CTI_CONTROL_REGISTER_2                                (0x338)
#define NI_0339_SD_LUMA_PEAK_CONTROL_REGISTER_1                          (0x339)
#define NI_033A_SD_LUMA_PEAK_CONTROL_REGISTER_2                          (0x33A)
#define NI_033B_SD_BURST_LOCK_CTRL0                                      (0x33B)
#define NI_033C_SD_BURST_LOCK_ACT                                        (0x33C)
#define NI_0346_SD_SLICE_MAX_LSB                                         (0x346)
#define NI_0347_SD_SLICE_MAX_MSB                                         (0x347)
#define NI_0348_SD_SLICE_MAX_VS_LSB                                      (0x348)
#define NI_0349_SD_SLICE_MAX_VS_MSB                                      (0x349)
#define NI_0501_VIDEO_OUT_CTRL_REG                                       (0x501)
#define NI_0502_VIDEO_OUT_MODE_SETTING_ID                                (0x502)
#define NI_0503_VIDEO_OUT_MODE_SETTING_FREE_RUN                          (0x503)
#define NI_0504_VIDEO_OUT_MODE_SETTING                                   (0x504)
#define NI_0505_VIDEO_OUT_BUFFER_SETTING_0                               (0x505)
#define NI_0506_VIDEO_OUT_BUFFER_SETTING_1                               (0x506)
#define NI_0507_VIDEO_OUT_CTRL                                           (0x507)
#define NI_0508_OUT_DATA_CHANGE                                          (0x508)
#define NI_0509_HALF_SCALING_FILTER_MODE                                 (0x509)
#define NI_050A_COEF3_OF_HALF_SCALING_FILTER                             (0x50A)
#define NI_050B_COEF2_OF_HALF_SCALING_FILTER                             (0x50B)
#define NI_050C_COEF1_OF_HALF_SCALING_FILTER                             (0x50C)
#define NI_050D_COEF0_OF_HALF_SCALING_FILTER                             (0x50D)
#define NI_050E_VIDEO_OUT_CLOCK_CTRL                                     (0x50E)
#define NI_050F_HALF_SCALING_IMAGE_WIDTH_LSB                             (0x50F)
#define NI_0510_HALF_SCALING_IMAGE_WIDTH_MSB                             (0x510)
#define NI_0511_VIDEO_OUT_CFG                                            (0x511)
#define NI_0512_VIDEO_OUT_ALL_VERTICAL_LSB                               (0x512)
#define NI_0513_VIDEO_OUT_ALL_VERTICAL_MSB                               (0x513)
#define NI_0514_VIDEO_OUT_ALL_LEVEL_LSB                                  (0x514)
#define NI_0515_VIDEO_OUT_ALL_LEVEL_MSB                                  (0x515)
#define NI_0516_VIDEO_OUT_IMG_WIDTH_LSB                                  (0x516)
#define NI_0517_VIDEO_OUT_IMG_WIDTH_MSB                                  (0x517)
#define NI_0518_VIDEO_OUT_BLK_VERTICAL_LSB                               (0x518)
#define NI_0519_VIDEO_OUT_BLK_VERTICAL_MSB                               (0x519)
#define NI_051A_VIDEO_OUT_SD_PIX_NUM_LSB                                 (0x51A)
#define NI_051B_VIDEO_OUT_SD_PIX_NUM_MSB                                 (0x51B)
#define NI_051C_VIDEO_OUT_SD_BLK_PIX_LSB                                 (0x51C)
#define NI_051D_VIDEO_OUT_SD_BLK_PIX_MSB                                 (0x51D)
#define NI_051E_VIDEO_OUT_FIFO_STSTUS                                    (0x51E)
#define NI_051F_VIDEO_OUT_H_DELAY_CTRL_LSB                               (0x51F)
#define NI_0520_VIDEO_OUT_H_DELAY_CTRL_MSB                               (0x520)
#define NI_0521_VIDEO_OUT_H_ACTIVE_CTRL_LSB                              (0x521)
#define NI_0522_VIDEO_OUT_H_ACTIVE_CTRL_MSB                              (0x522)
#define NI_0523_VIDEO_OUT_V_DELAY_CTRL                                   (0x523)
#define NI_0524_VIDEO_OUT_V_PLUSE_WIDTH                                  (0x524)
#define NI_0525_VIDEO_OUT_V_ACTIVE_LSB                                   (0x525)
#define NI_0526_VIDEO_OUT_V_ACTIVE_MSB                                   (0x526)
#define NI_0600_LSB_OF_DOT_LEVEL_OF                                      (0x600)
#define NI_0601_MSB_OF_DOT_LEVEL_OF                                      (0x601)
#define NI_0602_LSB_OF_RS485_DOT_BEG_OF                                  (0x602)
#define NI_0603_MSB_OF_RS485_DOT_BEG_OF                                  (0x603)
#define NI_0604_LSB_OF_RS485_DOT_END_OF                                  (0x604)
#define NI_0605_MSB_OF_RS485_DOT_END_OF                                  (0x605)
#define NI_0606_RS485_LINE_BEGIN_OF                                      (0x606)
#define NI_0607_RS485_LINE_END_OF                                        (0x607)
#define NI_0608_RS485_LINE_BEGIN_AND_END_OF_MSB                          (0x608)
#define NI_0609_UBDR_OF_SETTING_IN_THE_SENDER                            (0x609)
#define NI_060A_DATA_NUMBER_SETTING_IN_THE_RECEIVER                      (0x60A)
#define NI_060B_RS485_TRANSMIT_ENABLE                                    (0x60B)
#define NI_060C_RS485_CAP_TX_CAP                                         (0x60C)
#define NI_060D_RS485_DATA_TX                                            (0x60D)
#define NI_060E_RS485_CAP_RX                                             (0x60E)
#define NI_060F_RS485_DATA_RX                                            (0x60F)
#define NI_0610_RS485_VALID_LINE_BEGINS_                                 (0x610)
#define NI_0611_RS485_VALID_LINE_END_                                    (0x611)
#define NI_0612_RS485_VALID_SAMPLE_BEGINS_LSB                            (0x612)
#define NI_0613_RS485_VALID_SAMPLE_BEGINS_MSB                            (0x613)
#define NI_0614_RS485_VALID_SAMPLE_END_LSB                               (0x614)
#define NI_0615_RS485_VALID_SAMPLE_END_MSB                               (0x615)
#define NI_0616_RS485_RECEIVE_FREQ_SETTING                               (0x616)
#define NI_0617_FW485_TIMER_EN                                           (0x617)
#define NI_0618_FW485_TIMER_SET                                          (0x618)
#define NI_0619_CMD_TYPE                                                 (0x619)
#define NI_061A_ENCODE_TYPE                                              (0x61A)
#define NI_061B_PROTOCOL_SEL                                             (0x61B)
#define NI_061C_RS485_TEST_MODE_D2C                                      (0x61C)
#define NI_061D_RS485_TEST_DATA0_D2C                                     (0x61D)
#define NI_061E_RS485_TEST_DATA1_D2C                                     (0x61E)
#define NI_061F_RS485_FIFO_CLR_D2C                                       (0x61F)
#define NI_0620_RS485_UC_AT_LENGTH                                       (0x620)
#define NI_0621_RS485_UC_AT_LINE_START_LSB                               (0x621)
#define NI_0622_RS485_UC_AT_LINE_START_MSB                               (0x622)
#define NI_0623_RS485_UC_AT_LINE_END_LSB                                 (0x623)
#define NI_0624_RS485_UC_AT_LINE_END_MSB                                 (0x624)
#define NI_0625_RS485_UC_AT_POINT_START_LSB                              (0x625)
#define NI_0626_RS485_UC_AT_POINT_START_MSB                              (0x626)
#define NI_0627_RS485_UC_AT_HEAD_CFG0                                    (0x627)
#define NI_0628_RS485_UC_AT_HEAD_CFG1                                    (0x628)
#define NI_0629_RS485_UC_AT_HEAD_CFG2                                    (0x629)
#define NI_062A_RS485_UC_AT_HEAD_CFG3                                    (0x62A)
#define NI_062B_RS485_UC_AT_HEAD_CFG4                                    (0x62B)
#define NI_062C_RS485_UC_AT_DATA_CFG0                                    (0x62C)
#define NI_062D_RS485_UC_AT_DATA_CFG1                                    (0x62D)
#define NI_062E_RS485_UC_AT_DATA_CFG2                                    (0x62E)
#define NI_062F_RS485_UC_AT_DATA_CFG3                                    (0x62F)
#define NI_0630_RS485_UC_AT_DATA_CFG4                                    (0x630)
#define NI_0631_RS485_UC_AT_DATA_CFG5                                    (0x631)
#define NI_0632_RS485_UC_AT_DATA_CFG6                                    (0x632)
#define NI_0633_RS485_UC_AT_DATA_CFG7                                    (0x633)
#define NI_0634_RS485_UC_AT_DATA_CFG8                                    (0x634)
#define NI_0635_RS485_UC_AT_DATA_CFG9                                    (0x635)
#define NI_0636_RS485_UC_AT_DATA_CFG10                                   (0x636)
#define NI_0637_RS485_UC_AT_DATA_CFG11                                   (0x637)
#define NI_0638_RS485_UC_AT_DATA_CFG12                                   (0x638)
#define NI_0640_RS485_DC_AT_CFG                                          (0x640)
#define NI_0641_RS485_DC_AT_LINE_START_LSB                               (0x641)
#define NI_0642_RS485_DC_AT_LINE_START_MSB                               (0x642)
#define NI_0643_RS485_DC_AT_LINE_END_LSB                                 (0x643)
#define NI_0644_RS485_DC_AT_LINE_END_MSB                                 (0x644)
#define NI_0645_RS485_DC_AT_POINT_START_LSB                              (0x645)
#define NI_0646_RS485_DC_AT_POINT_START_MSB                              (0x646)
#define NI_0647_RS485_DC_AT_POINT_END_LSB                                (0x647)
#define NI_0648_RS485_DC_AT_POINT_END_MSB                                (0x648)
#define NI_0649_RS485_DC_AT_THR                                          (0x649)
#define NI_064A_RS485_DC_AT_HEAD_CFG                                     (0x64A)
#define NI_064B_RS485_DC_AT_DATA_CFG0                                    (0x64B)
#define NI_064C_RS485_DC_AT_DATA_CFG1                                    (0x64C)
#define NI_064D_RS485_DC_AT_DATA_CFG2                                    (0x64D)
#define NI_064E_RS485_DC_AT_DATA_CFG3                                    (0x64E)
#define NI_064F_RS485_DC_AT_DATA_CFG4                                    (0x64F)
#define NI_0650_RS485_DC_AT_DATA_CFG5                                    (0x650)
#define NI_0651_RS485_DC_AT_DATA_CFG6                                    (0x651)
#define NI_0652_RS485_DC_AT_DATA_CFG7                                    (0x652)
#define NI_0653_RS485_DC_AT_DATA_CFG8                                    (0x653)
#define NI_0654_RS485_DC_AT_DATA_CFG9                                    (0x654)
#define NI_0655_RS485_DC_AT_DATA_CFG10                                   (0x655)
#define NI_0656_RS485_DC_AT_DATA_CFG11                                   (0x656)
#define NI_0657_RS485_DC_AT_MIN_VAL_LSB                                  (0x657)
#define NI_0658_RS485_DC_AT_MAX_VAL_LSB                                  (0x658)
#define NI_0659_RS485_DC_AT_MIN_MAX_VAL_MSB                              (0x659)
#define NI_0700_AUDIO_START_POSITION_LSB                                 (0x700)
#define NI_0701_AUDIO_START_POSITION_MSB                                 (0x701)
#define NI_0702_AUDIO_STEP                                               (0x702)
#define NI_0703_AUDIO_FLAG_THRESHOLD                                     (0x703)
#define NI_0704_AUDIO_GAIN                                               (0x704)
#define NI_0705_AUDIO_NEW_EN                                             (0x705)
#define NI_0706_AUDIO_NEW_HEADER_BEG_LSB                                 (0x706)
#define NI_0707_AUDIO_NEW_HEADER_BEG_MSB                                 (0x707)
#define NI_0708_AUDIO_NEW_HEADER_END_LSB                                 (0x708)
#define NI_0709_AUDIO_NEW_HEADER_END_MSB                                 (0x709)
#define NI_070A_AUDIO_NEW_DATA_THR_LSB                                   (0x70A)
#define NI_070B_AUDIO_NEW_DATA_THR_MSB                                   (0x70B)
#define NI_070C_AUDIO_NEW_FRAME_THR_LSB                                  (0x70C)
#define NI_070D_AUDIO_NEW_FRAME_THR_MSB                                  (0x70D)
#define NI_070E_AUDIO_NEW_PAYLOAD_NUM                                    (0x70E)
#define NI_070F_AUDIO_NEW_STATE                                          (0x70F)
#define NI_0710_AUDIO_NEW_PEAK_THR_REF_LSB                               (0x710)
#define NI_0711_AUDIO_NEW_PEAK_THR_REF_MSB                               (0x711)
#define NI_0712_AUDIO_NEW_PEAK_THR_REF_MSB1                              (0x712)
#define NI_0713_AUDIO_NEW_DATA_THR_REF_LSB                               (0x713)
#define NI_0714_AUDIO_NEW_DATA_THR_REF_MSB                               (0x714)
#define NI_0715_AUDIO_OLD_DATA_NUM                                       (0x715)
#define NI_0800_CLK_GATE_EN                                              (0x800)
#define NI_0801_VDP_CLK_EN                                               (0x801)
#define NI_0802_VO_OUT_CLK_CTRL                                          (0x802)
#define NI_0803_SFT_RST                                                  (0x803)
#define NI_0804_ADC_CDC_CFG                                              (0x804)
#define NI_0805_HD_SD_EN                                                 (0x805)
#define NI_0806_VO_CLK_SEL                                               (0x806)
#define NI_0809_DATAIN_GAIN_LSB                                          (0x809)
#define NI_080A_DATAIN_GAIN_MSB                                          (0x80A)
#define NI_080B_DATAIN_OFFSET_LSB                                        (0x80B)
#define NI_080C_DATAIN_OFFSET_MSB                                        (0x80C)
#define NI_080D_DATAIN_GAIN_CTRL                                         (0x80D)
#define NI_080E_VDP_SFT_RST                                              (0x80E)
#define NI_0A00_Y_LPF_COFF_00_LSB                                        (0xA00)
#define NI_0A01_Y_LPF_COFF_00_MSB                                        (0xA01)
#define NI_0A02_Y_LPF_COFF_01_LSB                                        (0xA02)
#define NI_0A03_Y_LPF_COFF_01_MSB                                        (0xA03)
#define NI_0A04_Y_LPF_COFF_02_LSB                                        (0xA04)
#define NI_0A05_Y_LPF_COFF_02_MSB                                        (0xA05)
#define NI_0A06_Y_LPF_COFF_03_LSB                                        (0xA06)
#define NI_0A07_Y_LPF_COFF_03_MSB                                        (0xA07)
#define NI_0A08_Y_LPF_COFF_04_LSB                                        (0xA08)
#define NI_0A09_Y_LPF_COFF_04_MSB                                        (0xA09)
#define NI_0A0A_Y_LPF_COFF_05_LSB                                        (0xA0A)
#define NI_0A0B_Y_LPF_COFF_05_MSB                                        (0xA0B)
#define NI_0A0C_Y_LPF_COFF_06_LSB                                        (0xA0C)
#define NI_0A0D_Y_LPF_COFF_06_MSB                                        (0xA0D)
#define NI_0A0E_Y_LPF_COFF_07_LSB                                        (0xA0E)
#define NI_0A0F_Y_LPF_COFF_07_MSB                                        (0xA0F)
#define NI_0A10_Y_LPF_COFF_08_LSB                                        (0xA10)
#define NI_0A11_Y_LPF_COFF_08_MSB                                        (0xA11)
#define NI_0A12_Y_LPF_COFF_09_LSB                                        (0xA12)
#define NI_0A13_Y_LPF_COFF_09_MSB                                        (0xA13)
#define NI_0A14_Y_LPF_COFF_10_LSB                                        (0xA14)
#define NI_0A15_Y_LPF_COFF_10_MSB                                        (0xA15)
#define NI_0A16_Y_LPF_COFF_11_LSB                                        (0xA16)
#define NI_0A17_Y_LPF_COFF_11_MSB                                        (0xA17)
#define NI_0A18_Y_LPF_COFF_12_LSB                                        (0xA18)
#define NI_0A19_Y_LPF_COFF_12_MSB                                        (0xA19)
#define NI_0A1A_Y_LPF_COFF_13_LSB                                        (0xA1A)
#define NI_0A1B_Y_LPF_COFF_13_MSB                                        (0xA1B)
#define NI_0A1C_Y_LPF_COFF_14_LSB                                        (0xA1C)
#define NI_0A1D_Y_LPF_COFF_14_MSB                                        (0xA1D)
#define NI_0A1E_Y_LPF_COFF_15_LSB                                        (0xA1E)
#define NI_0A1F_Y_LPF_COFF_15_MSB                                        (0xA1F)
#define NI_0A20_Y_LPF_COFF_16_LSB                                        (0xA20)
#define NI_0A21_Y_LPF_COFF_16_MSB                                        (0xA21)
#define NI_0A22_Y_LPF_COFF_17_LSB                                        (0xA22)
#define NI_0A23_Y_LPF_COFF_17_MSB                                        (0xA23)
#define NI_0A24_Y_LPF_COFF_18_LSB                                        (0xA24)
#define NI_0A25_Y_LPF_COFF_18_MSB                                        (0xA25)
#define NI_0A26_Y_LPF_COFF_19_LSB                                        (0xA26)
#define NI_0A27_Y_LPF_COFF_19_MSB                                        (0xA27)
#define NI_0A28_Y_LPF_COFF_20_LSB                                        (0xA28)
#define NI_0A29_Y_LPF_COFF_20_MSB                                        (0xA29)
#define NI_0A2A_Y_LPF_COFF_21_LSB                                        (0xA2A)
#define NI_0A2B_Y_LPF_COFF_21_MSB                                        (0xA2B)
#define NI_0A2C_Y_LPF_COFF_22_LSB                                        (0xA2C)
#define NI_0A2D_Y_LPF_COFF_22_MSB                                        (0xA2D)
#define NI_0A2E_Y_LPF_COFF_23_LSB                                        (0xA2E)
#define NI_0A2F_Y_LPF_COFF_23_MSB                                        (0xA2F)
#define NI_0A30_Y_LPF_COFF_24_LSB                                        (0xA30)
#define NI_0A31_Y_LPF_COFF_24_MSB                                        (0xA31)
#define NI_0A32_C_BPF_COFF_00_LSB                                        (0xA32)
#define NI_0A33_C_BPF_COFF_00_MSB                                        (0xA33)
#define NI_0A34_C_BPF_COFF_01_LSB                                        (0xA34)
#define NI_0A35_C_BPF_COFF_01_MSB                                        (0xA35)
#define NI_0A36_C_BPF_COFF_02_LSB                                        (0xA36)
#define NI_0A37_C_BPF_COFF_02_MSB                                        (0xA37)
#define NI_0A38_C_BPF_COFF_03_LSB                                        (0xA38)
#define NI_0A39_C_BPF_COFF_03_MSB                                        (0xA39)
#define NI_0A3A_C_BPF_COFF_04_LSB                                        (0xA3A)
#define NI_0A3B_C_BPF_COFF_04_MSB                                        (0xA3B)
#define NI_0A3C_C_BPF_COFF_05_LSB                                        (0xA3C)
#define NI_0A3D_C_BPF_COFF_05_MSB                                        (0xA3D)
#define NI_0A3E_C_BPF_COFF_06_LSB                                        (0xA3E)
#define NI_0A3F_C_BPF_COFF_06_MSB                                        (0xA3F)
#define NI_0A40_C_BPF_COFF_07_LSB                                        (0xA40)
#define NI_0A41_C_BPF_COFF_07_MSB                                        (0xA41)
#define NI_0A42_C_BPF_COFF_08_LSB                                        (0xA42)
#define NI_0A43_C_BPF_COFF_08_MSB                                        (0xA43)
#define NI_0A44_C_BPF_COFF_09_LSB                                        (0xA44)
#define NI_0A45_C_BPF_COFF_09_MSB                                        (0xA45)
#define NI_0A46_C_BPF_COFF_10_LSB                                        (0xA46)
#define NI_0A47_C_BPF_COFF_10_MSB                                        (0xA47)
#define NI_0A48_C_BPF_COFF_11_LSB                                        (0xA48)
#define NI_0A49_C_BPF_COFF_11_MSB                                        (0xA49)
#define NI_0A4A_C_BPF_COFF_12_LSB                                        (0xA4A)
#define NI_0A4B_C_BPF_COFF_12_MSB                                        (0xA4B)
#define NI_0A4C_C_BPF_COFF_13_LSB                                        (0xA4C)
#define NI_0A4D_C_BPF_COFF_13_MSB                                        (0xA4D)
#define NI_0A4E_C_BPF_COFF_14_LSB                                        (0xA4E)
#define NI_0A4F_C_BPF_COFF_14_MSB                                        (0xA4F)
#define NI_0A50_C_BPF_COFF_15_LSB                                        (0xA50)
#define NI_0A51_C_BPF_COFF_15_MSB                                        (0xA51)
#define NI_0A52_C_BPF_COFF_16_LSB                                        (0xA52)
#define NI_0A53_C_BPF_COFF_16_MSB                                        (0xA53)
#define NI_0A54_C_BPF_COFF_17_LSB                                        (0xA54)
#define NI_0A55_C_BPF_COFF_17_MSB                                        (0xA55)
#define NI_0A56_C_BPF_COFF_18_LSB                                        (0xA56)
#define NI_0A57_C_BPF_COFF_18_MSB                                        (0xA57)
#define NI_0A58_C_BPF_COFF_19_LSB                                        (0xA58)
#define NI_0A59_C_BPF_COFF_19_MSB                                        (0xA59)
#define NI_0A5A_C_BPF_COFF_20_LSB                                        (0xA5A)
#define NI_0A5B_C_BPF_COFF_20_MSB                                        (0xA5B)
#define NI_0A5C_DDFS_INC_BYTE0                                           (0xA5C)
#define NI_0A5D_DDFS_INC_BYTE1                                           (0xA5D)
#define NI_0A5E_DDFS_INC_BYTE2                                           (0xA5E)
#define NI_0A5F_DDFS_INC_BYTE3                                           (0xA5F)
#define NI_0A60_FILTER_CFG                                               (0xA60)
#define NI_0A61_STD_HPF_COFF_00_LSB                                      (0xA61)
#define NI_0A62_STD_HPF_COFF_00_MSB                                      (0xA62)
#define NI_0A63_STD_HPF_COFF_01_LSB                                      (0xA63)
#define NI_0A64_STD_HPF_COFF_01_MSB                                      (0xA64)
#define NI_0A65_STD_HPF_COFF_02_LSB                                      (0xA65)
#define NI_0A66_STD_HPF_COFF_02_MSB                                      (0xA66)
#define NI_0A67_STD_HPF_COFF_03_LSB                                      (0xA67)
#define NI_0A68_STD_HPF_COFF_03_MSB                                      (0xA68)
#define NI_0A69_STD_HPF_COFF_04_LSB                                      (0xA69)
#define NI_0A6A_STD_HPF_COFF_04_MSB                                      (0xA6A)
#define NI_0A6B_STD_HPF_COFF_05_LSB                                      (0xA6B)
#define NI_0A6C_STD_HPF_COFF_05_MSB                                      (0xA6C)
#define NI_0A6D_STD_HPF_COFF_06_LSB                                      (0xA6D)
#define NI_0A6E_STD_HPF_COFF_06_MSB                                      (0xA6E)
#define NI_0A6F_STD_HPF_COFF_07_LSB                                      (0xA6F)
#define NI_0A70_STD_HPF_COFF_07_MSB                                      (0xA70)
#define NI_0A71_STD_HPF_COFF_08_LSB                                      (0xA71)
#define NI_0A72_STD_HPF_COFF_08_MSB                                      (0xA72)
#define NI_0A73_STD_HPF_COFF_09_LSB                                      (0xA73)
#define NI_0A74_STD_HPF_COFF_09_MSB                                      (0xA74)
#define NI_0A75_STD_HPF_COFF_10_LSB                                      (0xA75)
#define NI_0A76_STD_HPF_COFF_10_MSB                                      (0xA76)
#define NI_0A77_STD_HPF_COFF_11_LSB                                      (0xA77)
#define NI_0A78_STD_HPF_COFF_11_MSB                                      (0xA78)
#define NI_0A79_STD_HPF_COFF_12_LSB                                      (0xA79)
#define NI_0A7A_STD_HPF_COFF_12_MSB                                      (0xA7A)
#define NI_0A7B_STD_HPF_COFF_13_LSB                                      (0xA7B)
#define NI_0A7C_STD_HPF_COFF_13_MSB                                      (0xA7C)
#define NI_0A7D_STD_HPF_COFF_14_LSB                                      (0xA7D)
#define NI_0A7E_STD_HPF_COFF_14_MSB                                      (0xA7E)
#define NI_0A7F_STD_HPF_COFF_15_LSB                                      (0xA7F)
#define NI_0A80_STD_HPF_COFF_15_MSB                                      (0xA80)
#define NI_0A81_STD_HPF_COFF_16_LSB                                      (0xA81)
#define NI_0A82_STD_HPF_COFF_16_MSB                                      (0xA82)
#define NI_0A88_RX_PARA_CFG                                              (0xA88)
#define NI_0A89_HD_BL_SPL_LSB                                            (0xA89)
#define NI_0A8A_HD_BL_SPL_MSB                                            (0xA8A)
#define NI_0A8B_HD_MISS_WIDTH_LSB                                        (0xA8B)
#define NI_0A8C_HD_MISS_WIDTH_MSB                                        (0xA8C)
#define NI_0A8D_HD_LCOUNT_THR1_LSB                                       (0xA8D)
#define NI_0A8E_HD_LCOUNT_THR2_LSB                                       (0xA8E)
#define NI_0A8F_HD_LCOUNT_THR12_MSB                                      (0xA8F)
#define NI_0A90_HD_LCOUNT_THR3_LSB                                       (0xA90)
#define NI_0A91_HD_LCOUNT_THR4_LSB                                       (0xA91)
#define NI_0A92_HD_LCOUNT_THR34_MSB                                      (0xA92)
#define NI_0A93_HD_LCOUNT_THR5_LSB                                       (0xA93)
#define NI_0A94_HD_LCOUNT_THR5_MSB                                       (0xA94)
#define NI_0A95_HD_DUTY_CYS_THR3_LSB                                     (0xA95)
#define NI_0A96_HD_DUTY_CYS_THR4_LSB                                     (0xA96)
#define NI_0A97_HD_DUTY_CYS_THR34_MSB                                    (0xA97)
#define NI_0A98_HD_LINE_LENGTH_LSB                                       (0xA98)
#define NI_0A99_HD_LINE_LENGTH_MSB                                       (0xA99)
#define NI_0A9A_HD_STL_WIN_BEG_PT_NOM_LSB                                (0xA9A)
#define NI_0A9B_HD_STL_WIN_BEG_PT_NOM_MSB                                (0xA9B)
#define NI_0A9C_HD_STL_WIN_END_PT_NOM_LSB                                (0xA9C)
#define NI_0A9D_HD_STL_WIN_END_PT_NOM_MSB                                (0xA9D)
#define NI_0A9E_HD_HS_WIDTH_MAN_LSB                                      (0xA9E)
#define NI_0A9F_HD_HS_WIDTH_MAN_MSB                                      (0xA9F)
#define NI_0AA0_HD_LCOUNT1_THR1_LSB                                      (0xAA0)
#define NI_0AA1_HD_LCOUNT1_THR2_LSB                                      (0xAA1)
#define NI_0AA2_HD_LCOUNT1_THR12_MSB                                     (0xAA2)
#define NI_0AA3_HD_LCOUNT1_THR3_LSB                                      (0xAA3)
#define NI_0AA4_HD_LCOUNT1_THR4_LSB                                      (0xAA4)
#define NI_0AA5_HD_LCOUNT1_THR34_MSB                                     (0xAA5)
#define NI_0AA6_HD_LCOUNT1_THR5_LSB                                      (0xAA6)
#define NI_0AA7_HD_VS_THRESH_MAX_LSB                                     (0xAA7)
#define NI_0AA8_HD_VS_THRESH_MAX_MSB                                     (0xAA8)
#define NI_0AA9_HD_VS_THRESH_MIN_LSB                                     (0xAA9)
#define NI_0AAA_HD_VS_THRESH_MIN_MSB                                     (0xAAA)
#define NI_0AAB_HD_TC_RESET_THRESH_LSB                                   (0xAAB)
#define NI_0AAC_HD_TC_RESET_THRESH_MSB                                   (0xAAC)
#define NI_0AAD_HD_STANDARD_VS_LEN_LSB                                   (0xAAD)
#define NI_0AAE_HD_STANDARD_VS_LEN_MSB                                   (0xAAE)
#define NI_0AAF_HD_VSYNC_COUNTER_MAX_LSB                                 (0xAAF)
#define NI_0AB0_HD_VSYNC_COUNTER_MAX_MSB                                 (0xAB0)
#define NI_0AB1_HD_NOISY_VSYNC_THRESH_LSB                                (0xAB1)
#define NI_0AB2_HD_NOISY_VSYNC_THRESH_MSB                                (0xAB2)
#define NI_0AB3_HD_IIR_FILT_MAX_LSB                                      (0xAB3)
#define NI_0AB4_HD_IIR_FILT_MAX_MSB                                      (0xAB4)
#define NI_0AB5_HD_PIX_CNT_3L_SUB_VAL_LSB                                (0xAB5)
#define NI_0AB6_HD_PIX_CNT_3L_SUB_VAL_MSB                                (0xAB6)
#define NI_0AB7_HD_LOOP_UPDATE_THRESH_LSB                                (0xAB7)
#define NI_0AB8_HD_LOOP_UPDATE_THRESH_MSB                                (0xAB8)
#define NI_0AB9_HD_LCOUNT_NOMINAL_MAX_LSB                                (0xAB9)
#define NI_0ABA_HD_LCOUNT_NOMINAL_MAX_MSB                                (0xABA)
#define NI_0ABB_HD_LCOUNT_MAX_LSB                                        (0xABB)
#define NI_0ABC_HD_LCOUNT_MAX_MSB                                        (0xABC)
#define NI_0ABD_HD_LCOUNT_MIN_LSB                                        (0xABD)
#define NI_0ABE_HD_LCOUNT_MIN_MSB                                        (0xABE)
#define NI_0ABF_HD_LCOUNT_MAX_EXT_LSB                                    (0xABF)
#define NI_0AC0_HD_LCOUNT_MAX_EXT_MSB                                    (0xAC0)
#define NI_0AC1_HD_LCOUNT_MIN_EXT_LSB                                    (0xAC1)
#define NI_0AC2_HD_LCOUNT_MIN_EXT_MSB                                    (0xAC2)
#define NI_0AC3_HD_VSYNC_CNT_PEAK_MIN_LSB                                (0xAC3)
#define NI_0AC4_HD_VSYNC_CNT_PEAK_MIN_MSB                                (0xAC4)
#define NI_0AC5_HD_STD_LOCK_MAX                                          (0xAC5)
#define NI_0AC6_HD_STD_LOCK_MIN                                          (0xAC6)
#define NI_0AC7_HD_VS_MAX_START_LSB                                      (0xAC7)
#define NI_0AC8_HD_VS_MAX_START_MSB                                      (0xAC8)
#define NI_0AC9_HD_VS_MIN_START_LSB                                      (0xAC9)
#define NI_0ACA_HD_VS_MIN_START_MSB                                      (0xACA)
#define NI_0ACB_HD_LCOUNT_MIN_ADD_VAL_LSB                                (0xACB)
#define NI_0ACC_HD_LCOUNT_MIN_ADD_VAL_MSB                                (0xACC)
#define NI_0ACD_HD_VS_MIN_SUB_VAL_LSB                                    (0xACD)
#define NI_0ACE_HD_VS_MIN_SUB_VAL_MSB                                    (0xACE)
#define NI_0ACF_HD_VS_MAX_ADD_VAL_LSB                                    (0xACF)
#define NI_0AD0_HD_VS_MAX_ADD_VAL_MSB                                    (0xAD0)
#define NI_0AD1_HD_LLC_LC_MAX_LSB                                        (0xAD1)
#define NI_0AD2_HD_LLC_LC_MAX_MSB                                        (0xAD2)
#define NI_0AD3_HD_OP_SPL_NEW_LSB                                        (0xAD3)
#define NI_0AD4_HD_OP_SPL_NEW_MSB                                        (0xAD4)
#define NI_0AD5_HD_HCOUNT_MAX_LSB                                        (0xAD5)
#define NI_0AD6_HD_HCOUNT_MAX_MSB                                        (0xAD6)
#define NI_0AD7_HD_VBLANK_NUM                                            (0xAD7)
#define NI_0AD8_HD_HBLANK_OFFSET_LSB                                     (0xAD8)
#define NI_0AD9_HD_HBLANK_OFFSET_MSB                                     (0xAD9)
#define NI_0ADA_HD_VACTIVE_OFFSET_LSB                                    (0xADA)
#define NI_0ADB_HD_VACTIVE_OFFSET_MSB                                    (0xADB)
#define NI_0ADC_HD_LL_SPL_LSB                                            (0xADC)
#define NI_0ADD_HD_LL_SPL_MSB                                            (0xADD)
#define NI_0ADE_HD_NEW_LINE_POS_LSB                                      (0xADE)
#define NI_0ADF_HD_NEW_LINE_POS_MSB                                      (0xADF)
#define NI_0AE0_HD_CENTRE_FREQ_TMP_BYTE0                                 (0xAE0)
#define NI_0AE1_HD_CENTRE_FREQ_TMP_BYTE1                                 (0xAE1)
#define NI_0AE2_HD_CENTRE_FREQ_TMP_BYTE2                                 (0xAE2)
#define NI_0AE3_HD_CENTRE_FREQ_TMP_BYTE3                                 (0xAE3)
#define NI_0AE4_HD_HS_WIDTH_LSB                                          (0xAE4)
#define NI_0AE5_HD_HS_WIDTH_MSB                                          (0xAE5)
#define NI_0AE6_HD_ACTIVE_LINE_THR1_LSB                                  (0xAE6)
#define NI_0AE7_HD_ACTIVE_LINE_THR1_MSB                                  (0xAE7)
#define NI_0AE8_HD_ACTIVE_LINE_THR2_LSB                                  (0xAE8)
#define NI_0AE9_HD_ACTIVE_LINE_THR2_MSB                                  (0xAE9)
#define NI_0AEA_HD_ACTIVE_LINE_THR3_LSB                                  (0xAEA)
#define NI_0AEB_HD_ACTIVE_LINE_THR3_MSB                                  (0xAEB)
#define NI_0AEC_HD_ACTIVE_LINE_ALT_THR1_LSB                              (0xAEC)
#define NI_0AED_HD_ACTIVE_LINE_ALT_THR1_MSB                              (0xAED)
#define NI_0AEE_HD_ACTIVE_LINE_ALT_THR2_LSB                              (0xAEE)
#define NI_0AEF_HD_ACTIVE_LINE_ALT_THR2_MSB                              (0xAEF)
#define NI_0AF0_HD_ACTIVE_LINE_ALT_THR3_LSB                              (0xAF0)
#define NI_0AF1_HD_ACTIVE_LINE_ALT_THR3_MSB                              (0xAF1)
#define NI_0AF2_HD_BP_BEG_NUM_LSB                                        (0xAF2)
#define NI_0AF3_HD_BP_BEG_NUM_MSB                                        (0xAF3)
#define NI_0AF4_HD_BP_END_NUM_LSB                                        (0xAF4)
#define NI_0AF5_HD_BP_END_NUM_MSB                                        (0xAF5)
#define NI_0AF6_HD_AHDSTD_CONF_LSB                                       (0xAF6)
#define NI_0AF7_HD_AHDSTD_CONF_MSB                                       (0xAF7)
#define NI_0B00_VSTD_DETECT_EN                                           (0xB00)
#define NI_0B01_PN_DISTANCE_LSB                                          (0xB01)
#define NI_0B02_PN_DISTANCE_MSB                                          (0xB02)
#define NI_0B03_PEAK_NUM                                                 (0xB03)
#define NI_0B04_THRESHOLD_EXPAND                                         (0xB04)
#define NI_0B05_MODE_MAX_NUM_LSB                                         (0xB05)
#define NI_0B06_MODE_MAX_NUM_MSB                                         (0xB06)
#define NI_0B07_CONFIDENCE_INDEX_NUM                                     (0xB07)
#define NI_0B08_CONFIDENCE_INDEX_NUM_MAX                                 (0xB08)
#define NI_0B09_MAX_RELATE_LSB                                           (0xB09)
#define NI_0B0A_MAX_RELATE_MSB                                           (0xB0A)
#define NI_0B0B_VIDEO_MODE_LSB                                           (0xB0B)
#define NI_0B0C_VIDEO_MODE_LOCK                                          (0xB0C)
#define NI_0B0D_LOST_LOCK_TIME_LSB                                       (0xB0D)
#define NI_0B0E_LOST_LOCK_TIME_MSB                                       (0xB0E)
#define NI_0B0F_PN_VALID_CHOICE_LSB                                      (0xB0F)
#define NI_0B10_PN_VALID_CHOICE_MSB                                      (0xB10)
#define NI_0B11_START_DISTANCE_LSB                                       (0xB11)
#define NI_0B12_START_DISTANCE_MSB                                       (0xB12)
#define NI_0B13_MODE_LENGTH_LSB                                          (0xB13)
#define NI_0B14_MODE_LENGTH_MSB                                          (0xB14)
#define NI_0B15_MAX_SECOND_RAT                                           (0xB15)
#define NI_0B16_MAX_LASTMAX_RAT                                          (0xB16)
#define NI_0B17_MODE_CHOICE_RAT                                          (0xB17)
#define NI_0B18_PEAK_JUDGE_START_POINT_LSB                               (0xB18)
#define NI_0B19_PEAK_JUDGE_START_POINT_MSB                               (0xB19)
#define NI_0B1A_JUDGE_MODE_OVERTIME                                      (0xB1A)
#define NI_0B1B_JUDGE_MODE_WORK_RANGE_BYTE0                              (0xB1B)
#define NI_0B1C_JUDGE_MODE_WORK_RANGE_BYTE1                              (0xB1C)
#define NI_0B1D_JUDGE_MODE_WORK_RANGE_BYTE2                              (0xB1D)
#define NI_0B1E_JUDGE_MODE_SLICE_THR_BYTE0                               (0xB1E)
#define NI_0B1F_JUDGE_MODE_SLICE_THR_BYTE1                               (0xB1F)
#define NI_0B20_HD_SD_TRACK_SPEED                                        (0xB20)
#define NI_0B21_HD_SD_SET_BLANK_THRESHOLD_LSB                            (0xB21)
#define NI_0B22_HD_SD_SET_BLANK_THRESHOLD_MSB                            (0xB22)
#define NI_0B23_HD_SD_BK_FLOOR_EN                                        (0xB23)
#define NI_0B24_HD_SD_BK_FLOOR_THR                                       (0xB24)
#define NI_0B25_HD_SD_STATUS                                             (0xB25)
#define NI_0B26_HD_SD_SYN_CNT_MAX_LSB                                    (0xB26)
#define NI_0B27_HD_SD_SYN_CNT_MAX_MSB                                    (0xB27)
#define NI_0B28_HD_SD_SYN_THR_LSB                                        (0xB28)
#define NI_0B29_HD_SD_SYN_THR_MSB                                        (0xB29)
#define NI_0B2A_HD_SD_LINE_POINT_CNT_THR_LSB                             (0xB2A)
#define NI_0B2B_HD_SD_LINE_POINT_CNT_THR_MSB                             (0xB2B)
#define NI_0B2C_HD_SD_CONFIDENCE_INDEX_MAX_LSB                           (0xB2C)
#define NI_0B2D_HD_SD_CONFIDENCE_INDEX_MAX_MSB                           (0xB2D)
#define NI_0B2E_HD_SD_CONFIDENCE_INDEX_THR_LSB                           (0xB2E)
#define NI_0B2F_HD_SD_CONFIDENCE_INDEX_THR_MSB                           (0xB2F)
#define NI_0B30_HD_SD_LINE_POINT_LSB                                     (0xB30)
#define NI_0B31_HD_SD_LINE_POINT_MSB                                     (0xB31)
#define NI_0B32_HD_SD_LINE_POINT_THR_LSB                                 (0xB32)
#define NI_0B33_HD_SD_LINE_POINT_THR_MSB                                 (0xB33)
#define NI_0B34_HD_SD_SLICE_MAX_LSB                                      (0xB34)
#define NI_0B35_HD_SD_SLICE_MAX_MSB                                      (0xB35)
#define NI_0B36_HD_SD_LINE_LENGTH_LSB                                    (0xB36)
#define NI_0B37_HD_SD_LINE_LENGTH_MSB                                    (0xB37)
#define NI_0B38_HD_SD_DUTY_CYC_THR1_LSB                                  (0xB38)
#define NI_0B39_HD_SD_DUTY_CYC_THR2_LSB                                  (0xB39)
#define NI_0B3A_HD_SD_DUTY_CYC_THR3_LSB                                  (0xB3A)
#define NI_0B3B_HD_SD_DUTY_CYC_THR4_LSB                                  (0xB3B)
#define NI_0B3C_HD_SD_DUTY_CYC_THR_MSB                                   (0xB3C)
#define NI_0B40_HD_STD_DETECT_CTRL                                       (0xB40)
#define NI_0B41_HD_STD_LINE_LOCK_THR                                     (0xB41)
#define NI_0B42_HD_STD_PIX_LOCK_THR                                      (0xB42)
#define NI_0B43_HD_STD_PIX_UNLOCK_THR                                    (0xB43)
#define NI_0B44_HD_STD_STD_SET                                           (0xB44)
#define NI_0B45_HD_STD_AHD_MARK_NUM_THR                                  (0xB45)
#define NI_0B49_HD_STD_TVI_BL_WIN_MAN_LSB                                (0xB49)
#define NI_0B4A_HD_STD_TVI_BL_WIN_MAN_MSB                                (0xB4A)
#define NI_0B4B_HD_STD_AHD_BL_WIN_MAN_LSB                                (0xB4B)
#define NI_0B4C_HD_STD_AHD_BL_WIN_MAN_MSB                                (0xB4C)
#define NI_0B4D_HD_STD_TVI_THR_LSB                                       (0xB4D)
#define NI_0B4E_HD_STD_TVI_THR_MSB                                       (0xB4E)
#define NI_0B4F_HD_STD_AHD_THR_LSB                                       (0xB4F)
#define NI_0B50_HD_STD_AHD_THR_MSB                                       (0xB50)
#define NI_0B51_HD_STD_MARK_LOCK_THR                                     (0xB51)
#define NI_0B52_HD_STD_BST_WIN_MAN_LSB                                   (0xB52)
#define NI_0B53_HD_STD_BST_WIN_MAN_MSB                                   (0xB53)
#define NI_0B54_HD_STD_BST_WIN_WID_MAN                                   (0xB54)
#define NI_0B55_HD_STD_FREQ_THR_LSB                                      (0xB55)
#define NI_0B56_HD_STD_FREQ_THR_MSB                                      (0xB56)
#define NI_0B57_HD_STD_FREQ_LOCK_CNT                                     (0xB57)
#define NI_0B58_HD_STD_FREQ_UNLOCK_CNT                                   (0xB58)
#define NI_0B59_HD_STD_OLD_VSTD                                          (0xB59)
#define NI_0B5A_HD_STD_CHECK_CARRIER_FLAG                                (0xB5A)
#define NI_0B5B_HD_STD_VSYNC_SLICE_BEG_LSB                               (0xB5B)
#define NI_0B5C_HD_STD_VSYNC_SLICE_BEG_MSB                               (0xB5C)
#define NI_0B5D_HD_STD_VSYNC_SLICE_END_LSB                               (0xB5D)
#define NI_0B5E_HD_STD_VSYNC_SLICE_END_MSB                               (0xB5E)
#define NI_0B5F_HD_STD_LINE_SLICE_THR_LSB                                (0xB5F)
#define NI_0B60_HD_STD_LINE_SLICE_THR_MSB                                (0xB60)
#define NI_0B63_HD_STD_CVI_BL_WIN_MAN_LSB                                (0xB63)
#define NI_0B64_HD_STD_CVI_BL_WIN_MAN_MSB                                (0xB64)
#define NI_0B65_HD_STD_CVI_VB_MARK00                                     (0xB65)
#define NI_0B66_HD_STD_CVI_VB_MARK01                                     (0xB66)
#define NI_0B67_HD_STD_CVI_VB_MARK02                                     (0xB67)
#define NI_0B68_HD_STD_CVI_VB_MARK03                                     (0xB68)
#define NI_0B69_HD_STD_CVI_VB_MARK04                                     (0xB69)
#define NI_0B6A_HD_STD_CVI_VB_MARK05                                     (0xB6A)
#define NI_0B6B_HD_STD_TVI_VB_MARK00                                     (0xB6B)
#define NI_0B6C_HD_STD_TVI_VB_MARK01                                     (0xB6C)
#define NI_0B6D_HD_STD_TVI_VB_MARK02                                     (0xB6D)
#define NI_0B6E_HD_STD_TVI_VB_MARK03                                     (0xB6E)
#define NI_0B6F_HD_STD_TVI_VB_MARK04                                     (0xB6F)
#define NI_0B70_HD_STD_TVI_VB_MARK05                                     (0xB70)
#define NI_0B71_HD_STD_AHD_VB_MARK00                                     (0xB71)
#define NI_0B72_HD_STD_AHD_VB_MARK01                                     (0xB72)
#define NI_0B73_HD_STD_AHD_VB_MARK02                                     (0xB73)
#define NI_0B74_HD_STD_AHD_VB_MARK03                                     (0xB74)
#define NI_0B75_HD_STD_AHD_VB_MARK04                                     (0xB75)
#define NI_0B76_HD_STD_AHD_VB_MARK05                                     (0xB76)
#define NI_0B77_HD_STD_TVI_VB_MARK06                                     (0xB77)
#define NI_0B79_HD_STD_BLANK_MAN_LSB                                     (0xB79)
#define NI_0B7A_HD_STD_BLANK_MAN_MSB                                     (0xB7A)
#define NI_0B7B_HD_STD_FREQ_THR_PRE_LSB                                  (0xB7B)
#define NI_0B7C_HD_STD_FREQ_THR_PRE_MSB                                  (0xB7C)
#define NI_0B7F_HD_STD_CVI_THR_LSB                                       (0xB7F)
#define NI_0B80_HD_STD_CVI_THR_MSB                                       (0xB80)
#define NI_0B81_HD_STD_BURST_GATE_LSB                                    (0xB81)
#define NI_0B82_HD_STD_BURST_GATE_MSB                                    (0xB82)
#define NI_0B83_HD_STD_SLICE_FLAG_THR_LSB                                (0xB83)
#define NI_0B84_HD_STD_SLICE_FLAG_THR_MSB                                (0xB84)
#define NI_0B88_VDP_LPF_COFF_00_LSB                                      (0xB88)
#define NI_0B89_VDP_LPF_COFF_00_MSB                                      (0xB89)
#define NI_0B8A_VDP_LPF_COFF_01_LSB                                      (0xB8A)
#define NI_0B8B_VDP_LPF_COFF_01_MSB                                      (0xB8B)
#define NI_0B8C_VDP_LPF_COFF_02_LSB                                      (0xB8C)
#define NI_0B8D_VDP_LPF_COFF_02_MSB                                      (0xB8D)
#define NI_0B8E_VDP_LPF_COFF_03_LSB                                      (0xB8E)
#define NI_0B8F_VDP_LPF_COFF_03_MSB                                      (0xB8F)
#define NI_0B90_VDP_LPF_COFF_04_LSB                                      (0xB90)
#define NI_0B91_VDP_LPF_COFF_04_MSB                                      (0xB91)
#define NI_0B92_VDP_LPF_COFF_05_LSB                                      (0xB92)
#define NI_0B93_VDP_LPF_COFF_05_MSB                                      (0xB93)
#define NI_0B94_VDP_LPF_COFF_06_LSB                                      (0xB94)
#define NI_0B95_VDP_LPF_COFF_06_MSB                                      (0xB95)
#define NI_0B96_VDP_LPF_COFF_07_LSB                                      (0xB96)
#define NI_0B97_VDP_LPF_COFF_07_MSB                                      (0xB97)
#define NI_0B98_VDP_LPF_COFF_08_LSB                                      (0xB98)
#define NI_0B99_VDP_LPF_COFF_08_MSB                                      (0xB99)
#define NI_0B9A_VDP_LPF_COFF_09_LSB                                      (0xB9A)
#define NI_0B9B_VDP_LPF_COFF_09_MSB                                      (0xB9B)
#define NI_0B9C_VDP_LPF_COFF_10_LSB                                      (0xB9C)
#define NI_0B9D_VDP_LPF_COFF_10_MSB                                      (0xB9D)
#define NI_0B9E_VDP_LPF_COFF_11_LSB                                      (0xB9E)
#define NI_0B9F_VDP_LPF_COFF_11_MSB                                      (0xB9F)
#define NI_0BA0_VDP_LPF_COFF_12_LSB                                      (0xBA0)
#define NI_0BA1_VDP_LPF_COFF_12_MSB                                      (0xBA1)
#define NI_0E02_MIPI_CFG_CTRL2                                           (0xE02)
#define NI_0E03_MIPI_FRAME_NUM_MAX_LSB                                   (0xE03)
#define NI_0E04_MIPI_FRAME_NUM_MAX_MSB                                   (0xE04)
#define NI_0E05_MIPI_HALT_EN                                             (0xE05)
#define NI_0E06_MIPI_DATA_TYPE                                           (0xE06)
#define NI_0E07_MIPI_DATA_TYPE_CFG                                       (0xE07)
#define NI_0E08_MIPI_EN                                                  (0xE08)
#define NI_0E09_MIPI_VC_CFG                                              (0xE09)
#define NI_0E0A_MIPI_PRESSURE_TEST_LSB                                   (0xE0A)
#define NI_0E0B_MIPI_PRESSURE_TEST_MSB                                   (0xE0B)
#define NI_0E0C_MIPI_FE_CNT_LSB                                          (0xE0C)
#define NI_0E0D_MIPI_FE_CNT_MSB                                          (0xE0D)
#define NI_0E0E_MIPI_DATA_REPLACE_EN                                     (0xE0E)
#define NI_0E0F_MIPI_DATA_REPLACE_BYTE1                                  (0xE0F)
#define NI_0E10_MIPI_DATA_REPLACE_BYTE2                                  (0xE10)
#define NI_0E11_MIPI_DATA_REPLACE_BYTE3                                  (0xE11)
#define NI_0E12_MIPI_DATA_REPLACE_BYTE4                                  (0xE12)
#define NI_0E13_MIPI_FRAME_CNT_CLR                                       (0xE13)
#define NI_0E14_MIPI_FRAME_CNT_BYTE1                                     (0xE14)
#define NI_0E15_MIPI_FRAME_CNT_BYTE2                                     (0xE15)
#define NI_0E16_MIPI_FRAME_CNT_BYTE3                                     (0xE16)
#define NI_0E17_MIPI_FRAME_CNT_BYTE4                                     (0xE17)
#define NI_0E18_MIPI_DATALOSS_INT_CLR                                    (0xE18)
#define NI_0E19_MIPI_DATALOSS_TIMEOUT_LSB                                (0xE19)
#define NI_0E1A_MIPI_DATALOSS_TIMEOUT_MSB                                (0xE1A)
#define NI_0E1B_MIPI_ERR_PIXEL_CNT_CLR                                   (0xE1B)
#define NI_0E1C_MIPI_ERR_PIXEL_CNT_LSB                                   (0xE1C)
#define NI_0E1D_MIPI_ERR_PIXEL_CNT_MSB                                   (0xE1D)
#define NI_0E1E_MIPI_ERR_LINE_CNT_CLR                                    (0xE1E)
#define NI_0E1F_MIPI_ERR_LINE_CNT_LSB                                    (0xE1F)
#define NI_0E20_MIPI_ERR_LINE_CNT_MSB                                    (0xE20)
#define NI_0E21_MIPI_IRQ_MASK                                            (0xE21)
#define NI_0E22_MIPI_INT_LIST                                            (0xE22)

#define NI_1000_PHY_CTRL0                  (0x1000)
#define NI_1001_PLL_CTRL1                  (0x1001)
#define NI_1003_PLL_PreDiv                 (0x1003)
#define NI_1004_PLL_FBDiv                  (0x1004)
#define NI_1005_Clock_Lane_Skew_Phases_Set (0x1005)
#define NI_1006_LDO_Output_CFG             (0x1006)
#define NI_1007_Lane_01_Skew_Phases_Set    (0x1007)
#define NI_1008_Data_Lane_Vod_Range_Set    (0x1008)
#define NI_100B_Clock_Lane_Vod_Range_Set   (0x100B)
#define NI_1011_Data_Sample_Phases_Set     (0x1011)
#define NI_101F_Voltage_Select             (0x101F)
#define NI_1020_Reg_Dig_Rstn               (0x1020)
#define NI_1021_Inverting_Enable           (0x1021)
#define NI_1040_Di_Lane_Clk                (0x1040)
#define NI_1045_HS_Lane_Clk                (0x1045)
#define NI_1046_HS_Lane_Clk                (0x1046)
#define NI_1047_HS_Lane_Clk                (0x1047)
#define NI_1048_HS_Lane_Clk                (0x1048)
#define NI_1049_HS_Lane_Clk                (0x1049)
#define NI_104A_HS_Lane_Clk                (0x104A)
#define NI_104C_LP_Lane_Clk                (0x104C)
#define NI_104D_HS_Lane_Clk                (0x104D)
#define NI_104E_HS_Lane_Clk                (0x104E)
#define NI_1050_HS_Lane_Clk                (0x1050)
#define NI_1051_HS_Lane_Clk                (0x1051)
#define NI_1052_HS_Lane_Clk                (0x1052)
#define NI_1060_Di_Lane_Data0              (0x1060)
#define NI_1065_HS_Lane_Data0              (0x1065)
#define NI_1066_HS_Lane_Data0              (0x1066)
#define NI_1067_HS_Lane_Data0              (0x1067)
#define NI_1068_HS_Lane_Data0              (0x1068)
#define NI_1069_HS_Lane_Data0              (0x1069)
#define NI_106A_HS_Lane_Data0              (0x106A)
#define NI_106C_LP_Lane_Data0              (0x106C)
#define NI_106D_HS_Lane_Data0              (0x106D)
#define NI_106E_HS_Lane_Data0              (0x106E)
#define NI_1070_HS_Lane_Data0              (0x1070)
#define NI_1071_HS_Lane_Data0              (0x1071)
#define NI_1072_HS_Lane_Data0              (0x1072)
#define NI_1080_Di_Lane_Data1              (0x1080)
#define NI_1085_HS_Lane_Data1              (0x1085)
#define NI_1086_HS_Lane_Data1              (0x1086)
#define NI_1087_HS_Lane_Data1              (0x1087)
#define NI_1088_HS_Lane_Data1              (0x1088)
#define NI_1089_HS_Lane_Data1              (0x1089)
#define NI_108A_HS_Lane_Data1              (0x108A)
#define NI_108C_LP_Lane_Data1              (0x108C)
#define NI_108D_HS_Lane_Data1              (0x108D)
#define NI_108E_HS_Lane_Data1              (0x108E)
#define NI_1090_HS_Lane_Data1              (0x1090)
#define NI_1091_HS_Lane_Data1              (0x1091)
#define NI_1092_HS_Lane_Data1              (0x1092)
#define NI_10E3_Mode_CFG                   (0x10E3)
#define NI_10E5_TTL_Mode_IO_Drive_Strength (0x10E5)
#define NI_10EB_Clock_Data_Lane_Enable     (0x10EB)

/****************  sys reg   ***********************/
#define NI_4000_VOPLL_CFG0                   (0x4000)
#define NI_4001_VOPLL_CFG1                   (0x4001)
#define NI_4002_VOPLL_CFG2                   (0x4002)
#define NI_4003_VOPLL_CFG3                   (0x4003)
#define NI_4004_VOPLL_CFG4                   (0x4004)
#define NI_4005_VOPLL_CFG5                   (0x4005)
#define NI_4014_PLL_CFG                      (0x4014)
#define NI_4015_PLL_STATUS0                  (0x4015)
#define NI_4016_PLL_STATUS1                  (0x4016)
#define NI_4017_AUTO_CFG                     (0x4017)
#define NI_4018_SYS_CLK_ENABLE               (0x4018)
#define NI_4019_SYS_RST                      (0x4019)
#define NI_4020_OTHER_SFT_RST                (0x4020)
#define NI_4021_AUD_REG1_0                   (0x4021)
#define NI_4022_AUD_REG1_1                   (0x4022)
#define NI_4023_AUD_REG1_2                   (0x4023)
#define NI_4024_AUD_REG1_3                   (0x4024)
#define NI_4025_AUD_REG2_0                   (0x4025)
#define NI_4026_AUD_REG2_1                   (0x4026)
#define NI_4027_AUD_REG2_2                   (0x4027)
#define NI_4028_AUD_REG2_3                   (0x4028)
#define NI_4030_MIPI_CLK_CFG                 (0x4030)
#define NI_4031_MIPI_RST_CFG                 (0x4031)
#define NI_4080_IRQ_ENABLE                   (0x4080)
#define NI_4081_IRQ_STATUS                   (0x4081)
#define NI_4082_READ_WRITE_AUTO_INC_EN       (0x4082)
#define NI_40F0_DEVICE_ID_1                  (0x40F0)
#define NI_40F1_DEVICE_ID_0                  (0x40F1)
#define NI_4101_IO_MUX_REG                   (0x4101)
#define NI_4107_IIC_SCL_IO_CTRL              (0x4107)
#define NI_4108_IIC_SAD_IO_CTRL              (0x4108)
#define NI_4114_VO_VS_IO_CTRL                (0x4114)
#define NI_4115_VO_HS_IO_CTRL                (0x4115)
#define NI_4116_IIS_LRCKR_IO_CTRL            (0x4116)
#define NI_4117_IIS_LRCKP_IO_CTRL            (0x4117)
#define NI_4118_IIC_SDA_IO_CTRL              (0x4118)
#define NI_4119_IRQ_IO_CTRL                  (0x4119)
#define NI_4134_VD0_DATA0_IO_CTRL0           (0x4134)
#define NI_4135_VD0_DATA0_IO_CTRL1           (0x4135)
#define NI_4140_TXD1_IO_CTRL0                (0x4140)
#define NI_4141_TXD0_IO_CTRL0                (0x4141)
#define NI_4200_AFE_CFG_CH0                  (0x4200)
#define NI_4201_AFE_EQ_CFG0_CH0              (0x4201)
#define NI_4202_AFE_EQ_CFG1_CH0              (0x4202)
#define NI_4203_AFE_EQ_CFG2_CH0              (0x4203)
#define NI_4204_AFE_EQ_CFG3_CH0              (0x4204)
#define NI_4205_AFE_EQ_CFG4_CH0              (0x4205)
#define NI_4206_AFE_LPF_CFG0_CH0             (0x4206)
#define NI_4207_AFE_LPF_CFG1_CH0             (0x4207)
#define NI_4208_AFE_LPF_CFG2_CH0             (0x4208)
#define NI_4209_AFE_LPF_CFG3_CH0             (0x4209)
#define NI_420A_AFE_LPF_CFG4_CH0             (0x420A)
#define NI_420B_AFE_CLAMP_CFG0_CH0           (0x420B)
#define NI_420C_AFE_CLAMP_CFG1_CH0           (0x420C)
#define NI_420D_AFE_CLAMP_CFG2_CH0           (0x420D)
#define NI_420E_AFE_BUF_CFG0_CH0             (0x420E)
#define NI_420F_AFE_BUF_CFG1_CH0             (0x420F)
#define NI_4210_AFE_BUF_CFG2_CH0             (0x4210)
#define NI_4211_AFE_PG_DET_CFG_CH0           (0x4211)
#define NI_4212_AFE_PG_DET_STA_CH0           (0x4212)
#define NI_4213_AFE_PG_DET_THR_B0_CH0        (0x4213)
#define NI_4214_AFE_PG_DET_THR_B1_CH0        (0x4214)
#define NI_4215_AFE_PG_DET_THR_B2_CH0        (0x4215)
#define NI_4216_AFE_PG_DET_THR_B3_CH0        (0x4216)
#define NI_4300_ADC_CFG0_CH0                 (0x4300)
#define NI_4301_ADC_CFG1_CH0                 (0x4301)
#define NI_4302_ADC_CFG2_CH0                 (0x4302)
#define NI_4303_ADC_STA_CH0                  (0x4303)
#define NI_4408_AUDIO_IIS_DSP_ENCODE_CONTROL (0x4408)
#define NI_4411_EE_BYPASS_CFG                (0x4411)
#define NI_4412_MIPI_TTL_CFG                 (0x4412)

#define NI_5004_CSI2_RESETN    (0x5004)
#define NI_5020_INT_ST_MAIN    (0x5020)
#define NI_5024_INT_ST_VPG     (0x5024)
#define NI_5028_INT_ST_IDI     (0x5028)
#define NI_5030_INT_ST_PHY     (0x5030)
#define NI_5040_INT_MASK_N_VPG (0x5040)
#define NI_5044_INT_FORCE_VPG  (0x5044)
#define NI_5048_INT_MASK_N_IDI (0x5048)
#define NI_504C_INT_FORCE_IDI  (0x504C)
#define NI_5058_INT_MASK_N_PHY (0x5058)
#define NI_505C_INT_FORCE_PHY  (0x505C)
#define NI_5080_VPG_CTRL       (0x5080)
#define NI_50e0_PHY_RSTZ       (0x50e0)
#define NI_50e4_PHY_IF_CFG     (0x50e4)
#define NI_50e8_LPCLK_CTRL     (0x50e8)
#define NI_50f0_CLKMGR_CFG     (0x50f0)
#define NI_50fc_TO_CNT_CFG     (0x50fc)

#define NI_CABLE_LEN_MAX_LVL   (40)
#define NI_CABLE_LEN_MIN_LEVEL (0)
#define NI_EQ_REG_NUM          (4)
#define NI_MIPI_DEV_MAX_NUM    (1)

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

typedef enum tagNiVideoClass {
    NI_VIDEO_CLASS_CVI_720,
    NI_VIDEO_CLASS_CVI_1080,
    NI_VIDEO_CLASS_AHD_720,
    NI_VIDEO_CLASS_AHD_1080,
    NI_VIDEO_CLASS_TVI_720,
    NI_VIDEO_CLASS_TVI_1080,
    NI_VIDEO_CLASS_BUTT
} NI_VIDEO_CLASS_E;

typedef enum tagNiCableType {
    NI_CABLE_TYPE_COAXIAL,   /*同轴线缆*/
    NI_CABLE_TYPE_UTP_10OHM, /*10 ohm阻抗双绞线*/
    NI_CABLE_TYPE_AVIATION,  /*航空头车载线缆*/
    NI_CABLE_TYPE_BMW_4P,    /*宝马线4P规格线缆*/

    NI_CABLE_TYPE_BUTT
} NI_CABLE_TYPE_E;

typedef enum tagNiEqTableItem {
    EQ_TABLE_TOTAL_LV,
    NI_EQ_TABLE_CABLEN,
    NI_EQ_TABLE_EQREG,
    NI_EQ_TABLE_MCCCFG,
    NI_EQ_TABLE_MCCVAL,
    NI_EQ_TABLE_SATVAL,
    NI_EQ_TABLE_BUTT
} NI_EQ_TABLE_ITEM_E;

typedef struct tagNiSoftRestModule {
    bool Co485Rst;
    bool AudioRst;
    bool VoRst;
    bool VdpRst;
    bool AdcRst;
    bool ResampleRst;
    bool DwsampleRst;
    bool UpsampleRst;
    bool SampleRst;
    bool StdDetectRst;
    bool WaitTime;
} NI_SOFT_REST_MODULE_S;

/*视频锁定状态结构体*/
typedef struct tagNiLockStatus {
    union {
        struct {
            uint8_t u8CVIPnLock : 1;
            uint8_t : 7;
        };
        uint8_t u8Pn;
    } pn;
    union {
        struct {
            uint8_t : 6;
            uint8_t u8CarrierLock : 1;
            uint8_t u8MarkLock : 1;
        };
        uint8_t u8Std;
    } std;
    union {
        struct {
            uint8_t u8ColorKill : 1;
            uint8_t u8Vsync : 1;
            uint8_t u8HspllBe : 1;
            uint8_t u8HspllFe : 1;
            uint8_t u8Freerun : 1;
            uint8_t : 1;
            uint8_t u8SdBurst : 1;
            uint8_t u8HdSd : 1;
        };
        uint8_t u8Status;
    } status;
    union {
        struct {
            uint8_t u8FieldFreq : 1;
            uint8_t u8Pedestal : 1;
            uint8_t : 6;
        };
        uint8_t u8Status2;
    } status2;
    uint8_t u8PnFmt;
    uint8_t u8HdFmt;
    uint8_t u8SdFmt;
    uint8_t u8MarkFmt;
} NI_LOCK_STATUS_S;

/*标清视频状态结构体*/
typedef union UnNiSdStatus {
    struct {
        uint8_t u8SColorKill : 1;
        uint8_t u8SVsync : 1;
        uint8_t u8SspllBe : 1;
        uint8_t u8SspllFe : 1;
        uint8_t u8Freerun : 1;
        uint8_t : 1;
        uint8_t u8SdSyncDepMsb : 2;
    };
    uint8_t u8SDstatus;
} NiSdStatus;

/*VDP测试模式配置表结构体*/
typedef struct tagNiVdpTestMode {
    uint32_t u32ActiveLine;
    uint32_t u32TotalLine;
    uint32_t u32ActivePixel;
    uint32_t u32TotalPixel;
    uint32_t u32BufferMode;
    uint32_t u32BufferValue;
} NI_VDP_TEST_MODE_S;

NI_VDP_TEST_MODE_S gstNiVdpTestModeAttr[NI_VIDEO_FMT_BUTT] = {
    [NI_CVI_1280x720_25HZ]     = {720, 750, 1280, 11520 / 2, 1, 50},
    [NI_CVI_1280x720_30HZ]     = {720, 750, 1280, 9600 / 2, 1, 50},
    [NI_CVI_1280x720_50HZ]     = {720, 750, 1280, 5760 / 2, 0, 600},
    [NI_CVI_1280x720_60HZ]     = {720, 750, 1280, 4800 / 2, 0, 50},
    [NI_CVI_1280x720_30HZ_V20] = {720, 750, 1280, 9600 / 2, 0, 2000},
    [NI_CVI_1280x720_60HZ_V20] = {720, 750, 1280, 4800 / 2, 0, 50},
    [NI_AHD_1280x720_25HZ]     = {720, 750, 1280, 11520 / 2, 1, 50},
    [NI_AHD_1280x720_30HZ]     = {720, 750, 1280, 9600 / 2, 1, 50},
    [NI_AHD_1280x720_50HZ]     = {720, 750, 1280, 5760 / 2, 0, 50},
    [NI_AHD_1280x720_60HZ]     = {720, 750, 1280, 4800 / 2, 0, 50},
    [NI_TVI_1280x720_25HZ]     = {720, 750, 1280, 11520 / 2, 1, 50},
    [NI_TVI_1280x720_30HZ]     = {720, 750, 1280, 9600 / 2, 1, 50},
    [NI_TVI_1280x720_50HZ]     = {720, 750, 1280, 5760 / 2, 0, 50},
    [NI_TVI_1280x720_60HZ]     = {720, 750, 1280, 4800 / 2, 0, 50},
    [NI_TVI3_1280x720_25HZ]    = {720, 750, 1280, 11520 / 2, 1, 50},
    [NI_TVI3_1280x720_30HZ]    = {720, 750, 1280, 9600 / 2, 1, 50},

    [NI_CVI_1280x960_25HZ]  = {960, 1000, 1280, 4320, 0, 400},
    [NI_CVI_1280x960_30HZ]  = {960, 1000, 1280, 3600, 0, 50},
    [NI_CVI_1920x1080_15HZ] = {1080, 1125, 1920, 6400, 0, 50},
    [NI_TVI_1920x1080_15HZ] = {1080, 1125, 1920, 6400, 0, 50},

    [NI_CVI_1920x1080_25HZ]     = {1080, 1125, 1920, 7680 / 2, 0, 400},
    [NI_CVI_1920x1080_30HZ]     = {1080, 1125, 1920, 6400 / 2, 0, 50},
    [NI_CVI_1920x1080_30HZ_V20] = {1080, 1125, 1920, 6400 / 2, 0, 50},
    [NI_AHD_1920x1080_25HZ]     = {1080, 1125, 1920, 7680 / 2, 0, 50},
    [NI_AHD_1920x1080_30HZ]     = {1080, 1125, 1920, 6400 / 2, 0, 50},
    [NI_TVI_1920x1080_25HZ]     = {1080, 1125, 1920, 7680 / 2, 0, 50},
    [NI_TVI_1920x1080_30HZ]     = {1080, 1125, 1920, 6400 / 2, 0, 50},

    [NI_SD_NTSC_JM]   = {480 / 2, 525 / 2, 960, 108000000 / 30 / 525, 0, 50},
    [NI_SD_NTSC_443]  = {480 / 2, 525 / 2, 960, 108000000 / 30 / 525, 0, 50},
    [NI_SD_PAL_M]     = {480 / 2, 525 / 2, 960, 108000000 / 30 / 525, 0, 50},
    [NI_SD_PAL_60]    = {480 / 2, 525 / 2, 960, 108000000 / 30 / 525, 0, 50},
    [NI_SD_PAL_CN]    = {576 / 2, 625 / 2, 960, 108000000 / 25 / 625, 0, 50},
    [NI_SD_PAL_BGHID] = {576 / 2, 625 / 2, 960, 108000000 / 25 / 625, 0, 50},
};

/***************************同轴 EQ*************************************/
/********************gau32NIoaRegValue_CVI_720p********************/
uint32_t gau32NIoaRegValue_CVI_720p_OLD_LOSS[] = {0x0002, 0x0004, 0x0006, 0x0008, 0x000b, 0x0016, 0x001d, 0x0032, 0x0036, 0x005e, 0x008b, 0x00af, 0x00ea,
                                                  0x0110, 0x018c, 0x021b, 0x03bc, 0x0516, 0x0699, 0x0903, 0x0969, 0x2008, 0x200d, 0x2013, 0x2017, 0xffff};

uint32_t gau32NIoaRegValue_CVI_720p_NEW_LOSS[] = {0x0002, 0x0004, 0x0006, 0x0008, 0x000b, 0x0016, 0x001d, 0x0032, 0x0036, 0x005e, 0x008b, 0x00af, 0x00ea,
                                                  0x0110, 0x018c, 0x021b, 0x03bc, 0x0516, 0x0699, 0x0903, 0x0969, 0x2008, 0x200d, 0x2013, 0x2017, 0xffff};

uint32_t gau8NIoaRegValue_CVI_720p_SYNC[] = {
    0xffff, 0xb3, 0xae, 0xa9, 0xa5, 0x9f, 0x99, 0x97, 0x96, 0x93, 0x8f, 0x8b, 0x89,
    0x82,   0x7f, 0x7e, 0x75, 0x73, 0x6f, 0x6c, 0x58, 0x55, 0x53, 0x52, 0x51, 0x51,
};

uint16_t gau16NIoaRegValue_CVI_720p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00, 0x01, 0x0f, 0x0f, 0x0b, 0x0b, 0x0f, 0x01, 0x0f, 0x0f, 0x08, 0x0f, 0x0f,
            0x0f, 0x08, 0x0a, 0x0a, 0x0a, 0x0f, 0x0f, 0x0a, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
        },
    [1] =
        {
            0x00, 0x01, 0x02, 0x05, 0x04, 0x0a, 0x0f, 0x13, 0x16, 0x1a, 0x1f, 0x24, 0x28,
            0x33, 0x45, 0x50, 0x5e, 0x54, 0x59, 0x5b, 0x60, 0x66, 0x6b, 0x7f, 0x7f, 0x7f,
        },
    [2] =
        {
            0x00, 0x0e, 0x0e, 0x0a, 0x09, 0x0a, 0x0c, 0x0c, 0x0c, 0x0b, 0x0b, 0x0a, 0x0a,
            0x09, 0x06, 0x06, 0x07, 0x07, 0x05, 0x04, 0x04, 0x04, 0x03, 0x02, 0x02, 0x02,
        },
    [3] =
        {
            0x00, 0x04, 0x0a, 0x14, 0x14, 0x20, 0x26, 0x2b, 0x2d, 0x32, 0x35, 0x38, 0x40,
            0x4b, 0x66, 0x76, 0x70, 0xc4, 0xbe, 0xff, 0xff, 0xff, 0xff, 0x78, 0x70, 0x70,
        },
};

uint8_t gau8NIoaRegValue_CVI_720p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x89, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xe0, 0xf0, 0xf0,
};

uint8_t gau8NIoaRegValue_CVI_720p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NIoaRegValue_CVI_720p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
};

/********************gau32NIoaRegValue_CVI_1080p********************/
uint32_t gau32NIoaRegValue_CVI_1080p_OLD_LOSS[] = {
    0x000f, 0x0016, 0x002a, 0x0042, 0x0052, 0x00a2, 0x0110, 0x0250, 0x02aa, 0x0576, 0x200a, 0x2010, 0x2016,
};

uint32_t gau32NIoaRegValue_CVI_1080p_NEW_LOSS[] = {
    0x000f, 0x0016, 0x002a, 0x0042, 0x0052, 0x00a2, 0x0110, 0x0250, 0x02aa, 0x0576, 0x200a, 0x2010, 0x2016,
};

uint32_t gau8NIoaRegValue_CVI_1080p_SYNC[] = {
    0xff, 0xBF, 0xBA, 0xB5, 0xB1, 0xAC, 0xa4, 0x9e, 0x9d, 0x99, 0x92, 0x8e, 0x8c,
};

uint16_t gau16NIoaRegValue_CVI_1080p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
        },
    [1] =
        {
            0x01,
            0x02,
            0x02,
            0x02,
            0x04,
            0x06,
            0x08,
            0x0e,
            0x11,
            0x16,
            0x12,
            0x10,
            0x13,
        },
    [2] =
        {
            0x0e,
            0x0e,
            0x0a,
            0x0a,
            0x0a,
            0x0a,
            0x04,
            0x03,
            0x03,
            0x03,
            0x02,
            0x01,
            0x01,
        },
    [3] =
        {
            0x00,
            0x03,
            0x0C,
            0x12,
            0x16,
            0x1E,
            0x22,
            0x30,
            0x30,
            0x38,
            0x40,
            0x45,
            0x48,
        },
};

uint8_t gau8NIoaRegValue_CVI_1080p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf0,
};

uint8_t gau8NIoaRegValue_CVI_1080p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
};

uint8_t gau8NIoaRegValue_CVI_1080p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
};

/********************gau32NIoaRegValue_AHD_720p********************/
uint32_t gau32NIoaRegValue_AHD_720p_OLD_LOSS[] = {0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0006, 0x0008, 0x000b, 0x0010, 0x0014, 0x001d, 0x0027, 0x0032,
                                                  0x0038, 0x006b, 0x00a0, 0x00d8, 0x011e, 0x0140, 0x0170, 0x0182, 0x01f0, 0x0315, 0x0400, 0x0480, 0xffff};

uint32_t gau32NIoaRegValue_AHD_720p_NEW_LOSS[] = {0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0006, 0x0008, 0x000b, 0x0010, 0x0014, 0x001d, 0x0027, 0x0032,
                                                  0x0038, 0x006b, 0x00a0, 0x00d8, 0x011e, 0x0140, 0x0170, 0x0182, 0x01f0, 0x0315, 0x0400, 0x0480, 0xffff};

uint32_t gau8NIoaRegValue_AHD_720p_SYNC[] = {
    0xff, 0xca, 0xbe, 0xbd, 0xbd, 0xb2, 0xa6, 0x9e, 0xa9, 0x9c, 0x9b, 0x94, 0x94, 0x8d, 0x8c, 0x89, 0x8d, 0x8b, 0x80, 0x7e, 0x72, 0x70, 0x6e, 0x6c, 0x6a, 0x6a,
};

uint16_t gau16NIoaRegValue_AHD_720p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00, 0x02, 0x02, 0x02, 0x02, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
            0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
        },
    [1] =
        {
            0x00, 0x02, 0x09, 0x0C, 0x0E, 0x0c, 0x17, 0x1a, 0x23, 0x29, 0x2a, 0x2d, 0x2e,
            0x2f, 0x32, 0x3A, 0x4c, 0x50, 0x58, 0x60, 0x60, 0x70, 0x7d, 0x75, 0x77, 0x77,
        },
    [2] =
        {
            0x0e, 0x0e, 0x0d, 0x0d, 0x0C, 0x0A, 0x09, 0x09, 0x08, 0x07, 0x06, 0x06, 0x05,
            0x05, 0x05, 0x04, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        },
    [3] =
        {
            0x00, 0x02, 0x08, 0x0b, 0x10, 0x16, 0x21, 0x2a, 0x2e, 0x34, 0x36, 0x3a, 0x48,
            0x4e, 0x58, 0x5e, 0xa2, 0xaa, 0xb4, 0xc0, 0xd0, 0xc6, 0xbb, 0xc4, 0xe4, 0xe4,
        },
};

uint8_t gau8NIoaRegValue_AHD_720p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xA0, 0xc0, 0x80, 0x80, 0xaa, 0xaa,
};

uint8_t gau8NIoaRegValue_AHD_720p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NIoaRegValue_AHD_720p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
};

/********************gau32NIoaRegValue_AHD_1080p********************/
uint32_t gau32NIoaRegValue_AHD_1080p_OLD_LOSS[] = {0x0001, 0x0002, 0x0004, 0x0006, 0x0007, 0x000F, 0x0017, 0x0039, 0x0045, 0x0080, 0x00c0, 0x0137,
                                                   0x0177, 0x0239, 0x02A5, 0x03a9, 0x0553, 0x201a, 0x202d, 0x202e, 0x2030, 0x2032, 0x2070, 0xffff};

uint32_t gau32NIoaRegValue_AHD_1080p_NEW_LOSS[] = {0x0001, 0x0002, 0x0004, 0x0006, 0x0007, 0x000F, 0x0017, 0x0039, 0x0045, 0x0080, 0x00c0, 0x0137,
                                                   0x0177, 0x0239, 0x02A5, 0x03a9, 0x0553, 0x201a, 0x202d, 0x202e, 0x2030, 0x2032, 0x2070, 0xffff};

uint32_t gau8NIoaRegValue_AHD_1080p_SYNC[] = {
    0xBF, 0xBF, 0xBB, 0xB6, 0xB5, 0xAF, 0xAA, 0xA4, 0x9A, 0x9A, 0x93, 0x91, 0x8e, 0x8B, 0x84, 0x81, 0x7c, 0x77, 0x73, 0x70, 0x5D, 0x5B, 0x58, 0x58,
};

uint16_t gau16NIoaRegValue_AHD_1080p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
        },
    [1] =
        {
            0x00, 0x02, 0x04, 0x04, 0x04, 0x09, 0x09, 0x0f, 0x0f, 0x12, 0x16, 0x18, 0x18, 0x20, 0x30, 0x38, 0x40, 0x44, 0x4F, 0x4F, 0x5F, 0x5F, 0x72, 0x72,
        },
    [2] =
        {
            0x0E, 0x0E, 0x0E, 0x0C, 0x0C, 0x08, 0x07, 0x06, 0x05, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x03, 0x03, 0x03, 0x03, 0x04, 0x03, 0x02, 0x02,
        },
    [3] =
        {
            0x00, 0x06, 0x0A, 0x10, 0x10, 0x1a, 0x20, 0x28, 0x28, 0x32, 0x36, 0x40, 0x40, 0x56, 0x56, 0x56, 0x64, 0x68, 0xC2, 0xC2, 0xE0, 0xE0, 0x46, 0x46,
        },
};

uint8_t gau8NIoaRegValue_AHD_1080p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xD0, 0x80, 0x80, 0x80,
};

uint8_t gau8NIoaRegValue_AHD_1080p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
};

uint8_t gau8NIoaRegValue_AHD_1080p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x80, 0x9A, 0xC0, 0xC0,
};

/********************gau32NIoaRegValue_TVI_720p********************/
uint32_t gau32NIoaRegValue_TVI_720p_OLD_LOSS[] = {0x0002, 0x0003, 0x0006, 0x0008, 0x000b, 0x0013, 0x001d, 0x0030, 0x0035, 0x005f,
                                                  0x008e, 0x00f0, 0x010f, 0x013d, 0x025d, 0x02e5, 0x0500, 0x05d5, 0x080e, 0xffff};

uint32_t gau32NIoaRegValue_TVI_720p_NEW_LOSS[] = {0x0002, 0x0003, 0x0006, 0x0008, 0x000b, 0x0013, 0x001d, 0x0030, 0x0035, 0x005f,
                                                  0x008e, 0x00f0, 0x010f, 0x013d, 0x025d, 0x02e5, 0x0500, 0x05d5, 0x080e, 0xffff};

uint32_t gau8NIoaRegValue_TVI_720p_SYNC[] = {
    0xff, 0xdc, 0xd6, 0xcf, 0xce, 0xca, 0xc2, 0xb9, 0xab, 0xa7, 0x9f, 0x9d, 0x99, 0x95, 0x92, 0x8e, 0x89, 0x7e, 0x87, 0x7e,
};

uint16_t gau16NIoaRegValue_TVI_720p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00, 0x01, 0x01, 0x01, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
        },
    [1] =
        {
            0x00, 0x01, 0x02, 0x05, 0x05, 0x0a, 0x0d, 0x10, 0x13, 0x16, 0x1f, 0x23, 0x26, 0x2a, 0x36, 0x46, 0x50, 0x50, 0x36, 0x36,
        },
    [2] =
        {
            0x00, 0x0e, 0x0d, 0x0b, 0x0b, 0x0a, 0x0a, 0x09, 0x09, 0x07, 0x07, 0x06, 0x05, 0x05, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03,
        },
    [3] =
        {
            0x00, 0x06, 0x0b, 0x12, 0x14, 0x1a, 0x20, 0x26, 0x2b, 0x34, 0x38, 0x48, 0x42, 0x65, 0x7d, 0x7b, 0xc4, 0xc4, 0xff, 0xff,
        },
};

uint8_t gau8NIoaRegValue_TVI_720p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x20, 0x20,
};

uint8_t gau8NIoaRegValue_TVI_720p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NIoaRegValue_TVI_720p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
};

/********************gau32NIoaRegValue_TVI_1080p********************/
uint32_t gau32NIoaRegValue_TVI_1080p_OLD_LOSS[] = {0x0003, 0x0008, 0x0015, 0x0028, 0x003a, 0x0048, 0x0080, 0x00F9, 0x01a0, 0x0305, 0x0520,
                                                   0x08f0, 0x0a90, 0x13b6, 0x200B, 0x2016, 0x2020, 0x2048, 0x2050, 0x2058, 0x2063, 0xffff};

uint32_t gau32NIoaRegValue_TVI_1080p_NEW_LOSS[] = {0x0003, 0x0008, 0x0015, 0x0028, 0x003a, 0x0048, 0x0080, 0x00F9, 0x01a0, 0x0305, 0x0520,
                                                   0x08f0, 0x0a90, 0x13b6, 0x200B, 0x2016, 0x2020, 0x2048, 0x2050, 0x2058, 0x2063, 0xffff};

uint32_t gau8NIoaRegValue_TVI_1080p_SYNC[] = {
    0xff, 0xB7, 0xB4, 0xAB, 0xA2, 0x9B, 0x95, 0x8E, 0x88, 0x85, 0x7F, 0x79, 0x74, 0x6E,
};

uint16_t gau16NIoaRegValue_TVI_1080p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00,
            0x00,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
            0x0F,
        },
    [1] =
        {
            0x00,
            0x00,
            0x02,
            0x05,
            0x08,
            0x0B,
            0x12,
            0x19,
            0x2F,
            0x2F,
            0x3C,
            0x3A,
            0x3C,
            0x2C,
        },
    [2] =
        {
            0x0E,
            0x0E,
            0x0C,
            0x0C,
            0x0A,
            0x09,
            0x08,
            0x08,
            0x08,
            0x08,
            0x07,
            0x05,
            0x04,
            0x02,
        },
    [3] =
        {
            0x01,
            0x05,
            0x12,
            0x1E,
            0x22,
            0x26,
            0x32,
            0x3A,
            0x39,
            0x51,
            0x40,
            0x4A,
            0x53,
            0x67,
        },
};

uint8_t gau8NIoaRegValue_TVI_1080p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x6F, 0x80, 0x80, 0x80, 0x80, 0x63, 0x80, 0xB7, 0xFF, 0xA4, 0xD5, 0xFF, 0xFF,
};

uint8_t gau8NIoaRegValue_TVI_1080p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
};

uint8_t gau8NIoaRegValue_TVI_1080p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xFF, 0xFF, 0xFF, 0xFF,
};

/********************gau32NICarRegValue_CVI_720p********************/
uint32_t gau32NICarRegValue_CVI_720p_OLD_LOSS[] = {
    0x3, 0x7, 0x8, 0x11, 0x18, 0x20, 0x34, 0x45, 0x54, 0x8e, 0xb9, 0x117, 0x1a1,
};

uint32_t gau32NICarRegValue_CVI_720p_NEW_LOSS[] = {
    0x3, 0x7, 0x8, 0x11, 0x18, 0x20, 0x34, 0x45, 0x54, 0x8e, 0xb9, 0x117, 0x1a1,
};

uint32_t gau8NICarRegValue_CVI_720p_SYNC[] = {
    0xb5, 0xb3, 0xb2, 0xaf, 0xad, 0xa9, 0xa6, 0xa2, 0xa0, 0x9b, 0x98, 0x93, 0x91,
};

uint16_t gau16NICarRegValue_CVI_720p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x1,
            0x2,
            0x2,
            0x3,
            0x3,
            0x4,
            0x5,
            0x6,
            0x7,
            0x7,
            0x7,
            0x7,
            0x7,
        },
    [1] =
        {
            0x1,
            0x2,
            0x2,
            0x2,
            0x8,
            0xc,
            0xa,
            0xa,
            0xc,
            0xc,
            0xc,
            0x14,
            0x14,
        },
    [2] =
        {
            0x0,
            0xe,
            0xe,
            0xe,
            0xc,
            0xb,
            0x7,
            0x6,
            0x5,
            0x3,
            0x2,
            0x3,
            0x3,
        },
    [3] =
        {
            0x1,
            0x3,
            0x5,
            0x12,
            0x15,
            0x26,
            0x36,
            0x43,
            0x4b,
            0x65,
            0x52,
            0x70,
            0x75,
        },
};

uint8_t gau8NICarRegValue_CVI_720p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NICarRegValue_CVI_720p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NICarRegValue_CVI_720p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NICarRegValue_CVI_1080p********************/
uint32_t gau32NICarRegValue_CVI_1080p_OLD_LOSS[] = {
    0x2, 0x4, 0x5, 0x6, 0x7, 0xb, 0xc, 0x16, 0x19, 0x1e, 0x26, 0x2a,
};

uint32_t gau32NICarRegValue_CVI_1080p_NEW_LOSS[] = {
    0x2, 0x4, 0x5, 0x6, 0x7, 0xb, 0xc, 0x16, 0x19, 0x1e, 0x26, 0x2a,
};

uint32_t gau8NICarRegValue_CVI_1080p_SYNC[] = {
    0xaa, 0xa9, 0xa8, 0xa7, 0xa5, 0xa3, 0xa2, 0xa0, 0x9f, 0x9b, 0x9a, 0x98,
};

uint16_t gau16NICarRegValue_CVI_1080p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x0,
            0x1,
            0x1,
            0x2,
            0x2,
            0x3,
            0x3,
            0x3,
            0x3,
            0x5,
            0x5,
            0x5,
        },
    [1] =
        {
            0x0,
            0x1,
            0x1,
            0x2,
            0x8,
            0xe,
            0x10,
            0xa,
            0x7,
            0x16,
            0x13,
            0x16,
        },
    [2] =
        {
            0x0,
            0xe,
            0xe,
            0xe,
            0xe,
            0xe,
            0xd,
            0xc,
            0xc,
            0x7,
            0x7,
            0x7,
        },
    [3] =
        {
            0x0,
            0x1,
            0x8,
            0x15,
            0x1a,
            0x23,
            0x25,
            0x30,
            0x35,
            0x55,
            0x5a,
            0x6e,
        },
};

uint8_t gau8NICarRegValue_CVI_1080p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NICarRegValue_CVI_1080p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NICarRegValue_CVI_1080p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NICarRegValue_AHD_720p********************/
uint32_t gau32NICarRegValue_AHD_720p_OLD_LOSS[] = {
    0X3, 0x2, 0x4, 0x5, 0x5, 0x8, 0x6, 0xc, 0xc, 0x11, 0x14, 0x15,
};

uint32_t gau32NICarRegValue_AHD_720p_NEW_LOSS[] = {
    0X3, 0x2, 0x4, 0x5, 0x5, 0x8, 0x6, 0xc, 0xc, 0x11, 0x14, 0x15,
};

uint32_t gau8NICarRegValue_AHD_720p_SYNC[] = {
    0xc0, 0xbf, 0xbf, 0xbe, 0xbd, 0xb9, 0xb6, 0xb3, 0xb1, 0xac, 0xab, 0xa8,
};

uint16_t gau16NICarRegValue_AHD_720p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x1,
            0x1,
            0x1,
            0x2,
            0x2,
            0x2,
            0x2,
            0x3,
            0x3,
            0x3,
            0x3,
            0x3,
        },
    [1] =
        {
            0x1,
            0x1,
            0x4,
            0x1a,
            0x15,
            0x20,
            0x18,
            0x2a,
            0x1a,
            0x30,
            0x35,
            0x40,
        },
    [2] =
        {
            0xe,
            0xe,
            0xe,
            0xd,
            0xc,
            0xe,
            0xe,
            0xe,
            0xe,
            0xd,
            0xc,
            0xc,
        },
    [3] =
        {
            0x0,
            0x8,
            0xa,
            0x12,
            0x18,
            0x20,
            0x2b,
            0x2a,
            0x30,
            0x45,
            0x45,
            0x54,
        },
};

uint8_t gau8NICarRegValue_AHD_720p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NICarRegValue_AHD_720p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NICarRegValue_AHD_720p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NICarRegValue_AHD_1080p********************/
uint32_t gau32NICarRegValue_AHD_1080p_OLD_LOSS[] = {
    0x2, 0x3, 0x3, 0x6, 0x7, 0xa, 0xb, 0x10, 0x13, 0x1e, 0x25, 0x2f, 0x36,
};

uint32_t gau32NICarRegValue_AHD_1080p_NEW_LOSS[] = {
    0x2, 0x3, 0x3, 0x6, 0x7, 0xa, 0xb, 0x10, 0x13, 0x1e, 0x25, 0x2f, 0x36,
};

uint32_t gau8NICarRegValue_AHD_1080p_SYNC[] = {
    0xb5, 0xb2, 0xb1, 0xac, 0xaa, 0xa7, 0xa5, 0xa0, 0x9e, 0x98, 0x95, 0x8f, 0x8d,
};

uint16_t gau16NICarRegValue_AHD_1080p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x0,
            0x1,
            0x1,
            0x2,
            0x2,
            0x2,
            0x3,
            0x3,
            0x3,
            0x4,
            0x4,
            0x5,
            0x5,
        },
    [1] =
        {
            0x0,
            0x1,
            0x6,
            0x8,
            0x8,
            0xc,
            0xc,
            0xe,
            0xf,
            0x13,
            0x14,
            0x16,
            0x16,
        },
    [2] =
        {
            0x0,
            0xe,
            0xe,
            0xe,
            0xe,
            0xe,
            0xe,
            0xd,
            0xc,
            0xb,
            0xb,
            0x7,
            0x5,
        },
    [3] =
        {
            0x0,
            0x8,
            0xc,
            0x12,
            0x17,
            0x25,
            0x25,
            0x2e,
            0x32,
            0x40,
            0x42,
            0x5e,
            0xe9,
        },
};

uint8_t gau8NICarRegValue_AHD_1080p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NICarRegValue_AHD_1080p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NICarRegValue_AHD_1080p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NICarRegValue_TVI_720p********************/
uint32_t gau32NICarRegValue_TVI_720p_OLD_LOSS[] = {
    0x1, 0x3, 0x3, 0x5, 0x6, 0xb, 0xd, 0x14, 0x16, 0x25, 0x31, 0x59,
};

uint32_t gau32NICarRegValue_TVI_720p_NEW_LOSS[] = {
    0x1, 0x3, 0x3, 0x5, 0x6, 0xb, 0xd, 0x14, 0x16, 0x25, 0x31, 0x59,
};

uint32_t gau8NICarRegValue_TVI_720p_SYNC[] = {
    0xa0, 0xa0, 0x9f, 0xa0, 0x9f, 0x9f, 0xa0, 0x9f, 0x9f, 0xa0, 0xa0, 0x9f,
};

uint16_t gau16NICarRegValue_TVI_720p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x0,
            0x2,
            0x2,
            0x2,
            0x2,
            0x3,
            0x3,
            0x3,
            0x3,
            0x5,
            0x5,
            0x6,
        },
    [1] =
        {
            0x0,
            0x2,
            0x2,
            0x2,
            0x2,
            0x9,
            0x9,
            0xa,
            0xb,
            0x8,
            0xa,
            0x12,
        },
    [2] =
        {
            0x0,
            0xe,
            0xe,
            0xe,
            0xe,
            0xd,
            0xd,
            0xc,
            0xc,
            0x7,
            0x7,
            0x6,
        },
    [3] =
        {
            0x0,
            0x5,
            0xc,
            0x15,
            0x18,
            0x22,
            0x25,
            0x30,
            0x34,
            0x55,
            0x5b,
            0x6b,
        },
};

uint8_t gau8NICarRegValue_TVI_720p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NICarRegValue_TVI_720p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NICarRegValue_TVI_720p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NICarRegValue_TVI_1080p********************/
uint32_t gau32NICarRegValue_TVI_1080p_OLD_LOSS[] = {
    0xc, 0xc, 0x12, 0x25, 0x2b, 0x49, 0x60, 0xde, 0x96, 0x132, 0x169, 0x263, 0x29f,
};

uint32_t gau32NICarRegValue_TVI_1080p_NEW_LOSS[] = {
    0xc, 0xc, 0x12, 0x25, 0x2b, 0x49, 0x60, 0xde, 0x96, 0x132, 0x169, 0x263, 0x29f,
};

uint32_t gau8NICarRegValue_TVI_1080p_SYNC[] = {
    0xbc, 0xb5, 0xb0, 0xa6, 0xa1, 0x99, 0x96, 0x91, 0x8e, 0x89, 0x88, 0x83, 0x7d,
};

uint16_t gau16NICarRegValue_TVI_1080p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x2,
            0x3,
            0x3,
            0x5,
            0x5,
            0x7,
            0x7,
            0x8,
            0x8,
            0x7,
            0x7,
            0x7,
            0x7,
        },
    [1] =
        {
            0x0,
            0x0,
            0x3,
            0x6,
            0x4,
            0x8,
            0x9,
            0xa,
            0xc,
            0xb,
            0xb,
            0xe,
            0xc,
        },
    [2] =
        {
            0xe,
            0xe,
            0xc,
            0x7,
            0x7,
            0x5,
            0x5,
            0x4,
            0x4,
            0x3,
            0x3,
            0x3,
            0x3,
        },
    [3] =
        {
            0x5,
            0x8,
            0xf,
            0x1f,
            0x75,
            0x5c,
            0x65,
            0x6b,
            0x70,
            0x68,
            0x70,
            0x77,
            0x75,
        },
};

uint8_t gau8NICarRegValue_TVI_1080p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NICarRegValue_TVI_1080p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NICarRegValue_TVI_1080p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NIBMW4PRegValue_CVI_720p********************/
uint32_t gau32NIBMW4PRegValue_CVI_720p_NEW_LOSS[] = {
    0x0000, 0x0001, 0x0003, 0x0008, 0x001d, 0x0030,
};

uint32_t gau8NIBMW4PRegValue_CVI_720p_SYNC[] = {
    0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA,
};

uint16_t gau16NIBMW4PRegValue_CVI_720p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00,
            0x00,
            0x01,
            0x02,
            0x0f,
            0x0f,
        },
    [1] =
        {
            0x00,
            0x00,
            0x01,
            0x02,
            0x0f,
            0x12,
        },
    [2] =
        {
            0x00,
            0x0e,
            0x0e,
            0x0e,
            0x0c,
            0x0c,
        },
    [3] =
        {
            0x00,
            0x04,
            0x10,
            0x20,
            0x30,
            0x4e,
        },
};

uint8_t gau8NIBMW4PRegValue_CVI_720p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NIBMW4PRegValue_CVI_720p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NIBMW4PRegValue_CVI_720p_MVAL[NI_CABLE_LEN_MAX_LVL] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80};

/********************gau32NIBMW4PRegValue_CVI_1080p********************/
uint32_t gau32NIBMW4PRegValue_CVI_1080p_NEW_LOSS[] = {
    0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA,
};

uint32_t gau8NIBMW4PRegValue_CVI_1080p_SYNC[] = {
    0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA,
};

uint16_t gau16NIBMW4PRegValue_CVI_1080p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00,
            0x00,
            0x01,
            0x0f,
            0x0f,
            0x0f,
        },
    [1] =
        {
            0x00,
            0x01,
            0x04,
            0x04,
            0x08,
            0x0c,
        },
    [2] =
        {
            0x0e,
            0x0e,
            0x0e,
            0x0a,
            0x04,
            0x03,
        },
    [3] =
        {
            0x00,
            0x05,
            0x0f,
            0x28,
            0x52,
            0x70,
        },
};

uint8_t gau8NIBMW4PRegValue_CVI_1080p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NIBMW4PRegValue_CVI_1080p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NIBMW4PRegValue_CVI_1080p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NIBMW4PRegValue_AHD_720p********************/
uint32_t gau32NIBMW4PRegValue_AHD_720p_NEW_LOSS[] = {
    0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x000e,
};

uint32_t gau8NIBMW4PRegValue_AHD_720p_SYNC[] = {
    0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA,
};

uint16_t gau16NIBMW4PRegValue_AHD_720p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00,
            0x00,
            0x02,
            0x02,
            0x02,
            0x0f,
        },
    [1] =
        {
            0x00,
            0x00,
            0x09,
            0x0e,
            0x10,
            0x18,
        },
    [2] =
        {
            0x0e,
            0x0e,
            0x0d,
            0x0d,
            0x0c,
            0x0c,
        },
    [3] =
        {
            0x00,
            0x02,
            0x0e,
            0x24,
            0x36,
            0x46,
        },
};

uint8_t gau8NIBMW4PRegValue_AHD_720p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NIBMW4PRegValue_AHD_720p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NIBMW4PRegValue_AHD_720p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NIBMW4PRegValue_AHD_1080p********************/
uint32_t gau32NIBMW4PRegValue_AHD_1080p_NEW_LOSS[] = {
    0x0001, 0x0002, 0x0004, 0x000d, 0x0037, 0x008b,
};

uint32_t gau8NIBMW4PRegValue_AHD_1080p_SYNC[] = {
    0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA,
};

uint16_t gau16NIBMW4PRegValue_AHD_1080p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00,
            0x00,
            0x00,
            0x02,
            0x07,
            0x07,
        },
    [1] =
        {
            0x00,
            0x02,
            0x04,
            0x09,
            0x0f,
            0x12,
        },
    [2] =
        {
            0x0e,
            0x0e,
            0x0d,
            0x08,
            0x06,
            0x06,
        },
    [3] =
        {
            0x00,
            0x04,
            0x10,
            0x2d,
            0x46,
            0x5c,
        },
};

uint8_t gau8NIBMW4PRegValue_AHD_1080p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NIBMW4PRegValue_AHD_1080p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NIBMW4PRegValue_AHD_1080p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NIBMW4PRegValue_TVI_720p********************/
uint32_t gau32NIBMW4PRegValue_TVI_720p_OLD_LOSS[] = {
    0x0000, 0x0001, 0x0003, 0x0008, 0x001d, 0x0030,
};

uint32_t gau32NIBMW4PRegValue_TVI_720p_NEW_LOSS[] = {
    0x0000, 0x0001, 0x0003, 0x0008, 0x001d, 0x0030,
};

uint32_t gau8NIBMW4PRegValue_TVI_720p_SYNC[] = {
    0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA,
};

uint16_t gau16NIBMW4PRegValue_TVI_720p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00,
            0x00,
            0x01,
            0x02,
            0x0f,
            0x0f,
        },
    [1] =
        {
            0x00,
            0x00,
            0x01,
            0x02,
            0x0f,
            0x12,
        },
    [2] =
        {
            0x00,
            0x0e,
            0x0e,
            0x0e,
            0x0c,
            0x0c,
        },
    [3] =
        {
            0x00,
            0x04,
            0x10,
            0x20,
            0x30,
            0x4e,
        },
};

uint8_t gau8NIBMW4PRegValue_TVI_720p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NIBMW4PRegValue_TVI_720p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NIBMW4PRegValue_TVI_720p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

/********************gau32NIBMW4PRegValue_TVI_1080p********************/
uint32_t gau32NIBMW4PRegValue_TVI_1080p_NEW_LOSS[] = {
    0x0008, 0x000b, 0x0020, 0x00f4, 0x01c3, 0x0219,
};

uint32_t gau8NIBMW4PRegValue_TVI_1080p_SYNC[] = {
    0xd9, 0xd9, 0xd9, 0xd9, 0xd9, 0xd9,
};

uint16_t gau16NIBMW4PRegValue_TVI_1080p_EQ[NI_EQ_REG_NUM][NI_CABLE_LEN_MAX_LVL] = {
    [0] =
        {
            0x00,
            0x00,
            0x02,
            0x02,
            0x09,
            0x0f,
        },
    [1] =
        {
            0x00,
            0x00,
            0x04,
            0x09,
            0x0f,
            0x13,
        },
    [2] =
        {
            0x0e,
            0x0e,
            0x0e,
            0x0d,
            0x0c,
            0x08,
        },
    [3] =
        {
            0x00,
            0x04,
            0x0f,
            0x26,
            0x3e,
            0x56,
        },
};

uint8_t gau8NIBMW4PRegValue_TVI_1080p_SAT[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

uint8_t gau8NIBMW4PRegValue_TVI_1080p_MCC[NI_CABLE_LEN_MAX_LVL] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t gau8NIBMW4PRegValue_TVI_1080p_MVAL[NI_CABLE_LEN_MAX_LVL] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

void* gauImageMapToEq[NI_CABLE_TYPE_BUTT][NI_VIDEO_CLASS_BUTT][NI_EQ_TABLE_BUTT] = {
    {
        [NI_VIDEO_CLASS_CVI_720] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_CVI_720p_NEW_LOSS),
                gau32NIoaRegValue_CVI_720p_NEW_LOSS,
                gau16NIoaRegValue_CVI_720p_EQ,
                gau8NIoaRegValue_CVI_720p_MCC,
                gau8NIoaRegValue_CVI_720p_MVAL,
                gau8NIoaRegValue_CVI_720p_SAT,
            },
        [NI_VIDEO_CLASS_CVI_1080] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_CVI_1080p_NEW_LOSS),
                gau32NIoaRegValue_CVI_1080p_NEW_LOSS,
                gau16NIoaRegValue_CVI_1080p_EQ,
                gau8NIoaRegValue_CVI_1080p_MCC,
                gau8NIoaRegValue_CVI_1080p_MVAL,
                gau8NIoaRegValue_CVI_1080p_SAT,
            },
        [NI_VIDEO_CLASS_AHD_720] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_AHD_720p_NEW_LOSS),
                gau32NIoaRegValue_AHD_720p_NEW_LOSS,
                gau16NIoaRegValue_AHD_720p_EQ,
                gau8NIoaRegValue_AHD_720p_MCC,
                gau8NIoaRegValue_AHD_720p_MVAL,
                gau8NIoaRegValue_AHD_720p_SAT,
            },
        [NI_VIDEO_CLASS_AHD_1080] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_AHD_1080p_NEW_LOSS),
                gau32NIoaRegValue_AHD_1080p_NEW_LOSS,
                gau16NIoaRegValue_AHD_1080p_EQ,
                gau8NIoaRegValue_AHD_1080p_MCC,
                gau8NIoaRegValue_AHD_1080p_MVAL,
                gau8NIoaRegValue_AHD_1080p_SAT,
            },
        [NI_VIDEO_CLASS_TVI_720] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_TVI_720p_NEW_LOSS),
                gau32NIoaRegValue_TVI_720p_NEW_LOSS,
                gau16NIoaRegValue_TVI_720p_EQ,
                gau8NIoaRegValue_TVI_720p_MCC,
                gau8NIoaRegValue_TVI_720p_MVAL,
                gau8NIoaRegValue_TVI_720p_SAT,
            },
        [NI_VIDEO_CLASS_TVI_1080] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_TVI_1080p_NEW_LOSS),
                gau32NIoaRegValue_TVI_1080p_NEW_LOSS,
                gau16NIoaRegValue_TVI_1080p_EQ,
                gau8NIoaRegValue_TVI_1080p_MCC,
                gau8NIoaRegValue_TVI_1080p_MVAL,
                gau8NIoaRegValue_TVI_1080p_SAT,
            },
    },
    {
        [NI_VIDEO_CLASS_CVI_720] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_CVI_720p_NEW_LOSS),
                gau32NIoaRegValue_CVI_720p_NEW_LOSS,
                gau16NIoaRegValue_CVI_720p_EQ,
                gau8NIoaRegValue_CVI_720p_MCC,
                gau8NIoaRegValue_CVI_720p_MVAL,
                gau8NIoaRegValue_CVI_720p_SAT,
            },
        [NI_VIDEO_CLASS_CVI_1080] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_CVI_1080p_NEW_LOSS),
                gau32NIoaRegValue_CVI_1080p_NEW_LOSS,
                gau16NIoaRegValue_CVI_1080p_EQ,
                gau8NIoaRegValue_CVI_1080p_MCC,
                gau8NIoaRegValue_CVI_1080p_MVAL,
                gau8NIoaRegValue_CVI_1080p_SAT,
            },
        [NI_VIDEO_CLASS_AHD_720] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_AHD_720p_NEW_LOSS),
                gau32NIoaRegValue_AHD_720p_NEW_LOSS,
                gau16NIoaRegValue_AHD_720p_EQ,
                gau8NIoaRegValue_AHD_720p_MCC,
                gau8NIoaRegValue_AHD_720p_MVAL,
                gau8NIoaRegValue_AHD_720p_SAT,
            },
        [NI_VIDEO_CLASS_AHD_1080] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_AHD_1080p_NEW_LOSS),
                gau32NIoaRegValue_AHD_1080p_NEW_LOSS,
                gau16NIoaRegValue_AHD_1080p_EQ,
                gau8NIoaRegValue_AHD_1080p_MCC,
                gau8NIoaRegValue_AHD_1080p_MVAL,
                gau8NIoaRegValue_AHD_1080p_SAT,
            },
        [NI_VIDEO_CLASS_TVI_720] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_TVI_720p_NEW_LOSS),
                gau32NIoaRegValue_TVI_720p_NEW_LOSS,
                gau16NIoaRegValue_TVI_720p_EQ,
                gau8NIoaRegValue_TVI_720p_MCC,
                gau8NIoaRegValue_TVI_720p_MVAL,
                gau8NIoaRegValue_TVI_720p_SAT,
            },
        [NI_VIDEO_CLASS_TVI_1080] =
            {
                (void*)ARRAYSIZE(gau32NIoaRegValue_TVI_1080p_NEW_LOSS),
                gau32NIoaRegValue_TVI_1080p_NEW_LOSS,
                gau16NIoaRegValue_TVI_1080p_EQ,
                gau8NIoaRegValue_TVI_1080p_MCC,
                gau8NIoaRegValue_TVI_1080p_MVAL,
                gau8NIoaRegValue_TVI_1080p_SAT,
            },
    },
    {
        [NI_VIDEO_CLASS_CVI_720] =
            {
                (void*)ARRAYSIZE(gau32NICarRegValue_CVI_720p_NEW_LOSS),
                gau32NICarRegValue_CVI_720p_NEW_LOSS,
                gau16NICarRegValue_CVI_720p_EQ,
                gau8NICarRegValue_CVI_720p_MCC,
                gau8NICarRegValue_CVI_720p_MVAL,
                gau8NICarRegValue_CVI_720p_SAT,
            },
        [NI_VIDEO_CLASS_CVI_1080] =
            {
                (void*)ARRAYSIZE(gau32NICarRegValue_CVI_1080p_NEW_LOSS),
                gau32NICarRegValue_CVI_1080p_NEW_LOSS,
                gau16NICarRegValue_CVI_1080p_EQ,
                gau8NICarRegValue_CVI_1080p_MCC,
                gau8NICarRegValue_CVI_1080p_MVAL,
                gau8NICarRegValue_CVI_1080p_SAT,
            },
        [NI_VIDEO_CLASS_AHD_720] =
            {
                (void*)ARRAYSIZE(gau32NICarRegValue_AHD_720p_NEW_LOSS),
                gau32NICarRegValue_AHD_720p_NEW_LOSS,
                gau16NICarRegValue_AHD_720p_EQ,
                gau8NICarRegValue_AHD_720p_MCC,
                gau8NICarRegValue_AHD_720p_MVAL,
                gau8NICarRegValue_AHD_720p_SAT,
            },
        [NI_VIDEO_CLASS_AHD_1080] =
            {
                (void*)ARRAYSIZE(gau32NICarRegValue_AHD_1080p_NEW_LOSS),
                gau32NICarRegValue_AHD_1080p_NEW_LOSS,
                gau16NICarRegValue_AHD_1080p_EQ,
                gau8NICarRegValue_AHD_1080p_MCC,
                gau8NICarRegValue_AHD_1080p_MVAL,
                gau8NICarRegValue_AHD_1080p_SAT,
            },
        [NI_VIDEO_CLASS_TVI_720] =
            {
                (void*)ARRAYSIZE(gau32NICarRegValue_TVI_720p_NEW_LOSS),
                gau32NICarRegValue_TVI_720p_NEW_LOSS,
                gau16NICarRegValue_TVI_720p_EQ,
                gau8NICarRegValue_TVI_720p_MCC,
                gau8NICarRegValue_TVI_720p_MVAL,
                gau8NICarRegValue_TVI_720p_SAT,
            },
        [NI_VIDEO_CLASS_TVI_1080] =
            {
                (void*)ARRAYSIZE(gau32NICarRegValue_TVI_1080p_NEW_LOSS),
                gau32NICarRegValue_TVI_1080p_NEW_LOSS,
                gau16NICarRegValue_TVI_1080p_EQ,
                gau8NICarRegValue_TVI_1080p_MCC,
                gau8NICarRegValue_TVI_1080p_MVAL,
                gau8NICarRegValue_TVI_1080p_SAT,
            },
    },
    {
        [NI_VIDEO_CLASS_CVI_720] =
            {
                (void*)ARRAYSIZE(gau32NIBMW4PRegValue_CVI_720p_NEW_LOSS),
                gau32NIBMW4PRegValue_CVI_720p_NEW_LOSS,
                gau16NIBMW4PRegValue_CVI_720p_EQ,
                gau8NIBMW4PRegValue_CVI_720p_MCC,
                gau8NIBMW4PRegValue_CVI_720p_MVAL,
                gau8NIBMW4PRegValue_CVI_720p_SAT,
            },
        [NI_VIDEO_CLASS_CVI_1080] =
            {
                (void*)ARRAYSIZE(gau32NIBMW4PRegValue_CVI_1080p_NEW_LOSS),
                gau32NIBMW4PRegValue_CVI_1080p_NEW_LOSS,
                gau16NIBMW4PRegValue_CVI_1080p_EQ,
                gau8NIBMW4PRegValue_CVI_1080p_MCC,
                gau8NIBMW4PRegValue_CVI_1080p_MVAL,
                gau8NIBMW4PRegValue_CVI_1080p_SAT,
            },
        [NI_VIDEO_CLASS_AHD_720] =
            {
                (void*)ARRAYSIZE(gau32NIBMW4PRegValue_AHD_720p_NEW_LOSS),
                gau32NIBMW4PRegValue_AHD_720p_NEW_LOSS,
                gau16NIBMW4PRegValue_AHD_720p_EQ,
                gau8NIBMW4PRegValue_AHD_720p_MCC,
                gau8NIBMW4PRegValue_AHD_720p_MVAL,
                gau8NIBMW4PRegValue_AHD_720p_SAT,
            },
        [NI_VIDEO_CLASS_AHD_1080] =
            {
                (void*)ARRAYSIZE(gau32NIBMW4PRegValue_AHD_1080p_NEW_LOSS),
                gau32NIBMW4PRegValue_AHD_1080p_NEW_LOSS,
                gau16NIBMW4PRegValue_AHD_1080p_EQ,
                gau8NIBMW4PRegValue_AHD_1080p_MCC,
                gau8NIBMW4PRegValue_AHD_1080p_MVAL,
                gau8NIBMW4PRegValue_AHD_1080p_SAT,
            },
        [NI_VIDEO_CLASS_TVI_720] =
            {
                (void*)ARRAYSIZE(gau32NIBMW4PRegValue_TVI_720p_NEW_LOSS),
                gau32NIBMW4PRegValue_TVI_720p_NEW_LOSS,
                gau16NIBMW4PRegValue_TVI_720p_EQ,
                gau8NIBMW4PRegValue_TVI_720p_MCC,
                gau8NIBMW4PRegValue_TVI_720p_MVAL,
                gau8NIBMW4PRegValue_TVI_720p_SAT,
            },
        [NI_VIDEO_CLASS_TVI_1080] =
            {
                (void*)ARRAYSIZE(gau32NIBMW4PRegValue_TVI_1080p_NEW_LOSS),
                gau32NIBMW4PRegValue_TVI_1080p_NEW_LOSS,
                gau16NIBMW4PRegValue_TVI_1080p_EQ,
                gau8NIBMW4PRegValue_TVI_1080p_MCC,
                gau8NIBMW4PRegValue_TVI_1080p_MVAL,
                gau8NIBMW4PRegValue_TVI_1080p_SAT,
            },
    },
};

static uint16_t aiNIACModePreSetVal[4][14] = {
    [0] =
        {
            0x0f,
            0x0f,
            0x0f,
            0x0f,
            0x0f,
            0x0f,
            0x0f,
            0x0f,
            0x0f,
            0x0f,
            0x01,
            0x01,
            0x0f,
            0x0f,
        },
    [1] =
        {
            0x05,
            0x0a,
            0x18,
            0x1a,
            0x40,
            0x20,
            0x4a,
            0x20,
            0x7f,
            0x20,
            0x20,
            0x10,
            0x40,
            0x20,
        },
    [2] =
        {
            0x0e,
            0x0e,
            0x09,
            0x07,
            0x0e,
            0x0e,
            0x04,
            0x03,
            0x01,
            0x01,
            0x0f,
            0x0f,
            0x0f,
            0x0f,
        },
    [3] =
        {
            0x01,
            0x01,
            0x01,
            0x01,
            0x01,
            0x01,
            0x7e,
            0x7e,
            0xff,
            0xaa,
            0x7e,
            0x7e,
            0x7e,
            0x7e,
        },
};

static void NiSdkDevSysInit(I2CPortStruct* i2cPort) {
    NiSdkWriteByte(NI_4300_ADC_CFG0_CH0, 0x5);
    NiSdkWriteByte(NI_4300_ADC_CFG0_CH0, 0x15);
    SystemDelay(1);

    uint8_t readValue = 0;
    NiSdkReadByte(NI_4303_ADC_STA_CH0, &readValue);

    if (readValue == 0x2) {
        SystemDelay(1);
    }
    else {
        NiSdkWriteByte(NI_4300_ADC_CFG0_CH0, 0x5);
        NiSdkWriteByte(NI_4300_ADC_CFG0_CH0, 0x15);
        SystemDelay(1);
    }

    NiSdkWriteByte(NI_4080_IRQ_ENABLE, 0x07);
    NiSdkWriteByte(NI_4119_IRQ_IO_CTRL, 0x1);

    NiSdkWriteByte(NI_0803_SFT_RST, 0x00);

    NiSdkWriteByte(NI_4020_OTHER_SFT_RST, 0x00);

    NiSdkWriteByte(NI_080E_VDP_SFT_RST, 0x00);
    NiSdkWriteByte(NI_080E_VDP_SFT_RST, 0x20);
    NiSdkWriteByte(NI_080E_VDP_SFT_RST, 0x28);

    NiSdkWriteByte(NI_4020_OTHER_SFT_RST, 0x03);

    NiSdkWriteByte(NI_0803_SFT_RST, 0x1f);
}

static void NiSdkSetMipiPllClk(I2CPortStruct* i2cPort, uint16_t rate) {
    switch (rate) {
        case 300:
            NiSdkWriteByte(0x1003, 0x01);
            NiSdkWriteByte(0x1004, 0x16);

            break;
        case 400:
            NiSdkWriteByte(0x1003, 0x01);
            NiSdkWriteByte(0x1004, 0x1e);

            break;
        case 500:
            NiSdkWriteByte(0x1003, 0x01);
            NiSdkWriteByte(0x1004, 0x25);

            break;
        case 600:
            NiSdkWriteByte(0x1003, 0x29);
            NiSdkWriteByte(0x1004, 0x90);

            break;
        case 700:
            NiSdkWriteByte(0x1003, 0x29);
            NiSdkWriteByte(0x1004, 0xd3);

            break;
        case 800:
            NiSdkWriteByte(0x1003, 0x04);
            NiSdkWriteByte(0x1004, 0xed);

            break;
        case 1000:
            NiSdkWriteByte(0x1003, 0x01);
            NiSdkWriteByte(0x1004, 0x4a);

            break;
        case 1500:
            NiSdkWriteByte(0x1003, 0x01);
            NiSdkWriteByte(0x1004, 0x6f);

            break;
        default:
            NiSdkWriteByte(0x1003, 0x01);
            NiSdkWriteByte(0x1004, 0x6f);
            break;
    }
}

static void NiSdkSetMipiLaneFreq(I2CPortStruct* i2cPort, uint16_t rate) {
    switch (rate) {
        case 300:
            NiSdkWriteByte(0x1045, 0xc2);
            NiSdkWriteByte(0x1046, 0x7f);
            NiSdkWriteByte(0x1047, 0x18);
            NiSdkWriteByte(0x1048, 0x04);
            NiSdkWriteByte(0x1049, 0x48);
            NiSdkWriteByte(0x104a, 0x0c);
            NiSdkWriteByte(0x1050, 0x04);
            NiSdkWriteByte(0x1065, 0xc2);
            NiSdkWriteByte(0x1066, 0x7f);
            NiSdkWriteByte(0x1067, 0x06);
            NiSdkWriteByte(0x1068, 0x04);
            NiSdkWriteByte(0x1069, 0x48);
            NiSdkWriteByte(0x106a, 0x0c);
            NiSdkWriteByte(0x1070, 0x04);
            NiSdkWriteByte(0x1085, 0xc2);
            NiSdkWriteByte(0x1086, 0x7f);
            NiSdkWriteByte(0x1087, 0x06);
            NiSdkWriteByte(0x1088, 0x04);
            NiSdkWriteByte(0x1089, 0x48);
            NiSdkWriteByte(0x108a, 0x0c);
            NiSdkWriteByte(0x1090, 0x04);

            break;
        case 400:
            NiSdkWriteByte(0x1045, 0xc3);
            NiSdkWriteByte(0x1046, 0x7f);
            NiSdkWriteByte(0x1047, 0x1b);
            NiSdkWriteByte(0x1048, 0x08);
            NiSdkWriteByte(0x1049, 0x48);
            NiSdkWriteByte(0x104a, 0x08);
            NiSdkWriteByte(0x1050, 0x04);
            NiSdkWriteByte(0x1065, 0xc3);
            NiSdkWriteByte(0x1066, 0x7f);
            NiSdkWriteByte(0x1067, 0x07);
            NiSdkWriteByte(0x1068, 0x08);
            NiSdkWriteByte(0x1069, 0x48);
            NiSdkWriteByte(0x106a, 0x08);
            NiSdkWriteByte(0x1070, 0x04);
            NiSdkWriteByte(0x1085, 0xc3);
            NiSdkWriteByte(0x1086, 0x7f);
            NiSdkWriteByte(0x1087, 0x07);
            NiSdkWriteByte(0x1088, 0x08);
            NiSdkWriteByte(0x1089, 0x48);
            NiSdkWriteByte(0x108a, 0x08);
            NiSdkWriteByte(0x1090, 0x04);

            break;
        case 500:
            NiSdkWriteByte(0x1045, 0xc3);
            NiSdkWriteByte(0x1046, 0x7f);
            NiSdkWriteByte(0x1047, 0x1b);
            NiSdkWriteByte(0x1048, 0x08);
            NiSdkWriteByte(0x1049, 0x48);
            NiSdkWriteByte(0x104a, 0x08);
            NiSdkWriteByte(0x1050, 0x04);
            NiSdkWriteByte(0x1065, 0xc3);
            NiSdkWriteByte(0x1066, 0x7f);
            NiSdkWriteByte(0x1067, 0x07);
            NiSdkWriteByte(0x1068, 0x08);
            NiSdkWriteByte(0x1069, 0x48);
            NiSdkWriteByte(0x106a, 0x08);
            NiSdkWriteByte(0x1070, 0x04);
            NiSdkWriteByte(0x1085, 0xc3);
            NiSdkWriteByte(0x1086, 0x7f);
            NiSdkWriteByte(0x1087, 0x07);
            NiSdkWriteByte(0x1088, 0x08);
            NiSdkWriteByte(0x1089, 0x48);
            NiSdkWriteByte(0x108a, 0x08);
            NiSdkWriteByte(0x1090, 0x04);

            break;
        case 600:
            NiSdkWriteByte(0x1045, 0xc5);
            NiSdkWriteByte(0x1046, 0x7c);
            NiSdkWriteByte(0x1047, 0x1e);
            NiSdkWriteByte(0x1048, 0x30);
            NiSdkWriteByte(0x104a, 0x09);
            NiSdkWriteByte(0x1050, 0x04);
            NiSdkWriteByte(0x1065, 0xc5);
            NiSdkWriteByte(0x1066, 0x7c);
            NiSdkWriteByte(0x1067, 0x1e);
            NiSdkWriteByte(0x1068, 0x30);
            NiSdkWriteByte(0x106a, 0x09);
            NiSdkWriteByte(0x1070, 0x04);
            NiSdkWriteByte(0x1085, 0xc5);
            NiSdkWriteByte(0x1086, 0x7c);
            NiSdkWriteByte(0x1087, 0x1e);
            NiSdkWriteByte(0x1088, 0x30);
            NiSdkWriteByte(0x108a, 0x09);
            NiSdkWriteByte(0x1090, 0x04);

            break;
        case 700:
            NiSdkWriteByte(0x1045, 0xc5);
            NiSdkWriteByte(0x1046, 0x70);
            NiSdkWriteByte(0x1047, 0x1f);
            NiSdkWriteByte(0x1048, 0x30);
            NiSdkWriteByte(0x104a, 0x0a);
            NiSdkWriteByte(0x1050, 0x04);
            NiSdkWriteByte(0x1065, 0xc5);
            NiSdkWriteByte(0x1066, 0x70);
            NiSdkWriteByte(0x1067, 0x1f);
            NiSdkWriteByte(0x1068, 0x30);
            NiSdkWriteByte(0x106a, 0x0a);
            NiSdkWriteByte(0x1070, 0x04);
            NiSdkWriteByte(0x1085, 0xc5);
            NiSdkWriteByte(0x1086, 0x70);
            NiSdkWriteByte(0x1087, 0x1f);
            NiSdkWriteByte(0x1088, 0x30);
            NiSdkWriteByte(0x108a, 0x0a);
            NiSdkWriteByte(0x1090, 0x04);

            break;
        case 800:
            NiSdkWriteByte(0x1045, 0xc5);
            NiSdkWriteByte(0x1046, 0x70);
            NiSdkWriteByte(0x1047, 0x1f);
            NiSdkWriteByte(0x1048, 0x18);
            NiSdkWriteByte(0x1049, 0x4c);
            NiSdkWriteByte(0x104a, 0x0a);
            NiSdkWriteByte(0x1050, 0x04);
            NiSdkWriteByte(0x1065, 0xc5);
            NiSdkWriteByte(0x1066, 0x70);
            NiSdkWriteByte(0x1067, 0x09);
            NiSdkWriteByte(0x1068, 0x18);
            NiSdkWriteByte(0x1069, 0x4c);
            NiSdkWriteByte(0x106a, 0x0a);
            NiSdkWriteByte(0x1070, 0x04);
            NiSdkWriteByte(0x1085, 0xc5);
            NiSdkWriteByte(0x1086, 0x70);
            NiSdkWriteByte(0x1087, 0x09);
            NiSdkWriteByte(0x1088, 0x18);
            NiSdkWriteByte(0x1089, 0x4c);
            NiSdkWriteByte(0x108a, 0x0a);
            NiSdkWriteByte(0x1090, 0x04);

            break;
        case 1000:
            NiSdkWriteByte(0x1045, 0xc5);
            NiSdkWriteByte(0x1046, 0x40);
            NiSdkWriteByte(0x1047, 0x20);
            NiSdkWriteByte(0x1048, 0x30);
            NiSdkWriteByte(0x1049, 0x4f);
            NiSdkWriteByte(0x104a, 0x0a);
            NiSdkWriteByte(0x1050, 0x04);
            NiSdkWriteByte(0x1065, 0xc5);
            NiSdkWriteByte(0x1066, 0x40);
            NiSdkWriteByte(0x1067, 0x09);
            NiSdkWriteByte(0x1068, 0x30);
            NiSdkWriteByte(0x1069, 0x4f);
            NiSdkWriteByte(0x106a, 0x0a);
            NiSdkWriteByte(0x1070, 0x04);
            NiSdkWriteByte(0x1085, 0xc5);
            NiSdkWriteByte(0x1086, 0x40);
            NiSdkWriteByte(0x1087, 0x09);
            NiSdkWriteByte(0x1088, 0x30);
            NiSdkWriteByte(0x1089, 0x4f);
            NiSdkWriteByte(0x108a, 0x0a);
            NiSdkWriteByte(0x1090, 0x04);
            break;

        case 1500:
        default:
            NiSdkWriteByte(0x1045, 0xcd);
            NiSdkWriteByte(0x1046, 0x0c);
            NiSdkWriteByte(0x1047, 0x36);
            NiSdkWriteByte(0x1048, 0x0f);
            NiSdkWriteByte(0x1049, 0x59);
            NiSdkWriteByte(0x104a, 0x07);
            NiSdkWriteByte(0x1050, 0xc4);
            NiSdkWriteByte(0x1065, 0xcd);
            NiSdkWriteByte(0x1066, 0x0c);
            NiSdkWriteByte(0x1067, 0x0e);
            NiSdkWriteByte(0x1068, 0x0f);
            NiSdkWriteByte(0x1069, 0x59);
            NiSdkWriteByte(0x106a, 0x07);
            NiSdkWriteByte(0x1070, 0xc4);
            NiSdkWriteByte(0x1085, 0xcd);
            NiSdkWriteByte(0x1086, 0x0c);
            NiSdkWriteByte(0x1087, 0x0e);
            NiSdkWriteByte(0x1088, 0x0f);
            NiSdkWriteByte(0x1089, 0x59);
            NiSdkWriteByte(0x108a, 0x07);
            NiSdkWriteByte(0x1090, 0xc4);
            break;
    }
}

static void NiSdkMipiDevInit(I2CPortStruct* i2cPort, uint8_t lanes, uint16_t rate) {
    NiSdkMipiWriteReg(0xFC, 0xD59);

    NiSdkMipiWriteReg(0xE4, lanes == 2 ? 0x0001 : 0x0000);

    NiSdkMipiWriteReg(0xF0, 0x32);

    NiSdkMipiWriteReg(0xF0, 0xf);
    NiSdkMipiWriteReg(0xE8, 0x1);
    NiSdkMipiWriteReg(0x48, 0x3ff);
    NiSdkMipiWriteReg(0x58, 0x7);
    NiSdkMipiWriteReg(0xe0, 0x0);
    NiSdkMipiWriteReg(0xe0, 0x4);
    NiSdkMipiWriteReg(0xe0, 0x7);

    NiSdkSetMipiPllClk(i2cPort, rate);

    NiSdkWriteByte(0x1001, 0xe4);
    NiSdkWriteByte(0x1000, 0x4d);
    NiSdkWriteByte(0x1001, 0xe0);
    SystemDelay(1);
    NiSdkWriteByte(0x1020, 0x1e);
    NiSdkWriteByte(0x1020, 0x1f);

    NiSdkSetMipiLaneFreq(i2cPort, rate);

    NiSdkWriteRegBit(0x1060, 4, 4, 0);
    NiSdkWriteRegBit(0x1080, 4, 4, 0);
    NiSdkWriteRegBit(0x1040, 4, 4, 0);

    //NiSdkMipiWriteReg(0x04, 0x1);

    NiSdkMipiWriteReg(0xE8, 0x1);
}

static void NiSdkHdFmtToReg(NI_VIDEO_FORMAT_E enVideoFmt, uint8_t* pu8RegValue) {
    switch (enVideoFmt) {
        case NI_CVI_1280x720_25HZ:
            *pu8RegValue = 0x0;

            break;
        case NI_CVI_1280x720_30HZ:
            *pu8RegValue = 0x1;

            break;
        case NI_CVI_1280x720_50HZ:
            *pu8RegValue = 0x2;

            break;
        case NI_CVI_1280x720_60HZ:
            *pu8RegValue = 0x3;

            break;
        case NI_CVI_1920x1080_25HZ:
            *pu8RegValue = 0x4;

            break;
        case NI_CVI_1920x1080_30HZ:
            *pu8RegValue = 0x5;

            break;
        case NI_CVI_1280x720_30HZ_V20:
            *pu8RegValue = 0x1a;

            break;
        case NI_CVI_1280x720_60HZ_V20:
            *pu8RegValue = 0x1b;

            break;
        case NI_CVI_1920x1080_30HZ_V20:
            *pu8RegValue = 0x1c;

            break;
        case NI_CVI_1920x1080_15HZ:
            *pu8RegValue = 0x1d;

            break;
        case NI_CVI_1280x960_25HZ:
            *pu8RegValue = 0x1e;

            break;
        case NI_CVI_1280x960_30HZ:
            *pu8RegValue = 0x1f;

            break;
        case NI_AHD_1280x720_25HZ:
            *pu8RegValue = 0x40;

            break;
        case NI_AHD_1280x720_30HZ:
            *pu8RegValue = 0x41;

            break;
        case NI_AHD_1280x720_50HZ:
            *pu8RegValue = 0x42;

            break;
        case NI_AHD_1280x720_60HZ:
            *pu8RegValue = 0x43;

            break;
        case NI_AHD_1920x1080_25HZ:
            *pu8RegValue = 0x44;

            break;
        case NI_AHD_1920x1080_30HZ:
            *pu8RegValue = 0x45;

            break;
        case NI_TVI_1280x720_25HZ:
            *pu8RegValue = 0x80;

            break;
        case NI_TVI_1280x720_30HZ:
            *pu8RegValue = 0x81;

            break;
        case NI_TVI3_1280x720_25HZ:
        case NI_TVI_1280x720_50HZ:
            *pu8RegValue = 0x82;

            break;
        case NI_TVI3_1280x720_30HZ:
        case NI_TVI_1280x720_60HZ:
            *pu8RegValue = 0x83;

            break;
        case NI_TVI_1920x1080_25HZ:
            *pu8RegValue = 0x84;

            break;
        case NI_TVI_1920x1080_30HZ:
            *pu8RegValue = 0x85;

            break;
        case NI_TVI_1920x1080_15HZ:
            *pu8RegValue = 0x8d;

            break;
        default:
            *pu8RegValue = 0xff;
    }
}

static void NiSdkHdRegToFmt(uint8_t u8RegValue, NI_VIDEO_FORMAT_E* penVideoFmt) {
    switch (u8RegValue) {
        case 0x0:
            *penVideoFmt = NI_CVI_1280x720_25HZ;

            break;
        case 0x1:
            *penVideoFmt = NI_CVI_1280x720_30HZ;

            break;
        case 0x2:
            *penVideoFmt = NI_CVI_1280x720_50HZ;

            break;
        case 0x3:
            *penVideoFmt = NI_CVI_1280x720_60HZ;

            break;
        case 0x1a:
            *penVideoFmt = NI_CVI_1280x720_30HZ_V20;

            break;
        case 0x1b:
            *penVideoFmt = NI_CVI_1280x720_60HZ_V20;

            break;
        case 0x1e:
            *penVideoFmt = NI_CVI_1280x960_25HZ;

            break;
        case 0x1f:
            *penVideoFmt = NI_CVI_1280x960_30HZ;

            break;
        case 0x4:
            *penVideoFmt = NI_CVI_1920x1080_25HZ;

            break;
        case 0x5:
            *penVideoFmt = NI_CVI_1920x1080_30HZ;

            break;
        case 0x1c:
            *penVideoFmt = NI_CVI_1920x1080_30HZ_V20;

            break;
        case 0x40:
            *penVideoFmt = NI_AHD_1280x720_25HZ;

            break;
        case 0x41:
            *penVideoFmt = NI_AHD_1280x720_30HZ;

            break;
        case 0x42:
            *penVideoFmt = NI_AHD_1280x720_50HZ;

            break;
        case 0x43:
            *penVideoFmt = NI_AHD_1280x720_60HZ;

            break;
        case 0x44:
            *penVideoFmt = NI_AHD_1920x1080_25HZ;

            break;
        case 0x45:
            *penVideoFmt = NI_AHD_1920x1080_30HZ;

            break;
        case 0x80:
            *penVideoFmt = NI_TVI_1280x720_25HZ;

            break;
        case 0x81:
            *penVideoFmt = NI_TVI_1280x720_30HZ;

            break;
        case 0x82:
            *penVideoFmt = NI_TVI_1280x720_50HZ;

            break;
        case 0x83:
            *penVideoFmt = NI_TVI_1280x720_60HZ;

            break;
        case 0x84:
            *penVideoFmt = NI_TVI_1920x1080_25HZ;

            break;
        case 0x85:
            *penVideoFmt = NI_TVI_1920x1080_30HZ;

            break;
        default:
            *penVideoFmt = NI_VIDEO_FMT_BUTT;
    }
}

static void NiSdkSdRegToFmt(uint8_t u8RegValue, NI_VIDEO_FORMAT_E* penVideoFmt) {
    switch (u8RegValue) {
        case 0:
            *penVideoFmt = NI_SD_NTSC_JM;

            break;
        case 1:
            *penVideoFmt = NI_SD_NTSC_443;

            break;
        case 2:
            *penVideoFmt = NI_SD_PAL_M;

            break;
        case 3:
            *penVideoFmt = NI_SD_PAL_60;

            break;
        case 4:
            *penVideoFmt = NI_SD_PAL_CN;

            break;
        case 5:
            *penVideoFmt = NI_SD_PAL_BGHID;

            break;
        default:
            *penVideoFmt = NI_VIDEO_FMT_BUTT;
    }
}

static void NiSdkSetVdpTestMode(I2CPortStruct* i2cPort, NI_VIDEO_FORMAT_E enVideoFmt) {
    uint32_t u32ActiveLine;
    uint32_t u32TotalLine;
    uint32_t u32ActivePixel;
    uint32_t u32TotalPixel;

    u32ActiveLine  = gstNiVdpTestModeAttr[enVideoFmt].u32ActiveLine;
    u32TotalLine   = gstNiVdpTestModeAttr[enVideoFmt].u32TotalLine;
    u32ActivePixel = gstNiVdpTestModeAttr[enVideoFmt].u32ActivePixel;
    u32TotalPixel  = gstNiVdpTestModeAttr[enVideoFmt].u32TotalPixel;

    NiSdkWriteByte(NI_015A_TEST_PATTERN_ACT_PIX_LSB, u32ActivePixel & 0xff);
    NiSdkWriteByte(NI_015B_TEST_PATTERN_ACT_PIX_MSB, (u32ActivePixel & 0x3f00) >> 8);
    NiSdkWriteByte(NI_015C_TEST_PATTERN_TOCAL_PIX_LSB, u32TotalPixel & 0xff);
    NiSdkWriteByte(NI_015D_TEST_PATTERN_TOCAL_PIX_MSB, (u32TotalPixel & 0x3f00) >> 8);
    NiSdkWriteByte(NI_015E_TEST_PATTERN_ACT_LINE_LSB, u32ActiveLine & 0xff);
    NiSdkWriteByte(NI_015F_TEST_PATTERN_ACT_LINE_MSB, (u32ActiveLine & 0x3f00) >> 8);
    NiSdkWriteByte(NI_0160_TEST_PATTERN_TOCAL_LINE_LSB, u32TotalLine & 0xff);
    NiSdkWriteByte(NI_0161_TEST_PATTERN_TOCAL_LINE_MSB, (u32TotalLine & 0x3f00) >> 8);

    if ((enVideoFmt >= NI_SD_NTSC_JM) && (enVideoFmt <= NI_SD_PAL_BGHID)) {
        NiSdkWriteByte(NI_0165_TEST_PATTERN_VSTD_LSB, 0xff);
        NiSdkWriteByte(NI_0166_TEST_PATTERN_VSTD_MSB, enVideoFmt - NI_SD_NTSC_JM);
    }
    else if ((enVideoFmt >= NI_CVI_1280x720_25HZ) && (enVideoFmt <= NI_TVI_1920x1080_15HZ)) {
        uint8_t u8RegValue = 0;
        NiSdkHdFmtToReg(enVideoFmt, &u8RegValue);
        NiSdkWriteByte(NI_0165_TEST_PATTERN_VSTD_LSB, u8RegValue);
        NiSdkWriteByte(NI_0166_TEST_PATTERN_VSTD_MSB, 0xf);
    }
}

static void NiSdkSetFreeRun(I2CPortStruct* i2cPort, bool isEnabled, uint8_t color, uint8_t format) {
    NiSdkWriteByte(NI_0504_VIDEO_OUT_MODE_SETTING, (color << 4) | 9);
    NiSdkWriteRegBit(NI_0507_VIDEO_OUT_CTRL, 0, 0, color >= 8 ? 1 : 0);
    NiSdkWriteRegBit(NI_0507_VIDEO_OUT_CTRL, 4, 4, 0);
    //NiSdkVideoFmtToFreeRunFmt(pstFreeRunAttr->enFreeRunFormat, &enFreeRunFmt);

    NiSdkWriteByte(NI_0503_VIDEO_OUT_MODE_SETTING_FREE_RUN, format);
    NiSdkWriteRegBit(NI_0502_VIDEO_OUT_MODE_SETTING_ID, 5, 6, isEnabled ? 0x3 : 0x0);

    NiSdkSetVdpTestMode(i2cPort, format); //pstFreeRunAttr->enFreeRunFormat);
}

static void NiSdkMipiChnSwitch(I2CPortStruct* i2cPort, bool isEnabled) {
    NiSdkWriteByte(NI_0E08_MIPI_EN, isEnabled);
}

static void NiSdkDevInit(I2CPortStruct* i2cPort) {

    NiSdkWriteByte(NI_0100_HD_AFE_CLAMP_TARGET, 0x35);
    NiSdkWriteByte(NI_0104_HD_MANUAL_CLAMP_OFFSET_SETTING, 0x48);

    NiSdkWriteByte(NI_0300_SD_AFE_CLAMP_TARGET, 0x3f);

    NiSdkWriteByte(NI_0105_HD_AGC_CONTROL_REGISTER, 0xe1);
    NiSdkWriteByte(NI_0101_HD_LUMINANCE_GAIN_MANUAL_CONTROL_REGISTER, 0x42);
    NiSdkWriteByte(NI_0102_HD_COLOR_GAIN_MANUAL_CONTROL_REGISTER, 0x40);
    NiSdkWriteByte(NI_0116_HD_SYNC_DEPTH_CONTROL_REGISTER, 0x3c);
    NiSdkWriteByte(NI_0117_HD_BURST_POWER_CONTROL_REGISTER, 0x23);

    NiSdkWriteByte(NI_0333_SD_BURST_POWER_CONTROL_REGISTER, 0x23);
    NiSdkWriteByte(NI_0336_SD_BURST_LOCK_COEFF, 0x9e);

    NiSdkWriteByte(NI_0337_SD_CTI_CONTROL_REGISTER_1, 0xd9);
    NiSdkWriteByte(NI_0338_SD_CTI_CONTROL_REGISTER_2, 0x0a);

    NiSdkWriteByte(NI_01BF_HD_NEW_SEL, 0x4e);

    NiSdkWriteByte(NI_010E_HD_YOUT_GAIN_CTRL, 0x78);
    NiSdkWriteByte(NI_010F_HD_UOUT_GAIN_CTRL, 0x92);
    NiSdkWriteByte(NI_0110_HD_VOUT_GAIN_CTRL, 0x70);
    NiSdkWriteByte(NI_0111_HD_Y_BLANK_LEVEL, 0x40);

    NiSdkWriteByte(NI_0314_SD_SYNC_LOCK_CONTROL0, 0x66);

    NiSdkWriteByte(NI_0130_TBC_AND_BACK_END_HSYNC_LOOP_CONTROL_REGISTER, 0x10);

    NiSdkWriteByte(NI_0315_SD_SYNC_LOCK_CONTROL1, 0x23);

    NiSdkWriteByte(NI_0B64_HD_STD_CVI_BL_WIN_MAN_MSB, 0x2);

    NiSdkWriteByte(NI_01E2_HD_SLICE_THR3_MSB, 0x03);

    NiSdkWriteByte(NI_0B55_HD_STD_FREQ_THR_LSB, 0x80);
    NiSdkWriteByte(NI_0B56_HD_STD_FREQ_THR_MSB, 0x0);
    NiSdkWriteByte(NI_0B59_HD_STD_OLD_VSTD, 0x4);
    NiSdkWriteByte(NI_0B5A_HD_STD_CHECK_CARRIER_FLAG, 0x1);
    NiSdkWriteByte(NI_0B5C_HD_STD_VSYNC_SLICE_BEG_MSB, 0x7);
    NiSdkWriteByte(NI_0B5E_HD_STD_VSYNC_SLICE_END_MSB, 0x5);

    NiSdkWriteByte(NI_0B4B_HD_STD_AHD_BL_WIN_MAN_LSB, 0x10);
    NiSdkWriteByte(NI_0B4E_HD_STD_TVI_THR_MSB, 0x5);

    NiSdkWriteByte(NI_0B51_HD_STD_MARK_LOCK_THR, 0x21);

    NiSdkWriteByte(NI_0B30_HD_SD_LINE_POINT_LSB, 0xBC);
    NiSdkWriteByte(NI_0B31_HD_SD_LINE_POINT_MSB, 0x19);

    NiSdkWriteByte(NI_0B15_MAX_SECOND_RAT, 0x3);
    NiSdkWriteByte(NI_0B16_MAX_LASTMAX_RAT, 0x3);
    NiSdkWriteByte(NI_0B17_MODE_CHOICE_RAT, 0x3);
    NiSdkWriteByte(NI_0B07_CONFIDENCE_INDEX_NUM, 0x3);
    NiSdkWriteByte(NI_0B08_CONFIDENCE_INDEX_NUM_MAX, 0x5);
    NiSdkWriteByte(NI_0B1A_JUDGE_MODE_OVERTIME, 0x10);

    NiSdkWriteByte(NI_0158_COMB_FILT_CTRL, 0x3);
    NiSdkWriteByte(NI_0A88_RX_PARA_CFG, 0x20);

    NiSdkWriteByte(NI_0A61_STD_HPF_COFF_00_LSB, 0x09);
    NiSdkWriteByte(NI_0A62_STD_HPF_COFF_00_MSB, 0x00);
    NiSdkWriteByte(NI_0A63_STD_HPF_COFF_01_LSB, 0x0e);
    NiSdkWriteByte(NI_0A64_STD_HPF_COFF_01_MSB, 0x00);
    NiSdkWriteByte(NI_0A65_STD_HPF_COFF_02_LSB, 0xfc);
    NiSdkWriteByte(NI_0A67_STD_HPF_COFF_03_LSB, 0xe5);
    NiSdkWriteByte(NI_0A69_STD_HPF_COFF_04_LSB, 0xef);
    NiSdkWriteByte(NI_0A6B_STD_HPF_COFF_05_LSB, 0x1b);
    NiSdkWriteByte(NI_0A6D_STD_HPF_COFF_06_LSB, 0x2f);
    NiSdkWriteByte(NI_0A6F_STD_HPF_COFF_07_LSB, 0x00);
    NiSdkWriteByte(NI_0A71_STD_HPF_COFF_08_LSB, 0xc2);
    NiSdkWriteByte(NI_0A72_STD_HPF_COFF_08_MSB, 0xff);
    NiSdkWriteByte(NI_0A73_STD_HPF_COFF_09_LSB, 0xd0);
    NiSdkWriteByte(NI_0A74_STD_HPF_COFF_09_MSB, 0xff);
    NiSdkWriteByte(NI_0A75_STD_HPF_COFF_10_LSB, 0x29);
    NiSdkWriteByte(NI_0A77_STD_HPF_COFF_11_LSB, 0x57);
    NiSdkWriteByte(NI_0A78_STD_HPF_COFF_11_MSB, 0x00);
    NiSdkWriteByte(NI_0A79_STD_HPF_COFF_12_LSB, 0x10);
    NiSdkWriteByte(NI_0A7A_STD_HPF_COFF_12_MSB, 0x00);
    NiSdkWriteByte(NI_0A7B_STD_HPF_COFF_13_LSB, 0xaa);
    NiSdkWriteByte(NI_0A7D_STD_HPF_COFF_14_LSB, 0xb2);
    NiSdkWriteByte(NI_0A7F_STD_HPF_COFF_15_LSB, 0x24);
    NiSdkWriteByte(NI_0A80_STD_HPF_COFF_15_MSB, 0x00);
    NiSdkWriteByte(NI_0A81_STD_HPF_COFF_16_LSB, 0x69);
    NiSdkWriteByte(NI_0A82_STD_HPF_COFF_16_MSB, 0x00);

    NiSdkWriteByte(NI_0802_VO_OUT_CLK_CTRL, 0x2);

    // pstNiCtx->astNiDevAttr[u8DevId].astVideoAttr.stVoAttr.enNetraMode = NI_NETRA_MODE_SINGLE;
    NiSdkWriteByte(NI_0501_VIDEO_OUT_CTRL_REG, 0x81);

    NiSdkWriteByte(NI_0502_VIDEO_OUT_MODE_SETTING_ID, 0x00);

    NiSdkWriteByte(NI_0B74_HD_STD_AHD_VB_MARK03, 0xFC);
    NiSdkWriteByte(NI_01DC_HD_HACC4_THR_MSB, 0x01);
    NiSdkWriteByte(NI_0804_ADC_CDC_CFG, 0x04);
    NiSdkWriteByte(NI_4018_SYS_CLK_ENABLE, 0x01);
    NiSdkWriteByte(NI_0B56_HD_STD_FREQ_THR_MSB, 0x1);
    NiSdkWriteByte(NI_0B73_HD_STD_AHD_VB_MARK02, 0x2);
    NiSdkWriteByte(NI_4210_AFE_BUF_CFG2_CH0, 0xC);
    NiSdkWriteByte(NI_420B_AFE_CLAMP_CFG0_CH0, 0x2F);

    // pstNiCtx->astNiDevAttr[u8DevId].astVideoAttr.stFreeRunAttr.enFreeRunColor  = NI_FREERUN_COLOR_BLUE;
    // pstNiCtx->astNiDevAttr[u8DevId].astVideoAttr.stFreeRunAttr.enFreeRunFormat = NI_CVI_1280x720_25HZ;
    // stFreeRunAttr                                                              = pstNiCtx->astNiDevAttr[u8DevId].astVideoAttr.stFreeRunAttr;

    NiSdkSetFreeRun(i2cPort, true, 8, 0); // NI_CVI_1280x720_25HZ

    NiSdkWriteByte(NI_4030_MIPI_CLK_CFG, 0x15);
    NiSdkWriteByte(NI_4134_VD0_DATA0_IO_CTRL0, 0xa);
    // if (NI_CHIP_PACK_BT == pstNiCtx->stRegInfo.enChipPack[u8DevId]) {

    //     NiSdkWriteByte( NI_0803_SFT_RST, 0xf);
    //     NiSdkWriteByte( NI_4412_MIPI_TTL_CFG, 0x1);
    //     SystemDelay(1);
    //     NiSdkWriteByte( NI_0803_SFT_RST, 0x1f);
    //     NiSdkWriteByte( NI_10E3_Mode_CFG, 0x04);
    //     NiSdkWriteByte( NI_10EB_Clock_Data_Lane_Enable, 0xfd);

    //     NiSdkWriteByte( NI_0800_CLK_GATE_EN, 0x7);
    // }
    // else if (NI_CHIP_PACK_MIPI == pstNiCtx->stRegInfo.enChipPack[u8DevId]) {

    NiSdkWriteByte(NI_0803_SFT_RST, 0xf);
    NiSdkWriteByte(NI_4412_MIPI_TTL_CFG, 0x0);
    SystemDelay(1);
    NiSdkWriteByte(NI_0803_SFT_RST, 0x1f);
    NiSdkWriteByte(NI_10E3_Mode_CFG, 0x01);
    NiSdkWriteByte(NI_10EB_Clock_Data_Lane_Enable, 0x04);

    NiSdkMipiDevInit(i2cPort, CONFIG_XS9950_LANES, CONFIG_XS9950_LANE_RATE);
    // NiSdkMipiDevReset(i2cPort);

    NiSdkMipiChnSwitch(i2cPort, true);
    NiSdkWriteByte(NI_0800_CLK_GATE_EN, 0x3);
    //}

    NiSdkWriteByte(NI_0805_HD_SD_EN, 0x7);

    SystemDelay(1);
}

static void NiSdkVideoFmtToVidelClass(NI_VIDEO_FORMAT_E enVideoFmt, NI_VIDEO_CLASS_E* penVideoClass) {
    switch (enVideoFmt) {
        case NI_CVI_1280x720_25HZ:
        case NI_CVI_1280x720_30HZ:
        case NI_CVI_1280x720_30HZ_V20:
            *penVideoClass = NI_VIDEO_CLASS_CVI_720;

            break;
        case NI_CVI_1280x720_50HZ:
        case NI_CVI_1280x720_60HZ:
        case NI_CVI_1280x720_60HZ_V20:
        case NI_CVI_1920x1080_25HZ:
        case NI_CVI_1920x1080_30HZ_V20:
            *penVideoClass = NI_VIDEO_CLASS_CVI_1080;

            break;
        case NI_AHD_1280x720_25HZ:
        case NI_AHD_1280x720_30HZ:
            *penVideoClass = NI_VIDEO_CLASS_AHD_720;

            break;
        case NI_AHD_1280x720_50HZ:
        case NI_AHD_1280x720_60HZ:
        case NI_AHD_1920x1080_25HZ:
        case NI_AHD_1920x1080_30HZ:
            *penVideoClass = NI_VIDEO_CLASS_AHD_1080;

            break;
        case NI_TVI_1280x720_25HZ:
        case NI_TVI_1280x720_30HZ:
            *penVideoClass = NI_VIDEO_CLASS_TVI_720;

            break;
        case NI_TVI3_1280x720_25HZ:
        case NI_TVI3_1280x720_30HZ:
        case NI_TVI_1280x720_50HZ:
        case NI_TVI_1280x720_60HZ:
        case NI_TVI_1920x1080_25HZ:
        case NI_TVI_1920x1080_30HZ:
            *penVideoClass = NI_VIDEO_CLASS_TVI_1080;

            break;
        default:
            *penVideoClass = NI_VIDEO_CLASS_CVI_720;
    }
}

static void NiSdkConfigEq(I2CPortStruct* i2cPort, NI_VIDEO_FORMAT_E enVideoFmt, uint8_t u8Index, uint8_t u8MaxLen) {

    uint16_t* pu16CableLenValue;

    if ((enVideoFmt >= NI_CVI_1280x720_25HZ) && (enVideoFmt <= NI_TVI_1920x1080_15HZ)) {
        NI_VIDEO_CLASS_E enVideoClass;
        NiSdkVideoFmtToVidelClass(enVideoFmt, &enVideoClass);

        NI_CABLE_TYPE_E enCableType = NI_CABLE_TYPE_COAXIAL;

        pu16CableLenValue = (uint16_t*)gauImageMapToEq[enCableType][enVideoClass][NI_EQ_TABLE_EQREG];
    }
    else {
        pu16CableLenValue = (uint16_t*)aiNIACModePreSetVal;
    }

    uint8_t u8R1, u8C1, u8R2, u8C2;
    if (pu16CableLenValue != NULL && u8MaxLen != 0) {
        u8R1 = pu16CableLenValue[0 * u8MaxLen + u8Index];
        u8C1 = pu16CableLenValue[1 * u8MaxLen + u8Index];
        u8R2 = pu16CableLenValue[2 * u8MaxLen + u8Index];
        u8C2 = pu16CableLenValue[3 * u8MaxLen + u8Index];
    }
    else {
        u8R1 = 0;
        u8C1 = 0;
        u8R2 = 0;
        u8C2 = 0;
    }

    NiSdkWriteByte(NI_4201_AFE_EQ_CFG0_CH0, u8R1);
    NiSdkWriteByte(NI_4203_AFE_EQ_CFG2_CH0, u8C1);
    NiSdkWriteByte(NI_4202_AFE_EQ_CFG1_CH0, u8R2);
    NiSdkWriteByte(NI_4204_AFE_EQ_CFG3_CH0, u8C2);
}

static void NiSdkClearEq(I2CPortStruct* i2cPort) {
    NiSdkConfigEq(i2cPort, NI_VIDEO_FMT_BUTT, 0, 0);

    return;
}

static void NiSdkLockTypeCfg(I2CPortStruct* i2cPort, bool bLockType) {
    if (bLockType == 0) {
        NiSdkWriteByte(NI_420B_AFE_CLAMP_CFG0_CH0, 0x21);
        NiSdkWriteByte(NI_420D_AFE_CLAMP_CFG2_CH0, 0x05);
    }
    if (bLockType == 1) {
        NiSdkWriteByte(NI_420B_AFE_CLAMP_CFG0_CH0, 0x2f);
    }
}

static void NiSdkSwitchBurstFscCmd(I2CPortStruct* i2cPort, bool on_off) {
    if (on_off) {
        NiSdkWriteByte(NI_0A60_FILTER_CFG, 0x4);
    }
    else {
        NiSdkWriteByte(NI_0A60_FILTER_CFG, 0x0);
    }
}

static void NiSdkModuleSoftReset(I2CPortStruct* i2cPort) {
    NI_SOFT_REST_MODULE_S RestModule;

    RestModule.Co485Rst     = 0;
    RestModule.AudioRst     = 0;
    RestModule.VoRst        = 0;
    RestModule.VdpRst       = 1;
    RestModule.AdcRst       = 0;
    RestModule.ResampleRst  = 0;
    RestModule.DwsampleRst  = 0;
    RestModule.UpsampleRst  = 0;
    RestModule.SampleRst    = 0;
    RestModule.StdDetectRst = 0;
    RestModule.WaitTime     = 0;

    uint8_t value = (!RestModule.AdcRst) | (!RestModule.VoRst << 1) | (!RestModule.AudioRst << 2) | (!RestModule.Co485Rst << 3) | (1 << 4);
    NiSdkWriteByte(NI_0803_SFT_RST, value);

    value = (!RestModule.ResampleRst) | (!RestModule.DwsampleRst << 1) | (!RestModule.UpsampleRst << 2) | (!RestModule.SampleRst << 3) |
            (!RestModule.StdDetectRst << 4) | (!RestModule.VdpRst << 5);
    NiSdkWriteByte(NI_080E_VDP_SFT_RST, value);

    SystemDelay(10);

    NiSdkWriteByte(NI_0803_SFT_RST, 0x1f);
    NiSdkWriteByte(NI_080E_VDP_SFT_RST, 0x3f);
    NiSdkWriteByte(NI_080E_VDP_SFT_RST, 0x3f);
}

static void NiSdkDoLockClear(I2CPortStruct* i2cPort) {
    NiSdkMipiChnSwitch(i2cPort, false);

    NiSdkClearEq(i2cPort);
    NiSdkWriteByte(NI_0102_HD_COLOR_GAIN_MANUAL_CONTROL_REGISTER, 0x40);
    NiSdkWriteRegBit(NI_0105_HD_AGC_CONTROL_REGISTER, 5, 5, 0x1);
    NiSdkWriteByte(NI_0108_HD_SATURATION_CONTROL_REGISTER, 0x80);

    SystemDelay(1);

    NiSdkWriteByte(NI_080D_DATAIN_GAIN_CTRL, 0x0);

    NiSdkWriteByte(NI_0A60_FILTER_CFG, 0x0);
    NiSdkWriteByte(NI_0A88_RX_PARA_CFG, 0x20);
    NiSdkWriteByte(NI_0121_PAL_SYNC_CONTROL_REGISTER_1, 0x5a);
    NiSdkWriteByte(NI_0122_NTSC_SYNC_CONTROL_REGISTER_1, 0x4b);
    NiSdkWriteByte(NI_0125_PAL_SYNC_CONTROL_REGISTER_3, 0x73);

    //NiSdkWriteRegBit(NI_010C_HD_VIDEO_STD_CONFIG1, 0, 0, 0x0);
    NiSdkWriteByte(NI_010C_HD_VIDEO_STD_CONFIG1, 0x0);
    NiSdkLockTypeCfg(i2cPort, true);
    SystemDelay(1);

    NiSdkSwitchBurstFscCmd(i2cPort, false);
    NiSdkModuleSoftReset(i2cPort);
}

static void NiSdkClkSwitch(I2CPortStruct* i2cPort) {
    NiSdkWriteByte(NI_0800_CLK_GATE_EN, 0x4);
    NiSdkWriteByte(NI_0805_HD_SD_EN, 0x7);

    SystemDelay(1);

    bool isLocked = false;
    bool isHD     = false;

    for (uint8_t i = 0; i < 40; i++) {
        if (i % 10 == 0) {
            NiSdkWriteByte(NI_4200_AFE_CFG_CH0, (i / 10) << 1);
            SystemDelay(20);
        }

        uint8_t status = 0;
        NiSdkReadByte(0x0000, &status);
        NiSdkReadByte(NI_0B25_HD_SD_STATUS, &status);
        isLocked = status & BIT0;
        isHD     = (status >> 1) & BIT0;
        SystemDelay(1);
        if (isLocked)
            break;
    }

    if (isLocked) {
        if (isHD) {
            NiSdkWriteRegBit(NI_0800_CLK_GATE_EN, 0, 0, 1);
            NiSdkWriteRegBit(NI_0800_CLK_GATE_EN, 1, 1, 0);
            NiSdkWriteByte(NI_0805_HD_SD_EN, 0x5);
        }
        else {
            NiSdkWriteRegBit(NI_0800_CLK_GATE_EN, 0, 0, 0);
            NiSdkWriteRegBit(NI_0800_CLK_GATE_EN, 1, 1, 1);
            NiSdkWriteByte(NI_0805_HD_SD_EN, 0xe);
        }
    }
}

static void NiSdkGetLockStatus(I2CPortStruct* i2cPort, NI_LOCK_STATUS_S* pstLockStatus) {
    NiSdkReadByte(NI_0B0C_VIDEO_MODE_LOCK, &pstLockStatus->pn.u8Pn);
    NiSdkReadByte(NI_0B59_HD_STD_OLD_VSTD, &pstLockStatus->std.u8Std);
    NiSdkReadByte(NI_0000_VIDEO_STATUS_REGISTER_1, &pstLockStatus->status.u8Status);
    NiSdkReadByte(NI_0003_VSYNC_STATUS_REGISTER_2, &pstLockStatus->status2.u8Status2);
    NiSdkReadByte(NI_0B0B_VIDEO_MODE_LSB, &pstLockStatus->u8PnFmt);
    NiSdkReadByte(NI_0028_HD_MARK_STD_MODE, &pstLockStatus->u8MarkFmt);

    NiSdStatus stSdStatus;
    NiSdkReadByte(NI_0009_SD_STATUS, &stSdStatus.u8SDstatus);

    NiSdkReadByte(NI_0001_HD_VIDEO_STANDARD_READBACK, &pstLockStatus->u8HdFmt);

    NiSdkReadByte(NI_0002_SD_VIDEO_STANDARD_READBACK, &pstLockStatus->u8SdFmt);
}

static void NiSdkIsNewFmt(NI_VIDEO_FORMAT_E enVideoFmt, bool* pbNewFmt) {
    if (NI_TVI3_1280x720_25HZ == enVideoFmt || NI_TVI3_1280x720_30HZ == enVideoFmt) {
        *pbNewFmt = true;
    }
    else {
        *pbNewFmt = false;
    }
}

static NI_VIDEO_FORMAT_E NiSdkBurstAutoGain(I2CPortStruct* i2cPort, NI_VIDEO_FORMAT_E enInVideoFmt) {
    uint8_t           i = 0, j = 0;
    uint8_t           u8Index    = 0;
    uint8_t           u8BurstVal = 0;
    NI_VIDEO_FORMAT_E enVideoFmt;
    NI_LOCK_STATUS_S  stLockStatus;

    enVideoFmt = enInVideoFmt;

    NiSdkWriteByte(NI_0B4E_HD_STD_TVI_THR_MSB, 0x3f);
    NiSdkWriteByte(NI_0B50_HD_STD_AHD_THR_MSB, 0x3f);
    NiSdkWriteByte(NI_0B51_HD_STD_MARK_LOCK_THR, 0x52);

    SystemDelay(40);

    while (i <= 5) {
        NiSdkReadByte(NI_0B5A_HD_STD_CHECK_CARRIER_FLAG, &u8BurstVal);
        if (0 == u8BurstVal) {
            if (NI_AHD_1280x720_25HZ == enVideoFmt || NI_AHD_1280x720_30HZ == enVideoFmt) {
                NiSdkConfigEq(i2cPort, NI_VIDEO_FMT_BUTT, u8Index, sizeof(aiNIACModePreSetVal[0]) / 2);
            }
            else {
                NiSdkConfigEq(i2cPort, NI_VIDEO_FMT_BUTT, u8Index + 1, sizeof(aiNIACModePreSetVal[0]) / 2);
            }

            u8Index = u8Index + 2;
        }
        else {
            break;
        }

        i++;
        SystemDelay(40);
    }

    do {
        j++;
        NiSdkReadByte(NI_0B5A_HD_STD_CHECK_CARRIER_FLAG, &u8BurstVal);

        if (u8BurstVal) {
            NiSdkGetLockStatus(i2cPort, &stLockStatus);
            NiSdkHdRegToFmt(stLockStatus.u8HdFmt, &enVideoFmt);
        }

        SystemDelay(5);
    }
    while (!u8BurstVal && j < 15);

    NiSdkClearEq(i2cPort);

    NiSdkWriteByte(NI_0B4E_HD_STD_TVI_THR_MSB, 0x06);
    NiSdkWriteByte(NI_0B50_HD_STD_AHD_THR_MSB, 0x07);
    NiSdkWriteByte(NI_0B51_HD_STD_MARK_LOCK_THR, 0x21);

    return enVideoFmt;
}

static void NiSdkVideoFmtDistinguish(I2CPortStruct* i2cPort, NI_VIDEO_FORMAT_E enInVideoFmt, NI_VIDEO_FORMAT_E* penOutVideoFmt) {
    NI_LOCK_STATUS_S stLockStatus;

    bool bNewFmt;

    NiSdkGetLockStatus(i2cPort, &stLockStatus);

    *penOutVideoFmt = enInVideoFmt;

    NiSdkIsNewFmt(enInVideoFmt, &bNewFmt);
    if (bNewFmt != true) {
        if ((!stLockStatus.std.u8MarkLock && !stLockStatus.pn.u8CVIPnLock) && (stLockStatus.status.u8HspllBe || stLockStatus.status.u8HspllFe)) {
            if (enInVideoFmt >= NI_AHD_1280x720_25HZ && enInVideoFmt <= NI_AHD_1920x1080_30HZ) {
                *penOutVideoFmt = NiSdkBurstAutoGain(i2cPort, enInVideoFmt);
            }
        }
    }
}

static NI_VIDEO_FORMAT_E NiSdkDetect(I2CPortStruct* i2cPort) {
    NiSdkSetFreeRun(i2cPort, false, 8, 0);

    // Select Pin7
    // NiSdkWriteRegBit(NI_4200_AFE_CFG_CH0, 1, 2, 0x1);

    // Set lock mode auto
    NiSdkWriteRegBit(NI_0B44_HD_STD_STD_SET, 0, 2, 0x0);

    // Detect
    if (true) { //bDoLockClearEn) {
        NiSdkDoLockClear(i2cPort);
        SystemDelay(260);
    }
    else {
        NiSdkWriteRegBit(NI_010C_HD_VIDEO_STD_CONFIG1, 0, 0, 0x0);
    }

    NiSdkClkSwitch(i2cPort); // read HD/SD status and switch clock
    NiSdkWriteByte(NI_0B50_HD_STD_AHD_THR_MSB, 0x08);

    NI_VIDEO_FORMAT_E enVideoLockFmt;

    uint8_t i = 0, u8TmpTviCheck = 0, u8FmtLockCnt = 0, u8LastFmt = NI_VIDEO_FMT_BUTT;

    NI_LOCK_STATUS_S stLockStatus;
    while (i++ < 20) {
        NiSdkGetLockStatus(i2cPort, &stLockStatus);
        uint8_t status = 0;
        NiSdkReadByte(NI_0000_VIDEO_STATUS_REGISTER_1, &status);

        if (stLockStatus.pn.u8CVIPnLock) {
            NiSdkHdRegToFmt(stLockStatus.u8PnFmt, &enVideoLockFmt);
        }
        else if (stLockStatus.std.u8MarkLock) {

            NiSdkHdRegToFmt(stLockStatus.u8HdFmt, &enVideoLockFmt);

            NiSdkHdRegToFmt(stLockStatus.u8MarkFmt, &enVideoLockFmt);
        }
        else if (stLockStatus.status.u8Vsync && stLockStatus.status.u8HspllBe && stLockStatus.status.u8HspllFe) {
            NiSdkSdRegToFmt(stLockStatus.u8SdFmt, &enVideoLockFmt);

            if (NI_VIDEO_FMT_BUTT == enVideoLockFmt) {
                NiSdkHdRegToFmt(stLockStatus.u8HdFmt, &enVideoLockFmt);
            }
            else {
                break;
            }
        }
        else {
            enVideoLockFmt = NI_VIDEO_FMT_BUTT;
        }

        if (enVideoLockFmt != NI_VIDEO_FMT_BUTT) {
            if (u8LastFmt != enVideoLockFmt) {
                if ((NI_AHD_1280x720_50HZ == enVideoLockFmt || NI_AHD_1280x720_60HZ == enVideoLockFmt || NI_AHD_1920x1080_25HZ == enVideoLockFmt) && i > 0) {
                    u8TmpTviCheck = 1;
                    NiSdkWriteByte(NI_420C_AFE_CLAMP_CFG1_CH0, 0);
                }
                u8LastFmt    = enVideoLockFmt;
                u8FmtLockCnt = 0;
            }
            else {
                u8FmtLockCnt += 1;
                if (u8FmtLockCnt >= 2) {
                    break;
                }
            }
        }

        if (true) { //bDoLockClearEn) {
            SystemDelay(40);
        }
        else {
            SystemDelay(10);
        }
    }

    if (u8TmpTviCheck) {
        NiSdkWriteByte(NI_420C_AFE_CLAMP_CFG1_CH0, 0x5);
    }

    // if (NI_VIDEO_FMT_BUTT == enVideoLockFmt) {
    //     NiSdkCheckNewFmt(i2cPort, &enVideoLockFmt);
    // }

    // Left offset 35
    NiSdkWriteByte(NI_010A_HD_HBLANK_CONTROL_REGISTER, 35);

    NiSdkVideoFmtDistinguish(i2cPort, enVideoLockFmt, &enVideoLockFmt);

    if (enVideoLockFmt == NI_SD_NTSC_JM) {
        //NiSdkSetVdpTestMode(i2cPort, NI_SD_NTSC_JM);
    }

    return enVideoLockFmt;
}
