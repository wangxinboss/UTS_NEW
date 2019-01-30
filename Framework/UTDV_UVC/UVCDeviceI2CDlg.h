#pragma once


// UVCDeviceI2CDlg 對話方塊

class UVCDeviceI2CDlg : public CDialogEx
{
	DECLARE_DYNAMIC(UVCDeviceI2CDlg)

public:
	UVCDeviceI2CDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~UVCDeviceI2CDlg();

// 對話方塊資料
	enum { IDD = IDD_FRAME_I2C };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
};
