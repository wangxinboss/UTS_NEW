// UTOP_MeasureSuspendCurrent.h : UTOP_MeasureSuspendCurrent DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_MeasureSuspendCurrentApp
// 這個類別的實作請參閱 UTOP_MeasureSuspendCurrent.cpp
//

class CUTOP_MeasureSuspendCurrentApp : public CWinApp
{
public:
	CUTOP_MeasureSuspendCurrentApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_MeasureSuspendCurrentApp theApp;