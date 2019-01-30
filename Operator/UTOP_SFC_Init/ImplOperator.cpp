#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_SFC_Init.h"
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
            if (nullptr == gmsfc)
            {
                uts.log.Error(_T("Get global memory fail, maybe you foget add UTOP_CreateMemory.dll into InitList."));
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }
            gmsfc->sSFC_ServerTime = _T("1900-1-1-0-0-0");
            gmsfc->bOverAllResult = TRUE;
            gmsfc->sSFC_ErrorCode = EMPTY_STR;
            gmsfc->sSFC_ErrorDesc = EMPTY_STR;
            gmsfc->vecTestItemResult.clear();
            
            if (!SFC_Initialize())
            {
                wchar_t error[0x100];
                SFC_GetLastErrorMsg(error, 0x100);
                uts.log.Error(_T("SFC_Initialize Error: %s"), error);
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }
            else
            {
                uts.log.Info(_T("SFC_Initialize OK."));
            }

            LOGIN_DATA login;
            login.pnType = uts.info.strProjectType.GetBuffer();
            login.pn = uts.info.strProjectName.GetBuffer();
            login.sectionCode = uts.info.strStationCode.GetBuffer();
            login.sectionName = uts.info.strStationName.GetBuffer();
            login.stationCode = uts.info.strPositionCode.GetBuffer();
            login.stationLoc = uts.info.strPositionName.GetBuffer();
            login.lineCode = uts.info.strLineName.GetBuffer();
            login.tester = uts.info.strUserId.GetBuffer();
            login.testfilePaths = uts.info.strShopFlowFilePath.GetBuffer();
            login.filetypes = L"csv,rar,bmp,raw,bin";    
            login.virtualtypes = L"csv"; 

            uts.log.Debug(_T("== Shopflow login info =="));
            uts.log.Debug(_T("ProjType: %s"), login.pnType);
            uts.log.Debug(_T("ProjName: %s"), login.pn);
            uts.log.Debug(_T("StationCode: %s"), login.sectionCode);
            uts.log.Debug(_T("StationName: %s"), login.sectionName);
            uts.log.Debug(_T("StationLoc: %s"), login.stationLoc);
            uts.log.Debug(_T("Line: %s"), login.lineCode);
            uts.log.Debug(_T("Operater: %s"), login.tester);
            uts.log.Debug(_T("FilePath: %s"), login.testfilePaths);
            uts.log.Debug(_T("Filetypes: %s"), login.filetypes);
            uts.log.Debug(_T("Virtualtypes: %s"), login.virtualtypes);
            uts.log.Debug(_T("========================="));

            OSUtil::CreateMultipleDirectory(uts.info.strShopFlowFilePath);
            if (!SFC_TestLogin(&login))
            {
                wchar_t error[0x100];
                SFC_GetLastErrorMsg(error, 0x100);
                uts.log.Error(_T("SFC_TestLogin Error: %s"), error);
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }
        }
        else
        {
            uts.log.Warning(_T("ShopFlow NOT enable, SFC_Init omitted."));
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
