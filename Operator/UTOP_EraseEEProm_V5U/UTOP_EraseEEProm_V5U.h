// UTOP_EraseEEProm_V5U.h : UTOP_EraseEEProm_V5U DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_EraseEEProm_V5UApp
// 這個類別的實作請參閱 UTOP_EraseEEProm_V5U.cpp
//

class CUTOP_EraseEEProm_V5UApp : public CWinApp
{
public:
	CUTOP_EraseEEProm_V5UApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_EraseEEProm_V5UApp theApp;