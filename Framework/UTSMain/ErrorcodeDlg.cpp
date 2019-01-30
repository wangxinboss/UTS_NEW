// ErrorcodeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UTSMain.h"
#include "ErrorcodeDlg.h"
#include "afxdialogex.h"


// CErrorcodeDlg 对话框

IMPLEMENT_DYNAMIC(CErrorcodeDlg, CDialog)

CErrorcodeDlg::CErrorcodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CErrorcodeDlg::IDD, pParent)
{

}

CErrorcodeDlg::~CErrorcodeDlg()
{
}

void CErrorcodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CErrorcodeDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CErrorcodeDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CErrorcodeDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CErrorcodeDlg 消息处理程序


BOOL CErrorcodeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //------------------------------------------------------------------------------
    // 初始化属性控件
    CRect rc;
    GetClientRect(rc);
    rc.bottom -= 40;
    m_propertyGrid.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rc, this, (UINT)-1);
    HDITEM item; 
    item.cxy = 300; 
    item.mask = HDI_WIDTH; 
    m_propertyGrid.GetHeaderCtrl().SetItem(0, &item);
    m_propertyGrid.EnableHeaderCtrl(TRUE, _T("Return Value"), _T("Error Code"));
    m_propertyGrid.EnableDescriptionArea();
    m_propertyGrid.MarkModifiedProperties();
    m_propertyGrid.ExpandAll();

    if (!uts.errorcode.GetAllErrorCodeInfoFromDB())
    {
        uts.log.Error(_T("GetAllErrorCodeInfoFromDB Fail."));
        return FALSE;
    }

    //------------------------------------------------------------------------------
    // 将规格更新到界面控件上
    CString strKey;
    CString strComment;
    for (size_t i = 0; i < uts.errorcode.m_vecAllSetting.size(); i++)
    {
        strComment.Format(_T("Value returned from %s"), uts.errorcode.m_vecAllSetting[i].strOperatorFileName);
        CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(uts.errorcode.m_vecAllSetting[i].strOperatorFileName);
        for (size_t j = 0; j < uts.errorcode.m_vecAllSetting[i].vecSingleErrorcodeInfo.size(); j++)
        {
            strKey.Format(_T("%d"), uts.errorcode.m_vecAllSetting[i].vecSingleErrorcodeInfo[j].nReturnValue);
            CMFCPropertyGridProperty * pProp = new CMFCPropertyGridProperty(
                strKey,
                uts.errorcode.m_vecAllSetting[i].vecSingleErrorcodeInfo[j].strErrorCode,
                strComment);
            pGroup->AddSubItem(pProp);
        }
        m_propertyGrid.AddProperty(pGroup);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CErrorcodeDlg::OnBnClickedOk()
{
    //------------------------------------------------------------------------------
    // 将界面控件的内容存入DB
    for (int i = 0; i < m_propertyGrid.GetPropertyCount(); i++)
    {
        for (int j = 0; j < m_propertyGrid.GetProperty(i)->GetSubItemsCount(); j++)
        {
            CMFCPropertyGridProperty * pProp = m_propertyGrid.GetProperty(i)->GetSubItem(j);
            uts.errorcode.m_vecAllSetting[i].vecSingleErrorcodeInfo[j].strErrorCode = pProp->GetValue();
        }
    }
    
    if (!uts.dbCof.SetErrorcodeAllSetting(uts.errorcode.m_vecAllSetting))
    {
        AfxMessageBox(
            _T("Set Errorcode All Setting Fail."),
            MB_ICONERROR);
        return;
    }

    CDialog::OnOK();
}

void CErrorcodeDlg::OnBnClickedCancel()
{
    BOOL bChanged = FALSE;
    //------------------------------------------------------------------------------
    // 判断更改
    for (int i = 0; i < m_propertyGrid.GetPropertyCount(); i++)
    {
        for (int j = 0; j < m_propertyGrid.GetProperty(i)->GetSubItemsCount(); j++)
        {
            CMFCPropertyGridProperty * pProp = m_propertyGrid.GetProperty(i)->GetSubItem(j);
            if (uts.errorcode.m_vecAllSetting[i].vecSingleErrorcodeInfo[j].strErrorCode != pProp->GetValue())
            {
                bChanged = TRUE;
            }
        }
    }
    if (bChanged)
    {
        int nRet = AfxMessageBox(
            _T("Config changed, Do you want to discard the changes?"),
            MB_YESNO | MB_ICONQUESTION);
        if (nRet == IDNO)
        {
            return;
        }
    }

    CDialog::OnCancel();
}
