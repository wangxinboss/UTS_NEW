#include "StdAfx.h"
#include "UTOP_ClearSN.h"
#include "ImplOperator.h"

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
        uts.info.strSN = EMPTY_STR;
		uts.board.ShowSN(uts.info.strSN);

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
