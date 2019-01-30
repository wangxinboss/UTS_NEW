
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "UTSMain.h"
#include "MainFrm.h"
#include "AboutDlg.h"
#include "TestOperatorListDlg.h"
#include "OperatorSpecDlg.h"
#include "ProjectConfig.h"
#include "ErrorcodeDlg.h"
#include "PasswordDlg.h"
//#include "Global_Memory_SFC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
    ON_WM_MOVE()
    ON_WM_SIZING()
    // Config Menu
    //ON_COMMAND(ID_CHANGEPASSWORD_PE, &CMainFrame::OnChangepasswordPe)     // [Config] -> [Change Password] -> [PE]
    ON_COMMAND(ID_CHANGEPASSWORD_SW, &CMainFrame::OnChangepasswordSw)       // [Config] -> [Change Password] -> [SW]
    ON_COMMAND(ID_CONFIG_PROJECT, &CMainFrame::OnConfigProject)             // [Config] -> [Project]
    ON_COMMAND(ID_CONFIG_DEVICESETTING, &CMainFrame::OnConfigDevicesetting) // [Config] -> [Device Setting]
    ON_COMMAND(ID_CONFIG_TEST, &CMainFrame::OnOperatorList)                 // [Config] -> [Operator List]
    ON_COMMAND(ID_CONFIG_SPEC, &CMainFrame::OnConfigSpec)                   // [Config] -> [Test Spec]
    ON_COMMAND(ID_CONFIG_ERRORCODE, &CMainFrame::OnConfigErrorcode)         // [Config] -> [Error Code]
    // Control Menu
    ON_COMMAND(ID_CONTROL_TEST, &CMainFrame::OnControlTest)                 // [Control] -> [Test]
    ON_COMMAND(ID_CONTROL_STOP, &CMainFrame::OnControlStop)                 // [Control] -> [Stop]
    // Show Menu
    ON_COMMAND(ID_DEBUG_SHOWLISTLOG, &CMainFrame::OnDebugShowlistlog)       // [Show] -> [List log]
    ON_COMMAND(ID_SHOW_FPS, &CMainFrame::OnShowFps)                         // [Show] -> [FPS]
    // Tool Menu
    ON_COMMAND(ID_TOOL_SNAP, &CMainFrame::OnToolSnap)                       // [Tool] -> [Snap]
    // Path Menu
    ON_COMMAND(ID_PATH_ROOT, &CMainFrame::OnPathRoot)                       // [Path] -> [Root]
    ON_COMMAND(ID_PATH_BIN, &CMainFrame::OnPathBin)                         // [Path] -> [Bin]
    ON_COMMAND(ID_PATH_CONFIG, &CMainFrame::OnPathConfig)                   // [Path] -> [Config]
    ON_COMMAND(ID_PATH_RESULT, &CMainFrame::OnPathResult)                   // [Path] -> [Result]
    //ON_COMMAND(ID_PATH_DATA, &CMainFrame::OnPathData)                     // [Path] -> [ResultData]
    //ON_COMMAND(ID_PATH_IMAGE, &CMainFrame::OnPathImage)                   // [Path] -> [ResultImage]
    // Help Menu
    ON_COMMAND(ID_HELP_ABOUT, &CMainFrame::OnHelpAbout)
    // Config Menu
    //ON_UPDATE_COMMAND_UI(ID_CHANGEPASSWORD_PE, &CMainFrame::OnUpdateChangepasswordPe)
    ON_UPDATE_COMMAND_UI(ID_CHANGEPASSWORD_SW, &CMainFrame::OnUpdateChangepasswordSw)
    ON_UPDATE_COMMAND_UI(ID_CONFIG_PROJECT, &CMainFrame::OnUpdateConfigProject)
    ON_UPDATE_COMMAND_UI(ID_CONFIG_DEVICESETTING, &CMainFrame::OnUpdateConfigDevicesetting)
    ON_UPDATE_COMMAND_UI(ID_CONFIG_TEST, &CMainFrame::OnUpdateOperatorList)
    ON_UPDATE_COMMAND_UI(ID_CONFIG_SPEC, &CMainFrame::OnUpdateConfigSpec)
    ON_UPDATE_COMMAND_UI(ID_CONFIG_ERRORCODE, &CMainFrame::OnUpdateConfigErrorcode)
    // Control Menu
    ON_UPDATE_COMMAND_UI(ID_CONTROL_TEST, &CMainFrame::OnUpdateControlTest)
    ON_UPDATE_COMMAND_UI(ID_CONTROL_STOP, &CMainFrame::OnUpdateControlStop)
    // Show Menu
    ON_UPDATE_COMMAND_UI(ID_DEBUG_SHOWLISTLOG, &CMainFrame::OnUpdateDebugShowlistlog)
    ON_UPDATE_COMMAND_UI(ID_SHOW_FPS, &CMainFrame::OnUpdateShowFps)
    // Tool Menu
    ON_UPDATE_COMMAND_UI(ID_TOOL_SNAP, &CMainFrame::OnUpdateToolSnap)
    // Path Menu
    ON_UPDATE_COMMAND_UI(ID_PATH_ROOT, &CMainFrame::OnUpdatePathRoot)
    ON_UPDATE_COMMAND_UI(ID_PATH_BIN, &CMainFrame::OnUpdatePathBin)
    ON_UPDATE_COMMAND_UI(ID_PATH_CONFIG, &CMainFrame::OnUpdatePathConfig)
    ON_UPDATE_COMMAND_UI(ID_PATH_RESULT, &CMainFrame::OnUpdatePathResult)
    //ON_UPDATE_COMMAND_UI(ID_PATH_DATA, &CMainFrame::OnUpdatePathData)
    //ON_UPDATE_COMMAND_UI(ID_PATH_IMAGE, &CMainFrame::OnUpdatePathImage)
    // Help Menu
    ON_UPDATE_COMMAND_UI(ID_HELP_ABOUT, &CMainFrame::OnUpdateHelpAbout)
    ON_COMMAND(ID_TOOL_EXIT, &CMainFrame::OnToolExit)
    ON_WM_CLOSE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    //switch (uts.info.nUserType)
    //{
    //case USERTYPE_SOFTWARE:
    //    GetMenu()->DeleteMenu(0, MF_BYPOSITION);
    //    break;
    //default:

    //    break;
    //}

    CString strMsg;
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    {
        strMsg.Format(_T("[%s]Create Main window Fail!"), UTS_FN);
        AfxMessageBox(strMsg);
		return -1;
    }

    //------------------------------------------------------------------------------
    // 创建Display窗口
    m_DisplayWnd.m_pMainFrame = this;
	if (!m_DisplayWnd.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr))
    {
        strMsg.Format(_T("[%s]Create Display window Fail!"), UTS_FN);
        AfxMessageBox(strMsg);
		return -1;
	}

    //------------------------------------------------------------------------------
    // 创建Infoboard窗口
    m_InfoboardWnd.m_pMainFrame = this;
    if (!m_InfoboardWnd.Create(IDD_ControlDlg, this))
    {
        strMsg.Format(_T("[%s]Create Infoboard window Fail!"), UTS_FN);
        AfxMessageBox(strMsg);
        return -1;
    }
    else
    {
        // 设定board的初始宽度
        RECT rcControlDlg = {0};
        rcControlDlg.right = rcControlDlg.left + 280;
        m_InfoboardWnd.MoveWindow(&rcControlDlg);
    }
    
    //------------------------------------------------------------------------------
    // 初始化UTS
    if (!uts.InitializeUTS(
        this->GetSafeHwnd(),
        m_DisplayWnd.GetSafeHwnd(),
        m_InfoboardWnd.GetSafeHwnd()))
    {
        AfxMessageBox(_T("Initialize UTS Fail!"));
    }

    //------------------------------------------------------------------------------
    // Title Bar
    CString strUserType;
    switch (uts.info.nUserType)
    {
    case USERTYPE_OPERATOR:
        strUserType = _T("OP");
        break;
    case USERTYPE_PE:
        strUserType = _T("PE");
        break;
    case USERTYPE_SOFTWARE:
        strUserType = _T("SW");
        break;
    default:
        strUserType = EMPTY_STR;
        break;
    }
    CString strTitle;
    strTitle.Format(_T("%s - %s/%s/%s - UserType: %s - Id: %s - %s"),
#ifdef DEBUG
        _T("UTS(Debug)"),
#else
        _T("UTS"),
#endif
        uts.info.strProjectName,
        uts.info.strLineName,
        uts.info.strStationName,
        strUserType,
        uts.info.strUserId,
        uts.info.strVersion);
    SetWindowText(strTitle);

    //------------------------------------------------------------------------------
    // 创建AI脚本
    m_bAIScriptRunning = TRUE;
    m_pOpAIScriptThread = AfxBeginThread(OpAIScriptThreadProc, this);
    m_pOpAIScriptThread->m_bAutoDelete = FALSE;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 将焦点前移到视图窗口
	m_DisplayWnd.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 让视图第一次尝试该命令
	if (m_DisplayWnd.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 否则，执行默认处理
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnMove(int x, int y)
{
    CFrameWnd::OnMove(x, y);

    // TODO: 在此处添加消息处理程序代码
    AdjustWindow();
}


void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
    CFrameWnd::OnSizing(fwSide, pRect);

    // TODO: 在此处添加消息处理程序代码
    AdjustWindow();
}


