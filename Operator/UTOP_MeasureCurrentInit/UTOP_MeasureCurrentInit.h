// UTOP_MeasureCurrentInit.h : UTOP_MeasureCurrentInit DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_MeasureCurrentInitApp
// 這個類別的實作請參閱 UTOP_MeasureCurrentInit.cpp
//

class CUTOP_MeasureCurrentInitApp : public CWinApp
{
public:
	CUTOP_MeasureCurrentInitApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_MeasureCurrentInitApp theApp;