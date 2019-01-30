// UTOP_Start2.h : UTOP_Start2 DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_Start2App
// 這個類別的實作請參閱 UTOP_Start2.cpp
//

class CUTOP_Start2App : public CWinApp
{
public:
	CUTOP_Start2App();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_Start2App theApp;