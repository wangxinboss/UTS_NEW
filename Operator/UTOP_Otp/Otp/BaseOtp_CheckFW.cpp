#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"

#include <direct.h>

int BaseOtp::CheckFW(void *args)
{
	int ret = OTPCALI_ERROR_NO;

	ret = do_CheckFW(otp_param.ois_param.nFW_CorrectVer);

	return SET_ERROR(ret);;
}

