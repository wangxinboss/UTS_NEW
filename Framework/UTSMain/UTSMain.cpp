
// UTSMain.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "UTSMain.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUTSMainApp

BEGIN_MESSAGE_MAP(CUTSMainApp, CWinApp)
END_MESSAGE_MAP()


// CUTSMainApp 构造

CUTSMainApp::CUTSMainApp()
{
	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Goertek.CCM.UTS"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CUTSMainApp 对象

CUTSMainApp theApp;


// CUTSMainApp 初始化

BOOL CUTSMainApp::InitInstance()
{
	CWinApp::InitInstance();
    
	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("UTS"));

    //------------------------------------------------------------------------------
    // 用此函数跟踪内存泄露
    //_CrtSetBreakAlloc(705);
    //------------------------------------------------------------------------------

	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 创建并加载框架及其资源
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr,
		nullptr);

    //pFrame->MoveWindow(10, 10, 960, 720);

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	//pFrame->ShowWindow(SW_SHOW);
	//pFrame->UpdateWindow();
    
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	return TRUE;
}

int CUTSMainApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
    uts.log.Destroy();
	return CWinApp::ExitInstance();
}

// CUTSMainApp 消息处理程序




