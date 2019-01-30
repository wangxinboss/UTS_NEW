// UTOP_VCM_Check.h : UTOP_VCM_Check DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_VCM_CheckApp
// 這個類別的實作請參閱 UTOP_VCM_Check.cpp
//

class CUTOP_VCM_CheckApp : public CWinApp
{
public:
	CUTOP_VCM_CheckApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_VCM_CheckApp theApp;
