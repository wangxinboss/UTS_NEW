#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_MeasureCurrentInit.h"
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

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Com Port"), m_param.iComport, 12, _T("Com Port"));

		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
		
		COM_PARAM com_param;

		com_param.nportNo = m_param.iComport;
		com_param.nbaud = 9600;
		com_param.ndatabits = 8;
		com_param.nparity = NOPARITY;
		com_param.nstopbits = TWOSTOPBITS;

		uts.COM_RS232.ClosePort();

		// 打开串口
		BOOL bRet = uts.COM_RS232.OpenPort(
			&theApp,
			256,    // nlenInput
			256,    // nlenOutput
			com_param.nportNo,
			com_param.nbaud,
			com_param.nparity,
			com_param.ndatabits,
			com_param.nstopbits);
		if (!bRet)
		{
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}

		if (!uts.COM_RS232.GetComOpened())
		{
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}

		/*
		*cls
		conf:curr:dc 0.1
		curr:dc:nplc 0.02
		zero:auto 0
		trig:sour imm 
		trig:del 0
		trig:coun 1
		SYST:REM
		samp:coun 1
		READ?
		*/
		char cmmmand[300];
		int j = 0 ;

		memset(cmmmand,0,300*sizeof(char));
		j = sprintf(cmmmand,"*cls\n");
		j += sprintf(cmmmand+j,"conf:curr:dc 0.1\n");
		j += sprintf(cmmmand+j,"curr:dc:nplc 0.02\n");
		j += sprintf(cmmmand+j,"zero:auto 0\n");
		j += sprintf(cmmmand+j,"trig:sour imm\n");
		j += sprintf(cmmmand+j,"trig:del 0\n");
		j += sprintf(cmmmand+j,"trig:coun 1\n");
		j += sprintf(cmmmand+j,"syst:rem\n");
		j += sprintf(cmmmand+j,"samp:coun 1\n");
	
		uts.COM_RS232.WriteSyncPort(cmmmand, strlen(cmmmand));

		//uts.COM_RS232.ReadSyncPort(buff, 2000);

		*pnErrorCode = uts.errorcode.E_Pass;

		uts.log.Debug(_T("Finish RS232 initial"));
	
		//------------------------------------------------------------------------------
end:
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
