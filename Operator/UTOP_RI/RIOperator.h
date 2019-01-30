#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::RI;

#ifdef RI_STD_OPERATOR_EXPORTS
#define RI_STD_OPERATOR_API __declspec(dllexport)
#else
#define RI_STD_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _ri_operator_param_
    {
        double dLTMinY;         // 白板测试最小Y值
        double dLTMaxY;         // 白板测试最大Y值
        int nReCapture;         // 0：不动作  1：进行更换序列、抓图片、保存的动作   
        int nCenterType;        // 0: Optical Center 1: Logical Center
        int nROIWidth;
        int nROIHeight;
        double dRISpec;
        double dRIDeltaSpec;
    } RI_OPERATOR_PARAM;

    class RIOperator : public BaseOperator
    {
    public:
        RIOperator(void);
        ~RIOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        //------------------------------------------------------------------------------
        // 参数
        RI_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        RI_RESULT m_result;
        double m_dYvalue;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        RI_STD_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
