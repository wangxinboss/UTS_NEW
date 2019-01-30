#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::TV_Distortion;
using namespace UTS::Algorithm::TV_Distortion::TV_Distortion_Rectangle;

#ifdef TVD_OPERATOR_EXPORTS
#define TVD_OPERATOR_API __declspec(dllexport)
#else
#define TVD_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _tvd_operator_param_
    {
        int nReCapture;         // 0：不动作  1：进行更换序列、抓图片、保存的动作
        TVD_PARAM tvdParam;
    } TVD_OPERATOR_PARAM;

    class TvDistortionOperator : public BaseOperator
    {
    public:
        TvDistortionOperator(void);
        ~TvDistortionOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark);
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        //------------------------------------------------------------------------------
        // 参数
        TVD_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        TVD_RESULT m_result;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        TVD_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
