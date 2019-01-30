// UTOP_XCalibration_OC.h : UTOP_XCalibration_OC DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CUTOP_XCalibration_OCApp
// 這個類別的實作請參閱 UTOP_XCalibration_OC.cpp
//

class CUTOP_XCalibration_OCApp : public CWinApp
{
public:
	CUTOP_XCalibration_OCApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_XCalibration_OCApp theApp;