// UTOP_CheckWhiteChart.h : UTOP_CheckWhiteChart DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_CheckWhiteChartApp
// 這個類別的實作請參閱 UTOP_CheckWhiteChart.cpp
//

class CUTOP_CheckWhiteChartApp : public CWinApp
{
public:
	CUTOP_CheckWhiteChartApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_CheckWhiteChartApp theApp;