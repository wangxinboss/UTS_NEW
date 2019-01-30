// V5UDeviceI2CDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UTDV_V5U.h"
#include "V5UDeviceI2CDlg.h"
#include "afxdialogex.h"
#include "UTS.h"
#include "V5UDevice.h"
#include "VCM.h"
#include   <fstream> 

using   namespace   std;
// CV5UDeviceI2CDlg 对话框

#pragma comment(lib, "UTS_VCM.lib")
IMPLEMENT_DYNAMIC(CV5UDeviceI2CDlg, CDialog)
int nDac=0;
int nDacStep=0;

CV5UDeviceI2CDlg::CV5UDeviceI2CDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CV5UDeviceI2CDlg::IDD, pParent)
{

}

CV5UDeviceI2CDlg::~CV5UDeviceI2CDlg()
{
}

void CV5UDeviceI2CDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_VCM, m_vcmMoveBar);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_dataEdit);
	
}


BEGIN_MESSAGE_MAP(CV5UDeviceI2CDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CV5UDeviceI2CDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CV5UDeviceI2CDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_READ, &CV5UDeviceI2CDlg::OnBnClickedButtonRead)
    ON_BN_CLICKED(IDC_BUTTON_WRITE, &CV5UDeviceI2CDlg::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_DAC_INCREASE, &CV5UDeviceI2CDlg::OnBnClickedButtonDacIncrease)
	ON_BN_CLICKED(IDC_BUTTON_VCM_INIT, &CV5UDeviceI2CDlg::OnBnClickedButtonVcmInit)
	ON_BN_CLICKED(IDC_BUTTON_DAC_REDUCE, &CV5UDeviceI2CDlg::OnBnClickedButtonDacReduce)
	ON_BN_CLICKED(IDC_BUTTON_DAC_MOVE, &CV5UDeviceI2CDlg::OnBnClickedButtonDacMove)
	ON_BN_CLICKED(IDC_BUTTON_DAC_INCREASE4, &CV5UDeviceI2CDlg::OnBnClickedButtonDacIncrease4)
	ON_BN_CLICKED(IDC_BUTTON_DAC_REDUCE4, &CV5UDeviceI2CDlg::OnBnClickedButtonDacReduce4)
	ON_BN_CLICKED(IDC_BUTTON_DAC_INCREASE20, &CV5UDeviceI2CDlg::OnBnClickedButtonDacIncrease20)
	ON_BN_CLICKED(IDC_BUTTON_DAC_REDUCE20, &CV5UDeviceI2CDlg::OnBnClickedButtonDacReduce20)
	ON_EN_CHANGE(IDC_EDIT_DAC, &CV5UDeviceI2CDlg::OnEnChangeEditDac)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_VCM, &CV5UDeviceI2CDlg::OnNMCustomdrawSliderVcm)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CV5UDeviceI2CDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_CLEARALL, &CV5UDeviceI2CDlg::OnBnClickedButtonClearall)
END_MESSAGE_MAP()


