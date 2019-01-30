#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::Image;


#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _operator_param_
    {
        SIZE szPtCount;

		double dPixelSize;
		double dLensEFL;
		double dLensDFOV;

        MinMax<double> dRotateAngleSpec;
		MinMax<double> dTiltXAngleSpec;
		MinMax<double> dTiltYAngleSpec;
		MinMax<double> dShiftXSpec;
		MinMax<double> dShiftYSpec;
		MinMax<double> dScaleSpec;

    } OPERATOR_PARAM;

    typedef struct _operator_result_
    {
		double dShiftX;
		double dShiftY;
        double dRotateAngle;
		double dTiltX;
		double dTiltY;
		double dScale;
    } OPERATOR_RESULT;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);
		virtual BOOL OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);
        //------------------------------------------------------------------------------
        // 参数
        OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        OPERATOR_RESULT m_result;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}

