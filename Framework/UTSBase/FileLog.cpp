#include "stdafx.h"
#include "UTSBase.h"

namespace UTS
{
    FileLog::FileLog()
    {

    }

    FileLog::~FileLog()
    {
        if (m_file.m_hFile != INVALID_HANDLE_VALUE)
        {
            m_file.Close();
        }
    }

    BOOL FileLog::Init(LPCTSTR lpFileName, int nLevel)
    {
        m_nLevel = nLevel;
        m_strLogFileName = lpFileName;
        if (m_file.m_hFile != INVALID_HANDLE_VALUE)
        {
            m_file.Close();
        }

        OSUtil::CreateMultipleDirectory(OSUtil::GetParentDir(lpFileName));

        BOOL bRet =  m_file.Open(m_strLogFileName,
            CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
        if (bRet)
        {
            m_file.SeekToEnd();
        }
        return bRet;
    }

    void FileLog::DebugLog(LPCTSTR lpFormat, ...)
    {
        if (m_nLevel >= LogLevel_Debug)
        {
            va_list argList;
            va_start(argList, lpFormat);

            CString strMsg;
            strMsg.FormatV(lpFormat, argList);
            OutputLog(LogLevel_Debug, strMsg);

            va_end(argList);
        }
    }

    void FileLog::InfoLog(LPCTSTR lpFormat, ...)
    {
        if (m_nLevel >= LogLevel_Info)
        {
            va_list argList;
            va_start(argList, lpFormat);

            CString strMsg;
            strMsg.FormatV(lpFormat, argList);
            OutputLog(LogLevel_Info, strMsg);

            va_end(argList);
        }
    }

    void FileLog::WarningLog(LPCTSTR lpFormat, ...)
    {
        if (m_nLevel >= LogLevel_Warning)
        {
            va_list argList;
            va_start(argList, lpFormat);

            CString strMsg;
            strMsg.FormatV(lpFormat, argList);
            OutputLog(LogLevel_Warning, strMsg);

            va_end(argList);
        }
    }

    void FileLog::ErrorLog(LPCTSTR lpFormat, ...)
    {
        if (m_nLevel >= LogLevel_Error)
        {
            va_list argList;
            va_start(argList, lpFormat);

            CString strMsg;
            strMsg.FormatV(lpFormat, argList);
            OutputLog(LogLevel_Error, strMsg);

            va_end(argList);
        }
    }

    void FileLog::OutputLog(eLogLevel type, LPCTSTR lpText)
    {
        CString strType;
        switch (type)
        {
        case LogLevel_Debug:
            strType = _T("<DBG>");
            break;
        case LogLevel_Info:
            strType = _T("<INF>");
            break;
        case LogLevel_Warning:
            strType = _T("<WRN>");
            break;
        case LogLevel_Error:
            strType = _T("<ERR>");
            break;
        default:
            strType = _T("<------->");
            break;
        }

        CString strTime;
        SYSTEMTIME systime;
        ::GetLocalTime(&systime);
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d.%03d"),
            systime.wYear,
            systime.wMonth,
            systime.wDay,
            systime.wHour,
            systime.wMinute,
            systime.wSecond,
            systime.wMilliseconds);

        CString strMsg;
        strMsg.Format(_T("%s%s%s\n"), strTime, strType, lpText);
        m_file.Write(strMsg, strMsg.GetLength());
        m_file.Flush();
    }
}
