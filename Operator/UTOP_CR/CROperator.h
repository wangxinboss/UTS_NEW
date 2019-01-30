#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::ColorChart;
using namespace UTS::Algorithm::CR::CR_Std;

#ifdef CROPERATOR_EXPORTS
#define CROPERATOR_API __declspec(dllexport)
#else
#define CROPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _cr_operator_param_
    {
        int nReCapture;         // 0：不动作  1：进行更换序列、抓图片、保存的动作  
        CR_PARAM stCrParam;
        double dSpecDeltaE[MAX_COLOR_NUM];
    } CR_OPERATOR_PARAM;

    class CrOperator : public BaseOperator
    {
    public:
        CrOperator(void);
        ~CrOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark);
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);

    private:
        //------------------------------------------------------------------------------
        // 参数
        CR_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        CR_RESULT m_result;
    };

    extern "C"
    {
        CROPERATOR_API BaseOperator* GetOperator(void);
    }
}
