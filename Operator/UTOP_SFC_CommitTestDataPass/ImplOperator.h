#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"
#include "cJSON.h"
#include "Global_Memory_SFC.h"

#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    //typedef struct _operator_param_
    //{
    //    int nPort;
    //} OPERATOR_PARAM;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);

    private:
        void OutputCSV(const TestItemResult& resultItem);
        void AddJsonItem(cJSON *root, const TestItemResult& resultItem);
    //    //------------------------------------------------------------------------------
    //    // ²ÎÊý
    //    OPERATOR_PARAM m_param;
    //    //------------------------------------------------------------------------------
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}

