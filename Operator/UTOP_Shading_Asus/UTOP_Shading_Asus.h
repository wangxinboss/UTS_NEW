// UTOP_Shading_Asus.h : UTOP_Shading_Asus DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符?


// CUTOP_Shading_AsusApp
// 這??別的?作?參? UTOP_Shading_Asus.cpp
//

class CUTOP_Shading_AsusApp : public CWinApp
{
public:
	CUTOP_Shading_AsusApp();

// 覆?
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_Shading_AsusApp theApp;