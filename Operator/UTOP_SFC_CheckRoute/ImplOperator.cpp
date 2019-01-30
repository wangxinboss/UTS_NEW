#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_SFC_CheckRoute.h"
#include "SFCInterface.h"
#include "Global_Memory_SFC.h"

#pragma comment(lib, "UTTL_SfcIF.lib")

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
        if (uts.info.nShopFlowEn != 0)
        {
            GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
            gmsfc->sSFC_ServerTime = _T("1900-1-1-0-0-0");
            gmsfc->bOverAllResult = TRUE;
            gmsfc->vecTestItemResult.clear();

            const int nServerTimeLen = 20;
            wchar_t szSFC_ServerTime[nServerTimeLen] = {0};
            if (!SFC_GetServerTime(szSFC_ServerTime, nServerTimeLen))
            {
                wchar_t error[0x100];
                SFC_GetLastErrorMsg(error, 0x100);
                uts.log.Error(_T("SFC_GetServerTime Error: %s"), error);
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }
            else
            {
                gmsfc->sSFC_ServerTime = szSFC_ServerTime;
                uts.log.Debug(_T("SFC_GetServerTime OK. ServerTime[%s]"),
                    gmsfc->sSFC_ServerTime);
            }

            if (!SFC_CheckRoute(uts.info.strSN))
            {
                wchar_t error[0x100],logerror[0x120];
                SFC_GetLastErrorMsg(error, 0x100);
				wcscpy(logerror,L"SFC_CheckRoute Error:");
				wcscat(logerror,error);
				uts.log.Error(logerror);
				*pnErrorCode = uts.errorcode.E_Fail;

				MessageBox(NULL, logerror, _T("Error"), MB_OK|MB_TOPMOST);
                goto end;
            }
            else
            {
                uts.log.Debug(_T("SFC_CheckRoute OK. SN[%s]"), uts.info.strSN);
            }
        }
        else
        {
            uts.log.Warning(_T("ShopFlow NOT enable, SFC_CheckRoute omitted."));
            *pnErrorCode = uts.errorcode.E_Pass;
        }

end:
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);
        
        return m_bResult;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
    }
    
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