// CV5UDeviceI2CDlg 消息处理程序
BOOL CV5UDeviceI2CDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString strSection, strSlveAddr;
	int I2Cmode;
	if (!uts.dbCof.GetDeviceFilename(uts.info.nDeviceId, strSection))
	{
		uts.log.Error(_T("[%s]GetDeviceFilename Fail. nDeviceId = %d"), UTS_FN, uts.info.nDeviceId);
		return FALSE;
	}
	
	uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_SLAVEADDR"), strSlveAddr, _T("0x00"), _T("IIC Address"));

	//_stscanf_s(m_v5uRef.strHVS_SENSOR_SLAVEADDR, _T("%x"), &m_v5uRef.wHVS_SENSOR_SLAVEADDR);
	uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_I2CMODE"), I2Cmode, eI2CMode::BIT8_BIT8, _T("0:8_8, 1:8_16, 2:16_8, 3:16_16"));
	switch(I2Cmode)
	{
	case eI2CMode::BIT8_BIT8:
		SetDlgItemText(IDC_EDIT_ADDRESS_Length,_T("1"));
		SetDlgItemText(IDC_EDIT_VALUE_LENGTH,_T("1"));
		break;
	case BIT8_BIT16:
		SetDlgItemText(IDC_EDIT_ADDRESS_Length,_T("1"));
		SetDlgItemText(IDC_EDIT_VALUE_LENGTH,_T("2"));
		break;
	case BIT16_BIT8:
		SetDlgItemText(IDC_EDIT_ADDRESS_Length,_T("2"));
		SetDlgItemText(IDC_EDIT_VALUE_LENGTH,_T("1"));
		break;
	case BIT16_BIT16:
		SetDlgItemText(IDC_EDIT_ADDRESS_Length,_T("2"));
		SetDlgItemText(IDC_EDIT_VALUE_LENGTH,_T("2"));
		break;
	default:
		SetDlgItemText(IDC_EDIT_ADDRESS_Length,_T("1"));
		SetDlgItemText(IDC_EDIT_VALUE_LENGTH,_T("1"));
		break;
	}

	SetDlgItemText(IDC_EDIT_SLAVE_ADDRESS, strSlveAddr);
	//VCM滑块初始化
	m_vcmMoveBar.SetRange(0,1023);
	m_vcmMoveBar.SetTicFreq(100);//每1个单位画一刻度
	m_vcmMoveBar.SetPos(200);

	return TRUE;
}

void CV5UDeviceI2CDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    //CDialog::OnOK();
}

void CV5UDeviceI2CDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    //CDialog::OnCancel();
}

void CV5UDeviceI2CDlg::OnBnClickedButtonRead()
{
    // TODO: 在此添加控件通知处理程序代码
    unsigned int regData = 0;
	DWORD   slADDr  = -1,dwRegAddrLen = -1,dwRegAddr = -1,dwRegDataLen = -1;
	CString strDataBuf,strPreBuf;
	TCHAR	szSlaveAddr[25] = {0};
	TCHAR	szRegAddr[25] = {0};
	TCHAR	szRegAddrLen[25] = {0};
	TCHAR	szRegDataLen[25] = {0};
	unsigned char    dataBuf[1024] = {0};   

	//为空判断
	CString strSlaveEmpty = _T(""),strAddrLenEmpty = _T(""),strAddrEmpty = _T(""),strDataLenEmpty = _T("");
	GetDlgItem(IDC_EDIT_SLAVE_ADDRESS)->GetWindowText(strSlaveEmpty);
	GetDlgItem(IDC_EDIT_ADDRESS_Length)->GetWindowText(strAddrLenEmpty);
	GetDlgItem(IDC_EDIT_ADDRESS)->GetWindowText(strAddrEmpty);
	GetDlgItem(IDC_EDIT_VALUE_LENGTH)->GetWindowText(strDataLenEmpty);
	if(strSlaveEmpty.IsEmpty() || strAddrLenEmpty.IsEmpty() || strAddrEmpty.IsEmpty() || strDataLenEmpty.IsEmpty())
	{
		uts.log.Error(_T("Slave Address,AddrLen,Addr,DataLen can not be Empty!"));
		return;

	}

	GetDlgItemText(IDC_EDIT_SLAVE_ADDRESS, szSlaveAddr, 10);
	_stscanf(szSlaveAddr, _T("%x"), &slADDr);

	GetDlgItemText(IDC_EDIT_ADDRESS_Length, szRegAddrLen, 10);
	_stscanf(szRegAddrLen, _T("%d"), &dwRegAddrLen);

	GetDlgItemText(IDC_EDIT_ADDRESS, szRegAddr, 10);
	_stscanf(szRegAddr, _T("%x"), &dwRegAddr);

	GetDlgItemText(IDC_EDIT_VALUE_LENGTH, szRegDataLen, 10);
	_stscanf(szRegDataLen, _T("%d"), &dwRegDataLen);
	

	
    regData = ((V5UDevice*)uts.flow.m_pDevice)->IIc_Read_Atom((BYTE)slADDr,dwRegAddrLen,dwRegAddr,dwRegDataLen,dataBuf);
    if (!regData)
    {
        //_stprintf(szRegData, _T("%d"), regData);
        uts.log.Error(_T("Addr 0x%x Read Fail"), dwRegAddr);
    }
    else
    {

		for (int i= 0; i < dwRegDataLen; i++)
		{
			strPreBuf.Format(_T("%.2x\r\n"),dataBuf[i]);
			strDataBuf.Append(strPreBuf);
		}
		SetDlgItemText(IDC_EDIT_VALUE, strDataBuf);
    }
}

