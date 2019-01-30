// UTOP_UVC_LEDControl.h : UTOP_UVC_LEDControl DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_UVC_LEDControlApp
// 這個類別的實作請參閱 UTOP_UVC_LEDControl.cpp
//

class CUTOP_UVC_LEDControlApp : public CWinApp
{
public:
	CUTOP_UVC_LEDControlApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_UVC_LEDControlApp theApp;