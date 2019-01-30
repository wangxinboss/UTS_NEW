// stdafx.h : 可在此標頭檔中包含標準的系統 Include 檔，
// 或是經常使用卻很少變更的
// 專案專用 Include 檔案

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 從 Windows 標頭排除不常使用的成員
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 明確定義部分的 CString 建構函式

#include <afxwin.h>         // MFC 核心與標準元件
#include <afxext.h>         // MFC 擴充功能

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 類別
#include <afxodlgs.h>       // MFC OLE 對話方塊類別
#include <afxdisp.h>        // MFC Automation 類別
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 資料庫類別
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 資料庫類別
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 支援的 Internet Explorer 4 通用控制項
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 支援的 Windows 通用控制項
#endif // _AFX_NO_AFXCMN_SUPPORT


