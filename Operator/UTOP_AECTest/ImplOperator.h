#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#include "BaseOtp.h"

#include "Algorithm.h"
using namespace UTS::Algorithm;
using namespace UTS::Algorithm::FixPatternNoise::FixPatternNoise_WD;

#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

typedef enum _enum_image_type
{
	Image_TYPE_BMP = 1,
	Image_TYPE_RAW8,
	Image_TYPE_RAW10,
	Image_TYPE_RAW8BMP,
	Image_TYPE_MAXCount,
} eSfrType;


namespace UTS
{
	typedef struct _operator_param_
	{
		double dLTMinG;
		double dLTMaxG;
		int SensorBlack_level;
		ROI roi;

		double dFPNSpec;
		int nSaveOrReadGr;
		int nTestIndex;
		int nSaveImage;
	} OPERATOR_PARAM;

	typedef struct _awb_operator_result_
	{
	  float TestGrValue;
	  float ReadGrValue;
	  int nBrightNessRatio;
	} OPERATOR_RESULT;

	class ImplOperator : public BaseOperator
	{
	public:
		ImplOperator(void);
		~ImplOperator(void);

		virtual BOOL OnReadSpec();
		virtual BOOL OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark);
		virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
		virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
		virtual void OnGetRegisterList(vector<CString> &vecRegister);
	protected:
		virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

		BOOL UploadData(BYTE *data, int nLength, int nType);

	private:
		//------------------------------------------------------------------------------
		// 参数
		OPERATOR_PARAM m_param;
		//------------------------------------------------------------------------------
		// 结果
		OPERATOR_RESULT m_result;
		FPNInfo m_FPNInfo;
		int m_mid;
		BYTE m_data[10];
		UTS::SensorDriver *sensor;
		int m_nExposureValue;
		//------------------------------------------------------------------------------
	};

	extern "C"
	{
		OPERATOR_API BaseOperator* GetOperator(void);
	}
}
