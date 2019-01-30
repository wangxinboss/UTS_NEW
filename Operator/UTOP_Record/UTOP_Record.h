// UTOP_Record.h : UTOP_Record DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_RecordApp
// 這個類別的實作請參閱 UTOP_Record.cpp
//

class CUTOP_RecordApp : public CWinApp
{
public:
	CUTOP_RecordApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


extern CUTOP_RecordApp theApp;
