#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "AlgorithmNintendo.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::Image;
#pragma comment(lib, "UTSAlgorithm_Nintendo.lib")

#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
	typedef struct _operator_param_
	{
		double dShiftXSpec[TypeCount];
		double dShiftYSpec[TypeCount];
		double dScaleSpec[TypeCount];
		double dAngleSpec[TypeCount];
	} OPERATOR_PARAM;

	typedef struct _operator_result_
	{
		POINTFLOAT ptPL[4];
		POINTFLOAT ptSL[4];
		POINTFLOAT ptQL[4];

		POINTFLOAT ptPR[4];
		POINTFLOAT ptSR[4];
		POINTFLOAT ptQR[4];

		double dShiftX;
		double dShiftY;
		double dScale;
		double dAngle;
	} OPERATOR_RESULT;

	class ImplOperator : public BaseOperator
	{
	public:
		ImplOperator(void);
		~ImplOperator(void);

		virtual BOOL OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark);
		virtual BOOL OnReadSpec();
		virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
		virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);

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

