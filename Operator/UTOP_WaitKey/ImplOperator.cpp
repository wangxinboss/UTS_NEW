#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_WaitKey.h"

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
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("Çë¼ÌÐø"), _T("Message shown when Testing"));
        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        uts.board.ShowMsg(m_param.strHintMessage);
        uts.keyboard.BeginListen();
        if (uts.info.nWaitKeyType == 0)
        {
            uts.keyboard.WaitSpace(pbIsRunning);
        }
         else
        {
            uts.keyboard.WaitReturn(pbIsRunning);
        }
        uts.keyboard.EndListen();
        *pnErrorCode = uts.errorcode.E_Pass;
        uts.board.ShowMsg(EMPTY_STR);
        return TRUE;
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
          return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
