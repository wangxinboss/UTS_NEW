
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "UTSMain.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOWTEXT), nullptr);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO: 在此处添加消息处理程序代码
    //RECT rcClient;
    //GetClientRect(&rcClient);
    //dc.FillSolidRect(rcClient.left, rcClient.top,
    //    rcClient.right - rcClient.left,
    //    rcClient.bottom - rcClient.top,
    //    COLOR_BLACK);
	// 不要为绘制消息而调用 CWnd::OnPaint()
}



BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    return CWnd::OnEraseBkgnd(pDC);
}
