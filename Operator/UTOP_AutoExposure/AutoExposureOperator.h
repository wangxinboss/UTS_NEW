#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"
#include "SensorDriver.h"

using namespace UTS::Algorithm;


#ifdef AUTOEXPOSURE_OPERATOR_EXPORTS
#define AUTOEXPOSURE_OPERATOR_API __declspec(dllexport)
#else
#define AUTOEXPOSURE_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
	enum AE_CHN{
		AE_CHN_R,
		AE_CHN_G,
		AE_CHN_B,
		AE_CHN_Y
	};

	struct ROI {
		int x;
		int y;
		int width;
		int height;
	};

	struct AE_Param{
		int en;
		int trycnt;
		RECT rect;
		int chn;
		int filter;
		double target;
		double limit;
	};

	class AutoExposureOperator : public BaseOperator
	{
	public:
		AutoExposureOperator(void);
		~AutoExposureOperator(void);

		virtual BOOL OnReadSpec();
		virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
		virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
		virtual void OnGetRegisterList(vector<CString> &vecRegister);

		void GetAEROI(SIZE imagesz, ROI &roi);
		
		int auto_exposure(AE_Param *ae);
		int get_next_exposure(double p, double p_last, double target, int reg_delta);

	protected:
		virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

	private:
		//------------------------------------------------------------------------------
		// 参数
		AE_Param m_param;
		ROI ROI_AE;
		UTS::SensorDriver *sensor;
		//------------------------------------------------------------------------------
		// 结果
		double m_dvalue;
		
		//------------------------------------------------------------------------------
	};

	extern "C"
	{
		AUTOEXPOSURE_OPERATOR_API BaseOperator* GetOperator(void);
	}
}
