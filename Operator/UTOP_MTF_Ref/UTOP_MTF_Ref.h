// UTOP_MTF_Ref.h : UTOP_MTF_Ref DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_MTF_RefApp
// 這個類別的實作請參閱 UTOP_MTF_Ref.cpp
//

class CUTOP_MTF_RefApp : public CWinApp
{
public:
	CUTOP_MTF_RefApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
extern CUTOP_MTF_RefApp theApp;