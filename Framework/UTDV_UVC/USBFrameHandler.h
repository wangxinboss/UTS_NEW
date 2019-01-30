#pragma once
#include "capturevideo.h"


namespace UTS
{
    class CUSBFrameHandler :
        public CVdoFrameHandler
    {
    public:
        CUSBFrameHandler(void);
        ~CUSBFrameHandler(void);
        // 重载它，这里的pBuffer指针里面放的就是BGR24的的采集结果，lBufferSize就是pBuffer长度
        void VdoFrameData(double dblSampleTime, BYTE * pBuffer, long lBufferSize);

        CCaptureVideo m_cap_vdo; // 这个就是视频采集的类的对象了

    private:
        void SetWhiteScreen(BYTE *pBuffer, long lBufferSize);
        void SetBlackScreen(BYTE *pBuffer, long lBufferSize);
        void SetLightDefectScreen(BYTE *pBuffer, long lBufferSize);
        void SetDarkDefectScreen(BYTE *pBuffer, long lBufferSize);
    };
}
