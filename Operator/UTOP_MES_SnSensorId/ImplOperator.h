#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _operator_param_
    {
        int nDeviceIndex;
    } OPERATOR_PARAM;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);

		virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);  //william 20171026 保存SN和Sensor ID数据

    private:
        //------------------------------------------------------------------------------
        // 参数
        OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}

