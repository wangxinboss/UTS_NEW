// ProjectConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "UTSMain.h"
#include "ProjectConfig.h"
#include "afxdialogex.h"
#include "Algorithm.h"

#pragma comment(lib, "UTSAlgorithm.lib")

using namespace UTS::Algorithm;
// CProjectConfig 对话框

IMPLEMENT_DYNAMIC(CProjectConfig, CDialog)

CProjectConfig::CProjectConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectConfig::IDD, pParent)
{

}

CProjectConfig::~CProjectConfig()
{
}

void CProjectConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProjectConfig, CDialog)
    ON_BN_CLICKED(IDOK, &CProjectConfig::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CProjectConfig::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_PROJECT, &CProjectConfig::OnBnClickedButtonExportProject)
END_MESSAGE_MAP()


// CProjectConfig 消息处理程序


BOOL CProjectConfig::OnInitDialog()
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
    m_propertyGrid.EnableHeaderCtrl(TRUE, _T("Key"), _T("Value"));
    m_propertyGrid.EnableDescriptionArea();
    m_propertyGrid.MarkModifiedProperties();
    m_propertyGrid.ExpandAll();

    //------------------------------------------------------------------------------
    // 从DB中读取规格
    BOOL bRet = uts.dbCof.GetProjectAllSetting(m_vecAllSetting);
    if (!bRet)
    {
        AfxMessageBox(_T("Get Project Setting Fail."));
        return FALSE;
    }

    //------------------------------------------------------------------------------
    // 将规格更新到界面控件上
    for (size_t i = 0; i < m_vecAllSetting.size(); i++)
    {
        CMFCPropertyGridProperty * pProp = new CMFCPropertyGridProperty(
            m_vecAllSetting[i].strKey,
            m_vecAllSetting[i].strValue,
            m_vecAllSetting[i].strComment);
        m_propertyGrid.AddProperty(pProp);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CProjectConfig::OnBnClickedOk()
{
    BOOL bChanged = FALSE;
    //------------------------------------------------------------------------------
    // 将界面控件的内容存入DB
    for (int i = 0; i < m_propertyGrid.GetPropertyCount(); i++)
    {
        if (m_vecAllSetting[i].strValue != m_propertyGrid.GetProperty(i)->GetValue())
        {
            bChanged = TRUE;
            m_vecAllSetting[i].strValue = m_propertyGrid.GetProperty(i)->GetValue();
        }
    }

    if (bChanged)
    {
        if (!uts.dbCof.SetProjectAllSetting(m_vecAllSetting))
        {
            AfxMessageBox(
                _T("Set Project All Setting Fail."),
                MB_ICONERROR);
            return;
        }
        uts.MessageRestart(_T("Config changed, to enable new settings UTS must be restarted. Restart now?"));
    }

    CDialog::OnOK();
}

void CProjectConfig::OnBnClickedCancel()
{
    BOOL bChanged = FALSE;
    //------------------------------------------------------------------------------
    // 判断更改
    for (int i = 0; i < m_propertyGrid.GetPropertyCount(); i++)
    {
        if (m_vecAllSetting[i].strValue != m_propertyGrid.GetProperty(i)->GetValue())
        {
            bChanged = TRUE;
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


void CProjectConfig::OnBnClickedButtonExportProject()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFileName;
	BOOL bRet = ExportFromVectToFile(m_vecAllSetting, strFileName);
	//BOOL bRet = ExportFromVectToXMLFile(m_vecAllSpec, strFileName);
	if (bRet)
	{
		AfxMessageBox(_T("Success to export ") + strFileName, MB_ICONINFORMATION | MB_OK);
	}
	else
	{
		AfxMessageBox(_T("Fail to export ") + strFileName, MB_ICONERROR | MB_OK);
	}
}

BOOL CProjectConfig::ExportFromVectToFile(vector<SINGLE_ITEM> &vecSetting, CString &strFileName)
{
	//暂时将数据全部导出到ini文件中，后期可以改为XML
	//获取文件路径
	TCHAR szFileName[MAX_PATH] = {0};
	OPENFILENAME SFN;
	memset(&SFN, 0, sizeof(OPENFILENAME));
	SFN.lStructSize = sizeof(OPENFILENAME);
	SFN.hwndOwner = GetSafeHwnd();
	SFN.lpstrFilter = _T("Project Config  File(*.ini)\0*.ini\0\0");
	SFN.lpstrFile = szFileName;
	SFN.nMaxFile = MAX_PATH;
	if (!GetSaveFileName(&SFN))
	{
		uts.log.Error(_T("Did not select file."));
		return FALSE;
	}
	strFileName = szFileName;
	if (OSUtil::GetExtFileName(strFileName).MakeUpper() != _T("INI"))
	{
		strFileName.Append(_T(".ini"));
	}

	IniFile m_projectConfigFile(strFileName);
	
	//写入数据
	CString strProjectConfig;	
	strProjectConfig.Format(_T("Project Config"));

	for (size_t j = 0; j < vecSetting.size(); j++)
	{
		CString strKey = vecSetting[j].strKey;
		CString strValue = vecSetting[j].strValue;
		m_projectConfigFile.INIWrite_File(strProjectConfig,strKey,strValue);
	}

	return TRUE;
}
