// V5UDeviceSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UTDV_V5U.h"
#include "V5UDeviceSettingDlg.h"
#include "afxdialogex.h"
#include "V5UDevice.h"
#include "Algorithm.h"

// CV5UDeviceSettingDlg 对话框

using namespace UTS::Algorithm;


IMPLEMENT_DYNAMIC(CV5UDeviceSettingDlg, CDialog)

CV5UDeviceSettingDlg::CV5UDeviceSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CV5UDeviceSettingDlg::IDD, pParent)
{
    m_bInitialized = FALSE;
}

CV5UDeviceSettingDlg::~CV5UDeviceSettingDlg()
{
}

void CV5UDeviceSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RADIO_REGISTER, m_registerButton);
}


BEGIN_MESSAGE_MAP(CV5UDeviceSettingDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CV5UDeviceSettingDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CV5UDeviceSettingDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CV5UDeviceSettingDlg::OnBnClickedButtonImport)
    ON_BN_CLICKED(IDC_RADIO_REGISTER, &CV5UDeviceSettingDlg::OnBnClickedRadioRegister)
    ON_WM_MOVE()
    ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_V5U, &CV5UDeviceSettingDlg::OnBnClickedButtonExportV5u)
END_MESSAGE_MAP()


// CV5UDeviceSettingDlg 消息处理程序


BOOL CV5UDeviceSettingDlg::OnInitDialog()
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
    // 创建Register Dialog
    if (!m_registerDlg.Create(IDD_V5UDeviceRegisterDlg, this))
    {
        AfxMessageBox(_T("Create register dialog fail!"));
        return FALSE;
    }
    else
    {
        // 设定board的初始宽度
        RECT rcRegisterDlg = {0};
        rcRegisterDlg.right = rcRegisterDlg.left + 500;
        m_registerDlg.MoveWindow(&rcRegisterDlg);
        AdjustWindow();
    }

    //------------------------------------------------------------------------------
    // 从DB中读取规格
    BOOL bRet = FALSE;
    // 根据device_id读出所有规格
    bRet = uts.dbCof.GetDeviceSetting(uts.info.nDeviceId, m_deviceSetting);
    if (!bRet)
    {
        AfxMessageBox(_T("Get Device Setting Fail."));
        return FALSE;
    }
    bRet = uts.dbCof.GetAllRegister(m_registerDlg.m_vecOldRegisterList);
    if (!bRet)
    {
        AfxMessageBox(_T("Get All Register Fail."));
        return FALSE;
    }
    for (size_t i = 0; i < m_registerDlg.m_vecOldRegisterList.size(); i++)
    {
        m_registerDlg.m_vecNewRegisterList.push_back(m_registerDlg.m_vecOldRegisterList[i]);
    }

    //------------------------------------------------------------------------------
    // 将规格更新到界面控件上
    for (size_t i = 0; i < m_deviceSetting.vecSingleSetting.size(); i++)
    {
        CMFCPropertyGridProperty * pProp = new CMFCPropertyGridProperty(
            m_deviceSetting.vecSingleSetting[i].strKey,
            m_deviceSetting.vecSingleSetting[i].strValue,
            m_deviceSetting.vecSingleSetting[i].strComment);
        m_propertyGrid.AddProperty(pProp);
    }
    m_registerDlg.RefreshRegisterList(m_registerDlg.m_vecOldRegisterList);

    m_registerButton.SetCheck(BST_CHECKED);
    m_registerDlg.ShowWindow(SW_SHOW);
    m_bInitialized = TRUE;
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CV5UDeviceSettingDlg::OnMove(int x, int y)
{
    CDialog::OnMove(x, y);

    // TODO: 在此处添加消息处理程序代码
    if (m_bInitialized)
    {
        AdjustWindow();
    }
}

void CV5UDeviceSettingDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (m_bInitialized)
    {
        AdjustWindow();
    }
}

