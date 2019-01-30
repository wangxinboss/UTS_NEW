// UTOP_VCMMove.h : UTOP_VCMMove DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_VCMMoveApp
// 這個類別的實作請參閱 UTOP_VCMMove.cpp
//

class CUTOP_VCMMoveApp : public CWinApp
{
public:
	CUTOP_VCMMoveApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_VCMMoveApp theApp;