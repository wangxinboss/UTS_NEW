#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_EraseEEProm_V5U.h"

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
	
		int eepromsize = 8*1024;    //Here 5806 Download EEPROM size is 8*1024;

		//Erase Eeprom
		unsigned char dataBuf[128] = {0};
		memset(dataBuf, 0xFF, 128*sizeof(unsigned char));

		for (int i = 0 ; i< eepromsize ; i = i+ 32)
		{
			I2CWrite(0xA0, 2, i, 32, dataBuf);
			Sleep(10);
		}
	
	

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
