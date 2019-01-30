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
        CString strRegStrForCheck;
        CString strHintMessage;
		int sn_flag;  //0:sn!=sid 1:sn==sid
    } OPERATOR_PARAM;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);

    private:
        OPERATOR_PARAM m_param;
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}
