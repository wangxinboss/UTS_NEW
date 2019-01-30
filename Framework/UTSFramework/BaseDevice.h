#pragma once
#include "UTSDefine.h"
#include "UIMark.h"
#include "Export.h"

namespace UTS
{
    #define BUFFER_TYPE_MASK_RAW8    0x00000001
    #define BUFFER_TYPE_MASK_BMP     0x00000002
    #define BUFFER_TYPE_MASK_YUV24   0x00000004

    //------------------------------------------------------------------------------
    // enum
    typedef enum _e_device_type_
    {
        DeviceType_File = 1,
        DeviceType_V5U,
        DeviceType_UVC
    } eDeviceType;

    typedef enum _e_data_format_
    {
        YUV = 0,
        Raw8Bit = 1,
        Raw10Bit = 2,
		Raw2PD = 12,
    } eDataFormat;

    typedef enum _e_bayer_output_mode_
    {
        BGGR = 1,
        RGGB = 2,
        GBRG = 3,
        GRBG = 4,
        BlackWhite = 5,
    } eBayerOutputMode;

    typedef enum _e_mipi_mode_
    {
        Line1 = 0,
        Line2 = 1,
        Line4 = 7,
        Parallel = 9,
    } eMipiMode;

    typedef struct _buffer_info
    {
        DWORD dwBufferType;
        int nWidth;
        int nHeight;
    } BUFFER_INFO;

    typedef struct _testBuffer_
    {
        unsigned char *pFrameBuffer;    // original buffer
        unsigned char *pBmpBuffer;      // buffer converted to bmp(always auto)
        unsigned char *pRaw8Buffer;     // buffer converted to raw8(auto when sensor output raw)
        unsigned char *pYuv24Buffer;    // buffer converted to yuv24(auto when sensor output yuv)
        unsigned char *pYBuffer;        // Y buffer(always manual)
    } TEST_BUFFER;
    
    typedef enum _e_device_read_value_type_
    {
        DRVT_SENSOR_OUT_MODE = 0,
        DRVT_CURRENT_STANDBY_OFFSET,
        DRVT_CURRENT_DYNAMIC_OFFSET,
        DRVT_CURRENT_STANDBY_MEASURE,
        DRVT_CURRENT_DYNAMIC_MEASURE,
        DRVT_REG_READ,
		DRVT_READ_SENSOR_PROPERTY_BRIGHTNESS,
		DRVT_READ_SENSOR_PROPERTY_SHARPNESS,
		DRVT_READ_SENSOR_PROPERTY_WHITEBALANCE,
		DRVT_READ_SENSOR_PROPERTY_FOCUS,
		DRVT_READ_SENSOR_PROPERTY_EXPOSURE,
		DRVT_READ_SENSOR_REGISTER,
		DRVT_READ_EEPROM,
        DRVT_IIC_MODE,
        DRVT_SLAV_ADDR,
        DRVT_I2C_READ,
		DRVT_DUMPFW_EEPROM,
		DRVT_DUMPFW_USBOTP,
		DRVT_READ_GPIO,
    } eDeviceReadValueType;

    typedef enum _e_device_write_value_type_
    {
        DWVT_REGISTER_SET = 0,
        DWVT_I2C_WRITE,
        DWVT_REG_WRITE,
        DWVT_SWITCH_DEVICE_INDEX,
		DWVT_WRITE_SENSOR_PROPERTY_BRIGHTNESS,
		DWVT_WRITE_SENSOR_PROPERTY_SHARPNESS,
		DWVT_WRITE_SENSOR_PROPERTY_WHITEBALANCE,
		DWVT_WRITE_SENSOR_PROPERTY_FOCUS,
		DWVT_WRITE_SENSOR_PROPERTY_EXPOSURE,
		DWVT_WRITE_SENSOR_REGISTER,
		DWVT_WRITE_EEPROM,
        DWVT_SET_LED,
        DWVT_IIC_MODE,
        DWVT_SET_POWER_PIN,
		DWVT_LOADFW_EEPROM,
		DWVT_LOADFW_USBOTP,
		DWVT_SET_GPIO,
		DWVT_SENSOR_OUT_MODE,
		DWVT_SET_POWPERIN36,
		DWVT_SENSOR_SLAV_ADDR,
		DWVT_RESET_POWER_PIN,
		DWVT_RESET_POWERSEQUENCE_PIN,
		DWVT_RESET_IMAGESIZE,
		DWVT_RESET_DATAFORMAT,
		DWVT_RESET_MIPIMode,
		DWVT_RESET_I2CMode,
		DWVT_SET_MCLK,
    } eDeviceWriteValueType;

	typedef struct _ost_config_
	{
		TCHAR PinName[64][8];
		INT32 OSTPlusEn;
		INT32 OSTMinusEn;
		INT32 OSTPinMask[2];
		INT32 OSTCurrent[64];
		INT32 OSTOpenSpec[64];
		INT32 OSTShortSpec[64];
	} OST_CONFIG;

	typedef struct _ost_result_
	{
		INT32 OSTResult[64];
		INT32 OSTPlusVoltage[64];
		INT32 OSTMinusVoltage[64];
	} OST_RESULT;

    class UTS_FRAMEWORK_API BaseDevice
    {
    public:
        BaseDevice(void);
        virtual ~BaseDevice(void);

