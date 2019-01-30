#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef LIGHTOFFOPERATOR_EXPORTS
#define LIGHTOFFOPERATOR_API __declspec(dllexport)
#else
#define LIGHTOFFOPERATOR_API __declspec(dllimport)
#endif


namespace UTS
{
    class LightOffOperator : public BaseOperator
    {
    public:
        LightOffOperator(void);
        ~LightOffOperator(void);

        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
    };

    extern "C"
    {
        LIGHTOFFOPERATOR_API BaseOperator* GetOperator(void);
    }
}
