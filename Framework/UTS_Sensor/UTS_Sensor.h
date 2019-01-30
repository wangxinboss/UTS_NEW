// UTS_Sensor.h : UTS_Sensor DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUTS_SensorApp
// 有关此类实现的信息，请参阅 UTS_Sensor.cpp
//

class CUTS_SensorApp : public CWinApp
{
public:
	CUTS_SensorApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
