#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_DumpFW_V5U.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
	ImplOperator::ImplOperator(void)
	{
		OPERATOR_INIT;
	}

	ImplOperator::~ImplOperator(void)
	{
	}

	BOOL ImplOperator::OnReadSpec()
	{
		CString strSection = OSUtil::GetFileName(m_strModuleFile);
		
		
		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
		
		CFile fp;
		unsigned char *lpDumpData = NULL;
		int eepromsize = 8*1024;    //Here 5806 Download EEPROM size is 8*1024;
		lpDumpData = new unsigned char[eepromsize];

		memset(lpDumpData, 0xFF, eepromsize*sizeof(unsigned char));

		// 弹出选择单个文件的对话框
		TCHAR szFileName[MAX_PATH] = {0},strFileName[MAX_PATH] = {0};
		OPENFILENAME SFN;
		memset(&SFN, 0, sizeof(OPENFILENAME));
		SFN.lStructSize = sizeof(OPENFILENAME);
		SFN.hwndOwner = uts.info.hMainWnd;
	//	SFN.lpstrFilter = _T("bmp file(*.bmp)\0*.bmp\0jpg file(*.jpg)\0*.jpg\0raw file(*.raw)\0*.raw\0yuvbmp file(*.yuvbmp)\0*.yuvbmp\0");
		SFN.lpstrFilter = _T("rfw file(*.rfw)\0*.rfw\0");
		SFN.lpstrFile = szFileName;
		SFN.nMaxFile = MAX_PATH;

		if (!GetSaveFileName(&SFN))
		{
			uts.log.Error(_T("Did not select file."));
			goto end;
		}
		if (OSUtil::IsFileExist(szFileName))
		{
			CString strMsg;
			strMsg.Format(_T("%s is already exists. Do you want replace it?"), szFileName);
			int nRet = AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION);
			if (nRet == IDNO)
			{
				goto end;
			}
		}
		unsigned char dataBuf[256] = {0};

		for (int i = 0 ; i< eepromsize ; i = i+ 256)
		{
			I2CRead(2,i,256,dataBuf);
			memcpy(lpDumpData + i,dataBuf,256*sizeof(unsigned char));
		}

		wsprintf(strFileName,L"%s.rfw",szFileName);
		fp.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		fp.Write(lpDumpData, eepromsize);
		fp.Close();

end:
		if(lpDumpData)
		{
			delete lpDumpData;
			lpDumpData = NULL;
		}

		//------------------------------------------------------------------------------

		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		// 保存数据文件
		SaveData();

		return m_bResult;
	}

	 BOOL ImplOperator::I2CRead(DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }

        BYTE slAddr = 0xA0;

        unsigned char writeBuffer[512] = {0};
        int nUseSize = 0;

        /*
        id      type        supportDevice       vallue
        =================================================
        15      (*5)        V5U                 I2C_READ
        15 (*5):
        Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
        Output Format: [DWORD|nCnt][unsigned char[?]|buf]
        */
        memcpy(writeBuffer + nUseSize, &slAddr, sizeof(BYTE));
        nUseSize += sizeof(BYTE);
        memcpy(writeBuffer + nUseSize, &nAddrLen, sizeof(DWORD));
        nUseSize += sizeof(DWORD);
        memcpy(writeBuffer + nUseSize, &nAddr, sizeof(DWORD));
        nUseSize += sizeof(DWORD);
        memcpy(writeBuffer + nUseSize, &nCnt, sizeof(DWORD));
        nUseSize += sizeof(DWORD);
        memcpy(writeBuffer + nUseSize, buf, sizeof(unsigned char) * nCnt);
        nUseSize += sizeof(unsigned char) * nCnt;

        if (!m_pDevice->ReadValue(eDeviceReadValueType::DRVT_I2C_READ, writeBuffer, nUseSize))
        {
            return FALSE;
        }

        DWORD dwReadCnt = 0;
        memcpy(&dwReadCnt, writeBuffer, sizeof(DWORD));
        if (dwReadCnt != nCnt)
        {
            return FALSE;
        }
        memcpy(buf, writeBuffer + sizeof(DWORD), sizeof(unsigned char) * dwReadCnt);

        return TRUE;
    }

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NVMWrite);
	}

	void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		vecRegister.push_back(m_strOperatorName);
	}


	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		strHeader = _T("Time,SN,TestTime(ms),_Result,")
			
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%s,")
	
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
			
			, strVersion, uts.info.strUserId);
	}
	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
