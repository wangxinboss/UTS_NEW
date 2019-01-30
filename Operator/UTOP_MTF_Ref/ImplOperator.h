#pragma once
#include "UTSDefine.h"
#include "BaseOperator.h"
#include "Algorithm.h"
#include "AlgorithmNintendo.h"

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::MTF;
using namespace UTS::Algorithm::Image;

#pragma comment(lib, "UTSAlgorithm_Nintendo.lib")


#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
	typedef struct _fov_operator_param_
	{
		int nReCapture;         // 0：不动作  1：进行更换序列、抓图片、保存的动作
		int nMTFShiftMask;
		int nMTFShiftMaskGap;
		POINT ptInfoPos;
		MinMax<double> stRefBlackSpec;
		MinMax<double> stRefWhiteSpec;
		MinMax<double> stBalanceSpec;

		MTF::MTF_Nintendo::MTF_PARAM stMtfParam;
		MinMax<double> stMTFCenterSpec;
		MinMax<double> stMTFFieldSpec;

	} MTF_OPERATOR_PARAM;

	typedef struct _operator_result_
	{
		MTF::MTF_Nintendo::MTF_RESULT stMtfResult;
		double dPeakMTF;
		double dBalanceRTLD;
		double dBalanceLTRD;
	} OPERATOR_RESULT;

	class MTFOperator : public BaseOperator
	{
	public:
		MTFOperator(void);
		~MTFOperator(void);

		virtual BOOL OnReadSpec();
		virtual void OnPreviewStart();
		virtual BOOL OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark);
		virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
		virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
		virtual void OnGetRegisterList(vector<CString> &vecRegister);

	protected:
		virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

	private:
		//------------------------------------------------------------------------------
		// 参数
		MTF_OPERATOR_PARAM m_param;
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
