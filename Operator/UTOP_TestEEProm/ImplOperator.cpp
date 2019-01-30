#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_TestEEProm.h"
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

		int ret = eeprom->Read(0, data, m_param.nEEPRomDumpSize);
		if(ret < 0) 
		{
			*pnErrorCode = uts.errorcode.E_NVMWrite;
			uts.log.Error(_T("EEProm I2C fail !"));
			goto out;
		}
	
		uts.log.Debug(_T("Original Otp data:"));
		PrintDebugArray(data, m_param.nEEPRomDumpSize);

		//Write 0x00
		char *writedata = new char[m_param.nEEPRomDumpSize];
		memset(writedata,0,sizeof(char)*m_param.nEEPRomDumpSize);

		eeprom->Write(0, writedata, m_param.nEEPRomDumpSize);
		if(ret < 0) 
		{
			*pnErrorCode = uts.errorcode.E_NVMWrite;
			uts.log.Error(_T("EEProm I2C fail !"));
			goto out;
		}
		char *readdata = new char[m_param.nEEPRomDumpSize];
		memset(readdata,0,sizeof(char)*m_param.nEEPRomDumpSize);

		eeprom->Read(0, readdata, m_param.nEEPRomDumpSize);
		if(ret < 0) 
		{
			*pnErrorCode = uts.errorcode.E_NVMWrite;
			uts.log.Error(_T("EEProm I2C fail !"));
			goto out;
		}

		uts.log.Debug(_T("Set Otp data_0x00:"));
		PrintDebugArray(readdata, m_param.nEEPRomDumpSize);

		if (memcmp(writedata, readdata, m_param.nEEPRomDumpSize)) {
			*pnErrorCode = uts.errorcode.E_NVMCheck;
			uts.log.Error(_T("OtpData different!"));
		}

		memset(writedata,0xFF,sizeof(char)*m_param.nEEPRomDumpSize);
		eeprom->Write(0, writedata, m_param.nEEPRomDumpSize);
		if(ret < 0) 
		{
			*pnErrorCode = uts.errorcode.E_NVMWrite;
			uts.log.Error(_T("EEProm I2C fail !"));
			goto out;
		}
		eeprom->Read(0, readdata, m_param.nEEPRomDumpSize);
		if(ret < 0) 
		{
			*pnErrorCode = uts.errorcode.E_NVMWrite;
			uts.log.Error(_T("EEProm I2C fail !"));
			goto out;
		}

		uts.log.Debug(_T("Set Otp data_0xFF:"));
		PrintDebugArray(readdata, m_param.nEEPRomDumpSize);

		if (memcmp(writedata, readdata, m_param.nEEPRomDumpSize)) {
			*pnErrorCode = uts.errorcode.E_NVMCheck;
			uts.log.Error(_T("OtpData different!"));
		}

		RELEASE_ARRAY(data);
		RELEASE_ARRAY(writedata);
		RELEASE_ARRAY(readdata);
		//------------------------------------------------------------------------------
out:
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
		vecReturnValue.push_back(uts.errorcode.E_NVMCheck);
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
