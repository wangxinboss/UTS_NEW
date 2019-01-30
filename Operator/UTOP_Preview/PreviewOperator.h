#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef PREVIEWOPERATOR_EXPORTS
#define PREVIEWOPERATOR_API __declspec(dllexport)
#else
#define PREVIEWOPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _preview_operator_param_
    {
        CString strHintMessage;
    } PREVIEW_OPERATOR_PARAM;

    class PreviewOperator : public BaseOperator
    {
    public:
        PreviewOperator(void);
        ~PreviewOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);

    private:
        PREVIEW_OPERATOR_PARAM m_param;
    };

    extern "C"
    {
        PREVIEWOPERATOR_API BaseOperator* GetOperator(void);
    }
}
