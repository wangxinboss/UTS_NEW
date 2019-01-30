#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef VCM_CHECK_OPERATOR_EXPORTS
#define VCM_CHECK_OPERATOR_API __declspec(dllexport)
#else
#define VCM_CHECK_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _vcm_check_operator_param_
    {
        int nCurrent_AnaPin;    // 1/2/3/4/5/6
		int nVCM_MaxDac;
		int nVCM_DelayTime;
		double dAF_Current_Level;
    }VCM_CHECK_OPERATOR_PARAM;

    class VCMCheckOperator : public BaseOperator
    {
    public:
        VCMCheckOperator(void);
        ~VCMCheckOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        //------------------------------------------------------------------------------
        // ²ÎÊý
        VCM_CHECK_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
		// result
		double m_dAFCurrent_before;
		double m_dAFCurrent_after;
    };

    extern "C"
    {
        VCM_CHECK_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
