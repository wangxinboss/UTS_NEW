// UTOP_OC_Raw.h : UTOP_OC_Raw DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_OC_RawApp
// 這個類別的實作請參閱 UTOP_OC_Raw.cpp
//

class CUTOP_OC_RawApp : public CWinApp
{
public:
	CUTOP_OC_RawApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_OC_RawApp theApp;
