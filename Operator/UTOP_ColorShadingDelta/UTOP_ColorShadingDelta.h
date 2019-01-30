// UTOP_ColorShadingDelta.h : UTOP_ColorShadingDelta DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_ColorShadingDeltaApp
// 這個類別的實作請參閱 UTOP_ColorShadingDelta.cpp
//

class CUTOP_ColorShadingDeltaApp : public CWinApp
{
public:
	CUTOP_ColorShadingDeltaApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_ColorShadingDeltaApp theApp;