#include "StdAfx.h"
#include "UTOP_SN.h"
#include "ImplOperator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDP_SOCKETS_INIT_FAILED         103

BOOL WriteIniData(CString sSection, CString sSectionKey, CString nValue,CString sIniFileName)
{
	return WritePrivateProfileString(sSection,sSectionKey,nValue,sIniFileName);
}
//--------------------------

int ReadIniData(CString sSection,CString sSectionKey,int nDefault,CString sIniFileName)
{
	return GetPrivateProfileInt(sSection,sSectionKey,nDefault,sIniFileName);
}

namespace UTS
{
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;
        m_bMustStopOnFail = TRUE;   // fix bug #11: SN、LightOn失败不受StopOnFail限制。

		if (!AfxSocketInit())
		{
			AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
			return;
		}

		sIniFileName = _T("D:\\ColorTest\\123.ini");

		m_ClientSocket.Create();
		if(m_ClientSocket.Connect(_T("127.0.0.1"), 5000))
		{
			uts.log.Info(_T("Connect the Server Successful"));
		}
		else
		{
			uts.log.Error(_T("Connect the Server Failed"));
		}
    }

    ImplOperator::~ImplOperator(void)
    {
    }

    BOOL ImplOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("IP"), m_param.IP, _T("127.0.0.1"), _T("Message shown when wait scan SN"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("Color"), m_param.Port, _T("3100"), _T("Regex string for checking SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("1"), _T("Regex string for checking SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strRegStrForCheck"), m_param.strRegStrForCheck, _T(".*"), _T("Regex string for checking SN"));

        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		ColorTestEn_3100 = ReadIniData(_T("Test3Color"),_T("ColorTestEn_3100"),0, sIniFileName);
		ColorTestEn_4000 = ReadIniData(_T("Test3Color"),_T("ColorTestEn_4000"),0, sIniFileName);
		ColorTestEn_5100 = ReadIniData(_T("Test3Color"),_T("ColorTestEn_5100"),0, sIniFileName);

		if (ColorTestEn_3100 == 1 && m_param.Port == _T("3100"))
		{
			//Sleep(2000);
			OnSendbutton(m_param.Port);
		}
		else if (ColorTestEn_4000 == 1 && m_param.Port == _T("4000"))
		{
			Sleep(2000);
			OnSendbutton(m_param.Port);
		}
		else if (ColorTestEn_5100 == 1 && m_param.Port == _T("5100"))
		{
			Sleep(2000);
			OnSendbutton(m_param.Port);
		}
		
        *pnErrorCode = uts.errorcode.E_Pass;
        return TRUE;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_SNScan);
    }

	void ImplOperator::OnSendbutton(CString strMsg) 
	{
		// TODO: Add your control notification handler code here
		int nLen;

		strMsg.TrimLeft();
		strMsg.TrimRight();

		if(strMsg != "")
		{
			nLen = strMsg.GetLength();
			m_ClientSocket.Send(strMsg,nLen);
			Sleep(100);
		}
	}

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
