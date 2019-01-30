#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_DrawCenterROI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




namespace UTS
{
	ImplOperator::ImplOperator(void)
	{
		OPERATOR_INIT;
	}

	ImplOperator::~ImplOperator(void)
	{
	}

	BOOL ImplOperator::OnReadSpec()
	{
		CString strSection = OSUtil::GetFileName(m_strModuleFile);
	
		CString strValue;
		vector<int> vecValue;
		vector<double> vecDoubleValue;

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("RECT"), strValue, _T("160,120,320,240"), _T("ROI(左,上,宽,高"));
		SplitInt(strValue, vecValue);
		m_param.ColorROIPos.left = vecValue[0];
		m_param.ColorROIPos.top = vecValue[1];
		m_param.ColorROIPos.right = vecValue[0] + vecValue[2];
		m_param.ColorROIPos.bottom = vecValue[1] + vecValue[3];
			
	

		return TRUE;
	}

	BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_POINT pt;
		UI_TEXT text;
		UI_TEXT_REL textRel;
		UI_LINE line;

		// ref black
		rc.color = COLOR_YELLOW ;
		memcpy(&rc.rcPos, &m_param.ColorROIPos, sizeof(RECT));
		uiMark.vecUiRect.push_back(rc);
		


		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);

		//------------------------------------------------------------------------------
		// 初始化结果
		

		//------------------------------------------------------------------------------
		// 抓图


		//------------------------------------------------------------------------------


		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);


		return m_bResult;
	}

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
	}

	void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		
	}

	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{

	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
