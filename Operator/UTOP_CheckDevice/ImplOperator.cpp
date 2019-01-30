#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_CheckDevice.h"
#include "FindDevicePidVid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace UTS
{
	ImplOperator::ImplOperator(void)
	{
		OPERATOR_INIT;
		memset(&m_result,0,sizeof(OPERATOR_RESULT));
	}

	ImplOperator::~ImplOperator(void)
	{
	}

	BOOL ImplOperator::OnReadSpec()
	{
		CString strValue;
		vector<int> vecValue;
		vector<double> vecDoubleValue;

		CString strSection = OSUtil::GetFileName(m_strModuleFile);

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iTryCount"), m_param.iTryCount, 20, _T("TryCount"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("VID"), m_param.VID, _T("0BDA"), _T("VID"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("PID"), m_param.PID, _T("5806"), _T("PID"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("REV"), m_param.REV, _T("0100"), _T("REV"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("WaitDeviceTime"), m_param.iWaitDeviceTime,100, _T("Wait device ready Time"));
		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		UI_MARK uiMark;
		m_TimeCounter.SetStartTime();
	
		*pnErrorCode = uts.errorcode.E_Pass;
		//------------------------------------------------------------------------------
		char NOW_VID[5]={0};
		char NOW_PID[5]={0};
		char NOW_REV[5]={0};
		CHAR  CorrectUSBDeviceID[MAX_PATH];
		BOOL FWCheck = FALSE;
		USES_CONVERSION;

		sprintf_s(CorrectUSBDeviceID, "Vid_%s&Pid_%s&Rev_%s",T2A(m_param.VID),T2A(m_param.PID),T2A(m_param.REV));
	 
		for (int i = 0 ;i< m_param.iTryCount ;i++)
		{
			FWCheck=FindAllDeviceExist(CorrectUSBDeviceID,NOW_VID,NOW_PID,NOW_REV);
			
			if( FWCheck)
			{
				Sleep(m_param.iWaitDeviceTime); //wait device ready 
				break;
			}else 
			{
				Sleep(500);
				uts.log.Error(_T("Now VID: %s PID: %s REV: %s"),A2W(NOW_VID),A2W(NOW_PID),A2W(NOW_REV));
			}
		}

		if( FWCheck == FALSE)
		{
			uts.log.Error(_T("Now VID: %s PID: %s REV: %s"),A2W(NOW_VID),A2W(NOW_PID),A2W(NOW_REV));
			*pnErrorCode = uts.errorcode.E_Fail;
		}

		
		//------------------------------------------------------------------------------
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		return m_bResult;
	}

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		
	}

	void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		
	}

	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		//-------------------------------------------------------------------------
		// Head
		strHeader = _T("Time,SN,TestTime(ms),Result,");
		strData.Format(_T("%s,%s,%.1f,%s,"),
			lpTime,
			uts.info.strSN,
			m_TimeCounter.GetPassTime(),
			strResult);


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
