#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_CheckOCBank.h"

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
		//------------------------------------------------------------------------------
		// 初始化
		USES_CONVERSION;

		UI_MARK uiMark;
		m_TimeCounter.SetStartTime();
		*pnErrorCode = uts.errorcode.E_Pass;
		//------------------------------------------------------------------------------
		
		m_result.inf[0].m_Bank = -1;
		m_result.inf[1].m_Bank = -2;

		// 读取共享数据(R)
		CStringA strHeader, strKey;

		for (int i = 0 ;i < 2 ;i++)
		{
			if( i == 0) strHeader = "right";
			else strHeader = "left";

			strKey.Format("%s_OC Bank", strHeader);
			m_result.inf[i].m_Bank = (int)(uts.info.mapShared[strKey.GetBuffer()]);
		}

		if(m_result.inf[0].m_Bank != m_result.inf[1].m_Bank)
		{
			uts.log.Error(_T("OC Bank Check Error!"));
			*pnErrorCode = uts.errorcode.E_CheckOCBin;
		}



		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		//------------------------------------------------------------------------------
		// 保存数据文件
		SaveData();

		return m_bResult;
	}

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
		vecReturnValue.push_back(uts.errorcode.E_Rotate);
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

		//-------------------------------------------------------------------------
		// Head
		strHeader = _T("Time,SN,DeviceIndex,TestTime(ms),Result,");
		strData.Format(_T("%s,%s,%d,%.1f,%s,"),
			lpTime,
			uts.info.strSN,
			uts.info.nDeviceIndex,
			m_TimeCounter.GetPassTime(),
			strResult);

		//-------------------------------------------------------------------------
		// Body

		strHeader.Append(_T("R OC Bank,L OC Bank"));
		strData.AppendFormat(_T("%d,%d,"), m_result.inf[0].m_Bank,m_result.inf[1].m_Bank);
	
		//-------------------------------------------------------------------------
		// Tail
		strHeader.Append(_T("Version,OP_SN\n"));
		strData.AppendFormat(_T("%s,%s\n"),
			strVersion,
			uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