void CV5UDeviceI2CDlg::OnBnClickedButtonWrite()
{
    // TODO: 在此添加控件通知处理程序代码
	DWORD   slADDr,dwRegAddrLen,dwRegAddr,dwRegDataLen;
	CString strDataBuf;
	TCHAR	szSlaveAddr[25] = {0};
    TCHAR	szRegAddr[25] = {0};
	TCHAR	szRegAddrLen[25] = {0};
	TCHAR	szRegDataLen[25] = {0};
	unsigned char    szRegData[256] = {0};

	//为空判断
	CString strSlaveEmpty = _T(""),strAddrLenEmpty = _T(""),strAddrEmpty = _T(""),strDataLenEmpty = _T("");
	GetDlgItem(IDC_EDIT_SLAVE_ADDRESS)->GetWindowText(strSlaveEmpty);
	GetDlgItem(IDC_EDIT_ADDRESS_Length)->GetWindowText(strAddrLenEmpty);
	GetDlgItem(IDC_EDIT_ADDRESS)->GetWindowText(strAddrEmpty);
	GetDlgItem(IDC_EDIT_VALUE_LENGTH)->GetWindowText(strDataLenEmpty);
	if(strSlaveEmpty.IsEmpty() || strAddrLenEmpty.IsEmpty() || strAddrEmpty.IsEmpty() || strDataLenEmpty.IsEmpty())
	{
		uts.log.Error(_T("Slave Address,AddrLen,Addr,DataLen can not be Empty!"));
		return;

	}

	GetDlgItemText(IDC_EDIT_SLAVE_ADDRESS, szSlaveAddr, 8);
	 _stscanf(szSlaveAddr, _T("%x"), &slADDr);

	 GetDlgItemText(IDC_EDIT_ADDRESS_Length, szRegAddrLen, 10);
	 _stscanf(szRegAddrLen, _T("%d"), &dwRegAddrLen);

    GetDlgItemText(IDC_EDIT_ADDRESS, szRegAddr, 10);
    _stscanf(szRegAddr, _T("%x"), &dwRegAddr);

    GetDlgItemText(IDC_EDIT_VALUE_LENGTH, szRegDataLen, 10);
    _stscanf(szRegDataLen, _T("%d"), &dwRegDataLen);

	GetDlgItemText(IDC_EDIT_VALUE,strDataBuf);
	if (strDataBuf.IsEmpty())
	{
		uts.log.Error(_T("Data is Empty!!!!"));
		return;
	}

	int nlineCount = m_dataEdit.GetLineCount();
	CString strText;

	for (int i=0; i < nlineCount; i++)
	{
		int len = m_dataEdit.LineLength(m_dataEdit.LineIndex(i));
		m_dataEdit.GetLine(i, strText.GetBuffer(len), len);
		strText.ReleaseBuffer(len);
		 _stscanf(strText, _T("%x"), &szRegData[i]);
	}

    if (!((V5UDevice*)uts.flow.m_pDevice)->IIc_Write_Atom((BYTE)slADDr,dwRegAddrLen,dwRegAddr,dwRegDataLen,szRegData))
    {
		uts.log.Error(_T("Addr 0x%x Write Fail"), dwRegAddr);
    }
}


