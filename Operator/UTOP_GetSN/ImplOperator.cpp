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
 
        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
		UTS::EEPROMDriver *eeprom;
		char *data = new char[20];
		memset(data,0,sizeof(char)*20);

		USES_CONVERSION;
		*pnErrorCode = uts.errorcode.E_Pass;

		if (0 == _tcscmp(uts.info.strProjectName, _T("CA188")))
		{
			eeprom = GetEEPromDriverInstance(m_pDevice, 5);
			eeprom->Read(0x08, data, 16);
			uts.info.strSN.Format(_T("%s"),A2T(data));
		}else
		{
			if(uts.info.nOtpDBType == 0)
			{
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

					uts.log.Error(_T("Bind Fail. [SN = %s][SensorId = %s]"), uts.info.strSN, uts.info.strSensorId);
					*pnErrorCode = uts.errorcode.E_GetSN;
					goto end;
				}
				else
				{
					// find in db, use it
					uts.info.strSN = A2T(szNetSn);
				}
			}else
			{
				*pnErrorCode = uts.errorcode.E_DBConnect;
				goto end;
			}
		}

		uts.board.ShowSN(uts.info.strSN);

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
			uts.info.strSN,
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
