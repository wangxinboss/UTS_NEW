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

        //亮度
        int UVC_BrightnessFixEn;
        int UVC_BrightnessAuto;
        int UVC_BrightnessValue;
        //清晰度
        int UVC_SharpnessFixEn;
        int UVC_SharpnessAuto;
        int UVC_SharpnessValue;
        //白平衡
        int UVC_WhiteBalanceFixEn;
        int UVC_WhiteBalanceAuto;
        int UVC_WhiteBalanceValue;
        //焦点
        int UVC_FocusFixEn;
        int UVC_FocusAuto;
        int UVC_FocusValue;
        //曝光
        int UVC_ExposureFixEn;
        int UVC_ExposureAuto;
        int UVC_ExposureValue;
    } OPERATOR_PARAM;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);

    private:
        //------------------------------------------------------------------------------
        // 参数
        OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}

