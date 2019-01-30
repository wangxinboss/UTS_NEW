#pragma once


// CProjectConfig 对话框

class CProjectConfig : public CDialog
{
	DECLARE_DYNAMIC(CProjectConfig)

public:
	CProjectConfig(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProjectConfig();

// 对话框数据
	enum { IDD = IDD_ProjectDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonExportProject();

private:
    CMFCPropertyGridCtrl m_propertyGrid;
    vector<SINGLE_ITEM> m_vecAllSetting;

	BOOL ExportFromVectToFile(vector<SINGLE_ITEM> &vecSetting, CString &strFileName);
};
