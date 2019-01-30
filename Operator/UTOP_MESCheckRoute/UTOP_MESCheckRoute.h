// UTOP_MESCheckRoute.h : main header file for the UTOP_MESCheckRoute DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CUTOP_MESCheckRouteApp
// See UTOP_MESCheckRoute.cpp for the implementation of this class
//

class CUTOP_MESCheckRouteApp : public CWinApp
{
public:
	CUTOP_MESCheckRouteApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_MESCheckRouteApp theApp;