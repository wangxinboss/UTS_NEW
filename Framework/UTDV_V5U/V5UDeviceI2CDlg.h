#pragma once


// CV5UDeviceI2CDlg 对话框

class CV5UDeviceI2CDlg : public CDialog
{
	DECLARE_DYNAMIC(CV5UDeviceI2CDlg)

public:
	CV5UDeviceI2CDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CV5UDeviceI2CDlg();

// 对话框数据
	enum { IDD = IDD_FRAME_I2C };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonRead();
    afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonDacIncrease();
	afx_msg void OnBnClickedButtonVcmInit();
	afx_msg void OnBnClickedRadioDac20();
	afx_msg void OnBnClickedRadioDac4();
	afx_msg void OnBnClickedRadioDac1();
	afx_msg void OnBnClickedButtonDacReduce();
	afx_msg void OnBnClickedButtonDacMove();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButtonDacIncrease4();
	afx_msg void OnBnClickedButtonDacReduce4();
	afx_msg void OnBnClickedButtonDacIncrease20();
	afx_msg void OnBnClickedButtonDacReduce20();
	afx_msg void OnEnChangeEditDac();

	virtual BOOL OnInitDialog();
private:
	CSliderCtrl m_vcmMoveBar;
	CEdit m_dataEdit;
public:
	afx_msg void OnNMCustomdrawSliderVcm(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonClearall();
};
