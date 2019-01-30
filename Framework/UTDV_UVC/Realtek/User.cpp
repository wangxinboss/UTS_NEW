#include "StdAfx.h"
#include "User.h"


CUser::CUser(void)
{
}


CUser::~CUser(void)
{
}

void DownloadProgressCB(PVOID Context)
{
	ProgressInfo *progressInfo = (ProgressInfo *)Context;
	printf("Download Stage is: %d", progressInfo->nProgress*100/progressInfo->nTotal);
}