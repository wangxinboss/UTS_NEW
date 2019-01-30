// UTOP_RelShift_SFR_Dual.h : UTOP_RelShift_SFR_Dual DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_RelShift_SFR_DualApp
// 這個類別的實作請參閱 UTOP_RelShift_SFR_Dual.cpp
//

class CUTOP_RelShift_SFR_DualApp : public CWinApp
{
public:
	CUTOP_RelShift_SFR_DualApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_RelShift_SFR_DualApp theApp;