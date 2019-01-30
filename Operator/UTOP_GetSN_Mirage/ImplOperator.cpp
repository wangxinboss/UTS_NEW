#include "StdAfx.h"
#include "UTOP_GetSN.h"
#include "ImplOperator.h"
#include "EEProm.h"
#include "CommonFunc.h"
#include "otpdb.h"

#pragma comment(lib, "UTS_EEProm.lib")
#pragma comment(lib, "CommonFunc.lib")

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
		char *data = new char[20];
		memset(data,0,sizeof(char)*20);

		USES_CONVERSION;
		*pnErrorCode = uts.errorcode.E_Pass;

		if(uts.info.nOtpDBType == 0)
		{
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

			if (!uts.otpdb->IsOtpDbConnected())
			{
				uts.log.Error(_T("Otp Database is not connected."));
				*pnErrorCode = uts.errorcode.E_DBConnect;
				AfxMessageBox(_T("Otp Database is not connected, Please restart application."));
				goto end;
			}

			const int LEN = 255;
			char szNetSn[LEN] = {0};
			USES_CONVERSION;

			if (uts.otpdb->get_sn_by_sid(T2A(uts.info.strSensorId), szNetSn, LEN) < 0)
			{

				uts.log.Error(_T("Bind Fail. [SN = %s][SensorId = %s]"), uts.info.strSN_Single, uts.info.strSensorId);
				*pnErrorCode = uts.errorcode.E_GetSN;
				goto end;
			}
			else
			{
				// find in db, use it
				uts.info.strSN_Single = A2T(szNetSn);
			}
		}else
		{
			*pnErrorCode = uts.errorcode.E_DBConnect;
			goto end;
		}
		
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
		vecReturnValue.push_back(uts.errorcode.E_DBConnect);
		vecReturnValue.push_back(uts.errorcode.E_GetSN);
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
			uts.info.strSN_Single,
			m_TimeCounter.GetPassTime(),
			strResult);

		// peak
		strHeader.Append(_T("SensorID,"));
		strData.AppendFormat(_T("%s,"), uts.info.strSensorId);

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
