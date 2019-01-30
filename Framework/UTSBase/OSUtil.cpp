#include "StdAfx.h"
#include "UTSBase.h"
#include <strsafe.h>
#include <tlhelp32.h>

namespace UTS
{
    OSUtil::OSUtil(void)
    {
    }

    OSUtil::~OSUtil(void)
    {
    }

    CString OSUtil::GetParentDir(LPCTSTR lpCurrentDir)
    {
        if (nullptr == lpCurrentDir)
        {
            return _T("");
        }
        CString strCurrentDir = GetNoBSPath(lpCurrentDir);
        CString strParentDir = strCurrentDir.Left(strCurrentDir.ReverseFind('\\'));
        if (strParentDir.Right(1) == ':')
        {
            strParentDir.Append(_T("\\"));
        }
        return strParentDir;
    }

    CString OSUtil::GetNoBSPath(LPCTSTR lpInputDir)
    {
        if (nullptr == lpInputDir)
        {
            return _T("");
        }
        CString strInput = lpInputDir;
        if (strInput.Right(1) == '\\')
        {
            return strInput.Left(strInput.GetLength() - 1);
        }
        else
        {
            return strInput;
        }
    }

    CString OSUtil::GetBSPath(LPCTSTR lpInputDir)
    {
        if (nullptr == lpInputDir)
        {
            return _T("");
        }
        CString strInput = lpInputDir;
        if (strInput.Right(1) != '\\')
        {
            strInput.Append(_T("\\"));
        }
        return strInput;
    }

    CString OSUtil::GetFileName(LPCTSTR lpPathFileName)
    {
        if (nullptr == lpPathFileName)
        {
            return _T("");
        }
        CString strInput = lpPathFileName;
        strInput = strInput.Right(strInput.GetLength() - strInput.ReverseFind('\\') - 1);
        return strInput;
    }

    CString OSUtil::GetMainFileName(LPCTSTR lpFileName)
    {
        if (nullptr == lpFileName)
        {
            return _T("");
        }
        CString strInput = lpFileName;
        strInput = strInput.Left(strInput.ReverseFind('.'));
        return strInput;
    }

    CString OSUtil::GetExtFileName(LPCTSTR lpFileName)
    {
        if (nullptr == lpFileName)
        {
            return _T("");
        }
        CString strInput = lpFileName;
        strInput = strInput.Right(strInput.GetLength() - strInput.ReverseFind('.') - 1);
        return strInput;
    }

