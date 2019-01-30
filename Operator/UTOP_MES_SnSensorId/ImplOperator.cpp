#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_MES_SnSensorId.h"
#include "otpdb.h"



#include "PlatformInfo.h"

#import "MESDLL.tlb"
using namespace MESDLL;


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
		m_TimeCounter.SetStartTime();

		uts.log.Info(_T("[SN = %s][SensorId = %s]"), uts.info.strSN, uts.info.strSensorId);


		if(uts.info.strSN.GetLength()<1)  //空白SN判断
		{
			uts.log.Error(_T("strSN Error."));
			*pnErrorCode = uts.errorcode.E_SNScan;
			goto end;
		}

		if(uts.info.strSensorId.Left(6) == _T("000000"))
		{
			uts.log.Error(_T("GetSensorId Error."));
			*pnErrorCode = uts.errorcode.E_Band;
			goto end;
		}


		//if (uts.info.nMesEn != 0)
        {
			CoInitialize(NULL);
			MESDLL::ISQLServerPtr p_SQLServer(_uuidof(SQLServer));
			
			//uts.info.strSN = _T("25410100223181234567891");
			_bstr_t MESCheckResult= p_SQLServer->SendMessageToMES( (_bstr_t)uts.info.strSN);

		    uts.log.Debug(_T("MESCheckResult: %s"), (LPCTSTR)MESCheckResult);

			if(MESCheckResult == (_bstr_t)("OK"))
			{
				uts.log.Debug(_T("SendMessageToMES PASS. SN[%s]"), uts.info.strSN);
				m_bResult =*pnErrorCode = uts.errorcode.E_Pass;
			}
			else
			{
				uts.log.Debug(_T("SendMessageToMES FAIL: %s"), (LPCTSTR)MESCheckResult);
				m_bResult =*pnErrorCode = uts.errorcode.E_Fail;
			}
			CoUninitialize();
        }
        //else
        //{
        //    uts.log.Warning(_T("ShopFlow NOT enable, MES CheckRoute omitted."));
        //    *pnErrorCode = uts.errorcode.E_Pass;
        //}

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

	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		//-------------------------------------------------------------------------
		// Head
		strHeader = _T("Time,SN,TestTime(ms),Result,SN,SensorID,");

		strData.Format(_T("%s,%s,%.1f,%s,%s,%s,"),
			lpTime,
			uts.info.strSN,
			m_TimeCounter.GetPassTime(),
			strResult,
			uts.info.strSN,
			uts.info.strSensorId
			);


		//-------------------------------------------------------------------------
		// Tail
		strHeader.Append(_T("Version,OP_SN\n"));
		strData.AppendFormat(_T("%s,%s\n"),
			strVersion,
			uts.info.strUserId);
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
