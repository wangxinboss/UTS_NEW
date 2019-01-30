// UTOP_DDP.h : UTOP_DDP DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTOP_DDPApp
// 有关此类实现的信息，请参阅 UTOP_DDP.cpp
//

class CUTOP_DDPApp : public CWinApp
{
public:
	CUTOP_DDPApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_DDPApp theApp;
