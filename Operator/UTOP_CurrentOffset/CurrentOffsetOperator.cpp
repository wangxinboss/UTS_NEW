#include "StdAfx.h"
#include "CurrentOffsetOperator.h"
#include "UTOP_CurrentOffset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    CurrentOffsetOperator::CurrentOffsetOperator(void)
    {
        OPERATOR_INIT;
    }

    CurrentOffsetOperator::~CurrentOffsetOperator(void)
    {
    }

    BOOL CurrentOffsetOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        CStringA strHeader, strKey;
        int nArrDynamicCurrentOffset[6] = {0};
        int nArrStandbyCurrentOffset[6] = {0};
        //------------------------------------------------------------------------------
        // 测量Dynamic Offset值
        uts.board.ShowMsg(_T("Get OP Offset"));
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_CURRENT_DYNAMIC_OFFSET,
            nArrDynamicCurrentOffset, sizeof(nArrDynamicCurrentOffset)))
        {
            uts.log.Error(_T("Device ReadValue DRVT_CURRENT_DYNAMIC_OFFSET Error."));
            *pnErrorCode = uts.errorcode.E_Current;
            return FALSE;
        }
        for (int i = 0; i < ARRAYSIZE(nArrDynamicCurrentOffset); i++)
        {
            uts.log.Debug(_T("DynamicCurrentOffset[%d] = %d"), i, nArrDynamicCurrentOffset[i]);
            strKey.Format("%d_DynamicCurrentOffset[%d]", uts.info.nDeviceIndex, i);
            uts.info.mapShared[strKey.GetBuffer()] = nArrDynamicCurrentOffset[i];
        }
        
        ////------------------------------------------------------------------------------
        //// 切换Sensor序列
        //uts.board.ShowMsg(_T("Load ST Register"));
        //CString strRegName = _T("STANDBY_SET");
        //if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
        //    strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
        //{
        //    uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
        //    *pnErrorCode = uts.errorcode.E_Fail;
        //    return FALSE;
        //}
        //------------------------------------------------------------------------------
        // 测量Standby Offset值
        uts.board.ShowMsg(_T("Get ST Offset"));
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_CURRENT_STANDBY_OFFSET,
            nArrStandbyCurrentOffset, sizeof(nArrStandbyCurrentOffset)))
        {
            uts.log.Error(_T("Device ReadValue DRVT_CURRENT_STANDBY_OFFSET Error."));
            *pnErrorCode = uts.errorcode.E_Current;
            return FALSE;
        }
        for (int i = 0; i < ARRAYSIZE(nArrStandbyCurrentOffset); i++)
        {
            uts.log.Debug(_T("StandbyCurrentOffset[%d] = %d"), i, nArrStandbyCurrentOffset[i]);
            strKey.Format("%d_StandbyCurrentOffset[%d]", uts.info.nDeviceIndex, i);
            uts.info.mapShared[strKey.GetBuffer()] = nArrStandbyCurrentOffset[i];
        }
        return TRUE;
    }

    void CurrentOffsetOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_Current);
    }

    //void CurrentOffsetOperator::OnGetRegisterList(vector<CString> &vecRegister)
    //{
    //    vecRegister.clear();
    //    vecRegister.push_back(_T("STANDBY_SET"));
    //}

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new CurrentOffsetOperator);
    }
    //------------------------------------------------------------------------------
}