void CV5UDeviceSettingDlg::AdjustWindow()
{
    RECT rcSettingDlg = {0};
    GetWindowRect(&rcSettingDlg);

    //------------------------------------------------------------------------------
    // Register dialog
    RECT rcRegisterDlg = {0};
    m_registerDlg.GetWindowRect(&rcRegisterDlg);
    int nRightDlgWidth = rcRegisterDlg.right - rcRegisterDlg.left;
    rcRegisterDlg.left = rcSettingDlg.right;
    rcRegisterDlg.right = rcRegisterDlg.left + nRightDlgWidth;
    rcRegisterDlg.top = rcSettingDlg.top;
    rcRegisterDlg.bottom = rcSettingDlg.bottom;
    m_registerDlg.MoveWindow(&rcRegisterDlg);
    //------------------------------------------------------------------------------
    // propertyGrid
    RECT rcSettingDlgClient = {0};
    GetClientRect(&rcSettingDlgClient);
    RECT rcPropertyGrid = rcSettingDlgClient;
    rcPropertyGrid.bottom = rcSettingDlgClient.bottom - 40;
    m_propertyGrid.MoveWindow(&rcPropertyGrid);
    //------------------------------------------------------------------------------
    // Buttons
    RECT rcButton = {0};
    GetDlgItem(IDOK)->GetWindowRect(&rcButton);
    int nBtnWidth = rcButton.right - rcButton.left;
    int nBtnHeight = rcButton.bottom - rcButton.top;
    RECT rcDrawButton = {0};
    int nCtrlWidth = nBtnWidth + 10;
    rcDrawButton.left = rcSettingDlgClient.right - 5 * nCtrlWidth;
    rcDrawButton.right = rcDrawButton.left + nBtnWidth;
    rcDrawButton.bottom = rcSettingDlgClient.bottom - 5;
    rcDrawButton.top = rcDrawButton.bottom - nBtnHeight;
    GetDlgItem(IDC_BUTTON_EXPORT_V5U)->MoveWindow(&rcDrawButton);
    GetDlgItem(IDC_BUTTON_EXPORT_V5U)->Invalidate();
    // IDC_BUTTON_IMPORT
    rcDrawButton.left = rcSettingDlgClient.right - 4 * nCtrlWidth;
    rcDrawButton.right = rcDrawButton.left + nBtnWidth;
    rcDrawButton.bottom = rcSettingDlgClient.bottom - 5;
    rcDrawButton.top = rcDrawButton.bottom - nBtnHeight;
    GetDlgItem(IDC_BUTTON_IMPORT)->MoveWindow(&rcDrawButton);
    GetDlgItem(IDC_BUTTON_IMPORT)->Invalidate();
    // IDC_RADIO_REGISTER
    rcDrawButton.left = rcSettingDlgClient.right - 3 * nCtrlWidth;
    rcDrawButton.right = rcDrawButton.left + nBtnWidth;
    rcDrawButton.bottom = rcSettingDlgClient.bottom - 5;
    rcDrawButton.top = rcDrawButton.bottom - nBtnHeight;
    GetDlgItem(IDC_RADIO_REGISTER)->MoveWindow(&rcDrawButton);
    GetDlgItem(IDC_RADIO_REGISTER)->Invalidate();
    // IDOK
    rcDrawButton.left = rcSettingDlgClient.right - 2 * nCtrlWidth;
    rcDrawButton.right = rcDrawButton.left + nBtnWidth;
    rcDrawButton.bottom = rcSettingDlgClient.bottom - 5;
    rcDrawButton.top = rcDrawButton.bottom - nBtnHeight;
    GetDlgItem(IDOK)->MoveWindow(&rcDrawButton);
    GetDlgItem(IDOK)->Invalidate();
    // IDCANCEL
    rcDrawButton.left = rcSettingDlgClient.right - 1 * nCtrlWidth;
    rcDrawButton.right = rcDrawButton.left + nBtnWidth;
    rcDrawButton.bottom = rcSettingDlgClient.bottom - 5;
    rcDrawButton.top = rcDrawButton.bottom - nBtnHeight;
    GetDlgItem(IDCANCEL)->MoveWindow(&rcDrawButton);
    GetDlgItem(IDCANCEL)->Invalidate();
}

