// UTTL_EEAIF.h : UTTL_EEAIF DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTTL_EEAIFApp
// 有关此类实现的信息，请参阅 UTTL_EEAIF.cpp
//

class CUTTL_EEAIFApp : public CWinApp
{
public:
	CUTTL_EEAIFApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTTL_EEAIFApp theApp;
