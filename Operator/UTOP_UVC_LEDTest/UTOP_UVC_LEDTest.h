// UTOP_UVC_LEDTest.h : UTOP_UVC_LEDTest DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTOP_UVC_LEDTestApp
// 有关此类实现的信息，请参阅 UTOP_UVC_LEDTest.cpp
//

class CUTOP_UVC_LEDTestApp : public CWinApp
{
public:
	CUTOP_UVC_LEDTestApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
extern CUTOP_UVC_LEDTestApp theApp;