    BOOL OSUtil::IsFileExist(LPCTSTR lpFileName)
    {
        if (nullptr == lpFileName)
        {
            return FALSE;
        }
        BOOL bRet = TRUE;
        int iLen = 0;
        WIN32_FIND_DATA finddata;
        memset(&finddata, 0, sizeof(WIN32_FIND_DATA));
        HANDLE hd = FindFirstFile(lpFileName, &finddata);
        if (INVALID_HANDLE_VALUE == hd)
        {
            DWORD dwRet = GetLastError();
            if (ERROR_FILE_NOT_FOUND == dwRet || ERROR_PATH_NOT_FOUND == dwRet)
            {
                bRet = FALSE;
            }
        }
        else
        {
            if ((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            {
                bRet = FALSE;
            }
        }
        FindClose(hd);
        return bRet;
    }

    BOOL OSUtil::IsDirExist(LPCTSTR lpDirName)
    {
        if (nullptr == lpDirName)
        {
            return FALSE;
        }
        CString strDirName = OSUtil::GetNoBSPath(lpDirName);
        BOOL bRet = TRUE;
        int iLen = 0;
        WIN32_FIND_DATA finddata;
        memset(&finddata, 0, sizeof(WIN32_FIND_DATA));
        HANDLE hd = FindFirstFile(strDirName, &finddata);
        if (INVALID_HANDLE_VALUE == hd)
        {
            DWORD dwRet = GetLastError();
            if (ERROR_FILE_NOT_FOUND == dwRet || ERROR_PATH_NOT_FOUND == dwRet)
            {
                bRet = FALSE;
            }
        }
        else
        {
            if ((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                bRet = FALSE;
            }
        }
        FindClose(hd);
        return bRet;
    }

    void OSUtil::CreateMultipleDirectory(LPCTSTR lpPath)
    {
        if (nullptr == lpPath)
        {
            return;
        }
        CString strDir(lpPath);// 存放要创建的目录字符串
        // 确保以'\'结尾以创建最后一个目录
        if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\'))
        {
            strDir.AppendChar(_T('\\'));
        }
        std::vector<CString> vPath;//存放每一层目录字符串
        CString strTemp;//一个临时变量,存放目录字符串
        BOOL bSuccess = FALSE;//成功标志
        //遍历要创建的字符串
        for (int i=0;i<strDir.GetLength();++i)
        {
            if (strDir.GetAt(i) != _T('\\'))
            {//如果当前字符不是'\\'
                strTemp.AppendChar(strDir.GetAt(i));
            }
            else
            {//如果当前字符是'\\'
                vPath.push_back(strTemp);//将当前层的字符串添加到数组中
                strTemp.AppendChar(_T('\\'));
            }
        }

        //遍历存放目录的数组,创建每层目录
        std::vector<CString>::const_iterator vIter;
        for (vIter = vPath.begin(); vIter != vPath.end(); vIter++)
        {
            //如果CreateDirectory执行成功,返回true,否则返回false
            bSuccess = CreateDirectory(*vIter, nullptr) ? TRUE : FALSE;
        }
    }

    BOOL OSUtil::DirReplace(LPCTSTR lpInputDir, LPCTSTR lpSrcStr, LPCTSTR lpDestStr)
    {
        if (nullptr == lpInputDir || nullptr == lpSrcStr || nullptr == lpDestStr)
        {
            return FALSE;
        }
        if (!IsDirExist(lpInputDir))
        {
            return FALSE;
        }
        // 无反斜杠结尾的输入Dir
        CString strInputDir = GetNoBSPath(lpInputDir);
        // 有反斜杠结尾的父目录
        CString strParentDir = GetBSPath(GetParentDir(strInputDir));
        // 当前文件夹的名称
        CString strDirName = GetFileName(strInputDir);
        strDirName.Replace(lpSrcStr, lpDestStr);
        // 目标文件夹名称
        CString strCommand;
        strCommand.Format(_T("move %s %s"), strInputDir, strDirName);
        ExcuteCommand(strCommand);

        CString strDestDir = strParentDir + strDirName;
        CStringList list;
        GetFiles(strDestDir, list);

        POSITION rPos = list.GetHeadPosition();
        while (rPos != nullptr)
        {
            // 当前文件名称
            CString strFilePath = list.GetNext(rPos);
            CString strFileName = GetFileName(strFilePath);
            strFileName.Replace(lpSrcStr, lpDestStr);
            // 目标文件名称
            CString strDestFilePath = GetBSPath(GetParentDir(strFilePath)) + strFileName;
            MoveFileEx(strFilePath, strDestFilePath, MOVEFILE_COPY_ALLOWED);
        }
        return TRUE;
    }

    /*
    *    获取指定文件夹下面的所有文件(不包含子文件夹)
    */
    void OSUtil::GetFiles(LPCTSTR lpDir, CStringList &list)
    {
        list.RemoveAll();
        if (nullptr == lpDir)
        {
            return;
        }
        CString strInput = GetBSPath(lpDir);
        CFileFind fileFind;
        BOOL re = fileFind.FindFile(strInput + _T("*.*"));

        if (re)
        {
            while (re)
            {
                re = fileFind.FindNextFile();
                if(fileFind.IsDots()) continue;
                if (fileFind.IsDirectory()) continue;
                list.AddTail(fileFind.GetFilePath());
            }
        }
        fileFind.Close();
    }

    /*
    *    获取指定目录的子目录列表
    */
    void OSUtil::GetDirectorys(LPCTSTR lpDir, CStringList &list)
    {
        list.RemoveAll();
        if (nullptr == lpDir)
        {
            return;
        }
        CString strInput = GetBSPath(lpDir);
        CFileFind fileFind;
        BOOL re = fileFind.FindFile(strInput + _T("*.*"));

        if (re)
        {
            while (re)
            {
                re = fileFind.FindNextFile();
                if (fileFind.IsDots()) continue;
                if (fileFind.IsDirectory())
                {
                    list.AddTail(fileFind.GetFilePath());
                }
            }
        }
        fileFind.Close();
    }

    void OSUtil::MessageBoxLastError()
    {
        DWORD dwMessageID = ::GetLastError();
        TCHAR *szMessage = nullptr;
        TCHAR szTitle[32] = {0};
        ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            nullptr, dwMessageID, 0, (LPTSTR)&szMessage, 0, nullptr);
        ::MessageBox(nullptr, szMessage, szTitle, MB_OK);
    }

    void OSUtil::ExcuteCommand(LPCTSTR lpCommand, BOOL bWaitSyncEnd)
    {
        PROCESS_INFORMATION pidInfo;
        STARTUPINFO startInfo;
        startInfo.cb            = sizeof(STARTUPINFO);
        startInfo.lpReserved    = nullptr;
        startInfo.lpTitle       = nullptr;
        startInfo.lpDesktop     = nullptr;
        startInfo.dwX           = 0;
        startInfo.dwY           = 0;
        startInfo.dwXSize       = 0;
        startInfo.dwYSize       = 0;
        startInfo.dwXCountChars = 0;
        startInfo.dwYCountChars = 0;
        startInfo.dwFlags       = STARTF_USESTDHANDLES;
        startInfo.wShowWindow   = 0;   //SW_SHOWDEFAULT;
        startInfo.lpReserved2   = nullptr;
        startInfo.cbReserved2   = 0;
        startInfo.hStdInput     = GetStdHandle(STD_INPUT_HANDLE);
        startInfo.hStdOutput    = GetStdHandle(STD_OUTPUT_HANDLE);
        startInfo.hStdError     = GetStdHandle(STD_ERROR_HANDLE);
        BOOL bRet = CreateProcess(nullptr, (LPTSTR)lpCommand, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &startInfo, &pidInfo);
        if (!bRet)
        {
            return;
        }
        if (bWaitSyncEnd)
        {
            WaitForSingleObject(pidInfo.hProcess,INFINITE);
        }
        CloseHandle(pidInfo.hProcess);
        CloseHandle(pidInfo.hThread);
    }

    // 压缩文件
    void OSUtil::ZipFile(LPCTSTR lpDestFileName, LPCTSTR lpSourceFileName)
    {
        CString strCommand;
        strCommand.Append(_T("rar.exe a -df -ep "));
        strCommand.Append(lpDestFileName);
        strCommand.Append(_T(" "));
        strCommand.Append(lpSourceFileName);
        ExcuteCommand(strCommand);
    }

    // 获取文件版本信息
    BOOL OSUtil::GetFileVersion(LPCTSTR strFilePath, CString &strVersion)
    {
        BOOL bRet = FALSE;
        DWORD dwLen = 0;
        TCHAR *pData = NULL;

        do
        {
            strVersion = "";
            dwLen = GetFileVersionInfoSize(strFilePath, NULL);
            if (0 == dwLen)
            {
                bRet = FALSE;
                break;
            }

            pData = new TCHAR[dwLen + 1];

            bRet = GetFileVersionInfo(strFilePath, NULL, dwLen, pData);
            if (!bRet)
            {
                break;
            }

            LPVOID lpBuffer = NULL;
            UINT uLen = 0;
            bRet = VerQueryValue(pData,
                _T("\\StringFileInfo\\080404b0\\FileVersion"),
                &lpBuffer,
                &uLen);
            if (!bRet)
            {
                break;
            }
            strVersion = CString((TCHAR*)lpBuffer);
        } while (FALSE);

        if (pData != NULL)
        {
            delete [] pData;
        }
        return bRet;
    }

    // 获取文件时间(字符串)
    BOOL OSUtil::GetFileTimeStr(
        LPCTSTR lpFilePathName,
        CString &strCreateTime,
        CString &strAccessTime,
        CString &strWriteTime)
    {
        HANDLE hFile = CreateFile(
            lpFilePathName,
            GENERIC_READ,  //必须有GENERIC_READ属性才能得到时间
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            SYSTEMTIME sysTime;
            FILETIME fCreateTime, fAccessTime, fWriteTime, localTime;
            GetFileTime(hFile, &fCreateTime, &fAccessTime, &fWriteTime);// 获取文件时间

            //------------------------------------------------------------------------------
            // CreateTime
            // 将文件时间转换为本地系统时间
            FileTimeToLocalFileTime(&fCreateTime, &localTime); // 将文件时间转换为本地文件时间
            FileTimeToSystemTime(&localTime, &sysTime);        // 将文件时间转换为本地系统时间
            strCreateTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
                sysTime.wYear,
                sysTime.wMonth,
                sysTime.wDay,
                sysTime.wHour,
                sysTime.wMinute,
                sysTime.wSecond);

            //------------------------------------------------------------------------------
            // AccessTime
            // 将文件时间转换为本地系统时间
            FileTimeToLocalFileTime(&fCreateTime, &localTime); // 将文件时间转换为本地文件时间
            FileTimeToSystemTime(&localTime, &sysTime);        // 将文件时间转换为本地系统时间
            strAccessTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
                sysTime.wYear,
                sysTime.wMonth,
                sysTime.wDay,
                sysTime.wHour,
                sysTime.wMinute,
                sysTime.wSecond);

            //------------------------------------------------------------------------------
            // WriteTime
            // 将文件时间转换为本地系统时间
            FileTimeToLocalFileTime(&fCreateTime, &localTime); // 将文件时间转换为本地文件时间
            FileTimeToSystemTime(&localTime, &sysTime);        // 将文件时间转换为本地系统时间
            strWriteTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
                sysTime.wYear,
                sysTime.wMonth,
                sysTime.wDay,
                sysTime.wHour,
                sysTime.wMinute,
                sysTime.wSecond);
            CloseHandle(hFile);
        }
        else
        {
            return FALSE;
        }
        return TRUE;
    }

    void OSUtil::OpenFolderWindow(LPCTSTR lpDir)
    {
        ShellExecute(NULL, _T("Open"), lpDir, NULL, NULL, SW_SHOWNORMAL);
    }

    BOOL OSUtil::IsDllExportFunction(LPCTSTR lpDllFile, LPCSTR lpFunctionName)
    {
        BOOL bRet = TRUE;
        HMODULE hModule = ::LoadLibrary(lpDllFile);
        if (nullptr != hModule)
        {
            FARPROC lpfn = (FARPROC)GetProcAddress(hModule, lpFunctionName);
            if (nullptr == lpfn)
            {
                FreeLibrary(hModule);
                hModule = nullptr;
                bRet = FALSE;
            }
            else
            {
                FreeLibrary(hModule);
                hModule = nullptr;
            }
        }
        else
        {
            bRet = FALSE;
        }
        return bRet;
    }

    void OSUtil::ErrorExit(LPTSTR lpszFunction) 
    { 
        // Retrieve the system error message for the last-error code

        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError(); 

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

        // Display the error message and exit the process

        lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
            (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
        StringCchPrintf((LPTSTR)lpDisplayBuf, 
            LocalSize(lpDisplayBuf) / sizeof(TCHAR),
            TEXT("%s failed with error %d: %s"), 
            lpszFunction, dw, lpMsgBuf); 
        MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
        ExitProcess(dw); 
    }

    BOOL OSUtil::FindProcess(LPCTSTR lpProcessName)
    {
        BOOL bRet = FALSE;
        HANDLE hProcessSnap;
        PROCESSENTRY32 pe32;

        // Take a snapshot of all processes in the system.
        hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
        if( hProcessSnap == INVALID_HANDLE_VALUE )
        {
            return( FALSE );
        }

        // Set the size of the structure before using it.
        pe32.dwSize = sizeof( PROCESSENTRY32 );

        // Retrieve information about the first process,
        // and exit if unsuccessful
        if( !Process32First( hProcessSnap, &pe32 ) )
        {
            CloseHandle( hProcessSnap );          // clean the snapshot object
            return( FALSE );
        }

        // Now walk the snapshot of processes, and
        // display information about each process in turn
        do
        {
            if (0 == _tcscmp(pe32.szExeFile, lpProcessName))
            {
                bRet = TRUE;
                break;
            }
        } while( Process32Next( hProcessSnap, &pe32 ) );

        CloseHandle( hProcessSnap );
        return( bRet );
    }

    long OSUtil::GetFileSize(LPCTSTR lpFileName)
    {
        errno_t err;
        FILE* fp = nullptr;
        long size = -1;

        err = _tfopen_s(&fp, lpFileName, _T("rb"));
        if (err == 0)
        {
            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            fclose(fp);
        }
        return size;
    }
}
