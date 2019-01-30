// UTOP_RelShift_Single.h : UTOP_RelShift_Single DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符?


// CUTOP_RelShift_SingleApp
// 這??別的?作?參? UTOP_RelShift_Single.cpp
//

class CUTOP_RelShift_SingleApp : public CWinApp
{
public:
	CUTOP_RelShift_SingleApp();

// 覆?
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_RelShift_SingleApp theApp;