// UTOP_Resolution_Mirage.h : UTOP_Resolution_Mirage DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符?


// CUTOP_Resolution_MirageApp
// 這??別的?作?參? UTOP_Resolution_Mirage.cpp
//

class CUTOP_Resolution_MirageApp : public CWinApp
{
public:
	CUTOP_Resolution_MirageApp();

// 覆?
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_Resolution_MirageApp theApp;