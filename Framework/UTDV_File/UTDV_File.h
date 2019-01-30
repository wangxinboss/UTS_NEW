// UTDV_File.h : UTDV_File DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTDV_FileApp
// 有关此类实现的信息，请参阅 UTDV_File.cpp
//

class CUTDV_FileApp : public CWinApp
{
public:
	CUTDV_FileApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTDV_FileApp theApp;
