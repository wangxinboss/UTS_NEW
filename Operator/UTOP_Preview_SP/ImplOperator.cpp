#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_Preview_SP.h"
#include "VCM.h"

#pragma comment(lib, "UTS_VCM.lib")

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
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("Adjust position"), _T("Message shown when previewing"));
        CString strOperatorList;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strOperatorList"), strOperatorList, _T("UTOP_Empty,UTOP_Empty"), _T("Operator name list, like: UTOP_001, UTOP002, ..."));
        SplitString(strOperatorList, m_param.vecOperator);

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nAvgFrame"), m_param.nAvgFrame, 1, _T("Avg Frame"));

        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 切换Sensor序列 
        CString strRegName = m_strOperatorName;
        if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
            strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
        {
            uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
            *pnErrorCode = uts.errorcode.E_Fail;
            return FALSE;
        }

        uts.board.ShowMsg(m_param.strHintMessage);

        BUFFER_INFO bfInfo;
        m_pDevice->GetBufferInfo(bfInfo);

        // Preview开始前，读取序列，并执行OnPreviewStart方法
        vector<OPERATOR_INFO>::iterator itorOp = uts.flow.m_vecOpList.begin();
        for (; itorOp != uts.flow.m_vecOpList.end(); itorOp++)
        {
            //-------------------------------------------------------------------------
            // 不在指定列表中，跳过
            BOOL bFind = FALSE;
            vector<CString>::iterator itorOpSp = m_param.vecOperator.begin();
            for (; itorOpSp != m_param.vecOperator.end(); itorOpSp++)
            {
                if (OSUtil::GetMainFileName((*itorOp).strFileName).CompareNoCase(*itorOpSp) == 0)
                {
                    bFind = TRUE;
                }
            }
            if (!bFind)
            {
                continue;
            }
            //-------------------------------------------------------------------------
            BOOL bResult = (*itorOp).pInstance->OnReadSpec();
            if (!bResult)
            {
                uts.log.Error(_T("%s OnReadSpec Error."), (*itorOp).strFileName);
                break;
            }
            (*itorOp).pInstance->OnPreviewStart();
        }
        
        uts.keyboard.BeginListen();
        BOOL bKeyPress = FALSE;
        do
        {
            if (!m_pDevice->Recapture(m_bufferObj,0,m_param.nAvgFrame))
            {
             //   *pnErrorCode = uts.errorcode.E_NoImage;
              //  uts.board.ShowMsg(EMPTY_STR);
              //  uts.keyboard.EndListen();
               // return FALSE;
				continue;
            }
            //------------------------------------------------------------------------------
            // 调用当前Op之后的每个Op的Preview，获取UI Mark
            UI_MARK uiMark;
            vector<OPERATOR_INFO>::iterator itorOp = uts.flow.m_vecOpList.begin();
            for (; itorOp != uts.flow.m_vecOpList.end(); itorOp++)
            {
                //-------------------------------------------------------------------------
                // 不在指定列表中，跳过
                BOOL bFind = FALSE;
                vector<CString>::iterator itorOpSp = m_param.vecOperator.begin();
                for (; itorOpSp != m_param.vecOperator.end(); itorOpSp++)
                {
                    if (OSUtil::GetMainFileName((*itorOp).strFileName).CompareNoCase(*itorOpSp) == 0)
                    {
                        bFind = TRUE;
                    }
                }
                if (!bFind)
                {
                    continue;
                }

                //-------------------------------------------------------------------------
                BOOL bResult = (*itorOp).pInstance->OnPreview(
                    m_bufferObj.pBmpBuffer,
                    bfInfo.nWidth,
                    bfInfo.nHeight,
                    uiMark);
                if (!bResult)
                {
                    uts.log.Error(_T("%s OnPreview Error."), (*itorOp).strFileName);
                    break;
                }

                bResult = (*itorOp).pInstance->OnPreview(
                    m_bufferObj,
                    bfInfo.nWidth,
                    bfInfo.nHeight,
                    uiMark);
                if (!bResult)
                {
                    uts.log.Error(_T("%s OnPreview Error."), (*itorOp).strFileName);
                    break;
                }
            }
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
        uts.keyboard.EndListen();

        // Preview结束后，执行OnPreviewStop方法
        itorOp = uts.flow.m_vecOpList.begin();
        for (; itorOp != uts.flow.m_vecOpList.end(); itorOp++)
        {
            //-------------------------------------------------------------------------
            // 不在指定列表中，跳过
            BOOL bFind = FALSE;
            vector<CString>::iterator itorOpSp = m_param.vecOperator.begin();
            for (; itorOpSp != m_param.vecOperator.end(); itorOpSp++)
            {
                if (OSUtil::GetMainFileName((*itorOp).strFileName).CompareNoCase(*itorOpSp) == 0)
                {
                    bFind = TRUE;
                }
            }
            if (!bFind)
            {
                continue;
            }
            //-------------------------------------------------------------------------
            (*itorOp).pInstance->OnPreviewStop();
        }

        *pnErrorCode = uts.errorcode.E_Pass;
        uts.board.ShowMsg(EMPTY_STR);
        return TRUE;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
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
