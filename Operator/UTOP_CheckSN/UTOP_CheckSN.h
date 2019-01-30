// UTOP_CheckSN.h : UTOP_CheckSN DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTOP_CheckSNApp
// 有关此类实现的信息，请参阅 UTOP_CheckSN.cpp
//

class CUTOP_CheckSNApp : public CWinApp
{
public:
	CUTOP_CheckSNApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_CheckSNApp theApp;

