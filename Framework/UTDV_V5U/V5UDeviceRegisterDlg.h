#pragma once
#include "afxwin.h"
#include "V5UDeviceI2CDlg.h"

// CV5UDeviceRegisterDlg 对话框

class CV5UDeviceRegisterDlg : public CDialog
{
	DECLARE_DYNAMIC(CV5UDeviceRegisterDlg)

public:
	CV5UDeviceRegisterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CV5UDeviceRegisterDlg();

// 对话框数据
	enum { IDD = IDD_V5UDeviceRegisterDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    BOOL m_bInitialized;
    vector<REGISTER_ITEM> m_vecOldRegisterList;
    vector<REGISTER_ITEM> m_vecNewRegisterList;

    void AdjustWindow();
    void RefreshRegisterList(const vector<REGISTER_ITEM> &vecRegisterList);

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnLbnSelchangeListMarklist();
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnEnKillfocusEditRegister();
    afx_msg void OnBnClickedButtonDelete();
    afx_msg void OnBnClickedButtonAdd();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    CEdit m_registerEdit;
    CListBox m_markList;
    CListBox m_markList2;
    CV5UDeviceI2CDlg m_i2cDlg;
};
