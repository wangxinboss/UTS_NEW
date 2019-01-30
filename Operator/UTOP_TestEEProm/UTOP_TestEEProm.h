// UTOP_TestEEProm.h : UTOP_TestEEProm DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_TestEEPromApp
// 這個類別的實作請參閱 UTOP_TestEEProm.cpp
//

class CUTOP_TestEEPromApp : public CWinApp
{
public:
	CUTOP_TestEEPromApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


extern CUTOP_TestEEPromApp theApp;