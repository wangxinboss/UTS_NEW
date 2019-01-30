#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_GetOCBank.h"

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

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("DB Name"), m_param.DBName, _T("md702"), _T("DB Name"));
		return TRUE;
	}

	

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		USES_CONVERSION;
		CStringA strHeader, strKey;
		UI_MARK uiMark;
		m_TimeCounter.SetStartTime();
		*pnErrorCode = uts.errorcode.E_Pass;
		m_result.m_Bank = -1;
		//------------------------------------------------------------------------------
		int ret = 0;
		uts.otpdb->get_instance();

		DDM::OtpDBPara otpDBPara = {0};
		otpDBPara.dbType = uts.info.nOtpDBType;

		strcpy_s(otpDBPara.dbname, 0x20, T2A(m_param.DBName));
		strcpy_s(otpDBPara.host, 0x20, T2A(uts.info.strOtpDBHost));
		strcpy_s(otpDBPara.passwd, 0x20, T2A(uts.info.strOtpDBPassword));
		strcpy_s(otpDBPara.user, 0x20, T2A(uts.info.strOtpDBUser));

		uts.otpdb->Close();

		if (uts.otpdb->Connect(otpDBPara) < 0)
		{
			uts.log.Error(_T("Remote otp DB Coonect Fail!!!"));
			*pnErrorCode = uts.errorcode.E_DBConnect;
			goto end;
		}
		else
		{
			uts.log.Info(_T("[%s]Remote otp DB Initialize Success."), UTS_FN);
		}

		int mid = uts.otpdb->GetModuleID(T2A(uts.info.strSN_Single),false);
		if (mid < 0)
		{
			uts.log.Error(_T("update OC bank error!!!"));
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}

		uint8_t tmp[1];
		ret = uts.otpdb->GetOtpByType(mid, 15, (char *)tmp, 1);
		if (ret < 0)
		{
			uts.log.Error(_T("Get OC bank error!!!"));
			*pnErrorCode = uts.errorcode.E_DBConnect;
			goto end;
		}

		m_result.m_Bank = get_le_val( tmp, 1);

		//------------------------------------------------------------------------------
	    // 保存结果到共享(R)
		strHeader = (uts.info.nDeviceIndex == 0 ? "right" : "left");
		strKey.Format("%s_OC Bank", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.m_Bank;
		//------------------------------------------------------------------------------
end:
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
			uts.info.strSN_Single,
			uts.info.nDeviceIndex,
			m_TimeCounter.GetPassTime(),
			strResult);

		//-------------------------------------------------------------------------
		// Body

		strHeader.Append(_T("OC Bank,"));
		strData.AppendFormat(_T("%d,"), m_result.m_Bank);
	
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
