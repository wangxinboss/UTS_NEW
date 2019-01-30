// UTS_OIS.h : UTS_OIS DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTS_OISApp
// 這個類別的實作請參閱 UTS_OIS.cpp
//

class CUTS_OISApp : public CWinApp
{
public:
	CUTS_OISApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTS_OISApp theApp;