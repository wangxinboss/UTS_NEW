// UTOP_FixPatternNoise_Altek.h : main header file for the UTOP_FixPatternNoise_Altek DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CUTOP_FixPatternNoise_AltekApp
// See UTOP_FixPatternNoise_Altek.cpp for the implementation of this class
//

class CUTOP_FixPatternNoise_AltekApp : public CWinApp
{
public:
	CUTOP_FixPatternNoise_AltekApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_FixPatternNoise_AltekApp theApp;
