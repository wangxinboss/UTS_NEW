#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef LIGHTONOPERATOR_EXPORTS
#define LIGHTONOPERATOR_API __declspec(dllexport)
#else
#define LIGHTONOPERATOR_API __declspec(dllimport)
#endif


namespace UTS
{
    class LightOnOperator : public BaseOperator
    {
    public:
        LightOnOperator(void);
        ~LightOnOperator(void);

        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);
    };

    extern "C"
    {
        LIGHTONOPERATOR_API BaseOperator* GetOperator(void);
    }
}
