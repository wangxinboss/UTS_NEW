#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_StopAux.h"
#include "serialport.h"

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strWindown2Title"), m_param.strWindown2Title, _T("Test2"), _T("test app2 windown title"));

		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
		
		HWND hParamentWnd = NULL;

		//找到第二个测试程序窗口，发送STOP消息
		hParamentWnd = FindWindow(NULL,m_param.strWindown2Title);
		if(NULL != hParamentWnd)
		{
			DWORD dwID = 32773;
			//PostMessage(hParamentWnd,WM_COMMAND,MAKEWPARAM(dwID,BN_CLICKED),0);
			PostMessage(hParamentWnd,WM_COMMAND,dwID,0);
			uts.log.Info(_T("PostMessage to %d"),dwID);

		}
		else
		{
			uts.log.Error(_T("Can not find TestApp 2"));
		}
		

		*pnErrorCode = uts.errorcode.E_Pass;
	
		//------------------------------------------------------------------------------
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		// 保存数据文件
		//SaveData();

		return m_bResult;
	}

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NVMWrite);
	}

	void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		vecRegister.push_back(m_strOperatorName);
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
