#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_MESCommit.h"
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
		if (!uts.info.nMesCommit)
		{
			return TRUE;
		}
		uts.info.nMesCommit = FALSE;
		CString strVersion;
		OSUtil::GetFileName(m_strModuleFile);
        OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString App;
		if (uts.info.nMesOffice)
			App=_T("hlt_office");
		else
			App=_T("hlt");

		m_bResult= uts.errorcode.E_Pass;
		int nMesDataLength=uts.info.strMESInsData.GetLength();

		uts.info.strMESInsData.Delete(nMesDataLength-1,1);
		uts.log.Debug(_T("SN=%s"), uts.info.strSN);
		uts.log.Debug(_T("MesInsData=%s"),uts.info.strMESInsData);
		uts.log.Debug(_T("MesData Length=%d"),nMesDataLength);

		CString strMESTestResult;
		CString MesData=_T("");
		CString strError=_T("");
		if (uts.info.bMesResult==1)
		{
			strMESTestResult = "FAIL";
			strError.Format(_T("%d"),uts.info.nErrorCode);
		}
		else
		{
			strMESTestResult = "PASS";
		}
		MesData.AppendFormat(_T("Model_No=%s,BARCODE=%s,Station_Code=%s,LINE_CODE=%s,Version=%s,Result=%s,Defect_Code=%s,Appversion=%s,BARCODE_PART=%s,Employee_No=%s"),uts.info.strProjectName,uts.info.strSN,uts.info.strStationName,uts.info.strLineName,strVersion,strMESTestResult,strError, strVersion,"",uts.info.strUserId);
		uts.log.Debug(_T("MESData: %s"), (LPCTSTR)MesData);
		if (nMesDataLength==0)
			uts.info.strMESInsData.AppendFormat(_T("Final_Result=%s"),strMESTestResult);
		uts.log.Debug(_T("MESTestData: %s"), uts.info.strMESInsData);
		CoInitialize(NULL);
		MESDLL::ISQLServerPtr p_SQLServer(_uuidof(SQLServer));
		BOOL bStop=UTS::TestFlow::m_bIsRunning;
		_bstr_t MESCommitResult;
		if (bStop==FALSE)
		{
			m_bResult= *pnErrorCode=uts.errorcode.E_SFC_CheckRoute;
			goto KK;
		}

		MESCommitResult= p_SQLServer->CommitTestData((_bstr_t)App, (_bstr_t)MesData, (_bstr_t)uts.info.strMESInsData);	   
		uts.log.Debug(_T("Result=%s"),(LPCTSTR)MESCommitResult);
		if (MESCommitResult==(_bstr_t("Y")))
		{
			uts.log.Debug(_T("MESCommit PASS"));
			*pnErrorCode = uts.errorcode.E_Pass;
		}	
		else // if (MESCommitResult == (_bstr_t("N")))
		{
			uts.log.Debug(_T("Insert to table error!"));
			*pnErrorCode = uts.errorcode.E_SFC_CommitData;
		}

KK:
		CoUninitialize();

		uts.info.strMESInsData="";
		MesData="";
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);
        return m_bResult;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_SFC_CheckRoute);
		vecReturnValue.push_back(uts.errorcode.E_SFC_CommitData);		
    }
    
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
