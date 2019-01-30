// UTOP_ForTest.h : UTOP_ForTest DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTOP_ForTestApp
// 有关此类实现的信息，请参阅 UTOP_ForTest.cpp
//

class CUTOP_ForTestApp : public CWinApp
{
public:
	CUTOP_ForTestApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_ForTestApp theApp;
