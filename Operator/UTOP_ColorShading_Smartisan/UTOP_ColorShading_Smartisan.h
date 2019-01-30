// UTOP_ColorShading_Smartisan.h : UTOP_ColorShading_Smartisan DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_ColorShading_SmartisanApp
// 這個類別的實作請參閱 UTOP_ColorShading_Smartisan.cpp
//

class CUTOP_ColorShading_SmartisanApp : public CWinApp
{
public:
	CUTOP_ColorShading_SmartisanApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_ColorShading_SmartisanApp theApp;