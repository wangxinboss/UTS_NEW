// UTS_EEProm.cpp : 定義 DLL 的初始化常式。
//

#include "stdafx.h"

#include "UTS_EEProm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果這? DLL 是動態地對 MFC DLL 連結，
//		那麼從這? DLL 匯出的任何會呼叫
//		MFC 內部的函式，都必?在函式一開頭加上 AFX_MANAGE_STATE
//		巨集。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此?為正常函式主體
//		}
//
//		這?巨集一定要出現在每一?
//		函式中，才能夠呼叫 MFC 的內部。這意味著
//		它必?是函式內的第一??述式
//		，甚至必?在任何物件?數宣告前面
//		，因為它?的建?函式可能會產生對 MFC
//		DLL 內部的呼叫。
//
//		?參? MFC 技術提示 33 和 58 中的
//		??資料。
//

// CUTS_EEPromApp

BEGIN_MESSAGE_MAP(CUTS_EEPromApp, CWinApp)
END_MESSAGE_MAP()


// CUTS_EEPromApp 建?

CUTS_EEPromApp::CUTS_EEPromApp()
{
	// TODO: 在此加入建?程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}


// 僅有的一? CUTS_EEPromApp 物件

CUTS_EEPromApp theApp;


// CUTS_EEPromApp 初始設定

BOOL CUTS_EEPromApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
