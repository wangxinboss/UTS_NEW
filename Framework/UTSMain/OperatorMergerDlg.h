#pragma once
#include "CheckListCtrl.h"

// COperatorMergerDlg 对话框

class COperatorMergerDlg : public CDialog
{
	DECLARE_DYNAMIC(COperatorMergerDlg)

public:
	COperatorMergerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COperatorMergerDlg();

// 对话框数据
	enum { IDD = IDD_OperatorMergerDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	BOOL AddOperatorSpecData(vector<OPERATOR_SPEC> &allOpeSpec);
	
private:
	CMFCPropertyGridCtrl m_propertyGrid;
	CCheckListCtrl m_OperatorNameList;
	CListCtrl m_OperatorInfoList;

	vector<CString> MergerOpName;
	map<CString,vector<SINGLE_ITEM>> OperatorAndItemMap;
	
public:
	afx_msg void OnNMClickListOperatorSpec(NMHDR *pNMHDR, LRESULT *pResult);

	
};
