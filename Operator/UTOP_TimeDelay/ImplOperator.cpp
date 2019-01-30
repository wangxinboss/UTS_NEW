#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_TimeDelay.h"

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("Time delay"), _T("Message shown when previewing"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDelayTime"), m_param.delayTime, 0, _T("delay time."));
        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
		UTS::HighPrecisionTimer timeCounter;
		timeCounter.SetStartTime();
		do
        {
            if (!m_pDevice->Recapture(m_bufferObj))
            {
                *pnErrorCode = uts.errorcode.E_NoImage;
                goto end;
            }
 
            m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, nullptr);

        } while (*pbIsRunning && timeCounter.GetPassTime() <  m_param.delayTime);
        
		timeCounter.EndCollectTime();
        return TRUE;
end:
		timeCounter.EndCollectTime();
		return FALSE;
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
