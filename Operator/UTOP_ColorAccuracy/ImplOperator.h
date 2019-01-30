#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"


#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::ColorUniformity;
using namespace UTS::Algorithm::ColorUniformity::CtColorUniformity;

namespace UTS
{
	typedef struct ColorBlock_REF_param_
	{
		RECT ColorROIPos;
		RGBTRIPLE ColorValue;
	} COLORBLOCK_PARAM;

	typedef struct _operator_param_
	{
		COLORBLOCK_PARAM ColorBlock[3];
		

		int nReCapture;             // 0：不动作  1：进行更换序列、抓图片、保存的动作  
		MinMax<double> dDeltaSpec;

	} OPERATOR_PARAM;

	typedef struct _operator_result_
	{
		RGBTRIPLE rgb;
		double Delta;
	}CU_Result;

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

	private:
		//------------------------------------------------------------------------------
		// 参数
		OPERATOR_PARAM m_param;
		// 结果
		CU_Result m_CUResult[3];


		//------------------------------------------------------------------------------
	};

	extern "C"
	{
		OPERATOR_API BaseOperator* GetOperator(void);
	}
}

