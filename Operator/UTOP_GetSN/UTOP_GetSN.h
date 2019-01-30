// UTOP_GetSN.h : UTOP_GetSN DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符?


// CUTOP_GetSNApp
// 這??別的?作?參? UTOP_GetSN.cpp
//

class CUTOP_GetSNApp : public CWinApp
{
public:
	CUTOP_GetSNApp();

// 覆?
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_GetSNApp theApp;