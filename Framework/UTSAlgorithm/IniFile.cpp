#include "StdAfx.h"
#include "Algorithm.h"


namespace UTS
{
	namespace Algorithm
	{

		IniFile::IniFile(LPCTSTR lpFileName)
		{
			m_strFileName = lpFileName;
		}

		IniFile::~IniFile()
		{

		}

		int IniFile::ReadValue(LPCTSTR lpAppName, LPCTSTR lpKey, int nDefault)
		{
			return GetPrivateProfileInt(lpAppName, lpKey, nDefault, m_strFileName);
		}

		double IniFile::ReadValue(LPCTSTR lpAppName, LPCTSTR lpKey, double dDefault)
		{
			TCHAR szBuffer[1024] = {0};
			GetPrivateProfileString(lpAppName, lpKey, _T("0.0"), szBuffer, ARRAYSIZE(szBuffer), m_strFileName);
			return _ttof(szBuffer);
		}

		char* IniFile::ReadValue(LPCTSTR lpAppName, LPCTSTR lpKey, LPCTSTR lpDefault)
		{
			USES_CONVERSION;
			TCHAR szBuffer[1024] = {0};
			GetPrivateProfileString(lpAppName, lpKey, lpDefault, szBuffer, ARRAYSIZE(szBuffer), m_strFileName);
			//return CString(szBuffer);
			return W2A(szBuffer);
		}

		LPCTSTR IniFile::ReadBlock(LPCTSTR lpStartMark, LPCTSTR lpEndMark)
		{
			CString strBlockData = EMPTY_STR;

			BOOL	bwrite_start = FALSE;
			TCHAR	linebufIn[1024] = {0};
			TCHAR	tmp_cmd[50] = {0};
			FILE	*fp = nullptr;
			errno_t err = _tfopen_s(&fp, m_strFileName, _T("r, ccs=UTF-8"));
			if (0 != err)
			{
				return strBlockData;
			}

			// Read line
			while (_fgetts(linebufIn, 1024, fp))
			{
				// get line first String
				memset(tmp_cmd, 0, sizeof(tmp_cmd));
				_stscanf_s(linebufIn, _T("%s"), tmp_cmd, _countof(tmp_cmd));

				if ((0 == _tcscmp(lpStartMark, tmp_cmd)))
				{
					if (!bwrite_start)
					{
						bwrite_start = TRUE;
						continue;
					}
				}
				else if (0 == _tcscmp(lpEndMark, tmp_cmd) && bwrite_start)
				{
					bwrite_start = FALSE;
					break;
				}
				if (TRUE == bwrite_start)
				{
					strBlockData.Append(linebufIn);
				}
			}
			fclose(fp);
			return strBlockData;
		}

		BOOL IniFile::GetAllBlockStartMark(vector<CString> &vecBlockStartMark)
		{
			vecBlockStartMark.clear();

			TCHAR	linebufIn[1024] = {0};
			TCHAR	tmp_cmd[50] = {0};
			FILE	*fp = nullptr;
			errno_t err = _tfopen_s(&fp, m_strFileName, _T("r, ccs=UTF-8"));
			if (0 != err)
			{
				return FALSE;
			}

			CString strStartMark;
			// Read line
			while (_fgetts(linebufIn, 1024, fp))
			{
				// get line first String
				memset(tmp_cmd, 0, sizeof(tmp_cmd));
				_stscanf_s(linebufIn, _T("%s"), tmp_cmd, _countof(tmp_cmd));

				CString strCommand = tmp_cmd;
				if (strCommand.GetAt(0) == '[' && strCommand != _T("[END]"))
				{
					strStartMark = strCommand;
				}
				else if (strCommand == _T("[END]"))
				{
					vecBlockStartMark.push_back(strStartMark);
				}
			}
			fclose(fp);
			return TRUE;
		}

		//
		BOOL IniFile::INIWrite_File(LPCTSTR lpAppName, LPCTSTR lpKey, int val)
		{
			CString tmp;
			tmp.Format(_T("%d"), val);
			return WritePrivateProfileString(lpAppName, lpKey, tmp, m_strFileName);
		};
		BOOL IniFile::INIWrite_File(LPCTSTR lpAppName, LPCTSTR lpKey, float val)
		{
			CString tmp;
			tmp.Format(_T("%f"), val);
			return WritePrivateProfileString(lpAppName,lpKey, tmp, m_strFileName);
		};
		BOOL IniFile::INIWrite_File(LPCTSTR lpAppName, LPCTSTR lpKey, double val)
		{
			CString tmp;
			tmp.Format(_T("%lf"), val);
			return WritePrivateProfileString(lpAppName,lpKey, tmp, m_strFileName);
		};
		BOOL IniFile::INIWrite_File(LPCTSTR lpAppName, LPCTSTR lpKey, LPCWSTR str)
		{
			return WritePrivateProfileString(lpAppName, lpKey, str, m_strFileName);
		};
	}
}