void CMainFrame::OnDestroy()
{
    CFrameWnd::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    m_bAIScriptRunning = FALSE;
    uts.DestroyUTS();
    if (nullptr != m_pOpAIScriptThread)
    {
        DWORD dwExitCode = 0;
        do 
        {
            Sleep(50);
            GetExitCodeThread(m_pOpAIScriptThread->m_hThread, &dwExitCode);
        } while (dwExitCode == STILL_ACTIVE);
        RELEASE_POINTER(m_pOpAIScriptThread);
    }
}

void CMainFrame::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    int nRet = AfxMessageBox(
        _T("Exit program?"),
        MB_YESNO | MB_ICONQUESTION);
    if (nRet == IDNO)
    {
        return;
    }
    CFrameWnd::OnClose();
}

void CMainFrame::AdjustWindow(void)
{
    RECT rcMain = {0};
    GetWindowRect(&rcMain);

    RECT rcControlDlg = {0};
    m_InfoboardWnd.GetWindowRect(&rcControlDlg);
    int nBoardCurrentWidth = rcControlDlg.right - rcControlDlg.left;
    rcControlDlg.left = rcMain.right;
    rcControlDlg.top = rcMain.top;
    rcControlDlg.right = rcControlDlg.left + nBoardCurrentWidth;
    rcControlDlg.bottom = rcMain.bottom;
    m_InfoboardWnd.MoveWindow(&rcControlDlg);
    m_InfoboardWnd.AdjustWindow();
    m_InfoboardWnd.ShowWindow(SW_SHOW);
}

