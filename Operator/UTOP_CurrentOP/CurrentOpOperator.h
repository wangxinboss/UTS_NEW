#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef CURRENT_OP_OPERATOR_EXPORTS
#define CURRENT_OP_OPERATOR_API __declspec(dllexport)
#else
#define CURRENT_OP_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _current_op_operator_param_
    {
        int nCurrent_DigPin;    // 1/2/3/4/5/6
        int nCurrent_AnaPin;    // 1/2/3/4/5/6
        int nOP_DigLowThreshold;
        int nOP_DigHighThreshold;
        int nOP_AnaLowThreshold;
        int nOP_AnaHighThreshold;
    } CURRENT_OP_OPERATOR_PARAM;

    class CurrentOpOperator : public BaseOperator
    {
    public:
        CurrentOpOperator(void);
        ~CurrentOpOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        //------------------------------------------------------------------------------
        // ²ÎÊý
        CURRENT_OP_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // result
        double m_dOpDig;
        double m_dOpAna;
    };

    extern "C"
    {
        CURRENT_OP_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
