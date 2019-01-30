#include "StdAfx.h"
#include "LightOffOperator.h"
#include "UTOP_LightOff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    LightOffOperator::LightOffOperator(void)
    {
        OPERATOR_INIT;
    }

    LightOffOperator::~LightOffOperator(void)
    {
    }

    BOOL LightOffOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        if (nullptr == m_pDevice)
        {
            *pnErrorCode = uts.errorcode.E_Fail;
            return FALSE;
        }
        if (!m_pDevice->Stop())
        {
            *pnErrorCode = uts.errorcode.E_Fail;
            return FALSE;
        }

        *pnErrorCode = uts.errorcode.E_Pass;
        return TRUE;
    }

    void LightOffOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new LightOffOperator);
    }
    //------------------------------------------------------------------------------
}
