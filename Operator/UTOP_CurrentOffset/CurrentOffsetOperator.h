#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef CURRENT_OFFSET_OPERATOR_EXPORTS
#define CURRENT_OFFSET_OPERATOR_API __declspec(dllexport)
#else
#define CURRENT_OFFSET_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    class CurrentOffsetOperator : public BaseOperator
    {
    public:
        CurrentOffsetOperator(void);
        ~CurrentOffsetOperator(void);

        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        //virtual void OnGetRegisterList(vector<CString> &vecRegister);
    };

    extern "C"
    {
        CURRENT_OFFSET_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
