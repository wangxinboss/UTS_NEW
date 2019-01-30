#pragma once
#include "afxcmn.h"


// CTestOperatorListDlg 对话框

class CTestOperatorListDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestOperatorListDlg)

public:
	CTestOperatorListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestOperatorListDlg();

// 对话框数据
	enum { IDD = IDD_TestOperatorListDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonRefreshAll();
    afx_msg void OnBnClickedButtonAddInit();
    afx_msg void OnBnClickedButtonAddTest();
    afx_msg void OnBnClickedButtonAddFinal();
    afx_msg void OnBnClickedButtonUpIndexInitop();
    afx_msg void OnBnClickedButtonDownIndexInitop();
    afx_msg void OnBnClickedButtonDeleteInitop();
    afx_msg void OnBnClickedButtonUpIndexTestop();
    afx_msg void OnBnClickedButtonDownIndexTestop();
    afx_msg void OnBnClickedButtonDeleteTestop();
    afx_msg void OnBnClickedButtonApply();
    afx_msg void OnBnClickedButtonExportInitop();
    afx_msg void OnBnClickedButtonImportInitop();
    afx_msg void OnBnClickedButtonExportTestop();
    afx_msg void OnBnClickedButtonImportTestop();
    afx_msg void OnEnChangeEditFilter();
    afx_msg void OnBnClickedButtonUpIndexFinalop();
    afx_msg void OnBnClickedButtonDownIndexFinalop();
    afx_msg void OnBnClickedButtonDeleteFinalop();
    afx_msg void OnBnClickedButtonExportFinalop();
    afx_msg void OnBnClickedButtonImportFinalop();

private:
    BOOL m_bChanged;
    CListCtrl m_AllOperatorList;
    CListCtrl m_InitOperatorList;
    CListCtrl m_TestOperatorList;
    CListCtrl m_FinalOperatorList;
    void ShowAllOperatorList(void);
    void AddFileToListHead(CListCtrl &list, LPCTSTR lpFileName);
    void AddFileToListTail(CListCtrl &list, LPCTSTR lpFileName);
    void ListCtrlUpIndexCurrentSel(CListCtrl &list);
    void ListCtrlDownIndexCurrentSel(CListCtrl &list);
    BOOL ImportFromFileToList(CListCtrl &list);
    BOOL ExportFromListToFile(CListCtrl &list, CString &strFileName);
    // DB access
    void InitDBOperator(void);
    void ReadDBOperatorIndex(void);
    void UpdateDBOperatorIndex(void);
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
