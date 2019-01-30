// UTOP_DAC2DIS.h : UTOP_DAC2DIS DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTOP_DAC2DISApp
// 有关此类实现的信息，请参阅 UTOP_DAC2DIS.cpp
//

class CUTOP_DAC2DISApp : public CWinApp
{
public:
	CUTOP_DAC2DISApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_DAC2DISApp theApp;
