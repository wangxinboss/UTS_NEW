// AboutDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UTSMain.h"
#include "AboutDlg.h"
#include "afxdialogex.h"


// CAboutDlg 对话框

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_VERSION, m_versionList);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CAboutDlg::OnBnClickedCancel)
    ON_WM_SIZING()
END_MESSAGE_MAP()


// CAboutDlg 消息处理程序


BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // UTS version
    CString strFilename = OSUtil::GetBSPath(uts.info.strBinPath) + _T("UTSMain.exe");
    CString strFileVersion;
    OSUtil::GetFileVersion(strFilename, strFileVersion);
    vector<int> vecVersion;
    strFileVersion.Replace(_T("."), _T(","));
    SplitInt(strFileVersion, vecVersion);
    if (vecVersion.size() > 0)
    {
        strFileVersion.Format(_T("V%d.%d"), vecVersion[0], vecVersion[1]);
    }
    SetDlgItemText(IDC_STATIC_VERSION, strFileVersion);


    DWORD dwStyle = m_versionList.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT;    //选中某行使整行高亮（只适用与report风格的listctrl）
    dwStyle |= LVS_EX_GRIDLINES;        //网格线（只适用与report风格的listctrl）
    m_versionList.SetExtendedStyle(dwStyle);   //设置扩展风格
    // 插入列
    m_versionList.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 350);
    m_versionList.InsertColumn(1, _T("Version"), LVCFMT_LEFT, 100);
    m_versionList.InsertColumn(2, _T("Datetime"), LVCFMT_LEFT, 200);

    ShowFileVersion();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CAboutDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    //CDialog::OnOK();
}


void CAboutDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    CDialog::OnCancel();
}

void CAboutDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDialog::OnSizing(fwSide, pRect);

    // TODO: 在此处添加消息处理程序代码
    AdjustWindow();
}

void CAboutDlg::AdjustWindow(void)
{
    RECT rcClient = {0};
    GetClientRect(&rcClient);
    m_versionList.MoveWindow(&rcClient);
}

void CAboutDlg::ShowFileVersion(void)
{
    CStringList filelist;
    OSUtil::GetFiles(uts.info.strBinPath, filelist);
    POSITION pos = filelist.GetHeadPosition();
    while (pos != nullptr)
    {
        // 文件名
        CString strFilename = filelist.GetNext(pos);
        if (OSUtil::GetFileName(strFilename).Left(2).CompareNoCase(_T("UT")) != 0)
        {
            continue;
        }
        CString strFileExtName = OSUtil::GetExtFileName(strFilename).MakeUpper();
        if (strFileExtName != _T("DLL") && strFileExtName != _T("EXE"))
        {
            continue;
        }
        int nRow = m_versionList.InsertItem(0, OSUtil::GetFileName(strFilename));
        // 版本
        CString strFileVersion;
        OSUtil::GetFileVersion(strFilename, strFileVersion);
        m_versionList.SetItemText(nRow, 1, strFileVersion);
        // 日期
        CString strCreateTime;
        CString strAccessTime;
        CString strWriteTime;
        OSUtil::GetFileTimeStr(strFilename, strCreateTime, strAccessTime, strWriteTime);
        m_versionList.SetItemText(nRow, 2, strWriteTime);
    }
}

