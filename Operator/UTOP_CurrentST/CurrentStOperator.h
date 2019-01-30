#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef CURRENT_ST_OPERATOR_EXPORTS
#define CURRENT_ST_OPERATOR_API __declspec(dllexport)
#else
#define CURRENT_ST_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _current_st_operator_param_
    {
        int nCurrent_DigPin;    // 1/2/3/4/5/6
        int nCurrent_AnaPin;    // 1/2/3/4/5/6
        int nST_DigLowThreshold;
        int nST_DigHighThreshold;
        int nST_AnaLowThreshold;
        int nST_AnaHighThreshold;
    } CURRENT_ST_OPERATOR_PARAM;

    class CurrentStOperator : public BaseOperator
    {
    public:
        CurrentStOperator(void);
        ~CurrentStOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        //------------------------------------------------------------------------------
        // ²ÎÊý
        CURRENT_ST_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // result
        double m_dStDig;
        double m_dStAna;
    };

    extern "C"
    {
        CURRENT_ST_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
