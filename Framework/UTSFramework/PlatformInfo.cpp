#include "stdafx.h"
#include "PlatformInfo.h"


namespace UTS
{
    #define SECTION_NAME _T("Project")

    PlatformInfo::PlatformInfo()
    {
        TCHAR szPath[MAX_PATH] = {0};
        //GetCurrentDirectory(MAX_PATH, szPath);
        HMODULE hModule = GetModuleHandle(_T("UTSFramework.dll"));
        GetModuleFileName(hModule, szPath, MAX_PATH);

        strBinPath = OSUtil::GetBSPath(OSUtil::GetParentDir(szPath));
        strRootPath = OSUtil::GetBSPath(OSUtil::GetParentDir(strBinPath));
        strConfigPath = strRootPath + _T("Config\\");
        strResultPath = strRootPath + _T("Result\\");


#ifdef CONFIG_FROM_FILE
        IniFile file(strConfigPath + _T("UTS.ini"));
        strProjectName = file.ReadValue(SECTION_NAME, _T("ProjectName"), EMPTY_STR);
        strLineName = file.ReadValue(SECTION_NAME, _T("LineName"), EMPTY_STR);
        strStationName = file.ReadValue(SECTION_NAME, _T("StationName"), EMPTY_STR);
        strDeviceName = file.ReadValue(SECTION_NAME, _T("DeviceName"), EMPTY_STR);

        nLTDD_DummyFrame = file.ReadValue(SECTION_NAME, _T("nLTDD_DummyFrame"), 0);
        nLTDD_AvgFrame = file.ReadValue(SECTION_NAME, _T("nLTDD_AvgFrame"), 1);
        nSaveBmpFile = file.ReadValue(SECTION_NAME, _T("SaveBmpFile"), 0);
        nSaveRawFile = file.ReadValue(SECTION_NAME, _T("SaveRawFile"), 0);
        nSaveYuvBmpFile = file.ReadValue(SECTION_NAME, _T("nSaveYuvBmpFile"), 0);
        nZipFile = file.ReadValue(SECTION_NAME, _T("ZipFile"), 0);
        nStopOnFail = file.ReadValue(SECTION_NAME, _T("StopOnFail"), 1);

        CString strOp;
        CString strKey;
        // Operator List
        int index = 0;
        do 
        {
            strKey.Format(_T("Operator_%d"), index++);
            strOp = file.ReadValue(SECTION_NAME, strKey, EMPTY_STR);
            if (!strOp.IsEmpty())
            {
                lstOperator.AddTail(strOp);
            }
        } while (!strOp.IsEmpty());
        nOperatorCount = lstOperator.GetCount();

        // Initial start Operator List
        index = 0;
        do
        {
            strKey.Format(_T("InitOperator_%d"), index++);
            strOp = file.ReadValue(SECTION_NAME, strKey, EMPTY_STR);
            if (!strOp.IsEmpty())
            {
                lstInitOperator.AddTail(strOp);
            }
        } while (!strOp.IsEmpty());
        nInitOperatorCount = lstInitOperator.GetCount();
#endif

        //memset(nArrDynamicCurrentOffset, 0, sizeof(int) * 6);
        //memset(nArrStandbyCurrentOffset, 0, sizeof(int) * 6);
    }

    PlatformInfo::~PlatformInfo()
    {

    }
}
