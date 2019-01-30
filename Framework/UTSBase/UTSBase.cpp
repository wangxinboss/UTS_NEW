#include "stdafx.h"
#include "UTSBase.h"

namespace UTS
{
    // "aaa, bbb, ccc" => Vector("aaa", "bbb", "ccc")
    void SplitString(
        LPCTSTR lpString,
        vector<CString> &vecString,
        LPCTSTR lpSplitMark/* = _T(",")*/,
        BOOL bTrimResult/* = TRUE*/)
    {
        vecString.clear();
        CString strInput = lpString;
        strInput += lpSplitMark;
        do 
        {
            int nPos = strInput.Find(lpSplitMark);
            CString strValue = strInput.Left(nPos);
            if (bTrimResult)
            {
                vecString.push_back(strValue.Trim());
            }
            else
            {
                vecString.push_back(strValue);
            }
            strInput = strInput.Right(strInput.GetLength() - (nPos + 1));
        } while (strInput.GetLength() > 0);
    }

    // "1, 2, 3" => Vector(1, 2, 3)
    void SplitInt(LPCTSTR lpString, vector<int> &vecInt)
    {
        vecInt.clear();
        vector<CString> vecString;
        SplitString(lpString, vecString);

        for (size_t i = 0; i < vecString.size(); i++)
        {
            vecInt.push_back(_ttoi(vecString[i]));
        }
    }

    // "0.1, 0.2, 0.3" => Vector(0.1, 0.2, 0.3)
    void SplitDouble(LPCTSTR lpString, vector<double> &vecDouble)
    {
        vecDouble.clear();
        vector<CString> vecString;
        SplitString(lpString, vecString);

        for (size_t i = 0; i < vecString.size(); i++)
        {
            vecDouble.push_back(_ttof(vecString[i]));
        }
    }

    // "=" * 4 => "===="
    CString GetTimesString(LPCTSTR lpString, unsigned int unTimes/* = 1*/)
    {
        if (unTimes < 1)
        {
            unTimes = 1;
        }
        CString s;
        for (unsigned int i = 0; i < unTimes; ++i)
        {
            s += lpString;
        }
        return s;
    }
}