void CV5UDeviceSettingDlg::OnBnClickedOk()
{
    BOOL bChanged = FALSE;
    // 将界面内容写入内存变量
    for (int i = 0; i < m_propertyGrid.GetPropertyCount(); i++)
    {
        CMFCPropertyGridProperty * pProp = m_propertyGrid.GetProperty(i);
        if (m_deviceSetting.vecSingleSetting[i].strValue != pProp->GetValue())
        {
            bChanged = TRUE;
            m_deviceSetting.vecSingleSetting[i].strValue = pProp->GetValue();
        }
    }
    
    if (m_registerDlg.m_vecNewRegisterList.size() == m_registerDlg.m_vecOldRegisterList.size())
    {
        for (size_t i = 0; i < m_registerDlg.m_vecNewRegisterList.size(); i++)
        {
            if (m_registerDlg.m_vecNewRegisterList[i].strName != m_registerDlg.m_vecOldRegisterList[i].strName)
            {
                bChanged = TRUE;
                break;
            }
            if (m_registerDlg.m_vecNewRegisterList[i].strData != m_registerDlg.m_vecOldRegisterList[i].strData)
            {
                bChanged = TRUE;
                break;
            }
        }
    }
    else
    {
        bChanged = TRUE;
    }

    if (bChanged)
    {
        // 更新DB
        if (!uts.dbCof.SetDeviceSetting(m_deviceSetting))
        {
            AfxMessageBox(
                _T("Set Device Setting Fail."),
                MB_ICONERROR);
            return;
        }
        if (!uts.dbCof.DeleteV5UDeviceRegister())
        {
            AfxMessageBox(
                _T("DeleteV5UDeviceRegister Fail."),
                MB_ICONERROR);
            return;
        }
        for (size_t i = 0; i < m_registerDlg.m_vecNewRegisterList.size(); i++)
        {
            if (!uts.dbCof.AddV5UDeviceRegister(
                m_registerDlg.m_vecNewRegisterList[i].strName,
                m_registerDlg.m_vecNewRegisterList[i].strData))
            {
                AfxMessageBox(
                    _T("AddV5UDeviceRegister Fail."),
                    MB_ICONERROR);
                return;
            }
        }
        //uts.MessageRestart(_T("Config changed, to enable new settings UTS must be restarted. Restart now?"));
        BOOL bRet = ((V5UDevice*)uts.flow.m_pDevice)->ReadV5URef();
        if (!bRet)
        {
            AfxMessageBox(
                _T("Reload V5URef error!"),
                MB_ICONERROR);
            return;
        }
    }

    CDialog::OnOK();
}


