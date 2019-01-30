// UTOP_AutoExposure.h : UTOP_AutoExposure DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符?


// CUTOP_AutoExposureApp
// 這??別的?作?參? UTOP_AutoExposure.cpp
//

class CUTOP_AutoExposureApp : public CWinApp
{
public:
	CUTOP_AutoExposureApp();

// 覆?
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_AutoExposureApp theApp;