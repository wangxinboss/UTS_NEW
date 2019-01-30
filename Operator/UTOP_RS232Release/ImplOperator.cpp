#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_RS232Release.h"
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

		

		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
		
	
		if(uts.COM_RS232.GetComOpened() == TRUE)
		{
			uts.COM_RS232.ClosePort();
		}
		

		*pnErrorCode = uts.errorcode.E_Pass;

		uts.log.Debug(_T("Finish RS232 Release"));
	
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
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		strHeader = _T("Time,SN,TestTime(ms),_Result,")

			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%s,")

			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult

			, strVersion, uts.info.strUserId);
	}
	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
