#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef PLAYOPERATOR_EXPORTS
#define PLAYOPERATOR_API __declspec(dllexport)
#else
#define PLAYOPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    class PlayOperator : public BaseOperator
    {
    public:
        PlayOperator(void);
        ~PlayOperator(void);

        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);
    };

    extern "C"
    {
        PLAYOPERATOR_API BaseOperator* GetOperator(void);
    }
}
