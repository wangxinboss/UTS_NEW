#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_UVC_LEDControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace UTS::Algorithm::Image;

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nLedControl"), m_param.nLedControl, 0, _T("0:OFF 1:ON"));


        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		UI_RECT rc;

		int ledValue = 0;

		// Control LED
		if (m_param.nLedControl == 0)
		{
			ledValue = 2; //close
		}else if (m_param.nLedControl == 1)
			ledValue = 1; //open

		if(TRUE != m_pDevice->WriteValue(DWVT_SET_LED,&ledValue,sizeof(ledValue)))
		{
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;	
		}

		*pnErrorCode = uts.errorcode.E_Pass;

end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);


		return m_bResult;
	}

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_LED);
    }

    void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),_Result,")
            _T("LightValue,R_GDelta,B_GDelta,")
			_T("BlinkMax,BlinkMin,BlinkDelta,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%s,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
            , strVersion, uts.info.strUserId);
    }
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