BOOL CMainFrame::DestroyWindow()
{

    // TODO: 在此添加专用代码和/或调用基类


    return CFrameWnd::DestroyWindow();
}

UINT CMainFrame::OpAIScriptThreadProc(PVOID param)
{
    CMainFrame *pThis = (CMainFrame *)param;
    while (pThis->m_bAIScriptRunning)
    {
        switch (uts.flow.m_flowStatus)
        {
        case eFlowStatus::Initializing:
            uts.board.Initialize(pThis->m_InfoboardWnd.GetSafeHwnd());
            uts.flow.DoInitialOperatorList();
            break;
        case eFlowStatus::Ready:
            uts.board.Initialize(pThis->m_InfoboardWnd.GetSafeHwnd());
            uts.flow.Test();
            break;
        case eFlowStatus::Testing:
        case eFlowStatus::Stoping:
            uts.flow.WaitTestFinish();
            pThis->m_DisplayWnd.Invalidate(TRUE);
            break;
        default:
            break;
        }
        Sleep(100);
    }
    return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN)
        {
            uts.keyboard.OnKeyReturn();
        }
        else if (pMsg->wParam == VK_SPACE)
        {
            uts.keyboard.OnKeySpace();
        }
        else if (pMsg->wParam >= 0x30 && pMsg->wParam <= 0x39)  // 0-9
        {
            uts.keyboard.OnKeyChar(pMsg->wParam);
        }
        else if (pMsg->wParam >= 0x41 && pMsg->wParam <= 0x5A)  // A-Z
        {
            uts.keyboard.OnKeyChar(pMsg->wParam);
        }
		else if (pMsg->wParam == 0xbd || pMsg->wParam == 0x2d)  // "-"
		{
			uts.keyboard.OnKeyChar(0x2d);
		}
        //-------------------------------------------------------------------------
        // fix 改善 #7: 小键盘不起作用
        else if (pMsg->wParam >= VK_NUMPAD0 && pMsg->wParam <= VK_NUMPAD9)  // 小键盘0-9
        {
            uts.keyboard.OnKeyChar(pMsg->wParam - 0x30);
        }
        //-------------------------------------------------------------------------
        //------------------------------------------------------------------------------
        // fix bug #6: SN输入后不能删除或者更改
        else if (pMsg->wParam == VK_BACK)
        {
            uts.keyboard.OnKeyBackspace();
        }
        //------------------------------------------------------------------------------
    }
    return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OpenFolderWindow(LPCTSTR lpDir)
{
    CString strPath = lpDir;
    if (OSUtil::IsDirExist(strPath))
    {
        OSUtil::OpenFolderWindow(strPath);
    }
    else
    {
        uts.log.Error(_T("Path %s is NOT exist."), strPath);
    }
}

