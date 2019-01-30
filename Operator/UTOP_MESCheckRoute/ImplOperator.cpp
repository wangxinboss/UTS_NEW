#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_MESCheckRoute.h"
#include "PlatformInfo.h"

#import "MESDLL.tlb"
using namespace MESDLL;

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
        
    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        CString strVersion;
		OSUtil::GetFileName(m_strModuleFile);
        OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		//uts.info.strSN=_T("25410100223180000000002");
		CString App;
		if (uts.info.nMesOffice)
			App=_T("hlt_office");
		else
			App=_T("hlt");
		
		if (uts.info.nMesEn != 0)
        {
			CoInitialize(NULL);
			MESDLL::ISQLServerPtr p_SQLServer(_uuidof(SQLServer));

			uts.log.Debug(_T("App=%s,SN=%s,Station Name=%s,LineName=%s,Version=%s"),App,uts.info.strSN,uts.info.strStationName,uts.info.strLineName,strVersion);
			_bstr_t MESCheckResult= p_SQLServer->CheckRoute((_bstr_t)App, (_bstr_t)uts.info.strSN, (_bstr_t)uts.info.strStationName, (_bstr_t)uts.info.strLineName, (_bstr_t)_T(""));

			if(MESCheckResult == (_bstr_t)("Y"))
			{
				uts.log.Debug(_T("MESCheck PASS. SN=%s"), uts.info.strSN);
				UTS::TestFlow::m_bIsRunning=TRUE;
				uts.info.nMesCommit = TRUE;
				*pnErrorCode = uts.errorcode.E_Pass;
			}
			else
			{
				uts.log.Debug(_T("MESCheck FAIL: %s"), (LPCTSTR)MESCheckResult);
				UTS::TestFlow::m_bIsRunning=FALSE;
				*pnErrorCode = uts.errorcode.E_SFC_CheckRoute;
				uts.info.strMESInsData="";
			}
			CoUninitialize();
        }
       /* else
        {
            uts.log.Warning(_T("ShopFlow NOT enable, MES CheckRoute omitted."));
            *pnErrorCode = uts.errorcode.E_SFC_CheckRoute;
        }*/

        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);
        
        return m_bResult;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_SFC_CheckRoute);
    }
    
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
