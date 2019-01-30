#pragma once
#include "UVCDeviceI2CDlg.h"

// UVCDeviceRegisterDlg 對話方塊

class UVCDeviceRegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(UVCDeviceRegisterDlg)

public:
	UVCDeviceRegisterDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~UVCDeviceRegisterDlg();

// 對話方塊資料
	enum { IDD = IDD_UVCDeviceRegisterDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bInitialized;
	vector<REGISTER_ITEM> m_vecOldRegisterList;
	vector<REGISTER_ITEM> m_vecNewRegisterList;
	CEdit m_registerEdit;
	CListBox m_markList;
	CListBox m_markList2;
	UVCDeviceI2CDlg m_i2cDlg;

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


};