//------------------------------------------------------------------------------
#pragma region ON_COMMAND
// [Config] -> [Change Password] -> [PE]
//void CMainFrame::OnChangepasswordPe()
//{
//    CPasswordDlg dlg;
//    dlg.SetMode(USERTYPE_PE);
//    dlg.DoModal();
//}
// [Config] -> [Change Password] -> [SW]
void CMainFrame::OnChangepasswordSw()
{
    CPasswordDlg dlg;
    dlg.SetMode(USERTYPE_SOFTWARE);
    dlg.DoModal();
}
// [Config] -> [Project]
void CMainFrame::OnConfigProject()
{
    CProjectConfig dlg;
    dlg.DoModal();
}
// [Config] -> [Device Setting]
void CMainFrame::OnConfigDevicesetting()
{
    if (nullptr != uts.flow.m_pDevice)
    {
        uts.flow.m_pDevice->ShowSettingDlg();
    }
    else
    {
        uts.log.Error(_T("nullptr == uts.flow.m_pDevice"));
    }
}
// [Config] -> [Operator List]
void CMainFrame::OnOperatorList()
{
    CTestOperatorListDlg dlg;
    dlg.DoModal();
}
// [Config] -> [Test Spec]
void CMainFrame::OnConfigSpec()
{
    COperatorSpecDlg dlg;
    dlg.DoModal();
}
// [Config] -> [Error Code]
void CMainFrame::OnConfigErrorcode()
{
    CErrorcodeDlg dlg;
    dlg.DoModal();
}
// [Control] -> [Test]
void CMainFrame::OnControlTest()
{
    uts.flow.Test();
}

// [Control] -> [Stop]
void CMainFrame::OnControlStop()
{
    uts.log.Warning(_T("[%s]User Stop."), UTS_FN);

	//!!--Chi-jen.Liao
//     if (uts.info.nShopFlowEn != 0)
//     {
//         GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
//         gmsfc->bOverAllResult = FALSE;
//     }
    uts.flow.Stop();
}
// [Show] -> [List log]
void CMainFrame::OnDebugShowlistlog()
{
    uts.log.ShowWindow();
}
// [Show] -> [FPS]
void CMainFrame::OnShowFps()
{
    uts.info.bShowFPS = !uts.info.bShowFPS;
}
//-------------------------------------------------------------------------
// fix 改善 #2: Snap功能
// [Tool] -> [Snap]
void CMainFrame::OnToolSnap()
{
    // TODO: 在此添加命令处理程序代码
    if (nullptr != uts.flow.m_pDevice)
    {
        uts.flow.m_pDevice->SnapImage();
    }
    else
    {
        uts.log.Error(_T("nullptr == uts.flow.m_pDevice"));
    }
}
//-------------------------------------------------------------------------
// [Path] -> [Root]
void CMainFrame::OnPathRoot()
{
    OpenFolderWindow(uts.info.strRootPath);
}
// [Path] -> [Bin]
void CMainFrame::OnPathBin()
{
    OpenFolderWindow(uts.info.strBinPath);
}
// [Path] -> [Config]
void CMainFrame::OnPathConfig()
{
    OpenFolderWindow(uts.info.strConfigPath);
}
// [Path] -> [Result]
void CMainFrame::OnPathResult()
{
    OpenFolderWindow(uts.info.strResultPath);
}
// [Path] -> [ResultData]
//void CMainFrame::OnPathData()
//{
//    OpenFolderWindow(uts.info.strResultDataPath);
//}
//------------------------------------------------------------------------------
// [Path] -> [ResultImage]
//void CMainFrame::OnPathImage()
//{
//    OpenFolderWindow(uts.info.strResultImagePath);
//}
// [Help] -> [About]
void CMainFrame::OnHelpAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}
// [Tool] -> [Exit]
void CMainFrame::OnToolExit()
{
    OnClose();
}
#pragma endregion
//------------------------------------------------------------------------------

