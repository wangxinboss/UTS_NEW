#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm;

#ifdef FOR_TEST_OPERATOR_EXPORTS
#define FOR_TEST_OPERATOR_API __declspec(dllexport)
#else
#define FOR_TEST_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    class ForTestOperator : public BaseOperator
    {
    public:
        ForTestOperator(void);
        ~ForTestOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark);
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);

    private:
    };

    extern "C"
    {
        FOR_TEST_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
