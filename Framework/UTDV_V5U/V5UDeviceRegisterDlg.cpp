// V5UDeviceRegisterDlg.cpp : ÊµÏÖÎÄ¼þ
//

#include "stdafx.h"
#include "UTDV_V5U.h"
#include "V5UDeviceRegisterDlg.h"
#include "afxdialogex.h"


// CV5UDeviceRegisterDlg ¶Ô»°¿ò

IMPLEMENT_DYNAMIC(CV5UDeviceRegisterDlg, CDialog)

CV5UDeviceRegisterDlg::CV5UDeviceRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CV5UDeviceRegisterDlg::IDD, pParent)
{
    m_bInitialized = FALSE;
}

CV5UDeviceRegisterDlg::~CV5UDeviceRegisterDlg()
{
}

void CV5UDeviceRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_MARKLIST, m_markList);
    DDX_Control(pDX, IDC_LIST_MARKLIST2, m_markList2);
    DDX_Control(pDX, IDC_EDIT_REGISTER, m_registerEdit);
}


BEGIN_MESSAGE_MAP(CV5UDeviceRegisterDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CV5UDeviceRegisterDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CV5UDeviceRegisterDlg::OnBnClickedCancel)
    ON_LBN_SELCHANGE(IDC_LIST_MARKLIST, &CV5UDeviceRegisterDlg::OnLbnSelchangeListMarklist)
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_EN_KILLFOCUS(IDC_EDIT_REGISTER, &CV5UDeviceRegisterDlg::OnEnKillfocusEditRegister)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &CV5UDeviceRegisterDlg::OnBnClickedButtonDelete)
    ON_BN_CLICKED(IDC_BUTTON_ADD, &CV5UDeviceRegisterDlg::OnBnClickedButtonAdd)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CV5UDeviceRegisterDlg ÏûÏ¢´¦Àí³ÌÐò


BOOL CV5UDeviceRegisterDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ÔÚ´ËÌí¼Ó¶îÍâµÄ³õÊ¼»¯

    m_i2cDlg.Create(IDD_FRAME_I2C, this);
    m_i2cDlg.ShowWindow(SW_SHOW);

	int line = m_registerEdit.GetLimitText();
	//!! 2018.07.27 Chi-Jen.Liao 设定line*5(原本是line*2)
	m_registerEdit.SetLimitText(line *5);
    //-------------------------------------------------------------------------
    // Operator list
    vector<OPERATOR_INFO>::iterator itorOp;
    for (itorOp = uts.flow.m_vecOpList.begin(); itorOp != uts.flow.m_vecOpList.end(); itorOp++)
    {
        vector<CString> vecRegister;
        ((*itorOp).pInstance)->OnGetRegisterList(vecRegister);

        CString strRegister;
        for (size_t i = 0; i < vecRegister.size(); i++)
        {
            strRegister = vecRegister[i];
            strRegister = _T("[") + strRegister + _T("]");
            if (-1 == m_markList2.FindString(0, strRegister))
            {
                m_markList2.AddString(strRegister);
            }
        }

		vecRegister.clear();
    }

    AdjustWindow();

    m_bInitialized = TRUE;
    return TRUE;  // return TRUE unless you set the focus to a control
    // Òì³£: OCX ÊôÐÔÒ³Ó¦·µ»Ø FALSE
}

void CV5UDeviceRegisterDlg::AdjustWindow()
{
    RECT rcWindow;
    GetClientRect(&rcWindow);

    RECT rcMarkList;
    rcMarkList.left = rcWindow.left;
    rcMarkList.right = rcMarkList.left + (rcWindow.right - rcWindow.left) / 2;
    rcMarkList.top = rcWindow.top;
    rcMarkList.bottom = rcWindow.bottom / 5;
    m_markList.MoveWindow(&rcMarkList);

	RECT rcRegister;
	rcRegister.left = rcMarkList.right;
	rcRegister.right = rcWindow.right;
	rcRegister.top = rcWindow.top;
	rcRegister.bottom = rcWindow.bottom;
	m_registerEdit.MoveWindow(&rcRegister);

    RECT rcAddBtn;
    rcAddBtn.left = rcMarkList.left;
    rcAddBtn.right = rcAddBtn.left + 120;
    rcAddBtn.top = rcMarkList.bottom + 5;
    rcAddBtn.bottom = rcAddBtn.top + 25;
    GetDlgItem(IDC_BUTTON_ADD)->MoveWindow(&rcAddBtn);
    GetDlgItem(IDC_BUTTON_ADD)->Invalidate();

    RECT rcDeleteBtn;
    rcDeleteBtn.right = rcMarkList.right;
    rcDeleteBtn.left = rcDeleteBtn.right - 120;
    rcDeleteBtn.top = rcMarkList.bottom + 5;
    rcDeleteBtn.bottom = rcAddBtn.top + 25;
    GetDlgItem(IDC_BUTTON_DELETE)->MoveWindow(&rcDeleteBtn);
    GetDlgItem(IDC_BUTTON_DELETE)->Invalidate();

    RECT rcMarkList2;
    rcMarkList2.left = rcMarkList.left;
    rcMarkList2.right = rcMarkList.right;
    rcMarkList2.top = rcAddBtn.bottom + 5;
    rcMarkList2.bottom = rcWindow.bottom / 3 + 50 ;
    m_markList2.MoveWindow(&rcMarkList2);

    RECT rcI2CDlg;
    rcI2CDlg.left = rcMarkList.left;
    rcI2CDlg.right = rcMarkList2.right;
    rcI2CDlg.top = rcMarkList2.bottom + 2;
    rcI2CDlg.bottom = rcWindow.bottom;
    m_i2cDlg.MoveWindow(&rcI2CDlg);

	
}

