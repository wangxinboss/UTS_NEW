// UTOP_MESCommit.h : main header file for the UTOP_MESCommit DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CUTOP_MESCommitApp
// See UTOP_MESCommit.cpp for the implementation of this class
//

class CUTOP_MESCommitApp : public CWinApp
{
public:
	CUTOP_MESCommitApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_MESCommitApp theApp;