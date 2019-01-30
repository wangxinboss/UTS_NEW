#pragma once


// CErrorcodeDlg 对话框

class CErrorcodeDlg : public CDialog
{
	DECLARE_DYNAMIC(CErrorcodeDlg)

public:
	CErrorcodeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CErrorcodeDlg();

// 对话框数据
	enum { IDD = IDD_ErrorCodeDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();

private:
    CMFCPropertyGridCtrl m_propertyGrid;
};
