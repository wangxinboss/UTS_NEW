// UTOP_UVC_SetProperty.h : UTOP_UVC_SetProperty DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTOP_UVC_SetPropertyApp
// 有关此类实现的信息，请参阅 UTOP_UVC_SetProperty.cpp
//

class CUTOP_UVC_SetPropertyApp : public CWinApp
{
public:
	CUTOP_UVC_SetPropertyApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_UVC_SetPropertyApp theApp;
