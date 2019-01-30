#include "StdAfx.h"
#include "UTOP_CreateMemory.h"
#include "ImplOperator.h"
//#include "Global_Memory_SFC.h"

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
		//!!---2018.08.01 Chi-Jen.Liao Remove SFC
		/*
        //-------------------------------------------------------------------------
        // SFC / MES
        static GLOBAL_MEMORY_SFC gmsfc;
        uts.mapUserMemory["SFC"] = &gmsfc;
        gmsfc.sSFC_ServerTime = _T("1900-1-1-0-0-0");
        gmsfc.bOverAllResult = TRUE;
        gmsfc.sSFC_ErrorCode = EMPTY_STR;
        gmsfc.sSFC_ErrorDesc = EMPTY_STR;
        gmsfc.vecTestItemResult.clear();

        //-------------------------------------------------------------------------
		*/
        // other ....


        *pnErrorCode = uts.errorcode.E_Pass;
        return TRUE;
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
