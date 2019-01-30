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
		RECT WhitePatchROI;
		double dLTMinY;         // 白板测试最小Y值
		double dLTMaxY;         // 白板测试最大Y值
		int nXPixelCountSpec;
		int nYPixelCountSpec;
		double dPixel_Size;
		int nBank_En;
		int nBank0PixelsSpec;
	} OPERATOR_PARAM;

	typedef struct tagfPOINT
	{
		double  x;
		double  y;
	} dPOINT;

	typedef struct _awb_operator_result_
	{
		double dCenterYavg;
		dPOINT m_ptOcCenter;	
		double m_OffsetX;
		double m_OffsetY;
		int m_Bank;
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

		BOOL  is_otp_data_locked(int module_id);
		BOOL is_otp_programed(int module_id);
	protected:
		virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

	private:
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
