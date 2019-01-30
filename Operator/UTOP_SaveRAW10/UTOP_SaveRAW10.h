// UTOP_SaveRAW10.h : UTOP_SaveRAW10 DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_SaveRAW10App
// 這個類別的實作請參閱 UTOP_SaveRAW10.cpp
//

class CUTOP_SaveRAW10App : public CWinApp
{
public:
	CUTOP_SaveRAW10App();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_SaveRAW10App theApp;