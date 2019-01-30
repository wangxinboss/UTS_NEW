#include "stdafx.h"
#include <vector>
#include "FileSN.h"
#include "UTSDefine.h"
using namespace UTS;


FileSN::FileSN(void)
{

}

bool FileSN::Initialize(void)
{
    m_snFile = OSUtil::GetBSPath(uts.info.strBinPath) + _T("sn.csv");
    if (!OSUtil::IsFileExist(m_snFile))
    {
        uts.log.Error(_T("Can not find SN file.[%s]"), m_snFile);
        return false;
    }

    return true;
}

bool FileSN::GetNextSn(CString &sn)
{
    // read whole file
    TCHAR line[1024] = {0};
    USES_CONVERSION;
    FILE *fp = _tfopen(m_snFile, _T("r+"));
    if (!fp)
    {
        return false;
    }
    CString strFileBuffer;
    while (_fgetts(line, sizeof(line), fp))
    {
        strFileBuffer += line;
    }
    fclose(fp);

    strFileBuffer.Replace('\n', '~');
    vector<CString> vecLines;
    SplitString(strFileBuffer, vecLines, _T("~"), FALSE);

    bool bGetSN = false;
    fp = _tfopen(m_snFile, _T("w+"));
    if (!fp)
    {
        return false;
    }
    for (size_t i = 0; i < vecLines.size(); i++)
    {
        if (vecLines[i].GetAt(0) != '#')
        {
            if (!bGetSN)
            {
                sn = vecLines[i];
                vecLines[i] = _T("#") + vecLines[i];
                bGetSN = true;
            }
        }
        if (i < vecLines.size() - 1)
        {
            vecLines[i] += _T("\n");
        }
        _fputts(vecLines[i], fp);
    }
    fclose(fp);

    return bGetSN;
}
