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

		int		nDarckLedOnDelay;
		int     nDarkLedTestBlinkFrame;
		double	dDarkLedTestThresholdHigh;
		double	dDarkLedTestThresholdLow;
		double	dDarkLedTestR_GDeltaHigh;
		double	dDarkLedTestB_GDeltaHigh;
		double	dDarkLedTestR_GDeltaLow;
		double	dDarkLedTestB_GDeltaLow;
		double  dDarkLedTestBlinkDeltaSpec;
		RECT    TestROI;
	} OPERATOR_PARAM;

	typedef struct _operator_result_
	{
		double	 dLightValue;
		double	 dLightDelta;
		double	 dR_GDelta;
		double	 dB_GDelta;
		double   dLedBlinkMax;
		double   dLedBlinkMin;
		double   dLedBlinkDelta;
	}OPERATOR_RESULT;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
		virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);

		void LedTest(  unsigned char *_pBmpImage,
							  int Width,
							  int Height,
							  RECT    TestROI,
							  double &lightValue,
							  double &R_GDelta,
							  double &B_GDelta);
		void BlinkTest(RECT    TestROI,
					   double &BlinkMax,
					   double &BlinkMin,
					   double &BlinkDelta);

	protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        //------------------------------------------------------------------------------
        // ²ÎÊý
        OPERATOR_PARAM m_param;
	    OPERATOR_RESULT m_result;
		UI_MARK uiMark;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}