        // Control
        virtual BOOL Initialize(HWND hDisplay) = 0;
		virtual BOOL OST(OST_CONFIG OSTConfig, OST_RESULT *OSTResult){return 0;};
        virtual BOOL Start(LPCTSTR lpRegister) = 0;
        virtual BOOL Stop() = 0;
        virtual BOOL Close() = 0;

        /*
        id      type        supportDevice       vallue
        =================================================
        0       int         V5U, File           SENSOR_OUT_MODE
        1       int[6]      V5U                 CURRENT_STANDBY_OFFSET
        2       int[6]      V5U                 CURRENT_DYNAMIC_OFFSET
        3       (*1)        V5U                 CURRENT_STANDBY_MEASURE
        4       (*2)        V5U                 CURRENT_DYNAMIC_MEASURE
        5       (*3)        V5U                 REG_READ
		6       long[2]     UVC                 DRVT_READ_SENSOR_PROPERTY_BRIGHTNESS
		7       long[2]     UVC                 DRVT_READ_SENSOR_PROPERTY_SHARPNESS
		8       long[2]     UVC                 DRVT_READ_SENSOR_PROPERTY_WHITEBALANCE
		9       long[2]     UVC                 DRVT_READ_SENSOR_PROPERTY_FOCUS
		10      long[2]     UVC                 DRVT_READ_SENSOR_PROPERTY_EXPOSURE
		11      int[2]      UVC                 DRVT_READ_SENSOR_REGISTER
		12		(*4)		UVC					DRVT_READ_EEPROM
        13      int         V5U                 IIC_MODE
        14      int         V5U                 SLAV_ADDR
        15      (*5)        V5U                 I2C_READ
        (*1)(*2):
            Input  Format: [int|PinIndexBaseFrom1][int|OffsetValue]
            Output Format: [double|MeasuredCurrentValue]
        (*3):
            Input  Format: [WORD|wRegAddr]
            Output Format: [WORD|wRegData]
		long[2]:
			Output Format: [long|auto flag][long|value]
		11 int[2]:
			Output Format: [int|value][int|register addr]
		12 (*4):
            Output Format: [BYTE*|value][int|start addr]|[int|read length]
        15 (*5):
            Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
            Output Format: [DWORD|nCnt][unsigned char[?]|buf]
        */
        virtual BOOL ReadValue(eDeviceReadValueType eReadType, void *pBuffer, int nBufferSize);
        /*
        id      type        supportDevice       vallue
        =================================================
        0       TCHAR*      V5U                 REGISTER_SET
        1       (*1)        V5U                 I2C_WRITE
        2       (*2)        V5U                 REG_WRITE
        3       int*        V5U                 SWITCH_DEVICE_INDEX
		4       long[2]     UVC                 DWVT_WRITE_SENSOR_PROPERTY_BRIGHTNESS
		5       long[2]     UVC                 DWVT_WRITE_SENSOR_PROPERTY_SHARPNESS
		6       long[2]     UVC                 DWVT_WRITE_SENSOR_PROPERTY_WHITEBALANCE
		7       long[2]     UVC                 DWVT_WRITE_SENSOR_PROPERTY_FOCUS
		8       long[2]     UVC                 DWVT_WRITE_SENSOR_PROPERTY_EXPOSURE
		9		int[2]      UVC                 DWVT_WRITE_SENSOR_REGISTER
		10		(*3)		UVC					DWVT_WRITE_EEPROM
		11		int			UVC					DRVT_WRITE_LED
        12      int         V5U                 IIC_MODE
        13      (*4)        V5U                 SET_POWER_PIN
        (*1):
            Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
            Output Format: [Not Use]
        (*2):
            Input  Format: [WORD|wRegAddr][WORD|wRegData]
            Output Format: [Not Use]
		4~9 long[2]:
			Input  Format: [long|auto flag][long|value]
		9 int[2]:
			Input Format: [int|value][int|register addr]
		10 (*3):
			Input Format: [BYTE*|value][int|start addr]|[int|read length]
		11 int:
			Input Format: led light value
        12 int:
            Input Format: iic mode
        13 (*4):
            Input Format: [int|idx][float|volt]
        */
        virtual BOOL WriteValue(eDeviceWriteValueType eWriteType, void *pBuffer, int nBufferSize);

        // image
        virtual BOOL GetBufferInfo(BUFFER_INFO &bufferInfo) = 0;
        virtual BOOL Recapture(TEST_BUFFER &testBuffer, int nDummyCount = 0, int nAvgCount = 1) = 0;
        virtual void GetCapturedBuffer(TEST_BUFFER &testBuffer);
        virtual void DisplayImage(unsigned char *pBmpBuffer, const UI_MARK* pUiMark = nullptr);

        // other
        virtual void ShowSettingDlg(void);

        void SnapImage(void);   // fix ¸ÄÉÆ #2: Snap¹¦ÄÜ

    protected:
        void CreateTestBufferMemory();
        void ClearTestBufferMemory();
        void ReleaseTestBufferMemory();

        BOOL m_bDvStarting[64];
        int m_nWidth;
        int m_nHeight;
        HWND m_hDisplay;
        double m_dLowlevelFPS;
        double m_dDisplayFPS;
        FPSCounter m_lowLevelFpsCounter;
        FPSCounter m_displayFpsCounter;
        TEST_BUFFER m_TestBuffer;
    };
}
