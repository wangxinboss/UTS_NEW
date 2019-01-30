#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_ChangeEnv.h"

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
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("切换到XX"), _T("Message shown when Testing"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dMinY"), m_param.dMinY, 0.0, _T("Min Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dMaxY"), m_param.dMaxY, 255.0, _T("Max Y value"));
        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        // 重新设定Sensor序列
        CString strRegName = m_strOperatorName;
        if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
            strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
        {
            uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
            *pnErrorCode = uts.errorcode.E_Fail;
            m_bResult = FALSE;
            return m_bResult;
        }

        uts.board.ShowMsg(m_param.strHintMessage);

        m_pDevice->GetBufferInfo(m_bufferInfo);

        m_pDevice->Recapture(m_bufferObj, 1);

        uts.keyboard.BeginListen();
        BOOL bKeyPress = FALSE;
        double dYavg = 0.0;
        do
        {
            if (!m_pDevice->Recapture(m_bufferObj))
            {
                *pnErrorCode = uts.errorcode.E_NoImage;
                goto end;
            }
            UI_MARK uiMark;
            // 获取画面平均亮度
            if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_YUV24) != 0)
            {
                UTS::Algorithm::Image::CImageProc::GetInstance().GetYUV24_YBuffer(
                    m_bufferObj.pYuv24Buffer,
                    m_bufferInfo.nWidth,
                    m_bufferInfo.nHeight,
                    m_bufferObj.pYBuffer);

                // 画面亮度
                RECT rcWholeImage = {0, 0, m_bufferInfo.nWidth, m_bufferInfo.nHeight};
                UTS::Algorithm::GetBlockAvgY(
                    m_bufferObj.pYBuffer,
                    m_bufferInfo.nWidth,
                    m_bufferInfo.nHeight,
                    rcWholeImage,
                    dYavg);
            }
            else
            {
                UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, dYavg);
            }
            UI_TEXT uiText;
            uiText.color = COLOR_YELLOW;
            uiText.ptPos = CPoint(m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight / 4);
            uiText.strText.Format(_T("Yavg: %.1f"), dYavg);
            uiMark.vecUIText.push_back(uiText);
            // 屏幕绘制，with UI Mark
            m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
            if (uts.info.nWaitKeyType == 0)
            {
                bKeyPress = uts.keyboard.IsSpacePress();
            }
            else
            {
                bKeyPress = uts.keyboard.IsReturnPress();
            }
        } while (*pbIsRunning && !bKeyPress);

        // 检查测试结果
        if (m_param.dMinY <= dYavg && dYavg <= m_param.dMaxY)
        {
            *pnErrorCode = uts.errorcode.E_Pass;
        }
        else
        {
            *pnErrorCode = uts.errorcode.E_Linumance;
        }

end:
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        uts.keyboard.EndListen();
        uts.board.ShowMsg(EMPTY_STR);

        return m_bResult;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
    }

    void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
