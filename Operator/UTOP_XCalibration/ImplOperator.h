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
		double dRoll;    
		double dPitch;   
		double dYaw;     
		double dTilt;      
		double dShift_X;
		double dShift_Y;
		double dShift_All_X;
		double dShift_All_Y;
		MinMax<double> dTxSpec;
		double dPixel_Size;
		
	} OPERATOR_PARAM;

	typedef struct _awb_operator_result_
	{
		int Result_Cali;
		
	} OPERATOR_RESULT;
	//
	typedef struct XCalibration_s {
		double rms0;
		double rms1;
		double rms;

		double roll;
		double pitch;
		double yaw;

		double K0Matrix11;
		double K0Matrix12;
		double K0Matrix13;
		double K0Matrix21;
		double K0Matrix22;
		double K0Matrix23;
		double K0Matrix31;
		double K0Matrix32;
		double K0Matrix33;

		double K1Matrix11;
		double K1Matrix12;
		double K1Matrix13;
		double K1Matrix21;
		double K1Matrix22;
		double K1Matrix23;
		double K1Matrix31;
		double K1Matrix32;
		double K1Matrix33;

		double RMatrix11;
		double RMatrix12;
		double RMatrix13;
		double RMatrix21;
		double RMatrix22;
		double RMatrix23;
		double RMatrix31;
		double RMatrix32;
		double RMatrix33;

		double TMatrix11;
		double TMatrix12;
		double TMatrix13;
	} XCalibration_t;

	class ImplOperator : public BaseOperator
	{
	public:
		ImplOperator(void);
		~ImplOperator(void);

		virtual BOOL OnReadSpec();
		virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
		virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
		virtual void OnGetRegisterList(vector<CString> &vecRegister);
	protected:
		virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);
		virtual void ParseXCalibrationTxt(TCHAR *FileName, XCalibration_t *XCalibration);

	private:
		//------------------------------------------------------------------------------
		// 参数
		OPERATOR_PARAM m_param;
		//------------------------------------------------------------------------------
		// 结果
		OPERATOR_RESULT m_result;
		//------------------------------------------------------------------------------
		XCalibration_t *XCalibration;

		double dXTilt;
		double XShift1_X;
		double XShift1_Y;
		double XShift2_X;
		double XShift2_Y;
		double XShift_ALL_X;
		double XShift_ALL_Y;
		double dTx;
	};

	extern "C"
	{
		OPERATOR_API BaseOperator* GetOperator(void);
	}
}