void CV5UDeviceI2CDlg::OnBnClickedButtonVcmInit()
{
	TCHAR	szDac[25] = {0};

	nDac=0;
	_stprintf(szDac, _T("%d"), nDac);
	SetDlgItemText(IDC_EDIT_DAC, szDac);
	// TODO: 北兜矪瞶盽Α祘Α絏
	DLLSetVCM_Initial((V5UDevice*)uts.flow.m_pDevice,uts.info.nVCMType, NULL);
}


void CV5UDeviceI2CDlg::OnBnClickedButtonDacMove()
{
	TCHAR	szDac[25] = {0};
	
	GetDlgItemText(IDC_EDIT_DAC, szDac, 10);
	_stscanf(szDac, _T("%d"), &nDac);
	//if()
	//nDac+=nDacStep;
	if(nDac>1023)
		nDac=1023;
	if(nDac<0)
		nDac=0;

	DLLSetVCM_Move((V5UDevice*)uts.flow.m_pDevice, uts.info.nVCMType, nDac);
}


void CV5UDeviceI2CDlg::OnBnClickedButtonDacIncrease()
{
	TCHAR	szDac[25] = {0};

	// TODO: 在此加入控制?告知?理常式程式碼
	//if()
	//nDac+=nDacStep;
	nDac+=1;
	if(nDac>1023)
		nDac=1023;
	_stprintf(szDac, _T("%d"), nDac);
	SetDlgItemText(IDC_EDIT_DAC, szDac);
	DLLSetVCM_Move((V5UDevice*)uts.flow.m_pDevice, uts.info.nVCMType, nDac);

}


void CV5UDeviceI2CDlg::OnBnClickedButtonDacReduce()
{
	TCHAR	szDac[25] = {0};

	// TODO: 在此加入控制?告知?理常式程式碼
	//nDac-=nDacStep;
	nDac-=1;
	if(nDac<0)
		nDac=0;
	_stprintf(szDac, _T("%d"), nDac);
	SetDlgItemText(IDC_EDIT_DAC, szDac);
	DLLSetVCM_Move((V5UDevice*)uts.flow.m_pDevice, uts.info.nVCMType, nDac);
}


void CV5UDeviceI2CDlg::OnBnClickedButtonDacIncrease4()
{
	TCHAR	szDac[25] = {0};

	// TODO: 在此加入控制?告知?理常式程式碼
	//if()
	//nDac+=nDacStep;
	nDac+=4;
	if(nDac>1023)
		nDac=1023;
	_stprintf(szDac, _T("%d"), nDac);
	SetDlgItemText(IDC_EDIT_DAC, szDac);
	DLLSetVCM_Move((V5UDevice*)uts.flow.m_pDevice, uts.info.nVCMType, nDac);

}


void CV5UDeviceI2CDlg::OnBnClickedButtonDacReduce4()
{
	TCHAR	szDac[25] = {0};

	// TODO: 在此加入控制?告知?理常式程式碼
	//nDac-=nDacStep;
	nDac-=4;
	if(nDac<0)
		nDac=0;
	_stprintf(szDac, _T("%d"), nDac);
	SetDlgItemText(IDC_EDIT_DAC, szDac);
	DLLSetVCM_Move((V5UDevice*)uts.flow.m_pDevice, uts.info.nVCMType, nDac);
}


void CV5UDeviceI2CDlg::OnBnClickedButtonDacIncrease20()
{
	TCHAR	szDac[25] = {0};

	// TODO: 在此加入控制?告知?理常式程式碼
	//if()
	//nDac+=nDacStep;
	nDac+=20;
	if(nDac>1023)
		nDac=1023;
	_stprintf(szDac, _T("%d"), nDac);
	SetDlgItemText(IDC_EDIT_DAC, szDac);
	DLLSetVCM_Move((V5UDevice*)uts.flow.m_pDevice, uts.info.nVCMType, nDac);

}


