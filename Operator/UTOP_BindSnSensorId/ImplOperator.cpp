#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_BindSnSensorId.h"
#include "otpdb.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;
		m_param.nDeviceIndex = 0;
    }

    ImplOperator::~ImplOperator(void)
    {
    }
    
	BOOL ImplOperator::OnReadSpec()
	{
		CString strSection = OSUtil::GetFileName(m_strModuleFile);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSNFlag"), m_param.nDeviceIndex,0, _T("nSNFlag:0 SensorID,1 Scan SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensorIDFlag"), m_param.nSensorID,0, _T("nSensorIDFlag:0 Get SensorID,1  SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSNExtra"), m_param.nSNExtra,0, _T("nSNExtra:0 no Project,1 Add Project"));
		return TRUE;
	}

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
		//Sensor ID oI strSNIaI?¢G?20171205  
		if (m_param.nDeviceIndex == 0)
		{
			uts.info.strSN.Format(_T("%s-%s"),uts.info.strProjectName,uts.info.strSensorId);
			uts.board.ShowSN(uts.info.strSN);
			uts.board.ShowMsg(EMPTY_STR);            // E!IuIaE?
		}

	    if (m_param.nSensorID == 1)
		{
			uts.info.strSensorId.Format(_T("%s"),uts.info.strSN);
			//uts.board.ShowSN(uts.info.strSN);
			uts.board.ShowMsg(EMPTY_STR);            // E!IuIaE?
		}
	
		if (m_param.nSNExtra == 1)
		{
			uts.info.strSN.Format(_T("%s-%s"),uts.info.strProjectName,uts.info.strSN);
			uts.board.ShowSN(uts.info.strSN);
			uts.board.ShowMsg(EMPTY_STR);   
		}


		uts.log.Info(_T("[SN = %s][SensorId = %s]"), uts.info.strSN, uts.info.strSensorId);
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
			CString readsensorid;

			//Check  sensor id bind or not
			int ret = uts.otpdb->get_sn_by_sid(T2A(uts.info.strSensorId),szNetSn,LEN);
			if (ret > 0)
			{
				if (memcmp(szNetSn,T2A(uts.info.strSN),max(strlen(T2A(uts.info.strSN)),strlen(szNetSn)))!=0 )
				{
					//!!--- 2018.07.27 Chi-Jen.Liao ¨ÌÕu?§Lª©¥»
					uts.log.Error(_T("SID Already Bind SN=[%s]"), A2T(szNetSn));
					*pnErrorCode = uts.errorcode.E_Band;
					
				}
				goto end;
				
			}

			//search 
			int mid = uts.otpdb->GetModuleID(T2A(uts.info.strSN),false);
			if (mid > 0)
			{
				uts.log.Error(_T("Already Bind SN"), uts.info.strSN);
				*pnErrorCode = uts.errorcode.E_SNScan;
				goto end;
			}else
			{
				uts.otpdb->GetModuleID(T2A(uts.info.strSN));
			}

			 // bind it
			if (uts.otpdb->update_sid(T2A(uts.info.strSN), T2A(uts.info.strSensorId)) < 0)
			{
				uts.log.Error(_T("Bind Fail. [SN = %s][SensorId = %s]"), uts.info.strSN, uts.info.strSensorId);
				*pnErrorCode = uts.errorcode.E_DBConnect;
				goto end;
			}
		}else
		{
			*pnErrorCode = uts.errorcode.E_DBConnect;
			goto end;
		}

		//EOA?FuseID


        uts.log.Info(_T("Get Bind SN: [%s]"), uts.info.strSN);
		uts.board.ShowSN(uts.info.strSN);
end:
        // ¡Mu?YErrorcodeEeOA?a1u
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);
		//------------------------------------------------------------------------------
		// ¡Ó¢G¡¦aEy?YIA?t
		SaveData();

        return m_bResult;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_DBConnect);
        vecReturnValue.push_back(uts.errorcode.E_SNScan);
		vecReturnValue.push_back(uts.errorcode.E_Band);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
