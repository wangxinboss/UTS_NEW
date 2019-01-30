// UTOP_CheckOCBank.h : UTOP_CheckOCBank DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_CheckOCBankApp
// 這個類別的實作請參閱 UTOP_CheckOCBank.cpp
//

class CUTOP_CheckOCBankApp : public CWinApp
{
public:
	CUTOP_CheckOCBankApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_CheckOCBankApp theApp;