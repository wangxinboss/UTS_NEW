#include "StdAfx.h"
#include "LightOnOperator.h"
#include "UTOP_LightOn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    LightOnOperator::LightOnOperator(void)
    {
        OPERATOR_INIT;
        m_bMustStopOnFail = TRUE;   // fix bug #11: SN、LightOn失败不受StopOnFail限制。
    }

    LightOnOperator::~LightOnOperator(void)
    {
    }

    BOOL LightOnOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        uts.board.ShowErrorMsg(EMPTY_STR);  // fix bug #8: 结果fail显示PASS，上一次的Errorcode没有刷新
        uts.board.ShowStatus(eBoardStatus::Status_Testing);

        if (nullptr == m_pDevice)
        {
            *pnErrorCode = uts.errorcode.E_NoImage;
            return FALSE;
        }
        if (!m_pDevice->Start(m_strOperatorName))
        {
            *pnErrorCode = uts.errorcode.E_NoImage;
            return FALSE;
        }

        *pnErrorCode = uts.errorcode.E_Pass;
        return TRUE;
    }

    void LightOnOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
    }

    void LightOnOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new LightOnOperator);
    }
    //------------------------------------------------------------------------------
}