void CV5UDeviceI2CDlg::OnBnClickedButtonDacReduce20()
{
	TCHAR	szDac[25] = {0};

	// TODO: 在此加入控制?告知?理常式程式碼
	//nDac-=nDacStep;
	nDac-=20;
	if(nDac<0)
		nDac=0;
	_stprintf(szDac, _T("%d"), nDac);
	SetDlgItemText(IDC_EDIT_DAC, szDac);
	DLLSetVCM_Move((V5UDevice*)uts.flow.m_pDevice, uts.info.nVCMType, nDac);
}

void CV5UDeviceI2CDlg::OnEnChangeEditDac()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString editValue;
	GetDlgItemText(IDC_EDIT_DAC, editValue);

	int vcmValue = _ttoi(editValue);
	if (vcmValue>= 0 && vcmValue <= 1023)
	{
		m_vcmMoveBar.SetPos(vcmValue);
	}
}


void CV5UDeviceI2CDlg::OnNMCustomdrawSliderVcm(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	int nPos = m_vcmMoveBar.GetPos(); //获得滑块的当前位置
	//另外做一个编辑框 显示所调节的数据；
	CString str,editValue;
	GetDlgItemText(IDC_EDIT_DAC, editValue);

	int vcmValue = _ttoi(editValue);
	if (nPos != vcmValue)
	{
		str.Format(_T("%d"),nPos);
		SetDlgItemText(IDC_EDIT_DAC,str);
	}
}


void CV5UDeviceI2CDlg::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFileName[MAX_PATH] = {0};
	CString strFileName;
	OPENFILENAME SFN;
	memset(&SFN, 0, sizeof(OPENFILENAME));
	SFN.lStructSize = sizeof(OPENFILENAME);
	SFN.hwndOwner = GetSafeHwnd();
	SFN.lpstrFilter = _T("I2C  File(*.bin)\0*.bin\0\0");
	SFN.lpstrFile = szFileName;
	SFN.nMaxFile = MAX_PATH;
	if (!GetSaveFileName(&SFN))
	{
		uts.log.Error(_T("Did not select file."));
		//return FALSE;
	}
	strFileName = szFileName;
	if (OSUtil::GetExtFileName(strFileName).MakeUpper() != _T("BIN"))
	{
		strFileName.Append(_T(".bin"));
	}

	//获取Data edit数据
	int nlineCount = m_dataEdit.GetLineCount();
	CString strText;
	BYTE * szRegData = new BYTE[nlineCount];
	int testValue = 0;

	//因为读取会有换行，所以正常读取的参数会多出一行，所以保存时对长度进行了-1
	for (int i=0; i < nlineCount - 1; i++)
	{
		int len = m_dataEdit.LineLength(m_dataEdit.LineIndex(i));
		m_dataEdit.GetLine(i, strText.GetBuffer(len), len);
		strText.ReleaseBuffer(len);

		_stscanf(strText, _T("%x"), &testValue);
		szRegData[i] = (BYTE)testValue;
	}

	//写bin文件
	ofstream rs(strFileName,ios::binary);
	
	for(int i=0; i< nlineCount - 1;i++)
	{
		rs.write((char*)(&szRegData[i]),sizeof(szRegData[i]));
	}
	rs.close();

	if (szRegData)
	{
		delete [] szRegData;
		szRegData = NULL;
	}
	
}


void CV5UDeviceI2CDlg::OnBnClickedButtonClearall()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strDataBuf = _T("");
	GetDlgItemText(IDC_EDIT_VALUE,strDataBuf);
	if (strDataBuf.IsEmpty())
	{
		return;
	}

	//清空data框的内容
	m_dataEdit.SetSel(0,-1);
    m_dataEdit.Clear();
}
