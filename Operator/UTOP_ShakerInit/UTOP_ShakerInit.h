// UTOP_ShakerInit.h : UTOP_ShakerInit DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_ShakerInitApp
// 這個類別的實作請參閱 UTOP_ShakerInit.cpp
//

class CUTOP_ShakerInitApp : public CWinApp
{
public:
	CUTOP_ShakerInitApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


extern CUTOP_ShakerInitApp theApp;