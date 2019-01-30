#include "StdAfx.h"
#include "UTOP_CheckSN.h"
#include "ImplOperator.h"

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
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strRegStrForCheck"), m_param.strRegStrForCheck, _T(".*"), _T("Regex string for checking SN"));

        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        uts.board.ShowSN(uts.info.strSN);

        wcmatch mr;
        wregex rx(m_param.strRegStrForCheck);
        if (!regex_match((LPTSTR)(LPCTSTR)uts.info.strSN, mr, rx))
        {
            *pnErrorCode = uts.errorcode.E_SNScan;
            return FALSE;
        }

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
