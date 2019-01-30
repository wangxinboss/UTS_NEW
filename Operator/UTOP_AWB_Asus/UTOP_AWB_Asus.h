// UTOP_AWB_Asus.h : UTOP_AWB_Asus DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_AWB_AsusApp
// 這個類別的實作請參閱 UTOP_AWB_Asus.cpp
//

class CUTOP_AWB_AsusApp : public CWinApp
{
public:
	CUTOP_AWB_AsusApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_AWB_AsusApp theApp;