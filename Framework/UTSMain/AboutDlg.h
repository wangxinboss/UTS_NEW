#pragma once
#include "afxcmn.h"


// CAboutDlg 对话框

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAboutDlg();

// 对话框数据
	enum { IDD = IDD_AboutDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

private:
    CListCtrl m_versionList;

    void AdjustWindow(void);
    void ShowFileVersion(void);
};
