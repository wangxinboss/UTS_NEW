#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_UVC_LoadFW.h"

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iFWWriteMode"), m_param.iFWWriteMode, 1, _T("1:EEProm 2:USBOTP 3:USBOTP(ReWrite)"));
		
		
		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		uts.board.ShowStatus(eBoardStatus::Status_Testing);
		uts.board.ShowMsg(_T("LoadFW...")); 

		m_TimeCounter.SetStartTime();
		*pnErrorCode = uts.errorcode.E_Pass;
		
		LPBYTE lpData = NULL,lpDumpData = NULL;
		int eepromsize = 8*1024;    //Here 5806 Download EEPROM size is 8*1024;
		int Dumpsize = 0;

		CFile fp;
		if( !fp.Open(m_param.FWFilePath, CFile::modeRead | CFile::typeBinary))
		{
			uts.log.Error(_T("FW File not found!"));
			*pnErrorCode = uts.errorcode.E_NVMWrite;
			goto end;
		}

		eepromsize = (int) fp.GetLength();
		lpData = new BYTE[eepromsize];
		fp.Read(lpData, eepromsize);
		fp.Close();

		//Write Eeprom
		if(m_param.iFWWriteMode == 1) //EEProm
		{
			if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_LOADFW_EEPROM,
				lpData, eepromsize))
			{
				uts.log.Error(_T("Device WriteValue DWVT_LOADFW_EEPROM Error."));
				*pnErrorCode = uts.errorcode.E_Fail;
				goto end;
			}

			//Read Eeprom
			Dumpsize = 8*1024;
			lpDumpData = new BYTE[Dumpsize];
			memset(lpDumpData, 0xFF, (Dumpsize));

			if (!m_pDevice->ReadValue(eDeviceReadValueType::DRVT_DUMPFW_EEPROM,
				lpDumpData, Dumpsize))
			{
				uts.log.Error(_T("Device WriteValue DWVT_LOADFW_EEPROM Error."));
				*pnErrorCode = uts.errorcode.E_Fail;
				goto end;
			}
		}else if(m_param.iFWWriteMode == 2 || m_param.iFWWriteMode == 3) //USBOTP
		{
			//Mode 2: Check FW exit?
			if (m_param.iFWWriteMode == 2)
			{
				Dumpsize = 16*1024;
				lpDumpData = new BYTE[Dumpsize];
				memset(lpDumpData, 0xFF, (Dumpsize));

				if (!m_pDevice->ReadValue(eDeviceReadValueType::DRVT_DUMPFW_USBOTP,
					lpDumpData, Dumpsize))
				{
					uts.log.Error(_T("Device WriteValue DWVT_LOADFW_EEPROM Error."));
					*pnErrorCode = uts.errorcode.E_Fail;
					goto end;
				}
				
				for (int i = 0 ;i <Dumpsize ;i++)
				{
					if(lpDumpData[i] != 0xFF) //FW exit
					{
						uts.log.Debug(_T("OTP USB FW exist"));
						*pnErrorCode = uts.errorcode.E_Pass;
						goto end;
					}
				}
			}
				
			//Write OTP
			if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_LOADFW_USBOTP,
				lpData, eepromsize))
			{
				uts.log.Error(_T("Device WriteValue DWVT_LOADFW_EEPROM Error."));
				*pnErrorCode = uts.errorcode.E_Fail;
				goto end;
			}
			
			Dumpsize = 16*1024;
			lpDumpData = new BYTE[Dumpsize];
			memset(lpDumpData, 0xFF, (Dumpsize));

			if (!m_pDevice->ReadValue(eDeviceReadValueType::DRVT_DUMPFW_USBOTP,
				lpDumpData, Dumpsize))
			{
				uts.log.Error(_T("Device WriteValue DWVT_LOADFW_EEPROM Error."));
				*pnErrorCode = uts.errorcode.E_Fail;
				goto end;
			}
		}

		//Save Dump FW 
// 				CFile fp2;
// 				fp2.Open(_T("5806Dump.rfw"), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
// 				fp2.Write(lpDumpData, Dumpsize);
// 				fp2.Close();


// 				if (memcmp(lpData,lpDumpData,min(eepromsize,Dumpsize))!=0 ) 
// 				{
// 					*pnErrorCode = uts.errorcode.E_NVMWrite;
// 					uts.log.Error(_T("FW Read Write Compare Error!"));
// 				}

		for (int i = 0 ; i< min(eepromsize,Dumpsize);i++)
		{
			if((m_param.iFWWriteMode == 2 || m_param.iFWWriteMode == 3) && i == 2) continue; //USBOTP data[2] different 

			if(lpData[i] != lpDumpData[i])
			{
				*pnErrorCode = uts.errorcode.E_NVMWrite;
				uts.log.Error(_T("FW Read Write Compare Error!"));
			}
		}
		
		//------------------------------------------------------------------------------
end:
		uts.board.ShowMsg(EMPTY_STR);

		if(lpData)
		{
			delete lpData;
			lpData = NULL;
		}

		if(lpDumpData)
		{
			delete lpDumpData;
			lpDumpData = NULL;
		}

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
