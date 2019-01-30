#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_WaitKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL WriteIniData(CString sSection, CString sSectionKey, CString nValue,CString sIniFileName)
{
	return WritePrivateProfileString(sSection,sSectionKey,nValue,sIniFileName);
}
//--------------------------

int ReadIniData(CString sSection,CString sSectionKey,int nDefault,CString sIniFileName)
{
	return GetPrivateProfileInt(sSection,sSectionKey,nDefault,sIniFileName);
}

namespace UTS
{
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;
		sIniFileName = _T("D:\\ColorTest\\123.ini");
    }

    ImplOperator::~ImplOperator(void)
    {
    }

    BOOL ImplOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("Size"), m_param.strHintMessage, _T("A"), _T("Message shown when Testing"));
        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        uts.board.ShowMsg(m_param.strHintMessage);

		while(1)
		{
			OneKeyTestEn = ReadIniData(_T("Test3Color"),_T("OneKeyTestEn"),0, sIniFileName);

			if (OneKeyTestEn == 1)
			{
 				//WriteIniData(_T("Test3Color"),_T("OneKeyTestEn"), _T("0"), sIniFileName);
//				break;
				goto end;

			}
// 			else
// 			{
// 				*pnErrorCode = uts.errorcode.E_Fail;
// 				uts.board.ShowMsg(EMPTY_STR);
// 				return FALSE;
// 			}
		}

      
end:		
		*pnErrorCode = uts.errorcode.E_Pass;

        uts.board.ShowMsg(EMPTY_STR);
        return TRUE;
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
          return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}


