#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm_Smartisan;
using namespace UTS::Algorithm_Smartisan::ColorShading_Smartisan;


#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif


 
namespace UTS
{
	typedef struct _ri_operator_param_
	{
		double dLTMinY;         // 白板测试最小Y值
		double dLTMaxY;         // 白板测试最大Y值
		int nReCapture;         // 0：不动作  1：进行更换序列、抓图片、保存的动作   
		//MinMax<double> dColorShadingSpec;
		double dRG_Delta_Spec;
		double dBG_Delta_Spec;
		double dRG_Golden_Delta_Spec;
		double dBG_Golden_Delta_Spec;
		double dRG_Golden;
		double dBG_Golden;
	} ColorShading_OPERATOR_PARAM;

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

	private:
		//------------------------------------------------------------------------------
		// 参数
		ColorShading_OPERATOR_PARAM m_param;
		//------------------------------------------------------------------------------
		// 结果
		ColorShading_RESULT m_result;
		double m_dYvalue;
		//------------------------------------------------------------------------------
	};

	extern "C"
	{
		OPERATOR_API BaseOperator* GetOperator(void);
	}
}
