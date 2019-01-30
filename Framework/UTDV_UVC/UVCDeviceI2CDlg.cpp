// UVCDeviceI2CDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "UTDV_UVC.h"
#include "UVCDeviceI2CDlg.h"
#include "afxdialogex.h"


// UVCDeviceI2CDlg 對話方塊

IMPLEMENT_DYNAMIC(UVCDeviceI2CDlg, CDialogEx)

UVCDeviceI2CDlg::UVCDeviceI2CDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(UVCDeviceI2CDlg::IDD, pParent)
{

}

UVCDeviceI2CDlg::~UVCDeviceI2CDlg()
{
}

void UVCDeviceI2CDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(UVCDeviceI2CDlg, CDialogEx)
END_MESSAGE_MAP()


// UVCDeviceI2CDlg 訊息處理常式
