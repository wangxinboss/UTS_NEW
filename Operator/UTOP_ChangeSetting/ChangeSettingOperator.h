#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef ChangeSettingOPERATOR_EXPORTS
#define ChangeSettingOPERATOR_API __declspec(dllexport)
#else
#define ChangeSettingOPERATOR_API __declspec(dllimport)
#endif


namespace UTS
{
    class ChangeSettingOperator : public BaseOperator
    {
    public:
        ChangeSettingOperator(void);
        ~ChangeSettingOperator(void);

        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);
    };

    extern "C"
    {
        ChangeSettingOPERATOR_API BaseOperator* GetOperator(void);
    }
}
