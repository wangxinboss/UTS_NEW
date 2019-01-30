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
        CString strHintMessage;
    } OPERATOR_PARAM;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

		CString sIniFileName;
		int OneKeyTestEn;

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);

    private:
        //------------------------------------------------------------------------------
        // ²ÎÊý
        OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}

