// UTOP_TvDistortion.h : UTOP_TvDistortion DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTOP_TvDistortionApp
// 有关此类实现的信息，请参阅 UTOP_TvDistortion.cpp
//

class CUTOP_TvDistortionApp : public CWinApp
{
public:
	CUTOP_TvDistortionApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_TvDistortionApp theApp;
