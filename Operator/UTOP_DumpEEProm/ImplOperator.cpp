#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_DumpEEProm.h"
#include "EEProm.h"
#include "CommonFunc.h"

#pragma comment(lib, "UTS_EEProm.lib")
#pragma comment(lib, "CommonFunc.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
	void PrintDebugArray(const void *array, int len)
	{
		char buf[0x1000 - 27];

		int idx = 0;
		USES_CONVERSION;
		while (idx < len)
		{
			idx += Array2String((char *)array + idx, len - idx, buf, sizeof(buf), 16, " ", "\n");
			uts.log.Debug(A2T(buf));
		}
	}


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
		
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nEEPRomType"), m_param.nEEPRomType, 5, _T("nEEPRomType"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nEEPRomDumpSize"), m_param.nEEPRomDumpSize, 64000, _T("nEEPRomDumpSize"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDumpType"), m_param.nDumpType, 0, _T("nDumpType,0:Bin file 1:CSV File 2:TXT file 3:Bin+CSV File"));
		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
	
		*pnErrorCode = uts.errorcode.E_Pass;
		UTS::EEPROMDriver *eeprom;
		char *data = new char[m_param.nEEPRomDumpSize];
		memset(data,0,sizeof(char)*m_param.nEEPRomDumpSize);
		eeprom = GetEEPromDriverInstance(m_pDevice, m_param.nEEPRomType);

		eeprom->Read(0, data, m_param.nEEPRomDumpSize);
	
		uts.log.Debug(_T("Otp data:"));

		PrintDebugArray(data, m_param.nEEPRomDumpSize);

		//Save Dump FW 
		CFile fp2;
		CString strDirPath;
		CString strFilePath;
		CString strData;

		SYSTEMTIME st;
		GetLocalTime(&st);

		strDirPath.Format(_T("%s\\Data-%s-%s-%s-%04d%02d%02d_DumpBin\\"),
			uts.info.strResultPath,
			uts.info.strProjectName,
			uts.info.strLineName,
			uts.info.strStationName,
			st.wYear,
			st.wMonth, 
			st.wDay);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		if (m_param.nDumpType==0)
		{
			strFilePath.Format(_T("%s\\%s.bin"),
				strDirPath,
				uts.info.strSN);
			fp2.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
			fp2.Write(data, m_param.nEEPRomDumpSize);
			fp2.Close();
		}
		else if (m_param.nDumpType==1)
		{
			
			strFilePath.Format(_T("%s\\%s.csv"),
 				strDirPath,
 				uts.info.strSN);
			FILE *fp =NULL;
			long fileLength ;
			errno_t etRet = _tfopen_s(&fp, strFilePath, _T("w"));
			
			if (0 != etRet)
			{
				uts.log.Error(_T("Open file Fail. path = %s"), strFilePath);
				return FALSE;
			}
			fseek(fp, 0, SEEK_END);
			fileLength = ftell(fp);
			
			for(int i=0;i<m_param.nEEPRomDumpSize;i++)
			{
				_ftprintf_s(fp, _T("0x%02x,\n"), data[i]);
			}
			fclose(fp);
		}
		else if (m_param.nDumpType==2)
		{
			strFilePath.Format(_T("%s\\%s.txt"),
 				strDirPath,
 				uts.info.strSN);
			FILE *fp =NULL;
			long fileLength ;
			errno_t etRet = _tfopen_s(&fp, strFilePath, _T("w"));
			
			if (0 != etRet)
			{
				uts.log.Error(_T("Open file Fail. path = %s"), strFilePath);
				return FALSE;
			}
			fseek(fp, 0, SEEK_END);
			fileLength = ftell(fp);
			
			for(int i=0;i<m_param.nEEPRomDumpSize;i++)
			{
				_ftprintf_s(fp, _T("0x%02x\n"), data[i]);
			}
			fclose(fp);			
		}
		else if (m_param.nDumpType==3) //Bin/csv
		{
			strFilePath.Format(_T("%s\\%s.bin"),
				strDirPath,
				uts.info.strSN);
			fp2.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
			fp2.Write(data, m_param.nEEPRomDumpSize);
			fp2.Close();
			//-----------------------------------------
			CString strcsvPath;
			strcsvPath.Format(_T("%s\\%s.csv"),
				strDirPath,
				uts.info.strSN);

			FILE *fp =NULL;
			long fileLength ;
			errno_t etRet = _tfopen_s(&fp, strcsvPath, _T("w"));
			
			if (0 != etRet)
			{
				uts.log.Error(_T("Open file Fail. path = %s"), strcsvPath);
				return FALSE;
			}
			fseek(fp, 0, SEEK_END);
			fileLength = ftell(fp);
			
			for(int i=0;i<m_param.nEEPRomDumpSize;i++)
			{
				_ftprintf_s(fp, _T("0x%02x,\n"), data[i]);
			}
			fclose(fp);	
			//-----------------------------------------
		}
		

		delete(data);

		//------------------------------------------------------------------------------

		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		// 保存数据文件
		SaveData();

		return m_bResult;
	}

	BOOL ImplOperator::I2CWrite(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf)
    {
        unsigned char writeBuffer[256] = {0};
        int nUseSize = 0;

        /*
        id      type        supportDevice       vallue
        =================================================
        1       (*1)        V5U                 I2C_WRITE
        (*1):
            Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
            Output Format: [Not Use]
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

        return m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_I2C_WRITE, writeBuffer, nUseSize);
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
