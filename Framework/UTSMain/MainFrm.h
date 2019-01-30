
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "ChildView.h"
#include "RightDlg.h"


class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 特性
public:
    CChildView  m_DisplayWnd;
    CRightDlg   m_InfoboardWnd;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual BOOL DestroyWindow();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

private:
    void AdjustWindow(void);
    void OpenFolderWindow(LPCTSTR lpDir);

    BOOL m_bAIScriptRunning;
    CWinThread *m_pOpAIScriptThread;
    static UINT OpAIScriptThreadProc(PVOID param);

public:
    afx_msg void OnClose();
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

    //------------------------------------------------------------------------------
    // 菜单
    afx_msg void OnControlTest();
    afx_msg void OnControlStop();
    afx_msg void OnDebugShowlistlog();
    afx_msg void OnHelpAbout();
    afx_msg void OnPathRoot();
    afx_msg void OnPathBin();
    afx_msg void OnPathConfig();
    afx_msg void OnPathResult();
    afx_msg void OnPathData();
    afx_msg void OnPathImage();
    afx_msg void OnShowFps();
    afx_msg void OnOperatorList();
    afx_msg void OnToolSnap();
    afx_msg void OnToolExit();
    afx_msg void OnConfigSpec();
    afx_msg void OnConfigProject();
    afx_msg void OnConfigErrorcode();
    afx_msg void OnConfigDevicesetting();
    afx_msg void OnChangepasswordPe();
    afx_msg void OnChangepasswordSw();
    afx_msg void OnUpdateControlPlay(CCmdUI *pCmdUI);
    afx_msg void OnUpdateControlTest(CCmdUI *pCmdUI);
    afx_msg void OnUpdateControlStop(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePathRoot(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePathBin(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePathConfig(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePathResult(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePathData(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePathImage(CCmdUI *pCmdUI);
    afx_msg void OnUpdateShowFps(CCmdUI *pCmdUI);
    afx_msg void OnUpdateChangepasswordPe(CCmdUI *pCmdUI);
    afx_msg void OnUpdateChangepasswordSw(CCmdUI *pCmdUI);
    afx_msg void OnUpdateConfigProject(CCmdUI *pCmdUI);
    afx_msg void OnUpdateConfigDevicesetting(CCmdUI *pCmdUI);
    afx_msg void OnUpdateOperatorList(CCmdUI *pCmdUI);
    afx_msg void OnUpdateConfigSpec(CCmdUI *pCmdUI);
    afx_msg void OnUpdateConfigErrorcode(CCmdUI *pCmdUI);
    afx_msg void OnUpdateDebugShowlistlog(CCmdUI *pCmdUI);
    afx_msg void OnUpdateHelpAbout(CCmdUI *pCmdUI);
    afx_msg void OnUpdateToolSnap(CCmdUI *pCmdUI);
    afx_msg void OnDestroy();
};