void CV5UDeviceRegisterDlg::OnBnClickedOk()
{
    // ½ûÓÃ
    //CDialog::OnOK();
}

void CV5UDeviceRegisterDlg::OnBnClickedCancel()
{
    // ½ûÓÃ
    //CDialog::OnCancel();
}

void CV5UDeviceRegisterDlg::RefreshRegisterList(const vector<REGISTER_ITEM> &vecRegisterList)
{
    m_markList.ResetContent();
    for (size_t i = 0; i < vecRegisterList.size(); i++)
    {
        m_markList.AddString(vecRegisterList[i].strName);
    }
    m_markList.SetCurSel(-1);
    m_registerEdit.SetWindowText(EMPTY_STR);
}

void CV5UDeviceRegisterDlg::OnLbnSelchangeListMarklist()
{
    int nCurSel = m_markList.GetCurSel();
    if (nCurSel >= 0)
    {
        CString strRegisterData;
        strRegisterData = m_vecNewRegisterList[nCurSel].strData;
        strRegisterData.Replace(_T("\n"), _T("\r\n"));
        m_registerEdit.SetWindowText(strRegisterData);
    }
}

void CV5UDeviceRegisterDlg::OnEnKillfocusEditRegister()
{
    CString strRegisterData;
    m_registerEdit.GetWindowText(strRegisterData);
    strRegisterData.Replace(_T("\r\n"), _T("\n"));
    int nCurSel = m_markList.GetCurSel();
    if (nCurSel >= 0)
    {
        m_vecNewRegisterList[nCurSel].strData = strRegisterData;
    }
}

void CV5UDeviceRegisterDlg::OnBnClickedButtonAdd()
{
    CString strMarkName;
    int nCurSel = m_markList2.GetCurSel();
    if (nCurSel >= 0)
    {
        m_markList2.GetText(nCurSel, strMarkName);
        m_markList.AddString(strMarkName);
        REGISTER_ITEM ri = {0};
        ri.strName = strMarkName;
        ri.strData = EMPTY_STR;
        m_vecNewRegisterList.push_back(ri);
    }
}

void CV5UDeviceRegisterDlg::OnBnClickedButtonDelete()
{
    int nCurSel = m_markList.GetCurSel();
    if (nCurSel >= 0)
    {
        vector<REGISTER_ITEM>::iterator itor;
        int nIndex = 0;
        for (itor = m_vecNewRegisterList.begin(); itor != m_vecNewRegisterList.end(); itor++)
        {
            if (nIndex == nCurSel)
            {
                m_vecNewRegisterList.erase(itor);
                break;
            }
            nIndex++;
        }
    }
    m_markList.DeleteString(nCurSel);
}

void CV5UDeviceRegisterDlg::OnMove(int x, int y)
{
    CDialog::OnMove(x, y);

    // TODO: ÔÚ´Ë´¦Ìí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂë
    if (m_bInitialized)
    {
        AdjustWindow();
    }
}

void CV5UDeviceRegisterDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: ÔÚ´Ë´¦Ìí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂë
    if (m_bInitialized)
    {
        AdjustWindow();
    }
}

void CV5UDeviceRegisterDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    // TODO: ÔÚ´Ë´¦Ìí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂë
    if (bShow)
    {
        m_i2cDlg.ShowWindow(SW_SHOW);
    }
    else
    {
        m_i2cDlg.ShowWindow(SW_HIDE);
    }
}
