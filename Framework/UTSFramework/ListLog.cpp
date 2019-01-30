#include "StdAfx.h"
#include "ListLog.h"
#include "ListLogDlg.h"
#include "CommonFunc.h"

namespace UTS
{
    const int MAX_LOG_COUNT = 1000;
    static CListLogDlg g_dlgListLog;
    static CCriticalSection g_csLogList;

    BOOL ListLog::Initialize(void)
    {
        AFX_MANAGE_STATE(AfxGetStaticModuleState());
        // 创建ListLog窗口
        if (!g_dlgListLog.Create(IDD_LISTLOG, NULL))
        {
            AfxMessageBox(_T("Create Listlog Dialog Error."));
            return FALSE;
        }
        g_dlgListLog.ShowWindow(SW_HIDE);
        g_dlgListLog.SetMaxLogCount(MAX_LOG_COUNT);
        m_LogList = new CString[MAX_LOG_COUNT];
        m_LogIndex = 0;

        return TRUE;
    }

    void ListLog::Debug(LPCTSTR lpFormat, ...)
    {
        va_list argList;
        va_start(argList, lpFormat);

        CString strMsg;
        strMsg.FormatV(lpFormat, argList);
        Output(COLOR_WHITE, strMsg);
        printk(_T("[Debug]") + strMsg + _T("\n"));

        va_end(argList);
    }

    void ListLog::Info(LPCTSTR lpFormat, ...)
    {
        va_list argList;
        va_start(argList, lpFormat);

        CString strMsg;
        strMsg.FormatV(lpFormat, argList);
        Output(COLOR_GREEN, strMsg);
        printk(_T("[Info ]") + strMsg + _T("\n"));

        va_end(argList);
    }

    void ListLog::Warning(LPCTSTR lpFormat, ...)
    {
        va_list argList;
        va_start(argList, lpFormat);

        CString strMsg;
        strMsg.FormatV(lpFormat, argList);
        Output(COLOR_YELLOW, strMsg);
        printk(_T("[Warn ]") + strMsg + _T("\n"));

        va_end(argList);
    }

    void ListLog::Error(LPCTSTR lpFormat, ...)
    {
        va_list argList;
        va_start(argList, lpFormat);

        CString strMsg;
        strMsg.FormatV(lpFormat, argList);
        Output(COLOR_RED, strMsg);
        printk(_T("[Error]") + strMsg + _T("\n"));

        va_end(argList);

        // 发生Error时，弹出ListLog
        //ShowWindow();
    }

    void ListLog::Output(COLORREF color, LPCTSTR lpMsg)
    {
        CSingleLock sl(&g_csLogList, TRUE);

        CString strTime;
        SYSTEMTIME systime;
        ::GetLocalTime(&systime);
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d.%03d > "),
            systime.wYear,
            systime.wMonth,
            systime.wDay,
            systime.wHour,
            systime.wMinute,
            systime.wSecond,
            systime.wMilliseconds);

        CString strMsg = lpMsg;
        strMsg.Replace(_T("\r"), EMPTY_STR);
        strMsg.Replace(_T("\n"), EMPTY_STR);
        strMsg = strTime + strMsg;
        m_LogList[m_LogIndex] = strMsg;

        ::SendMessage(
            g_dlgListLog.GetSafeHwnd(),
            WM_LISTLOG,
            (WPARAM)m_LogIndex,
            (LPARAM)color);
        m_LogIndex++;
        if (m_LogIndex >= MAX_LOG_COUNT)
        {
            m_LogIndex = 0;
        }
    }


    void ListLog::ShowWindow(void)
    {
        g_dlgListLog.ShowWindow(SW_SHOW);
    }

    void ListLog::Destroy(void)
    {
        AFX_MANAGE_STATE(AfxGetStaticModuleState());
        RELEASE_ARRAY(m_LogList);
        g_dlgListLog.DestroyWindow();
    }
}
