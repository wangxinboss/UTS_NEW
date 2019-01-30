#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"

#include <direct.h>

int BaseOtp::HallCali(void *args)
{
	int ret = OTPCALI_ERROR_NO;

	uts.board.ShowMsg(_T("Hall Cal..."));  
	ret = do_hallCali();
	uts.board.ShowMsg(_T(" "));              // 取消提示

	return SET_ERROR(ret);
}

