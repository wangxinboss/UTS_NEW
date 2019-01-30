#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_ReadSensorSetting.h"

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("FWFilePath"), m_param.FWFilePath, _T("../FW/"), _T("FW Path"));
		
		
		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
		*pnErrorCode = uts.errorcode.E_Pass;
		
		int iResult = -1;
		short *lpDumpData = NULL;
		int writeBuffer[4] = {0};
		memset(writeBuffer,0,sizeof(int)*4);
		int Dumpsize = 0;

		//Read Setting
		Dumpsize = 0xFF;
		lpDumpData = new short[Dumpsize];
		memset(lpDumpData, 0x0000, (Dumpsize));

		for (int i = 0 ;i<Dumpsize ;i++)
		{
			writeBuffer[1] = i;

			if (!m_pDevice->ReadValue(eDeviceReadValueType::DRVT_READ_SENSOR_REGISTER,
				writeBuffer, sizeof(short)))
			{
				uts.log.Error(_T("Device WriteValue DRVT_READ_SENSOR_REGISTER Error."));
				*pnErrorCode = uts.errorcode.E_Fail;
			}

			lpDumpData[i] = writeBuffer[0];
		}
		
		
		CFile fp;
		fp.Open(m_param.FWFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		fp.Write(lpDumpData, Dumpsize*sizeof(short));
		fp.Close();


		//------------------------------------------------------------------------------

		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		// 保存数据文件
		SaveData();

		return m_bResult;
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
