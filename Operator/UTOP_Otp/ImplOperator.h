#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"
#include "Otp/BaseOtp.h"


#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
	typedef struct _operator_param_
	{
		// common
		CString strOTPTestOpt;
		int nGROUP_FOR_USE;
		int nSensorBlackLevel;
		int nEEPRomType;

		// WB
		int nWB_Type;
		int nWB_ROI_Width;
		int nWB_ROI_Height;
		double dWB_RG_TARGET;
		double dWB_BG_TARGET;
		double dWB_RG_DELTA_SPEC;
		double dWB_BG_DELTA_SPEC;
		double dWB_GOLDEN_SPEC;
		double dWB_BEGODLEN_SPEC;
		double dWB_GOLDEN_SPEC_GBGR;
		CString strWB_RGrGbB_L;
		CString strWB_RGrGbB_H;
		//add by wx
		CString strWB_RGrGbB_G;
		CString strWB_LightConfident;
		int nWB_GODLEN_ValidTime;
		int nWB_GODLEN_WarnTime;
		CString strGoldenSampleName;
		double dWB_RG_MULTI;
		double dWB_BG_MULTI;
		CString strWB_GOLDEN_OFFLINE_RGrGbB;

		// LSC
		LSC_PARAM lsc_param;
		int nLSC_ROI_Width;
		int nLSC_ROI_Height;
		double dLSC_GOLDEN_SPEC;
		CString strLSCGoldenSampleName;
		int nReCapture;

		//AF
		AF_PARAM af_param;
		//OtpOffCheck
		OtpOffCheck otp_off_param;

		//OIS
		OIS_PAPRAM ois_param;

		//PDAF
		PDAF_PAPRAM pdaf_param;

		//FPN
		FPN_PAPRAM fpn_param;

		//CCT
		OTP_CCT   otp_cct;

	} OPERATOR_PARAM;

	class ImplOperator : public BaseOperator
	{
	public:
		ImplOperator(void);
		~ImplOperator(void);

		virtual BOOL OnReadSpec();
		virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
		virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
		virtual void OnGetRegisterList(vector<CString> &vecRegister);
		virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

		virtual void OnPreviewStart();

	private:
		BOOL SetOtpParam();

	private:
		vector<int> m_vecOtpTestOpt;
		BaseOtp *m_pOtp;
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
