// UTOP_ChangeReg.h : UTOP_ChangeReg DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTOP_ChangeRegApp
// 有关此类实现的信息，请参阅 UTOP_ChangeReg.cpp
//

class CUTOP_ChangeRegApp : public CWinApp
{
public:
	CUTOP_ChangeRegApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_ChangeRegApp theApp;
