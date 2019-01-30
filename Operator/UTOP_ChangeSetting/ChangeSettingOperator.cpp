#include "StdAfx.h"
#include "ChangeSettingOperator.h"
#include "UTOP_ChangeSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    ChangeSettingOperator::ChangeSettingOperator(void)
    {
        OPERATOR_INIT;
        m_bMustStopOnFail = TRUE;   // fix bug #11: SN、ChangeSetting失败不受StopOnFail限制。
    }

    ChangeSettingOperator::~ChangeSettingOperator(void)
    {
    }

    BOOL ChangeSettingOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        uts.board.ShowErrorMsg(EMPTY_STR);  // fix bug #8: 结果fail显示PASS，上一次的Errorcode没有刷新
        uts.board.ShowStatus(eBoardStatus::Status_Testing);

        if (nullptr == m_pDevice)
        {
            *pnErrorCode = uts.errorcode.E_NoImage;
             return FALSE;
        }
		m_pDevice->Start(m_strOperatorName);
        //if (!m_pDevice->Start(m_strOperatorName))
        //{
        //    *pnErrorCode = uts.errorcode.E_NoImage;
        //    return FALSE;
        //}

        *pnErrorCode = uts.errorcode.E_Pass;
        return TRUE;
    }

    void ChangeSettingOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
    }

    void ChangeSettingOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ChangeSettingOperator);
    }
    //------------------------------------------------------------------------------
}
