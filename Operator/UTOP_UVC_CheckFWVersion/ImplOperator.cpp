#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_UVC_CheckFWVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;
		m_bMustStopOnFail = TRUE;
    }

    ImplOperator::~ImplOperator(void)
    {
    }

    BOOL ImplOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("Set property"), _T("Message shown when previewing"));

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_FWCheckOffset"), m_param.UVC_FWCheckOffset, 0, _T(" Camera FW Version Offset"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_FWchecksize"), m_param.UVC_FWchecksize, 0, _T(" Camera FW Version size"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_FWCheckValue"), m_param.UVC_FWCheckValue,  _T("000000"), _T("Camera FW version!"));

        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		CString strRegName = m_strOperatorName;
		BYTE FWVersionBuf[10] = {0};
		BYTE pBuffer[50] = {0};
		int nBufferSize = sizeof(int) * 3;

		CStringA strTmp;
		BYTE *pTmp = (BYTE *)pBuffer;
		int addr = (int)(&FWVersionBuf);
		memcpy(pTmp, &addr, sizeof(int));
		pTmp += sizeof(int);
		memcpy(pTmp,&m_param.UVC_FWCheckOffset,sizeof(int));
		pTmp += sizeof(int);
		memcpy(pTmp,&m_param.UVC_FWchecksize,sizeof(int));

		if(TRUE != m_pDevice->ReadValue(DRVT_READ_EEPROM,(void *)pBuffer,nBufferSize))
			goto end;	
		
		USES_CONVERSION;
		strTmp = T2A(m_param.UVC_FWCheckValue);
		if (strTmp.Compare((LPCSTR)FWVersionBuf))
		{
			*pnErrorCode = uts.errorcode.E_NVMCheck;
			goto end;
		}
		
		
		return TRUE;
end:
		uts.log.Error(_T("FW Version check   [%s] Error."),strRegName);
		return FALSE;
	}

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_NVMCheck);
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
