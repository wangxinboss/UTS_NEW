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
    }

    ImplOperator::~ImplOperator(void)
    {
    }
    
    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
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
					uts.log.Error(_T("SID repeat"), uts.info.strSN);
					*pnErrorCode = uts.errorcode.E_Band;
				}
				goto end;
			}

			//search 
			int mid = uts.otpdb->GetModuleID(T2A(uts.info.strSN),false);
			if (mid > 0)
			{
				uts.log.Error(_T("SN repeat"), uts.info.strSN);
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

        uts.log.Info(_T("Get Bind SN: [%s]"), uts.info.strSN);
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
