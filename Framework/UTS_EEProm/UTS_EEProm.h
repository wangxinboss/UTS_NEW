// UTS_EEProm.h : UTS_EEProm DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符?


// CUTS_EEPromApp
// 這??別的?作?參? UTS_EEProm.cpp
//

class CUTS_EEPromApp : public CWinApp
{
public:
	CUTS_EEPromApp();

// 覆?
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
