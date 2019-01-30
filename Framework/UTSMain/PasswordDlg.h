#pragma once

// CPasswordDlg 对话框

class CPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPasswordDlg();

// 对话框数据
	enum { IDD = IDD_PasswordDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
    int m_nUserType;
public:
    void SetMode(int nUserType);
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
