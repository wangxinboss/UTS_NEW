#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_OTPCheck.h"
#include "EEProm.h"
#include "CommonFunc.h"
#include "UtsBase.h"
#include "BaseOtp.h"

#pragma comment(lib, "UTS_EEProm.lib")
#pragma comment(lib, "CommonFunc.lib")


#include "otpdb.h"
#pragma comment(lib, "ddm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//UTOP_OTPCheck

BOOL IsFileExist(const char* csFile)
{
	DWORD dwAttrib = GetFileAttributesA(csFile);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReBoot"), m_param.nReBoot, 1, _T("power down and reboot for otp check"));



		CString strValue;
		vector<double> vecDoubleValue;
		vector<int> vecIntValue;


		//AWB spec
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dAWBSpec_RG"),strValue,  _T("200,244"), _T("AWB_RG Spec [SpecMin <= (RGr_Ratio*512) <= SpecMax] will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dAWBSpec_RGr.min = vecDoubleValue[0];
		m_param.dAWBSpec_RGr.max = vecDoubleValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dAWBSpec_BG"),strValue,  _T("288,348"), _T("AWB_BG Spec [SpecMin <= (BGr_Ratio*512 ) <= SpecMax] will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dAWBSpec_BGr.min = vecDoubleValue[0];
		m_param.dAWBSpec_BGr.max = vecDoubleValue[1];


	
		//AF spec
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dAFSpec_Inf"),strValue,  _T("100,460"), _T("AFSpec_Inf [SpecMin <= Inf Code <= SpecMax] will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dAFSpec_Inf.min = vecDoubleValue[0];
		m_param.dAFSpec_Inf.max = vecDoubleValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dAFSpec_Macro"),strValue,  _T("400,700"), _T("AFSpec_Macro [SpecMin <= Macro Code <= SpecMax] will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dAFSpec_Macro.min = vecDoubleValue[0];
		m_param.dAFSpec_Macro.max = vecDoubleValue[1];


		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nModuleID"), m_param.nModuleID, 1, _T("nModuleID"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nLensID"), m_param.nLensID, 1, _T("nLensID"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nVCMID"), m_param.nVCMID, 1, _T("nVCMID"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDriverICID"), m_param.nDriverICID, 1, _T("nDriverICID"));

		uts.log.Debug(_T("read spec nModuleID %x nLensID %x nVCMID %x nDriverICID %x"),m_param.nModuleID,m_param.nLensID,m_param.nVCMID, m_param.nDriverICID);


		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
	
		*pnErrorCode = uts.errorcode.E_Pass;

		if(m_param.nReBoot)
		{
			if (nullptr == m_pDevice)
			{
				*pnErrorCode = uts.errorcode.E_NoImage;
				return FALSE;
			}

			if (!m_pDevice->Stop())
			{
				*pnErrorCode = uts.errorcode.E_Fail;
				return FALSE;
			}

			Sleep(200);


			if (!m_pDevice->Start(_T("LightOn")))
			{
				*pnErrorCode = uts.errorcode.E_NoImage;
				return FALSE;
			}
			Sleep(300);

			//显示影像
			m_pDevice->GetBufferInfo(m_bufferInfo);
			if(!m_pDevice->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
			{
				*pnErrorCode = uts.errorcode.E_NoImage;
				return FALSE;
			}
			m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		}

		int data_size = sizeof(OTPData);
		uts.log.Debug(_T("OTP Data size is %d"),data_size);

		UTS::EEPROMDriver *eeprom;
		char *data = new char[data_size];
		memset(data,0,sizeof(char)*data_size);

		eeprom = GetEEPromDriverInstance(m_pDevice, m_param.nEEPRomType);
		eeprom->Read(0, data, data_size);


		memcpy(&m_otpdata,data,data_size);
	
		if(!CheckOTPData())
		{
			*pnErrorCode = OTPCALI_ERROR_CHKERR;
		}
		else
		{
			USES_CONVERSION;
			uts.otpdb->get_instance();
			if(uts.otpdb->lock_otp_data(uts.otpdb->GetModuleID(T2A(uts.info.strSN)))<0)
			{
				uts.log.Error(_T("lock_otp_data FAIL!"));
				*pnErrorCode = OTPCALI_ERROR_DB;
			}
			else
			{
				uts.log.Debug(_T("lock_otp_data PASS!"));
			}
		} 


		//Save bin
		CFile fp_bin;
		CString strDirPath;
		CString strFilePath;

		SYSTEMTIME st;
		GetLocalTime(&st);

		strDirPath.Format(_T("%s\\Data-%s-%s-%s-%04d%02d%02d_OTPCheck\\"),
			uts.info.strResultPath,
			uts.info.strProjectName,
			uts.info.strLineName,
			uts.info.strStationName,
			st.wYear,
			st.wMonth, 
			st.wDay);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		strFilePath.Format(_T("%s\\%s_%02d%02d%02d.bin"),
			strDirPath,
			uts.info.strSN,st.wHour,st.wMinute,st.wSecond);

		fp_bin.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		fp_bin.Write(data, data_size);
		fp_bin.Close();

		RELEASE_POINTER(data);

		//------------------------------------------------------------------------------

		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		// 保存数据文件
		SaveData();

		return m_bResult;
	}



	bool ImplOperator::CheckOTPData()
	{
		bool bRet = true;

		//check minfo flag,moudle id, lens id
		if(m_otpdata.minfo.FlagOfInfo != 0x01|| m_otpdata.minfo.ModuleID != m_param.nModuleID || m_otpdata.minfo.LensID != m_param.nLensID )
		{
			bRet = false;
			uts.log.Error(_T("check minfo Flag 0x%x module_id 0x%x  lens_id 0x%x   Error!"), 
				m_otpdata.minfo.FlagOfInfo,m_otpdata.minfo.ModuleID,m_otpdata.minfo.LensID);
		}
		else
		{
			uts.log.Debug(_T("check minfo Flag 0x%x module_id 0x%x  lens_id 0x%x   PASS!"), 
				m_otpdata.minfo.FlagOfInfo,m_otpdata.minfo.ModuleID,m_otpdata.minfo.LensID);
		}


		//check DriverICID,VCMID
		if( m_otpdata.minfo.DriverICID != m_param.nDriverICID || m_otpdata.minfo.VCMID != m_param.nVCMID )
		{
			bRet = false;
			uts.log.Error(_T("check minfo DriverICID 0x%x VCMID 0x%x   Error!"), m_otpdata.minfo.DriverICID,m_otpdata.minfo.VCMID);
		}
		else
		{
			uts.log.Debug(_T("check minfo DriverICID 0x%x VCMID 0x%x   PASS!"), m_otpdata.minfo.DriverICID,m_otpdata.minfo.VCMID);
		}


		//check date
		if(m_otpdata.minfo.Year<18 || m_otpdata.minfo.Year>22)
		{
			bRet = false;
			uts.log.Error(_T("check year %d is not in range 18~22 Error!"),m_otpdata.minfo.Year);
		}

		if(m_otpdata.minfo.Month<1 || m_otpdata.minfo.Month>12)
		{
			bRet = false;
			uts.log.Error(_T("check month is not in range  Error!"));
		}


		if(m_otpdata.minfo.Day<1 || m_otpdata.minfo.Day>31)
		{
			bRet = false;
			uts.log.Error(_T("check Day is not in range  Error!"));
		}

		//check MirrorFlip
		if( m_otpdata.minfo.MirrorFlip != 0x00 )
		{
			bRet = false;
			uts.log.Error(_T("check minfo MirrorFlip 0x%x   Error!"), m_otpdata.minfo.MirrorFlip);
		}
		else
		{
			uts.log.Debug(_T("check minfo MirrorFlip 0x%x  PASS!"),m_otpdata.minfo.MirrorFlip);
		}



		int nDataLenth = sizeof(MINFO);
		sum = CheckSum(&m_otpdata.minfo.CalToolVersion,nDataLenth-2);
		sum = sum%0xFF;

		if(m_otpdata.minfo.BasicInfoChecksum!= sum)
		{
			bRet = false;
			uts.log.Error(_T("Check MINFO checksum Error %d != %d!"),m_otpdata.minfo.BasicInfoChecksum,sum);
		}
		else
		{
			uts.log.Debug(_T("Check MINFO checksum %d PASS!"),m_otpdata.minfo.BasicInfoChecksum);
		}



		//check wb data
		if(m_otpdata.wb.FlagOfWB != 0x01)
		{
			bRet = false;
			uts.log.Error(_T("Check FlagOfWB 0x%x Error!"),m_otpdata.wb.FlagOfWB);

		}
		else
		{
			uts.log.Debug(_T("Check FlagOfWB 0x%x PASS!"),m_otpdata.wb.FlagOfWB);
		}

		sum = CheckSum(&m_otpdata.wb.WBHex[0],sizeof(m_otpdata.wb.WBHex))%0xFF;

		if(m_otpdata.wb.AWBChecksum != sum)
		{
			bRet = false;
			uts.log.Error(_T("Check AWBChecksum 0x%x Error!"),m_otpdata.wb.AWBChecksum);
		}
		else
		{
			uts.log.Debug(_T("Check AWBChecksum 0x%x PASS!"),m_otpdata.wb.AWBChecksum);
		}


		//check awb data spec
		memset(m_result.nAWBdata,0,sizeof(int)*8);
		for(int i=0;i<3;i++)
		{
			m_result.nAWBdata[i] = (m_otpdata.wb.WBHex[i*2+1]<<8)|m_otpdata.wb.WBHex[i*2];
			m_result.nAWBdata[i+3] = (m_otpdata.wb.WBHex[i*2+6+1]<<8)|m_otpdata.wb.WBHex[i*2+6];
		}



		//check lsc data
		if(m_otpdata.lsc.FlagOfLSC != 0x01)
		{
			bRet = false;
			uts.log.Error(_T("Check lsc_flag 0x%x Error!"),m_otpdata.lsc.FlagOfLSC);

		}
		else
		{
			uts.log.Debug(_T("Check lsc_flag 0x%x PASS!"),m_otpdata.lsc.FlagOfLSC);
		}

		sum = CheckSum(&m_otpdata.lsc.lscData[0],LSC_DATA_LENGTH)%0xFF;

		if(m_otpdata.lsc.ChecksumOfLSC != sum)
		{
			bRet = false;
			uts.log.Error(_T("Check lsc_checksum 0x%x Error!"),m_otpdata.lsc.ChecksumOfLSC);
		}
		else
		{
			uts.log.Debug(_T("Check lsc_checksum 0x%x PASS!"),m_otpdata.lsc.ChecksumOfLSC);
		}



		//check af data
		if(m_otpdata.af.FlagOfAF != 0x01)
		{
			bRet = false;
			uts.log.Error(_T("Check FlagOfAF 0x%x Error!"),m_otpdata.af.FlagOfAF);

		}
		else
		{
			uts.log.Debug(_T("Check FlagOfAF 0x%x PASS!"),m_otpdata.af.FlagOfAF);
		}

		sum = CheckSum(&m_otpdata.af.AFData[0],4)%0xFF;
		if(m_otpdata.af.AFChecksum != sum)
		{
			bRet = false;
			uts.log.Error(_T("Check AFChecksum 0x%x Error!"),m_otpdata.af.AFChecksum);
		}
		else
		{
			uts.log.Debug(_T("Check AFChecksum 0x%x PASS!"),m_otpdata.af.AFChecksum);
		}



		//check af spec inf,macro,start
		int nAFInf = 0, nAFMacro = 0;
		nAFInf = get_le_val(&m_otpdata.af.AFData[0],2);
		nAFMacro = get_le_val(&m_otpdata.af.AFData[2],2);
		m_result.nAFData[0] = nAFInf;
		m_result.nAFData[1] = nAFMacro;

		//检查AF数据是否在规格内
		if(IS_IN_RANGE(m_result.nAFData[0],m_param.dAFSpec_Inf.min,m_param.dAFSpec_Inf.max) && IS_IN_RANGE(m_result.nAFData[1],m_param.dAFSpec_Macro.min,m_param.dAFSpec_Macro.max))
		{

			uts.log.Debug(_T("check af_data  is in spec INF:%d %.2f %.2f MACRO:%d %.2f %.2f" )
				,m_result.nAFData[0],m_param.dAFSpec_Inf.min,m_param.dAFSpec_Inf.max,
				m_result.nAFData[1],m_param.dAFSpec_Macro.min,m_param.dAFSpec_Macro.max);
		}
		else
		{
			bRet = false;
			uts.log.Error(_T("af_result value is out of spec INF: %d %.2f %.2f MACRO:%d %.2f %.2f" )
				,m_result.nAFData[0],m_param.dAFSpec_Inf.min,m_param.dAFSpec_Inf.max,
				m_result.nAFData[1],m_param.dAFSpec_Macro.min,m_param.dAFSpec_Macro.max);
		}

		//cheeck Gain map
		if(m_otpdata.pdaf.FlagOfGainMap != 0x01)
		{
			bRet = false;
			uts.log.Error(_T("Check PDAF FlagOfGainMap Flag 0x%x Error!"),m_otpdata.pdaf.FlagOfGainMap);

		}
		else
		{
			uts.log.Debug(_T("Check PDAF FlagOfGainMap Flag 0x%x  PASS!"),m_otpdata.pdaf.FlagOfGainMap);
		}


		sum = CheckSum(&m_otpdata.pdaf.GainMapData[0],sizeof(m_otpdata.pdaf.GainMapData))%0xFF;
		if(m_otpdata.pdaf.GainMapChecksum != sum)
		{
			bRet = false;
			uts.log.Error(_T("Check PDAF GainMapChecksum %d Error!"),m_otpdata.pdaf.GainMapChecksum);
		}
		else
		{
			uts.log.Debug(_T("Check PDAF GainMapChecksum %d PASS!"),m_otpdata.pdaf.GainMapChecksum);
		}

		//cheeck dcc
		if(m_otpdata.pdaf.FlagDCC != 0x01)
		{
			bRet = false;
			uts.log.Error(_T("Check PDAF FlagOfGainMap Flag 0x%x Error!"),m_otpdata.pdaf.FlagDCC);

		}
		else
		{
			uts.log.Debug(_T("Check PDAF FlagOfGainMap Flag 0x%x  PASS!"),m_otpdata.pdaf.FlagDCC);
		}


		sum = CheckSum(&m_otpdata.pdaf.DCCData[0],sizeof(m_otpdata.pdaf.DCCData))%0xFF;
		if(m_otpdata.pdaf.DCC_Checksum != sum)
		{
			bRet = false;
			uts.log.Error(_T("Check PDAF DCC_Checksum %d Error!"),m_otpdata.pdaf.DCC_Checksum);
		}
		else
		{
			uts.log.Debug(_T("Check PDAF DCC_Checksum %d PASS!"),m_otpdata.pdaf.DCC_Checksum);
		}


		//check AEC
		if(m_otpdata.aec.FlagOfAEC != 0x01)
		{
			bRet = false;
			uts.log.Error(_T("Check FlagOfAEC 0x%x Error!"),m_otpdata.aec.FlagOfAEC);

		}
		else
		{
			uts.log.Debug(_T("Check FlagOfAEC 0x%x  PASS!"),m_otpdata.aec.FlagOfAEC);
		}


		sum = CheckSum(&m_otpdata.aec.AECData[0],sizeof(m_otpdata.aec.AECData))%0xFF;
		if(m_otpdata.aec.AECChecksum != sum)
		{
			bRet = false;
			uts.log.Error(_T("Check AECChecksum %d Error!"),m_otpdata.aec.AECChecksum);
		}
		else
		{
			uts.log.Debug(_T("Check AECChecksum %d PASS!"),m_otpdata.aec.AECChecksum);
		}

		m_result.nAECData[0] = get_be_val(&m_otpdata.aec.AECData[0],2);
		m_result.nAECData[1] = get_be_val(&m_otpdata.aec.AECData[2],2);
		m_result.nAECData[2] = get_be_val(&m_otpdata.aec.AECData[4],2);
		m_result.nAECData[3] = get_be_val(&m_otpdata.aec.AECData[6],2);
		m_result.nAECData[4] = get_be_val(&m_otpdata.aec.AECData[8],2);


		//check arcsoft dual data
		if(m_otpdata.arcsort.FlagOfDual != 0x01)
		{
			bRet = false;
			uts.log.Error(_T("Check FlagOfDual 0x%x Error!"),m_otpdata.arcsort.FlagOfDual);

		}
		else
		{
			uts.log.Debug(_T("Check FlagOfDual 0x%x  PASS!"),m_otpdata.arcsort.FlagOfDual);
		}


		sum = CheckSum(&m_otpdata.arcsort.DualData[0],sizeof(m_otpdata.arcsort.DualData))%0xFF;
		if(m_otpdata.arcsort.DualChecksum != sum)
		{
			bRet = false;
			uts.log.Error(_T("Check DualChecksum %d Error!"),m_otpdata.arcsort.DualChecksum );
		}
		else
		{
			uts.log.Debug(_T("Check DualChecksum %d PASS!"),m_otpdata.arcsort.DualChecksum );
		}

		return bRet;
	}
	
	CStringA ImplOperator::GetModulePath()
	{
		int  i    = 0;
		int  len  = 0;
		bool flag = false;
		CStringA myStr;
		char ModulePath[MAX_PATH] = {0};
		char path[128]={0};
		GetModuleFileNameA(NULL, ModulePath, MAX_PATH);//return  real  lenghth
		len = strlen(ModulePath);
		for (i = len - 1; i >= 0; i--)
		{
			if (ModulePath[i] == '\\')
			{
				ModulePath[i + 1] = 0;
				flag = true;
				strcpy(path, ModulePath);	
				break;
			}
		}

		if (!flag)
		{
			strcpy(path, "");

		}
		myStr.Format(path);
		return myStr;
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

		strHeader = _T("Time,SN,TestTime(ms),_Result,AWB_RGr,AWB_BGr,AWB_GbGr,AWB_RGr_g,AWB_BGr_g,AWB_GbGrg,AF_Inf,AF_Macor,AEC_BrightNessRatio,MainGain,MainExposure,AuxGain,AuxExposure,")
			
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%s,")
			_T("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
			,m_result.nAWBdata[0],m_result.nAWBdata[1],m_result.nAWBdata[2],m_result.nAWBdata[3],m_result.nAWBdata[4],m_result.nAWBdata[5],m_result.nAFData[0],m_result.nAFData[1]
			,m_result.nAECData[0],m_result.nAECData[1],m_result.nAECData[2],m_result.nAECData[3],m_result.nAECData[4]
			,strVersion, uts.info.strUserId);


			//mes
			if (uts.info.nMesEn!=0)
			{
				CString  str;  
				str.Format(_T("AWB_RGr=%d,AWB_BGr=%d,AWB_GbGr=%d,AWB_RGr_g=%d,AWB_BGr_g=%d,AWB_GbGrg=%d,AF_Inf=%d,AF_Macor=%d,AEC_BrightNessRatio=%d,MainGain=%d,MainExposure=%d,AuxGain=%d,AuxExposure=%d")
					,m_result.nAWBdata[0],m_result.nAWBdata[1],m_result.nAWBdata[2],m_result.nAWBdata[3],m_result.nAWBdata[4],m_result.nAWBdata[5],m_result.nAFData[0],m_result.nAFData[1],m_result.nAECData[0],m_result.nAECData[1],m_result.nAECData[2],m_result.nAECData[3],m_result.nAECData[4]);
				uts.log.Debug((LPCTSTR)str);
				uts.info.strMESInsData += str;
			}
	}
	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