void CV5UDeviceSettingDlg::OnBnClickedCancel()
{
    BOOL bChanged = FALSE;
    //------------------------------------------------------------------------------
    // 判断更改
    for (int i = 0; i < m_propertyGrid.GetPropertyCount(); i++)
    {
        CMFCPropertyGridProperty * pProp = m_propertyGrid.GetProperty(i);
        if (m_deviceSetting.vecSingleSetting[i].strValue != pProp->GetValue())
        {
            bChanged = TRUE;
        }
    }

    if (m_registerDlg.m_vecNewRegisterList.size() == m_registerDlg.m_vecOldRegisterList.size())
    {
        for (size_t i = 0; i < m_registerDlg.m_vecNewRegisterList.size(); i++)
        {
            if (m_registerDlg.m_vecNewRegisterList[i].strName != m_registerDlg.m_vecOldRegisterList[i].strName)
            {
                bChanged = TRUE;
                break;
            }
            if (m_registerDlg.m_vecNewRegisterList[i].strData != m_registerDlg.m_vecOldRegisterList[i].strData)
            {
                bChanged = TRUE;
                break;
            }
        }
    }
    else
    {
        bChanged = TRUE;
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


void CV5UDeviceSettingDlg::OnBnClickedButtonImport()
{
    //------------------------------------------------------------------------------
    // 弹出选择单个文件的对话框
    TCHAR szFileName[MAX_PATH] = {0};
    OPENFILENAME SFN;
    memset(&SFN, 0, sizeof(OPENFILENAME));
    SFN.lStructSize = sizeof(OPENFILENAME);
    SFN.hwndOwner = GetSafeHwnd();
    SFN.lpstrFilter = _T("V5U Booting File(*.ini)\0*.ini\0\0");
    SFN.lpstrFile = szFileName;
    SFN.nMaxFile = MAX_PATH;
    SFN.Flags = OFN_FILEMUSTEXIST;
    if (!GetOpenFileName(&SFN))
    {
        return;
    }
    //------------------------------------------------------------------------------
    // 从配置文件中读入配置
    IniFile v5uRefFile(szFileName);
    V5U_REF v5uRef;
    v5uRef.DEVICE_COUNT = 1;
	v5uRef.BOARD_SET_INDEX_NUM = 0;
    //v5uRef.BOARD_SET_INDEX_NUM = v5uRefFile.ReadValue(_T("BOARD_SET"), _T("INDEX_NUM"), 0);

    // pin on
    v5uRef.power_on[0] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin1_On"), 1);
    v5uRef.power_on[1] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin2_On"), 1);
    v5uRef.power_on[2] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin3_On"), 1);
    v5uRef.power_on[3] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin4_On"), 1);
    v5uRef.power_on[4] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin35_On"), 1);
    v5uRef.power_on[5] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin36_On"), 1);
    v5uRef.power_on[6] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPinIO_On"), 0); //io power.

    // pin volt
    v5uRef.power_volt[0] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin1_Volt"), 2.80);
    v5uRef.power_volt[1] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin2_Volt"), 2.80);
    v5uRef.power_volt[2] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin3_Volt"), 2.80);
    v5uRef.power_volt[3] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin4_Volt"), 2.80);
    v5uRef.power_volt[4] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin35_Volt"), 2.80);
    v5uRef.power_volt[5] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPin36_Volt"), 2.80);

    if (1 == v5uRef.power_on[6])
    {
        v5uRef.power_volt[6] = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PowerPinIO_Volt"), 2.80);
    }
    else
    {
        v5uRef.power_volt[6] = 1.8;   // default sensor IO volt.
    }

    v5uRef.HW_AVDDSlowUPEn        = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_AVDDSlowUPEn"), 0);
    v5uRef.HW_PGMFREQ             = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_PGMFREQ"), 12);
    v5uRef.HW_OSC_TYPE            = v5uRefFile.ReadValue(_T("HVS_HW"), _T("HW_OSC_TYPE"), 0);

    // sensor set
    v5uRef.HVS_SENSOR_VCLK        = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_VCLK"), 1);
    v5uRef.HVS_SENSOR_RESET       = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_RESET"), 1);
    v5uRef.HVS_SENSOR_ENB         = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_ENB"), 1);
    v5uRef.HVS_SENSOR_VSYNC       = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_VSYNC"), 1);
    v5uRef.strHVS_SENSOR_SLAVEADDR= v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_SLAVEADDR"), _T("0x00"));
    _stscanf_s(v5uRef.strHVS_SENSOR_SLAVEADDR, _T("%x"), &v5uRef.wHVS_SENSOR_SLAVEADDR);
    v5uRef.HVS_SENSOR_SIZE_X      = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_SIZE_X"), 0);
    v5uRef.HVS_SENSOR_SIZE_Y      = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_SIZE_Y"), 0);
    v5uRef.HVS_SENSOR_DATA_FORMAT = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_DATA_FORMAT"), eDataFormat::YUV);
    v5uRef.HVS_SENSOR_OUT_MODE    = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_OUT_MODE"), eBayerOutputMode::BGGR);
    v5uRef.HVS_SENSOR_MIPIMODE    = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_MIPIMODE"), eMipiMode::Line4);
    v5uRef.HVS_SENSOR_I2CMODE     = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_I2CMODE"), eI2CMode::BIT8_BIT8);
    v5uRef.HVS_SENSOR_SleepTime   = v5uRefFile.ReadValue(_T("SENSOR_SET"), _T("HVS_SENSOR_SleepTime"), 0);

    //------------------------------------------------------------------------------
    // 更新界面
    for (int i = 0; i < m_propertyGrid.GetPropertyCount(); i++)
    {
        CMFCPropertyGridProperty * pProp = m_propertyGrid.GetProperty(i);
        CString strKey = pProp->GetName();
        CString strTmp;
        if (strKey == _T("DEVICE_COUNT"))
        {
            strTmp.Format(_T("%d"), v5uRef.DEVICE_COUNT);
        }
        else if (strKey == _T("BOARD_SET_INDEX_NUM"))
        {
            strTmp.Format(_T("%d"), v5uRef.BOARD_SET_INDEX_NUM);
        }
        else if (strKey == _T("HW_PowerPin1_On"))
        {
            strTmp.Format(_T("%d"), v5uRef.power_on[0]);
        }
        else if (strKey == _T("HW_PowerPin2_On"))
        {
            strTmp.Format(_T("%d"), v5uRef.power_on[1]);
        }
        else if (strKey == _T("HW_PowerPin3_On"))
        {
            strTmp.Format(_T("%d"), v5uRef.power_on[2]);
        }
        else if (strKey == _T("HW_PowerPin4_On"))
        {
            strTmp.Format(_T("%d"), v5uRef.power_on[3]);
        }
        else if (strKey == _T("HW_PowerPin35_On"))
        {
            strTmp.Format(_T("%d"), v5uRef.power_on[4]);
        }
        else if (strKey == _T("HW_PowerPin36_On"))
        {
            strTmp.Format(_T("%d"), v5uRef.power_on[5]);
        }
        else if (strKey == _T("HW_PowerPinIO_On"))
        {
            strTmp.Format(_T("%d"), v5uRef.power_on[6]);
        }
        else if (strKey == _T("HW_PowerPin1_Volt"))
        {
            strTmp.Format(_T("%f"), v5uRef.power_volt[0]);
        }
        else if (strKey == _T("HW_PowerPin2_Volt"))
        {
            strTmp.Format(_T("%f"), v5uRef.power_volt[1]);
        }
        else if (strKey == _T("HW_PowerPin3_Volt"))
        {
            strTmp.Format(_T("%f"), v5uRef.power_volt[2]);
        }
        else if (strKey == _T("HW_PowerPin4_Volt"))
        {
            strTmp.Format(_T("%f"), v5uRef.power_volt[3]);
        }
        else if (strKey == _T("HW_PowerPin35_Volt"))
        {
            strTmp.Format(_T("%f"), v5uRef.power_volt[4]);
        }
        else if (strKey == _T("HW_PowerPin36_Volt"))
        {
            strTmp.Format(_T("%f"), v5uRef.power_volt[5]);
        }
        else if (strKey == _T("HW_PowerPinIO_Volt"))
        {
            strTmp.Format(_T("%f"), v5uRef.power_volt[6]);
        }
        else if (strKey == _T("HW_AVDDSlowUPEn"))
        {
            strTmp.Format(_T("%d"), v5uRef.HW_AVDDSlowUPEn);
        }
        else if (strKey == _T("HW_PGMFREQ"))
        {
            strTmp.Format(_T("%d"), v5uRef.HW_PGMFREQ);
        }
        else if (strKey == _T("HW_OSC_TYPE"))
        {
            strTmp.Format(_T("%d"), v5uRef.HW_OSC_TYPE);
        }
        else if (strKey == _T("HVS_SENSOR_VCLK"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_VCLK);
        }
        else if (strKey == _T("HVS_SENSOR_RESET"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_RESET);
        }
        else if (strKey == _T("HVS_SENSOR_ENB"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_ENB);
        }
        else if (strKey == _T("HVS_SENSOR_VSYNC"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_VSYNC);
        }
        else if (strKey == _T("HVS_SENSOR_SLAVEADDR"))
        {
            strTmp.Format(_T("%s"), v5uRef.strHVS_SENSOR_SLAVEADDR);
        }
        else if (strKey == _T("HVS_SENSOR_SIZE_X"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_SIZE_X);
        }
        else if (strKey == _T("HVS_SENSOR_SIZE_Y"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_SIZE_Y);
        }
        else if (strKey == _T("HVS_SENSOR_DATA_FORMAT"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_DATA_FORMAT);
        }
        else if (strKey == _T("HVS_SENSOR_OUT_MODE"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_OUT_MODE);
        }
        else if (strKey == _T("HVS_SENSOR_MIPIMODE"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_MIPIMODE);
        }
        else if (strKey == _T("HVS_SENSOR_I2CMODE"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_I2CMODE);
        }
        else if (strKey == _T("HVS_SENSOR_SleepTime"))
        {
            strTmp.Format(_T("%d"), v5uRef.HVS_SENSOR_SleepTime);
        }
        pProp->SetValue(strTmp);
    }

    //------------------------------------------------------------------------------
    // 读取序列
    m_registerDlg.m_vecNewRegisterList.clear();
    vector<CString> vecMark;
    v5uRefFile.GetAllBlockStartMark(vecMark);
    for (vector<CString>::iterator itor = vecMark.begin(); itor != vecMark.end(); itor++)
    {
        REGISTER_ITEM item;
        
		if (*itor != _T("[REGISTER]"))
		{
			item.strName = *itor;
		}
		else
		{
			item.strName = _T("[LightOn]");
		}
        item.strData = v5uRefFile.ReadBlock(*itor, _T("[END]"));
        m_registerDlg.m_vecNewRegisterList.push_back(item);
    }
    m_registerDlg.RefreshRegisterList(m_registerDlg.m_vecNewRegisterList);
}

void CV5UDeviceSettingDlg::OnBnClickedRadioRegister()
{
    m_registerButton.SetCheck(((m_registerButton.GetCheck() + 1 ) % 2));
    if (m_registerButton.GetCheck() == BST_CHECKED)
    {
        m_registerDlg.ShowWindow(SW_SHOW);
    }
    else
    {
        m_registerDlg.ShowWindow(SW_HIDE);
    }
    AdjustWindow();
}

void CV5UDeviceSettingDlg::OnBnClickedButtonExportV5u()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFileName;
	BOOL bRet = ExportFromVectToFile(m_deviceSetting, strFileName);
	if (bRet)
	{
		AfxMessageBox(_T("Success to export ") + strFileName, MB_ICONINFORMATION | MB_OK);
	}
	else
	{
		AfxMessageBox(_T("Fail to export ") + strFileName, MB_ICONERROR | MB_OK);
	}
}

BOOL CV5UDeviceSettingDlg::ExportFromVectToFile(DEVICE_SETTING &deviceSetting, CString &strFileName)
{
	//暂时将数据全部导出到ini文件中，后期可以改为XML
	//获取文件路径
	TCHAR szFileName[MAX_PATH] = {0};
	OPENFILENAME SFN;
	memset(&SFN, 0, sizeof(OPENFILENAME));
	SFN.lStructSize = sizeof(OPENFILENAME);
	SFN.hwndOwner = GetSafeHwnd();
	SFN.lpstrFilter = _T("Device Setting  File(*.ini)\0*.ini\0\0");
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

	//用来清空文件内容
	FILE *fp = nullptr;
	errno_t nError = _tfopen_s(&fp, strFileName, _T("w"));
	if (nError != 0)
	{
		uts.log.Error(_T("Open file %s error."), strFileName);
		return FALSE;
	}
	fclose(fp);

	

	IniFile m_DevSettingFile(strFileName);

	//写入数据 BOARD_SET,HVS_HW,SENSOR_SET
	CString strBoardSet, strHvsHW,strSensorset;	
	strBoardSet.Format(_T("BOARD_SET"));
	strHvsHW.Format(_T("HVS_HW"));
	strSensorset.Format(_T("SENSOR_SET"));

	for (size_t j = 0; j < deviceSetting.vecSingleSetting.size(); j++)
	{
		CString strKey =deviceSetting.vecSingleSetting[j].strKey;
		CString strValue = deviceSetting.vecSingleSetting[j].strValue;

		if (strKey == _T("DEVICE_COUNT"))
		{
			//m_DevSettingFile.INIWrite_File(strBoardSet,strKey,strValue);
		}
		else if (strKey == _T("BOARD_SET_INDEX_NUM"))
		{
			//m_DevSettingFile.INIWrite_File(strBoardSet,_T("INDEX_NUM"),strValue);
		}
		else if (strKey == _T("HVS_SENSOR_VCLK"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_RESET"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_ENB"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_VSYNC"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_SLAVEADDR"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_SIZE_X"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_SIZE_Y"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_DATA_FORMAT"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_OUT_MODE"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_MIPIMODE"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_I2CMODE"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else if (strKey == _T("HVS_SENSOR_SleepTime"))
		{
			m_DevSettingFile.INIWrite_File(strSensorset,strKey,strValue);
		}
		else
		{
			m_DevSettingFile.INIWrite_File(strHvsHW,strKey,strValue);
		}
	}

	//写入Register的数据
	fp = nullptr;
	nError = _tfopen_s(&fp, strFileName, _T("a"));
	if (nError != 0)
	{
		uts.log.Error(_T("Open file %s error."), strFileName);
		return FALSE;
	}

	for (vector<REGISTER_ITEM>::iterator itor = m_registerDlg.m_vecOldRegisterList.begin(); itor != m_registerDlg.m_vecOldRegisterList.end(); itor++)
	{
		REGISTER_ITEM item;
		CString strName = itor->strName;
		
		CString strData = itor->strData;
		_ftprintf_s(fp, _T("\n\n"));

		if (strName == _T("[LightOn]"))
		{
			_ftprintf_s(fp, _T("[REGISTER]\n"));
		}
		else
		{
			_ftprintf_s(fp, _T("%s\n"), strName);
		}
		
		if (!strData.IsEmpty())
		{
			_ftprintf_s(fp, _T("%s\n"), strData);
		}

		_ftprintf_s(fp, _T("[END]\n"));

	}
	fclose(fp);
	return TRUE;
}

