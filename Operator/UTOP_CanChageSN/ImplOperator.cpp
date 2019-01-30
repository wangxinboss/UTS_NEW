#include "StdAfx.h"
#include "UTOP_CanChageSN.h"
#include "ImplOperator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;
        m_bMustStopOnFail = TRUE;   // fix bug #11: SN、LightOn失败不受StopOnFail限制。
    }

    ImplOperator::~ImplOperator(void)
    {
    }

    BOOL ImplOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("Scan SN"), _T("Message shown when wait scan SN"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strRegStrForCheck"), m_param.strRegStrForCheck, _T(".*"), _T("Regex string for checking SN"));

        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        uts.board.ShowMsg(m_param.strHintMessage); // 提示输入SN
        uts.info.strSN = EMPTY_STR;
        uts.board.ShowSN(EMPTY_STR);
        uts.keyboard.BeginListen();
        uts.keyboard.WaitReturn(pbIsRunning);
		uts.board.ShowSN(uts.info.strSN);

		uts.keyboard.WaitSpace(pbIsRunning);
        uts.keyboard.EndListen();
        uts.log.Debug(_T("Get SN [%s]"), uts.info.strSN);
        
        uts.board.ShowMsg(EMPTY_STR);              // 取消提示

        wcmatch mr;
        wregex rx(m_param.strRegStrForCheck);
        if (!regex_match((LPTSTR)(LPCTSTR)uts.info.strSN, mr, rx))  // fig bug #17: SN长度超长时不起管控作用，例如{8,8}，应该输入11位，但是输入11位以上都可以测试
        {
            *pnErrorCode = uts.errorcode.E_SNScan;
            return FALSE;
        }

		if(wcslen(uts.info.strSN) > 16)
		{
			CString str;
			str.Format(_T("%s-"),uts.info.strSN);
			uts.info.strSN = str;
			uts.log.Debug(_T("Get SN [%s]"), uts.info.strSN);
		}

		

        uts.board.ShowErrorMsg(EMPTY_STR);
        *pnErrorCode = uts.errorcode.E_Pass;
        return TRUE;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_SNScan);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
