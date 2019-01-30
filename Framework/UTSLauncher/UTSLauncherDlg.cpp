
// UTSLauncherDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UTSLauncher.h"
#include "UTSLauncherDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUTSLauncherDlg 对话框




CUTSLauncherDlg::CUTSLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUTSLauncherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUTSLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUTSLauncherDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, &CUTSLauncherDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CUTSLauncherDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CUTSLauncherDlg::OnBnClickedButtonLogin)
    ON_BN_CLICKED(IDC_RADIO_OP, &CUTSLauncherDlg::OnBnClickedRadioOp)
    ON_BN_CLICKED(IDC_RADIO_PE, &CUTSLauncherDlg::OnBnClickedRadioPe)
    ON_BN_CLICKED(IDC_RADIO_SW, &CUTSLauncherDlg::OnBnClickedRadioSw)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CUTSLauncherDlg 消息处理程序

BOOL CUTSLauncherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    CheckRadioButton(IDC_RADIO_OP, IDC_RADIO_SW, IDC_RADIO_OP);

    CString strTitle;
#ifdef DEBUG
    strTitle = _T("UTSLauncher(Debug)");
#else
    strTitle = _T("UTSLauncher");
#endif
    SetWindowText(strTitle);
    if (!uts.dbCof.Initialize())
    {
        AfxMessageBox(_T("Load DB Error."));
    }

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CUTSLauncherDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);

    // TODO: 在此处添加消息处理程序代码
    OnBnClickedRadioOp();
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUTSLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUTSLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUTSLauncherDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    //CDialogEx::OnOK();
    OnBnClickedButtonLogin();
}


void CUTSLauncherDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    CDialogEx::OnCancel();
}

void CUTSLauncherDlg::OnBnClickedRadioOp()
{
    m_nUserType = USERTYPE_OPERATOR;
    GetDlgItem(IDC_EDIT_ID)->SetWindowText(EMPTY_STR);
    GetDlgItem(IDC_EDIT_PW)->SetWindowText(EMPTY_STR);
    GetDlgItem(IDC_EDIT_ID)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_PW)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_ID)->SetFocus();
}


void CUTSLauncherDlg::OnBnClickedRadioPe()
{
    m_nUserType = USERTYPE_PE;
    GetDlgItem(IDC_EDIT_ID)->SetWindowText(EMPTY_STR);
    GetDlgItem(IDC_EDIT_PW)->SetWindowText(EMPTY_STR);
    GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_PW)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_PW)->SetFocus();
}


void CUTSLauncherDlg::OnBnClickedRadioSw()
{
    m_nUserType = USERTYPE_SOFTWARE;
    GetDlgItem(IDC_EDIT_ID)->SetWindowText(EMPTY_STR);
    GetDlgItem(IDC_EDIT_PW)->SetWindowText(EMPTY_STR);
    GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_PW)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_PW)->SetFocus();
}

void CUTSLauncherDlg::OnBnClickedButtonLogin()
{
    CString strID, strPW;
    GetDlgItem(IDC_EDIT_ID)->GetWindowText(strID);
    GetDlgItem(IDC_EDIT_PW)->GetWindowText(strPW);

    if (m_nUserType == USERTYPE_OPERATOR)
    {
        if (strID.GetLength() != 8)
        {
            AfxMessageBox(_T("ID error, correct format is 8 number."));
            return;
        }
    }

    CStringA strDBPW;
    BOOL bRet = uts.dbCof.GetUserPassword(m_nUserType, strDBPW);
	//OutputDebugStringA(strDBPW.GetString()); 
    if (!bRet)
    {
        AfxMessageBox(_T("GetUserPassword from DB error."));
        return;
    }
    USES_CONVERSION;
    CString strDBPW_CONV = A2T(strDBPW);
    if (strDBPW_CONV == strPW)
    {
        CString strCmd;
        strCmd.AppendFormat(
            _T("%s%s |%d|%s|%s"),
            uts.info.strBinPath,
            _T("UTSMain.exe"),
            m_nUserType,
            strID,
            strPW);
        OSUtil::ExcuteCommand(strCmd, FALSE);
        OnBnClickedCancel();
    }
    else
    {
        AfxMessageBox(_T("Password error."));
        GetDlgItem(IDC_EDIT_PW)->SetWindowText(EMPTY_STR);
        GetDlgItem(IDC_EDIT_PW)->SetFocus();
        return;
    }
}

