#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_SwitchDevice.h"

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
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDeviceIndex"), m_param.nDeviceIndex, 0, _T("Index of device."));
        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_SWITCH_DEVICE_INDEX,
            &m_param.nDeviceIndex, sizeof(int)))
        {
            uts.log.Error(_T("Device WriteValue DWVT_SWITCH_DEVICE_INDEX [%s] Error."), m_param.nDeviceIndex);
            *pnErrorCode = uts.errorcode.E_Fail;
        }
        else
        {
            uts.info.nDeviceIndex = m_param.nDeviceIndex;
            uts.log.Info(_T("Switch device index to [%d]."), m_param.nDeviceIndex);
            *pnErrorCode = uts.errorcode.E_Pass;
        }
        
        return TRUE;
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
