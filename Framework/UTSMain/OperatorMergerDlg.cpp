// OperatorMergerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UTSMain.h"
#include "OperatorMergerDlg.h"
#include "afxdialogex.h"
#include "OperatorSpecDlg.h"
#include "CheckHeadCtrl.h"

// COperatorMergerDlg 对话框

IMPLEMENT_DYNAMIC(COperatorMergerDlg, CDialog)

COperatorMergerDlg::COperatorMergerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COperatorMergerDlg::IDD, pParent)
{

}

COperatorMergerDlg::~COperatorMergerDlg()
{
}

void COperatorMergerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OPERATOR_SPEC, m_OperatorNameList);
	DDX_Control(pDX, IDC_LIST_OPERATOR_INFO, m_OperatorInfoList);
}


BEGIN_MESSAGE_MAP(COperatorMergerDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COperatorMergerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COperatorMergerDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OPERATOR_SPEC, &COperatorMergerDlg::OnNMClickListOperatorSpec)
END_MESSAGE_MAP()


// COperatorMergerDlg 消息处理程序
BOOL COperatorMergerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 初始化属性控件
	
	// All Operator List
	DWORD dwStyle = m_OperatorNameList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;    //选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;        //网格线（只适用与report风格的listctrl）
	m_OperatorNameList.SetExtendedStyle(dwStyle| LVS_EX_CHECKBOXES);   //设置扩展风格
	// 插入列
	m_OperatorNameList.InsertColumn(0, _T("Operator Name"), LVCFMT_LEFT, 200);
	m_OperatorNameList.Init();

	//------------------------------------------------------------------------------
	// Initial start Operator List
	m_OperatorInfoList.SetExtendedStyle(dwStyle);
	m_OperatorInfoList.InsertColumn(0, _T("Key"), LVCFMT_LEFT, 200);
	m_OperatorInfoList.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 200);
	
	return TRUE;
}

BOOL COperatorMergerDlg::AddOperatorSpecData(vector<OPERATOR_SPEC> &allOpeSpec)
{
	m_OperatorNameList.DeleteAllItems();
	m_OperatorInfoList.DeleteAllItems();

	m_OperatorNameList.ShowWindow(SW_HIDE);
	for (size_t i = 0; i < allOpeSpec.size(); i++)
	{
		m_OperatorNameList.InsertItem(i, allOpeSpec[i].strOperatorFileName);
		OperatorAndItemMap.insert(make_pair(allOpeSpec[i].strOperatorFileName,allOpeSpec[i].vecSingleSpec));
	}
	m_OperatorNameList.ShowWindow(SW_SHOW);
	return TRUE;
}

void COperatorMergerDlg::OnBnClickedOk()
{
	//获取Operator中CheckBox选中的信息
	MergerOpName.clear();

	for(int i=0; i<m_OperatorNameList.GetItemCount(); i++ )
	{
		if(m_OperatorNameList.GetCheck(i))
		{
			MergerOpName.push_back(m_OperatorNameList.GetItemText(i,0));
		}
	}

	//调用父窗口进行数据合并
	COperatorSpecDlg *pDlg = (COperatorSpecDlg*)this->GetParent();
	pDlg->doMergerOp(MergerOpName,OperatorAndItemMap);
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}


void COperatorMergerDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}


void COperatorMergerDlg::OnNMClickListOperatorSpec(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	// TODO: 在此添加控件通知处理程序代码
	//获取选中的Operator Name
	int iSelectRow = -1;
	if(pNMItemActivate != NULL)
	{
		if(pNMItemActivate->iItem != -1)
			iSelectRow = pNMItemActivate->iItem;
		else 
			return;
	}
	CString operatorName= m_OperatorNameList.GetItemText(iSelectRow,0);
	//显示内容信息
	m_OperatorInfoList.DeleteAllItems();

	map<CString,vector<SINGLE_ITEM>>::iterator iter;
	vector<SINGLE_ITEM> vecSingleSpec;
	iter = OperatorAndItemMap.find(operatorName);  
	if(iter != OperatorAndItemMap.end())  
	{  
		vecSingleSpec = iter->second;  
	}  

	m_OperatorInfoList.ShowWindow(SW_HIDE);
	for (size_t i = 0; i < vecSingleSpec.size(); i++)
	{
		m_OperatorInfoList.InsertItem(i,vecSingleSpec[i].strKey);
		m_OperatorInfoList.SetItemText(i,1,vecSingleSpec[i].strValue);
	}
	m_OperatorInfoList.ShowWindow(SW_SHOW);
}


