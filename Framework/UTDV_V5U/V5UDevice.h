#pragma once
#include "basedevice.h"
#include "HVSI2C.h"

#ifdef V5UDEVICE_EXPORTS
#define V5UDEVICE_API __declspec(dllexport)
#else
#define V5UDEVICE_API __declspec(dllimport)
#endif

namespace UTS
{
    //------------------------------------------------------------------------------
    // YUV Data mode 
    typedef enum _e_yuv_data_mode_
    {
        YCbYCr = 1,
        YCrYCb = 2,
        CbYCrY = 3,
        CrYCbY = 4,
    } eYuvDataMode;

    typedef struct _v5u_ref_
    {
        // BOARD_SET
        int DEVICE_COUNT;
        int BOARD_SET_INDEX_NUM;
        // HVS_HW
        int power_on[7];
        double power_volt[7];
        int HW_AVDDSlowUPEn;
        int HW_PGMFREQ;
        int HW_OSC_TYPE;
        // SENSOR_SET
        int HVS_SENSOR_VCLK;
        int HVS_SENSOR_RESET;
        int HVS_SENSOR_ENB;
        int HVS_SENSOR_VSYNC;
        CString strHVS_SENSOR_SLAVEADDR;
        WORD wHVS_SENSOR_SLAVEADDR;
        int HVS_SENSOR_SIZE_X;
        int HVS_SENSOR_SIZE_Y;
        int HVS_SENSOR_DATA_FORMAT;
        int HVS_SENSOR_OUT_MODE;
        int HVS_SENSOR_MIPIMODE;
        int HVS_SENSOR_I2CMODE;
        int HVS_SENSOR_SleepTime;
    } V5U_REF;

    class V5UDevice : public BaseDevice
    {
    public:
        V5UDevice(void);
        ~V5UDevice(void);

        virtual BOOL Initialize(HWND hDisplay);
		virtual BOOL OST(OST_CONFIG OSTConfig, OST_RESULT *OSTResult);
        virtual BOOL Start(LPCTSTR lpRegister);
        virtual BOOL Stop();
        virtual BOOL Close();
        virtual BOOL ReadValue(eDeviceReadValueType eReadType, void *pBuffer, int nBufferSize);
        virtual BOOL WriteValue(eDeviceWriteValueType eWriteType, void *pBuffer, int nBufferSize);

        // image
        virtual BOOL GetBufferInfo(BUFFER_INFO &bufferInfo);
        virtual BOOL Recapture(TEST_BUFFER &testBuffer, int nDummyCount = 0, int nAvgCount = 1);

        // other
        virtual void ShowSettingDlg(void);

        // V5U only
        int IIC_Read_Proc(DWORD dwRegAddr);
        BOOL IIC_Write_Proc(DWORD dwRegAddr, DWORD dwRegData);
        BOOL i2c_write(int mode, int slave_addr, int start_addr, const char *data, int len);
        BOOL i2c_read(int mode, int slave_addr, int start_addr, char *data, int len);
		BOOL IIc_Write_Atom(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf);
		BOOL IIc_Read_Atom(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf);
        int GetIICMode(void);
        BOOL ReadV5URef();

    private:
        V5U_REF m_v5uRef;
        CHVSI2C m_HVSI2C;
        DWORD m_mipiX;
        DWORD m_mipiY;

        BOOL Program_PowerOn();
        void Oscillator_Set();
        void BoardControl();
        BOOL RegisterSet(LPCTSTR lpSectionName);
#if 0
        BOOL ParseRegisterFileBoard(LPCTSTR lpSectionName, LPCTSTR lpFilename);
#else
        BOOL ExecuteCommandList(LPCTSTR lpCommandList);
#endif
        BOOL ExecuteCommand(LPCTSTR linebufIn);
        void Reset_High();
        void Reset_Low();
        void ENB_High();
        void ENB_Low();
        void VCLK_ByPass();
        void VCLK_Inv();
        void VSYNC_ByPass();
        void VSYNC_Inv();
        void MCLK_Off();
        DWORD GetFrameSize();
        BOOL GetOneFrame(DWORD dwCount, int nTimeout);
    };

    EXTERN_C
    {
        V5UDEVICE_API BaseDevice* GetDevice(void);
    }
}
