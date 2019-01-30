// UTOP_ColorAccuracy_Altek.h : UTOP_ColorAccuracy_Altek DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_ColorAccuracy_AltekApp
// 這個類別的實作請參閱 UTOP_ColorAccuracy_Altek.cpp
//

class CUTOP_ColorAccuracy_AltekApp : public CWinApp
{
public:
	CUTOP_ColorAccuracy_AltekApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_ColorAccuracy_AltekApp theApp;