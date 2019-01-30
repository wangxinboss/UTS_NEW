// UTOP_RS232Release.h : UTOP_RS232Release DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_RS232ReleaseApp
// 這個類別的實作請參閱 UTOP_RS232Release.cpp
//

class CUTOP_RS232ReleaseApp : public CWinApp
{
public:
	CUTOP_RS232ReleaseApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_RS232ReleaseApp theApp;