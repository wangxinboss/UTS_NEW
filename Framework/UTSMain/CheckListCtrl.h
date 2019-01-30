#if !defined(AFX_CHECKLISTCTRL_H__B02295B1_44F0_46AE_985A_6067632408FB__INCLUDED_)
#define AFX_CHECKLISTCTRL_H__B02295B1_44F0_46AE_985A_6067632408FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckListCtrl.h : header file
//
#include "CheckHeadCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CCheckListCtrl window

class CCheckListCtrl : public CListCtrl
{
// Construction
public:
	CCheckListCtrl();

// Attributes
public:
	
// Operations
private:
	BOOL	m_blInited;
	CImageList	m_checkImgList;


public:
	BOOL Init();

protected:
	CCheckHeadCtrl	m_checkHeadCtrl;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCheckListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckListCtrl)
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);		
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKLISTCTRL_H__B02295B1_44F0_46AE_985A_6067632408FB__INCLUDED_)