//-------------------------------------------------------------------------
#pragma region ON_UPDATE_COMMAND_UI
// Config Menu
//void CMainFrame::OnUpdateChangepasswordPe(CCmdUI *pCmdUI)
//{
//    // TODO: 在此添加命令更新用户界面处理程序代码
//}
void CMainFrame::OnUpdateChangepasswordSw(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        pCmdUI->Enable(TRUE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
void CMainFrame::OnUpdateConfigProject(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        pCmdUI->Enable(TRUE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
void CMainFrame::OnUpdateConfigDevicesetting(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        pCmdUI->Enable(TRUE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
void CMainFrame::OnUpdateOperatorList(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        pCmdUI->Enable(TRUE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
void CMainFrame::OnUpdateConfigSpec(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        pCmdUI->Enable(TRUE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
void CMainFrame::OnUpdateConfigErrorcode(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        pCmdUI->Enable(TRUE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
// Control Menu
void CMainFrame::OnUpdateControlTest(CCmdUI *pCmdUI)
{
    switch (uts.flow.m_flowStatus)
    {
    case eFlowStatus::Ready:
        pCmdUI->Enable(TRUE);
        break;
    case eFlowStatus::Testing:
        pCmdUI->Enable(FALSE);
        break;
    case eFlowStatus::Stoping:
        pCmdUI->Enable(FALSE);
        break;
    default:
        break;
    }
}
void CMainFrame::OnUpdateControlStop(CCmdUI *pCmdUI)
{
    switch (uts.flow.m_flowStatus)
    {
    case eFlowStatus::Ready:
        pCmdUI->Enable(FALSE);
        break;
    case eFlowStatus::Testing:
        pCmdUI->Enable(TRUE);
        break;
    case eFlowStatus::Stoping:
        pCmdUI->Enable(FALSE);
        break;
    default:
        break;
    }
}
// Show Menu
void CMainFrame::OnUpdateDebugShowlistlog(CCmdUI *pCmdUI)
{
}
void CMainFrame::OnUpdateShowFps(CCmdUI *pCmdUI)
{
    uts.info.bShowFPS ? pCmdUI->SetCheck(1) : pCmdUI->SetCheck(0);
}
//-------------------------------------------------------------------------
// fix 改善 #2: Snap功能
// Tool Menu
void CMainFrame::OnUpdateToolSnap(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        pCmdUI->Enable(TRUE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
//-------------------------------------------------------------------------
// Path Menu
void CMainFrame::OnUpdatePathRoot(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        OSUtil::IsDirExist(uts.info.strRootPath) ? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
void CMainFrame::OnUpdatePathBin(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        OSUtil::IsDirExist(uts.info.strBinPath) ? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
void CMainFrame::OnUpdatePathConfig(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        OSUtil::IsDirExist(uts.info.strConfigPath) ? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
void CMainFrame::OnUpdatePathResult(CCmdUI *pCmdUI)
{
    switch (uts.info.nUserType)
    {
    case USERTYPE_SOFTWARE:
        OSUtil::IsDirExist(uts.info.strResultPath) ? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
        break;
    default:
        pCmdUI->Enable(FALSE);
        break;
    }
}
//void CMainFrame::OnUpdatePathData(CCmdUI *pCmdUI)
//{
//    OSUtil::IsDirExist(uts.info.strResultDataPath) ? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
//}
//void CMainFrame::OnUpdatePathImage(CCmdUI *pCmdUI)
//{
//    OSUtil::IsDirExist(uts.info.strResultImagePath) ? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
//}
// Help Menu
void CMainFrame::OnUpdateHelpAbout(CCmdUI *pCmdUI)
{
}
#pragma endregion
//-------------------------------------------------------------------------



