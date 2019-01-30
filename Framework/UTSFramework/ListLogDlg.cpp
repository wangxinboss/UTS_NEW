// ListLogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ListLogDlg.h"


// ListLogDlg 对话框

namespace UTS
{
    IMPLEMENT_DYNAMIC(CListLogDlg, CDialog)

        CListLogDlg::CListLogDlg(CWnd* pParent /*=NULL*/)
        : CDialog(CListLogDlg::IDD, pParent)
    {
    }

    CListLogDlg::~CListLogDlg()
    {
    }

    void CListLogDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_LIST1, m_listbox);
    }


    BEGIN_MESSAGE_MAP(CListLogDlg, CDialog)
        ON_BN_CLICKED(IDOK, &CListLogDlg::OnBnClickedOk)
        ON_BN_CLICKED(IDCANCEL, &CListLogDlg::OnBnClickedCancel)
        ON_WM_SIZING()
        ON_MESSAGE(WM_LISTLOG, &CListLogDlg::OnListLog)
    END_MESSAGE_MAP()


    // ListLogDlg 消息处理程序

    BOOL CListLogDlg::OnInitDialog()
    {
        CDialog::OnInitDialog();

        // TODO:  在此添加额外的初始化

        return TRUE;  // return TRUE unless you set the focus to a control
        // 异常: OCX 属性页应返回 FALSE
    }


    void CListLogDlg::OnBnClickedOk()
    {
        // TODO: 在此添加控件通知处理程序代码
        //CDialog::OnOK();
    }


    void CListLogDlg::OnBnClickedCancel()
    {
        // TODO: 在此添加控件通知处理程序代码
        //CDialog::OnCancel();
        ShowWindow(SW_HIDE);
    }


    void CListLogDlg::OnSizing(UINT fwSide, LPRECT pRect)
    {
        CDialog::OnSizing(fwSide, pRect);

        // TODO: 在此处添加消息处理程序代码
        AdjustWindow();
    }

    void CListLogDlg::AdjustWindow(void)
    {
        RECT rcClient = {0};
        GetClientRect(&rcClient);
        m_listbox.MoveWindow(&rcClient);
        m_listbox.Invalidate(TRUE);
    }


    LRESULT CListLogDlg::OnListLog(WPARAM wparam, LPARAM lparam)
    {
        int nIndex = (int)wparam;
        CString strMsg = uts.log.m_LogList[nIndex];
        int nCount = m_listbox.GetCount();
        if (nCount >= m_nMaxLogCount)
        {
            m_listbox.DeleteString(0);
        }
        //m_listbox.AddString(strMsg);
        COLORREF bgColor = (COLORREF)lparam;
        m_listbox.AppendString(strMsg, COLOR_BLACK, bgColor);
        m_listbox.SetCurSel(nCount);
        return 0;
    }

    void CListLogDlg::SetMaxLogCount(int nCount)
    {
        m_nMaxLogCount = nCount;
    }
}
