#include "StdAfx.h"
#include "UTSBase.h"

namespace UTS
{
    #define RET_OK      0
    #define RET_ERROR   (-1)

    HighPrecisionTimer::HighPrecisionTimer(void)
    {
        m_llCollectStartTime = -1;
    }

    HighPrecisionTimer::~HighPrecisionTimer(void)
    {
    }

    /*************************************************************************
    【函数名称】       SetStartTime
    【函数功能】       记录采集开始时间
    【参数】           无
    【返回值】         0：成功 / -1：失败
    *************************************************************************/
    int HighPrecisionTimer::SetStartTime(void)
    {
        LARGE_INTEGER litmp;
        if (FALSE == QueryPerformanceCounter(&litmp))
        {
            return RET_ERROR;
        }
        m_llCollectStartTime = litmp.QuadPart;
        return RET_OK;
    }

    /*************************************************************************
    【函数名称】       GetPassTime
    【函数功能】       获得经过的时间值(ms)
    【参数】           无
    【返回值】         double 经过的时间值(ms)
    *************************************************************************/
    double HighPrecisionTimer::GetPassTime()
    {
        if (m_llCollectStartTime < 0)
        {
            return 0;
        }

        LARGE_INTEGER litmp;
        LONGLONG qt2;
        double dft,dff,dfm;
        if (FALSE == QueryPerformanceFrequency(&litmp)) //获得时钟频率
        {
            return RET_ERROR;
        }
        dff = (double)litmp.QuadPart;

        if (FALSE == QueryPerformanceCounter(&litmp))   //获得终止值
        {
            return RET_ERROR;
        }
        qt2 = litmp.QuadPart;
        dfm = (double)(qt2 - m_llCollectStartTime);
        dft = (dfm * 1000.0) / dff;    //获得经过的时间值(ms)
        return dft;
    }

    void HighPrecisionTimer::EndCollectTime()
    {
        m_llCollectStartTime = -1;
    }
}
