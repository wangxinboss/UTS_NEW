// UTOP_AWB_Altek.h : main header file for the UTOP_AWB_Altek DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CUTOP_AWB_AltekApp
// See UTOP_AWB_Altek.cpp for the implementation of this class
//

class CUTOP_AWB_AltekApp : public CWinApp
{
public:
	CUTOP_AWB_AltekApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_AWB_AltekApp theApp;