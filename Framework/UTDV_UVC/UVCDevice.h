#pragma once
#include "basedevice.h"
#include "USBFrameHandler.h"
#include "BaseDsp.h"

#ifdef UVCDEVICE_EXPORTS
#define UVCDEVICE_API __declspec(dllexport)
#else
#define UVCDEVICE_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _uvc_ref_
    {
        int nResolutionX;
        int nResolutionY;
        int nDspType;
        int sensor_i2c_mode;
    } UVC_REF;

    class UVCDevice : public BaseDevice
    {
    public:
        UVCDevice(void);
        ~UVCDevice(void);

        virtual BOOL Initialize(HWND hDisplay);
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

        void UpdateCurrentBuffer(unsigned char *pBmpBuffer);
        BOOL ReadUVCRef();
		BOOL RegisterSet(LPCTSTR lpSectionName);
		BOOL ExecuteCommandList(LPCTSTR lpCommandList);
		BOOL ExecuteCommand(LPCTSTR linebufIn);

    private:
        UVC_REF m_uvcRef;
		BaseDsp *m_pDsp;
        CUSBFrameHandler m_USBHandler;
        int m_nFrameCount;
    };

    extern "C"
    {
        UVCDEVICE_API BaseDevice* GetDevice(void);
    }

    extern UVCDevice* GetUVCDeviceInstance(void);
}
