// UTOP_DefectPixel_DarkDead.h : main header file for the UTOP_DefectPixel_DarkDead DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CUTOP_DefectPixel_DarkDeadApp
// See UTOP_DefectPixel_DarkDead.cpp for the implementation of this class
//

class CUTOP_DefectPixel_DarkDeadApp : public CWinApp
{
public:
	CUTOP_DefectPixel_DarkDeadApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_DefectPixel_DarkDeadApp theApp;
