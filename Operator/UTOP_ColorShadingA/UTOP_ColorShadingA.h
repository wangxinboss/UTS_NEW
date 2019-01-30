// UTOP_ColorShadingA.h : UTOP_ColorShadingA DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTOP_ColorShadingAApp
// 有关此类实现的信息，请参阅 UTOP_ColorShadingA.cpp
//

class CUTOP_ColorShadingAApp : public CWinApp
{
public:
	CUTOP_ColorShadingAApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_ColorShadingAApp theApp;
