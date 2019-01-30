// R_LSCCalibrationDLL.h : main header file for the R_LSCCALIBRATIONDLL DLL
//

#if !defined(AFX_R_LSCCALIBRATIONDLL_H__CC2493DA_9BEB_464F_8FFF_CA91DC4A25BE__INCLUDED_)
#define AFX_R_LSCCALIBRATIONDLL_H__CC2493DA_9BEB_464F_8FFF_CA91DC4A25BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CR_LSCCalibrationDLLApp
// See R_LSCCalibrationDLL.cpp for the implementation of this class
//

class CR_LSCCalibrationDLLApp : public CWinApp
{
public:
	CR_LSCCalibrationDLLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CR_LSCCalibrationDLLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CR_LSCCalibrationDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_R_LSCCALIBRATIONDLL_H__CC2493DA_9BEB_464F_8FFF_CA91DC4A25BE__INCLUDED_)
