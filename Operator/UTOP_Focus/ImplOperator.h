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
		int iDAC_Start;
		int iDAC_End;
		int iMove_DAC;
		int iDelayTime;
		int iFinalDelayTime;

		int iReadyMaxCountSpec;
		double dSobelDropRange;

		//Focus ROI
		int iROIMethod;
		int iFocusMethod;
		int iSaveDAC;
		RECT FocusROI;

		int dVCMInitMethod;

		double dMinFocusSpec;
	} OPERATOR_PARAM;

	typedef struct _operator_result_
	{
		double dFocusValue;
		double dMaxFocusValue;
		int iMaxDAC;
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
		virtual void OnPreviewStart();
	protected:
		virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

		void AutoROI();
	private:
		//------------------------------------------------------------------------------
		// ²ÎÊý
		OPERATOR_PARAM m_param;
		OPERATOR_RESULT m_result;
		//------------------------------------------------------------------------------
	};

	extern "C"
	{
		OPERATOR_API BaseOperator* GetOperator(void);
	}
}

