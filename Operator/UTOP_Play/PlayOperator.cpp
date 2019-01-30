#include "StdAfx.h"
#include "PlayOperator.h"
#include "UTOP_Play.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    PlayOperator::PlayOperator(void)
    {
        OPERATOR_INIT;
    }


    PlayOperator::~PlayOperator(void)
    {
    }

    BOOL PlayOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // ÇÐ»»SensorÐòÁÐ
        CString strRegName = m_strOperatorName;
        if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
            strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
        {
            uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
            *pnErrorCode = uts.errorcode.E_Fail;
            return FALSE;
        }

        uts.board.ShowMsg(_T("Press Space"));
        uts.keyboard.BeginListen();
        while (*pbIsRunning && !uts.keyboard.IsSpacePress())
        {
            if (!m_pDevice->Recapture(m_bufferObj))
            {
                *pnErrorCode = uts.errorcode.E_NoImage;
                return FALSE;
            }
            m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);
        }
        uts.keyboard.EndListen();

        *pnErrorCode = uts.errorcode.E_Pass;
        uts.board.ShowMsg(EMPTY_STR);
        return TRUE;
    }

    void PlayOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
    }

    void PlayOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new PlayOperator);
    }
    //------------------------------------------------------------------------------
}
