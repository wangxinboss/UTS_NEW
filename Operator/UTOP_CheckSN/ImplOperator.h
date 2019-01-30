#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef SNOPERATOR_EXPORTS
#define SNOPERATOR_API __declspec(dllexport)
#else
#define SNOPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _operator_param_
    {
        CString strRegStrForCheck;
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
        SNOPERATOR_API BaseOperator* GetOperator(void);
    }
}
