#include "StdAfx.h"
#include "USBFrameHandler.h"
#include "UVCDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    CUSBFrameHandler::CUSBFrameHandler(void)
    {
    }


    CUSBFrameHandler::~CUSBFrameHandler(void)
    {
    }

    void CUSBFrameHandler::VdoFrameData(double dblSampleTime, BYTE * pBuffer, long lBufferSize)
    {
        GetUVCDeviceInstance()->UpdateCurrentBuffer(pBuffer);
    }

    //-------------------------------------------------------------------------
    // ²âÊÔÓÃ
    // È«°×
    void CUSBFrameHandler::SetWhiteScreen(BYTE *pBuffer, long lBufferSize)
    {
        memset(pBuffer, 255, lBufferSize);
    }
    // È«ºÚ
    void CUSBFrameHandler::SetBlackScreen(BYTE *pBuffer, long lBufferSize)
    {
        memset(pBuffer, 0, lBufferSize);
    }
    // Light Defect
    void CUSBFrameHandler::SetLightDefectScreen(BYTE *pBuffer, long lBufferSize)
    {
        memset(pBuffer, 255, lBufferSize);
        memset(&pBuffer[640*200*3+50*3], 100, 3);
        memset(&pBuffer[640*203*3+50*3], 100, 3);
        memset(&pBuffer[640*201*3+350*3], 100, 3);
    }
    // Dark Defect
    void CUSBFrameHandler::SetDarkDefectScreen(BYTE *pBuffer, long lBufferSize)
    {
        memset(pBuffer, 0, lBufferSize);
        memset(&pBuffer[640*200*3+50*3], 100, 3);
        memset(&pBuffer[640*203*3+50*3], 100, 3);
        memset(&pBuffer[640*201*3+350*3], 100, 3);
    }
}
