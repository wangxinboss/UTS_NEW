#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include <direct.h>

#pragma comment(lib, "UTS_Sensor.lib")
#pragma comment(lib, "UTS_EEProm.lib")
#pragma comment(lib, "UTS_OIS.lib")


#include "Project/OV16885_MA84F/OtpCali_OV16885_MA84F.h"
#include "Project/S5K4H7_MF86A/OtpCali_S5K4H7_MF86A.h"
#include "Project/S5K4H7_HNE5030/OtpCali_S5K4H7_HNE5030.h"
#include "Project/S5K3P9SX_HNF1065/OtpCali_S5K3P9SX_HNF1065.h"
#include "Project/S5K3P9SX_MF83Y/OtpCali_S5K3P9SX_MF83Y.h"

//-------------------------------------------------------------------------------------------------
//Device
UTS::OISDriver *BaseOtp::ois = NULL;
UTS::SensorDriver *BaseOtp::sensor = NULL;
UTS::EEPROMDriver *BaseOtp::eeprom = NULL;
//-------------------------------------------------------------------------------------------------
int BaseOtp::otpdb_data_cache_len = -1;
uint8_t *BaseOtp::otpdb_data_cache = NULL;
uint8_t *BaseOtp::uniform = NULL;
//-------------------------------------------------------------------------------------------------

BaseOtp* GetOtpInstance(
    const TCHAR *pProjectName,
    BaseDevice *pDevice)
{
    static BaseOtp *pOtpCali = nullptr;
    if (nullptr == pOtpCali)
    {
		if (0 == _tcscmp(pProjectName, _T("MA84F")))
		{
			pOtpCali = new OtpCali_OV16885_MA84F::OtpCali_OV16885_MA84F(pDevice);
		}
		else if (0 == _tcscmp(pProjectName, _T("MF86A")))
		{
			pOtpCali = new OtpCali_S5K4H7_MF86A::OtpCali_S5K4H7_MF86A(pDevice);
		}
		else if (0 == _tcscmp(pProjectName, _T("HNE5030")))
		{
			pOtpCali = new OtpCali_S5K4H7_HNE5030::OtpCali_S5K4H7_HNE5030(pDevice);
		}
		else if (0 == _tcscmp(pProjectName, _T("HNF1065")))
		{
			pOtpCali = new OtpCali_S5K3P9SX_HNF1065::OtpCali_S5K3P9SX_HNF1065(pDevice);
		}
		else if (0 == _tcscmp(pProjectName, _T("MF83Y")))
		{
			pOtpCali = new OtpCali_S5K3P9SX_MF83Y::OtpCali_S5K3P9SX_MF83Y(pDevice);
		}
    }

    return pOtpCali;
}

BaseOtp::BaseOtp(BaseDevice *dev) : dev(dev)
{
	otp_type = OTP_TYPE_OTP;
	otp_data_len = -1;
	otp_lsc_len = -1;
	//otp_data_eeprom_len = -1;

	otp_data_in_db = otp_data_in_sensor = otp_lsc_data = NULL;
	otpdb_data_cache_len = -1;
	otpdb_data_cache = (u8*)dlmalloc(SIZE_K*SIZE_K);
	if (!uniform) uniform = (uint8_t*)dlmalloc(SIZE_K*SIZE_K);
	otp_operator_attr = 0;
}

BaseOtp::~BaseOtp(void)
{
    if (otp_data_in_db) free(otp_data_in_db);
    if (otp_data_in_sensor) free(otp_data_in_sensor);
    if (otp_lsc_data) free(otp_lsc_data);
    if (otpdb_data_cache) dlfree(otpdb_data_cache);
	if (uniform) dlfree(uniform);

}

//---------------------------------------------------------------------------------------
OTP_OPTION BaseOtp::options[] =
{
	{OtpCaliOpt_ProgOtp, _T("Prog"),    NULL, &BaseOtp::prog_otp,  0},
    {OtpCaliOpt_ChkOtp,  _T("Check"),   NULL, &BaseOtp::check_otp,   0},
	{OtpCaliOpt_LscCali, _T("LSC"),     _T("OTP_WBLSC_TEST"), &BaseOtp::LscCali,  OTP_ATTR_LT|OTP_ATTR_UP},
	{OtpCaliOpt_WBCali,  _T("WB"),      _T("OTP_WBLSC_TEST"), &BaseOtp::WBCali,   OTP_ATTR_LT|OTP_ATTR_UP},
    //{OtpCaliOpt_TestEEPROM, "TstEEP", NULL, &OtpCali::TestEEProm, 0},
    //{OtpCaliOpt_TestOtpLife,"TstLife",NULL, &OtpCali::TestLife,0},

    {OtpCaliOpt_LOADLSC, _T("LoadLSC"), _T("OTP_WBLSC_TEST"), &BaseOtp::check_otp_off_lsc, OTP_ATTR_LT},
    {OtpCaliOpt_LOADWB,  _T("LoadWB"),  _T("OTP_WBLSC_TEST"), &BaseOtp::check_otp_off_wb, OTP_ATTR_LT},
	{OtpCaliOpt_DUMP,    _T("DUMP"),    NULL, &BaseOtp::DumpOtp,   0,},
    {OtpCaliOpt_SPC,     _T("SPC"),      _T("OTP_SPC_TEST"), &BaseOtp::SPCCali,   OTP_ATTR_LT|OTP_ATTR_UP},
    {OtpCaliOpt_DCC0,    _T("DCC15"),   _T("OTP_DCC_TEST"), &BaseOtp::DCC0Cali,  OTP_ATTR_UP},
    {OtpCaliOpt_DCC1,    _T("DCC50"),   _T("OTP_DCC_TEST"), &BaseOtp::DCC1Cali,  OTP_ATTR_UP},
	{OtpCaliOpt_PDAFVerify, _T("PDAFVerify"),   _T("OTP_DCC_TEST"), &BaseOtp::PDAFVerify,  OTP_ATTR_UP},
    //{OtpCaliOpt_ChkHis,  "CHKHIS",   NULL, &OtpCali::check_otp_program_history,  0},
    //{OtpCaliOpt_ResetEeprom, "ResetEEPROM", NULL, &OtpCali::reset_eeprom, 0},
    //{OtpCaliOpt_OffChk,  "OffChk",   NULL, &OtpCali::otp_off_chk, 0},
	{OtpCaliOpt_OffChk,  _T("OffChk"),   NULL, &BaseOtp::otp_off_chk, 0},
	{OtpCaliOpt_LOADFW,	  _T("LoadFW"),     NULL, &BaseOtp::LoadFW,  0},
	{OtpCaliOpt_CHECKFW,  _T("CheckFW"),     NULL, &BaseOtp::CheckFW,  0},
	{OtpCaliOpt_HallCali, _T("HallCali"),     NULL, &BaseOtp::HallCali,  OTP_ATTR_UP},
	{OtpCaliOpt_GyroCali, _T("GyroCali"),     _T("OTP_GyroCali_TEST"), &BaseOtp::GyroCali,  0},
	{OtpCaliOpt_OISCheck, _T("OISCheck"),     _T("OTP_GyroCali_TEST"), &BaseOtp::OISCheck,  0},
	{OtpCaliOpt_SENSORSPC,_T("SensorSPC"), NULL, &BaseOtp::SensorSPCCali,OTP_ATTR_LT|OTP_ATTR_UP},
	{OtpCaliOpt_GyroCaliNonShaker,_T("GyroCaliNonShaker"), NULL, &BaseOtp::GyroCali_NonShaker,0},
    {OtpCaliOpt_WBRAW10Cali,_T("WB_RAW10"),      _T("OTP_WBLSC_TEST"), &BaseOtp::WBCali_RAW10,   OTP_ATTR_LT|OTP_ATTR_UP},
	{OtpCaliOpt_WB2RAW10Cali,_T("WB2_RAW10"),      _T("OTP_WB2LSC_TEST"), &BaseOtp::WBCali_RAW10,   OTP_ATTR_LT|OTP_ATTR_UP},
	{OtpCaliOpt_WB3RAW10Cali,_T("WB3_RAW10"),      _T("OTP_WB3LSC_TEST"), &BaseOtp::WBCali_RAW10,   OTP_ATTR_LT|OTP_ATTR_UP},
	{OtpCaliOpt_CrossTalk,_T("CrossTalk"),      _T("OTP_CrossTalk_TEST"), &BaseOtp::CrossTalk,   OTP_ATTR_LT|OTP_ATTR_UP},
	{OtpCaliOpt_LSCMTK,_T("LSCMTK"),      _T("OTP_LSCMTK_TEST"), &BaseOtp::LSCMTK_Cail,   OTP_ATTR_LT|OTP_ATTR_UP},
	//{OtpCaliOpt_ProgSN,  "ProgSN",   NULL, &OtpCali::sn_prog, 0},
};

OTP_OPTION* BaseOtp::get_option(int opt)
{
    for (auto iter = begin(options); iter != end(options); ++iter)
    {
        if (iter->opt == opt) return &*iter;
    }
    return nullptr;
}

BOOL BaseOtp::Initialize_param(const OTP_INIT_PARAM& init_param)
{
	// set param
	memcpy(&otp_param, &init_param.stOtpParam, sizeof(OTP_PARAM));
	_tcscpy(m_szSN, init_param.sn);
	
	eeprom = GetEEPromDriverInstance(dev, init_param.nEEPRomType);
    sensor = GetSensorInstance(dev,uts.info.nSensorType);
    ois = GetOISDriverInstance(dev,uts.info.nOISType);

	otpDB = uts.otpdb;

	USES_CONVERSION;
	otp_data_from_db_valid_time = 0;

	if (init_param.otpDBPara.dbType >= 0)
	{
		if (!uts.otpdb->IsOtpDbConnected())
		{
			return SET_ERROR(OTPCALI_ERROR_DB);
		}

		if ((mid = otpDB->GetModuleID(T2A((TCHAR *)init_param.sn))) < 0)
		{
			return SET_ERROR(OTPCALI_ERROR_DB);
		}
	}

	if (otp_data_len > 0)
	{
		if (nullptr == otp_data_in_db)
		{
			otp_data_in_db = (uint8_t *)malloc(otp_data_len); 
		}
		if (nullptr == otp_data_in_sensor)
		{
			otp_data_in_sensor = (uint8_t*)malloc(otp_data_len);
		}
	}
	if (otp_lsc_len > 0)
	{
		if (nullptr == otp_lsc_data)
		{
			otp_lsc_data = (uint8_t *)malloc(otp_lsc_len);
		}
	}
	return 0;
}

BOOL BaseOtp::is_otp_programed(int module_id)
{
    time_t time;
    if (otpDB->GetOtpBurnTime(module_id, &time) < 0)
    {
        return FALSE;
    }
    return (time > 0 ? TRUE : FALSE);
}

int BaseOtp::is_otp_data_locked(int module_id)
{
    time_t time;

	if(uts.info.nOtpDBType == -1) return FALSE;


    if (otpDB->get_otp_data_lock_time(module_id, &time) < 0)
    {
        return is_otp_programed(module_id);
    }
    return (time > 0 ? TRUE : FALSE);
}

bool BaseOtp::is_otp_mem_def()
{
    bool multi = !!((otp_type - 1) & otp_type);

    for (int i = 0; i < sizeof(otp_type) * 8; i++) 
	{

        int opt = (1 << i);
        if (!(otp_type & opt)) continue;

        int val, len = otp_data_len;
        uint8_t *addr = otp_data_in_sensor;
        if (opt == OTP_TYPE_EEPROM) 
		{
            continue;
#if 0
            val = 0xFF;
            if (multi) len = otp_data_eeprom_len;
#endif
        } 
		else if (opt == OTP_TYPE_OTP) 
		{
           /* if (otp_param.group_for_use != 0)
                return true;*/
            val = 0x00;
            if (multi) 
			{
                addr += otp_data_eeprom_len;
                len -= otp_data_eeprom_len;
            }
        }

        if (!is_mem_val(addr, val, len))
            return false;
    }
    return true;
}

int BaseOtp::do_dump_eeprom()
{
	/*int len = eeprom->GetSize();
	char *data = (char *)dlmalloc(len);*/

	int len = otp_data_len;
	//int len = 0x1412;
    unsigned char *databuf = (unsigned char *)dlmalloc(len);
	int ret= eeprom->Read(0, databuf, len);

	
	CFile fp_bin;
	CString strDirPath;
	CString strFilePath;
	long int nStartAddr=0x00;
	SYSTEMTIME st;
	GetLocalTime(&st);

	strDirPath.Format(_T("%sData-%s-%s-%s-%04d%02d%02d_OTPDump\\"),
		uts.info.strResultPath,
		uts.info.strProjectName,
		uts.info.strLineName,
		uts.info.strStationName,
		st.wYear,
		st.wMonth, 
		st.wDay);

	UTS::OSUtil::CreateMultipleDirectory(strDirPath);

	//bin file name
	strFilePath.Format(_T("%s%s_OTPDump.bin"),
		strDirPath,
		uts.info.strSN);

	fp_bin.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	fp_bin.Write(databuf,len);
	fp_bin.Close();


	//Save to csv
	strFilePath.Format(_T("%s%s_OTPDump.csv"),
		strDirPath,
		uts.info.strSN);

	CFile LogFile;
	CString strData;
	CString strTemp;
	if (LogFile.Open(strFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		strData = _T("Addr,Data\r\n");;
		for(int i=0;i<len;i++)
		{
			strTemp.Format(_T("0x%04x,0x%02x\r\n"),nStartAddr+i,databuf[i]);
			strData += strTemp;
		}

		int nCount = strData.GetLength()*2;
		LogFile.Write(strData, nCount);

		LogFile.Close();
		//////////////////////////////////////////////////////////////////////////
	}
	else
	{
		MessageBox(NULL, _T("Can't write data to the dump data file!"), _T("Save Log Error"), MB_OK | MB_ICONERROR);
	}


    uts.log.Debug(_T("OtpData in Sensor:"));
	/*for(int i=0;i<otp_data_len/16;i++)
	{
		PrintDebugArray(databuf+i*16, 16);
		uts.log.Debug(_T(""));
	}
	PrintDebugArray(databuf+(otp_data_len/16)*16, otp_data_len-(otp_data_len/16)*16);*/

    dlfree(databuf);
    return 0;
}

int BaseOtp::do_dump_otp()
{
 /*   int len = sensor->GetPageSize();
    char *data = (char *)dlmalloc(len);

	
    int pages = sensor->GetUserPageNum();
    for (int i = 0; i < pages; i++) {
        sensor->ReadOtp(sensor->user_pages[i], sensor->GetStartAddr(), data, len);
        uts.log.Debug(_T("Otp data Page[%d]:"), sensor->user_pages[i]);
        PrintDebugArray(data, len);
    }*/
	int len = otp_data_len;
    unsigned char *databuf = (unsigned char *)dlmalloc(len);
	int ret=get_otp_data_from_sensor(databuf);

	/*uts.log.Info(_T("ret=%d"),ret);

	if (ret < 0)
	{
		dlfree(databuf);

		return 0;
	}

	FILE *input = fopen("otp-buffer.txt","w");
	
	if(input != NULL)
	{
		
		for(int i=0;i<otp_data_len;i++)
		{
			fprintf(input,"0x%04x\t0x%02x\n", i+0x0201,databuf[i]);
		}
	}*/

	CFile fp_bin;
	CString strDirPath;
	CString strFilePath;
	long int nStartAddr=0x0A3D;
	SYSTEMTIME st;
	GetLocalTime(&st);

	strDirPath.Format(_T("%sData-%s-%s-%s-%04d%02d%02d_OTPDump\\"),
		uts.info.strResultPath,
		uts.info.strProjectName,
		uts.info.strLineName,
		uts.info.strStationName,
		st.wYear,
		st.wMonth, 
		st.wDay);

	UTS::OSUtil::CreateMultipleDirectory(strDirPath);

	//bin file name
	strFilePath.Format(_T("%s%s_OTPDump.bin"),
		strDirPath,
		uts.info.strSN);

	fp_bin.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	fp_bin.Write(databuf,otp_data_len);
	fp_bin.Close();


	//Save to csv
	strFilePath.Format(_T("%s%s_OTPDump.csv"),
		strDirPath,
		uts.info.strSN);

	CFile LogFile;
	CString strData;
	CString strTemp;
	if (LogFile.Open(strFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		strData = _T("Addr,Data\r\n");;
		for(int i=0;i<otp_data_len;i++)
		{
			strTemp.Format(_T("0x%04x,0x%02x\r\n"),nStartAddr+i,databuf[i]);
			strData += strTemp;
		}

		int nCount = strData.GetLength()*2;
		LogFile.Write(strData, nCount);

		LogFile.Close();
		//////////////////////////////////////////////////////////////////////////
	}
	else
	{
		MessageBox(NULL, _T("Can't write data to the dump data file!"), _T("Save Log Error"), MB_OK | MB_ICONERROR);
	}


    uts.log.Debug(_T("OtpData in Sensor:"));
	for(int i=0;i<otp_data_len/16;i++)
	{
		PrintDebugArray(databuf+i*16, 16);
		uts.log.Debug(_T(""));
	}
	PrintDebugArray(databuf+(otp_data_len/16)*16, otp_data_len-(otp_data_len/16)*16);

    dlfree(databuf);
    return 0;
}

int BaseOtp::DumpOtp(void *args)
{
    for (int i = 0; i < sizeof(otp_type) * 8; i++) {

        int opt = (1 << i);
        if (!(otp_type & opt)) continue;

        if (opt == OTP_TYPE_OTP) {
            do_dump_otp();
        }
        else if (opt == OTP_TYPE_EEPROM) {
            do_dump_eeprom();
        }
    }

    return 0;
}

int BaseOtp::SET_ERROR(int errorNo)
{
    this->errorNo = errorNo; 
    return -errorNo;
}

int BaseOtp::GetErrorCode()
{
	switch (errorNo) {
	case OTPCALI_ERROR_SENSOR: return NVM_DATA_CONSIST;
	case OTPCALI_ERROR_DB:
		{
			int err = otpDB->GetOtpDBErrorNo();
			switch (err) {
			case DDM::ERR_OTPDB_SETUP: return NVM_DB_CONNECT;
			case DDM::ERR_BURNED_BEFORE: return NVM_BURNED_BEFORE;
			case DDM::ERR_NOINFO: return NVM_NOINFO;
			}
		}
		return uts.errorcode.E_Fail;
	case OTPCALI_ERROR_NODATA: return NVM_NOINFO;
	case OTPCALI_ERROR_PROGERR: return NVM_WRITE;
	case OTPCALI_ERROR_READERR: return NVM_NOINFO;
	case OTPCALI_ERROR_CHKERR: return NVM_CHECK;
	case OTPCALI_ERROR_EEPROM: return NVM_DATA_CONSIST;
	case OTPCALI_ERROR_LSCCALI: return NVM_LSC;
	case OTPCALI_ERROR_WBCALI: return NVM_WB;
	case OTPCALI_ERROR_OMIT: return NVM_OTP_TEST_OMIT;
	case OTPCALI_ERROR_DATACONSIST: return NVM_DATA_CONSIST;
	case OTPCALI_ERROR_NO: return uts.errorcode.E_Pass;
	case OTPCALI_ERROR_OPTNOTSUPPORT: return NVM_NOINFO;
	case OTPCALI_ERROR_PDAF_SPC: return ERR_SPC;
	case OTPCALI_ERROR_PDAF_DCC: return ERR_DCC;
	case OTPCALI_ERROR_NO_GOLDEN: return NVM_NOINFO;
	case OTPCALI_ERROR_SHOULDBE_GOLDEN: return NVM_BEGOLDEN;
	case OTPCALI_ERROR_AE_TIMEOUT: return AE_TIMEOUT;
	case OTPCALI_ERROR_NO_PROG_HISTORY: return NVM_NO_PROG_HISTORY;
	case OTPCALI_ERROR_INVALID_GROUP: return NVM_CHECK;
	case OTPCALI_ERROR_LOADFW: return NVM_FW; 
	case OTPCALI_ERROR_OISCHECK : return ERR_OISCHECK;
	case OTPCALI_ERROR_GYROCALI : return ERR_GYROCALI;
	case OTPCALI_ERROR_CCT 	    : return ERR_CCT;
	case OTPCALI_ERROR_NOTDEFINEED:

	default: return uts.errorcode.E_Fail;
	}
}

//---------------------------------------------------------------------------------------

int BaseOtp::get_otp_from_raw_data(int type, void *out, int len)
{
    uts.log.Debug(_T("Start to get otp for type[%d]..."), type);
    otp_item *otp = get_otp_item(type, otpdb_data_cache, otpdb_data_cache_len); 
    if (!otp) {
        uts.log.Error(_T("Not find otp for type[%d]!"), type);
        return SET_ERROR(OTPCALI_ERROR_NODATA);
    }
    if (otp->len != len) {
        uts.log.Error(_T("otp data len[%d] error, should be[%d]!"), otp->len, len);
        return SET_ERROR(OTPCALI_ERROR_NODATA);
    }
    memcpy(out, otp->data, len);
    return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::get_minfo_from_raw_data(void *out, int len)
{
    return get_otp_from_raw_data(OTPDB_OTPTYPE_MODULE, out, len);
}

int BaseOtp::get_lsc_from_raw_data(void *out, int len)
{
	return get_otp_from_raw_data(OTPDB_OTPTYPE_LSC, out, len);
}


int BaseOtp::get_wb_from_raw_data(WB_DATA_UCHAR *wb, WB_DATA_UCHAR *wb_g)
{
    WB_DATA_UCHAR wb_tmp[2];

    int ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB, wb_tmp, sizeof(wb_tmp));
    if (ret < 0) return ret;

    if (CheckWBLimit(&wb_tmp[0]) < 0 || CheckWBLimit(&wb_tmp[1]) < 0)
        return SET_ERROR(OTPCALI_ERROR_WBCALI);

    memcpy(wb, &wb_tmp[0], sizeof(WB_DATA_UCHAR));
    memcpy(wb_g, &wb_tmp[1], sizeof(WB_DATA_UCHAR));

    return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::get_wb_from_raw_data(WB_DATA_USHORT *wb, WB_DATA_USHORT *wb_g)
{
    WB_DATA_USHORT wb_tmp[2];

    int ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB, wb_tmp, sizeof(wb_tmp));
    if (ret < 0) return ret;

    if (CheckWBLimit_Ushort(&wb_tmp[0]) < 0 || CheckWBLimit_Ushort(&wb_tmp[1]) < 0)
        return SET_ERROR(OTPCALI_ERROR_WBCALI);

    memcpy(wb, &wb_tmp[0], sizeof(WB_DATA_USHORT));
    memcpy(wb_g, &wb_tmp[1], sizeof(WB_DATA_USHORT));

    return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::get_wb_from_raw_data(WB_DATA_USHORT *wb, WB_DATA_USHORT *wb_g, int wbType)
{
    WB_DATA_USHORT wb_tmp[2];

    //wbType = 1 => 4000k , wbType = 2 => 3100k, other 5100k
    int ret ;


//    if(wbType == 1)
//		ret= get_otp_from_raw_data(OTPDB_OTPTYPE_AWB2, wb_tmp, sizeof(wb_tmp));	
//    else if(wbType == 2)
//		ret= get_otp_from_raw_data(OTPDB_OTPTYPE_AWB3, wb_tmp, sizeof(wb_tmp));
//    else
//    		ret= get_otp_from_raw_data(OTPDB_OTPTYPE_AWB, wb_tmp, sizeof(wb_tmp));

    if (ret < 0) 
		{
			uts.log.Error(_T("get DB Error!!"));
			return ret;
	}

    if (CheckWBLimit_Ushort(&wb_tmp[0]) < 0 || CheckWBLimit_Ushort(&wb_tmp[1]) < 0)
        return SET_ERROR(OTPCALI_ERROR_WBCALI);

    memcpy(wb, &wb_tmp[0], sizeof(WB_DATA_USHORT));
    memcpy(wb_g, &wb_tmp[1], sizeof(WB_DATA_USHORT));

    return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::get_af_from_raw_data(int *sta, int *inf, int *mac)
{
	using namespace DDM;
	AF_PARAM *af = &otp_param.af;
	int *dac_raw, *dac_off, *dac_out;

	for (int i = 0; i < 3; i++) {
		int type = 0, *dac = NULL;
		switch (i) {
		case 0: 
			dac = sta; 
			type = OTPDB_OTPTYPE_AFSTART; 
			dac_raw = &af->raw_sta; 
			dac_off = &af->off_sta;
			dac_out = &af->sta;
			break;
		case 1: 
			dac = inf; 
			type = OTPDB_OTPTYPE_AFINF; 
			dac_raw = &af->raw_inf; 
			dac_off = &af->off_inf;
			dac_out = &af->inf;
			break;
		case 2: 
			dac = mac; 
			type = OTPDB_OTPTYPE_AFMUP; 
			dac_raw = &af->raw_mac; 
			dac_off = &af->off_mac;  
			dac_out = &af->mac;
			break;
		}
		if (!dac) continue;

		uint8_t dac_tmp[2];
		int ret = get_otp_from_raw_data(type, dac_tmp, sizeof(dac_tmp));
		if (ret < 0) return ret;

		*dac_raw = get_le_val(dac_tmp, sizeof(dac_tmp));
		if (*dac_raw < 0 || *dac_raw > 1023) {
			uts.log.Error(_T("Af data[%d] out of limit, type[%d]!"), *dac, type);
			return SET_ERROR(OTPCALI_ERROR_NODATA);
		}
		*dac = *dac_raw - *dac_off;
		*dac_out = *dac;
	}
	return SET_ERROR(OTPCALI_ERROR_NO);
}

bool BaseOtp::is_mem_val(const void *mem, int val, int len)
{
    const uint8_t *_data = (const uint8_t *)mem;

    while (len-- > 0) 
	{
        if (_data[len] != val) 
			return false;
    }
    return true;
}




void BaseOtp::PrintDebugArray(const void *array, int len)
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



int BaseOtp::get_group(uint8_t flag, int max_group, int group_flag)
{
    max_group = MIN(max_group, 4);
    for (int i = 0; i < max_group; i++) {
        if (GET_BITS(flag, 6, 0x03) == group_flag) 
            return i;
        flag <<= 2;
    }
    return SET_ERROR(OTPCALI_ERROR_INVALID_GROUP);
}




#pragma region check & prog

int BaseOtp::_get_otp_data_from_db(void *args)
{
    if (otp_data_from_db_valid_time != 0) return 0;

    if (otp_operator_attr & OTP_OPERATOR_ATTR_USECACHE) 
	{
        otpdb_data_cache_len = otpDB->get_otp_by_mid(mid, otpdb_data_cache, SIZE_K*SIZE_K);
        if (otpdb_data_cache_len < 0) 
		{
            uts.log.Error(_T("Got otp data from db error!"));
            return SET_ERROR(OTPCALI_ERROR_DB);
        }
    }
	
    memset(otp_data_in_db, 0xFF, otp_data_len);
    int ret = get_otp_data_from_db(args);
    if (ret < 0) return ret;

    otp_data_from_db_valid_time = time(NULL); 

    return ret;
}

int BaseOtp::prog_otp(void *args)
{
	int ret = OTPCALI_ERROR_NO;
	//---------------------------------
#if 1
	uts.log.Info(_T("start get otp from db"));
	ret = _get_otp_data_from_db(otp_data_in_db);
	if (ret < 0) { return -1;}
  
	CFile fp_bin;
	CString strDirPath;
	CString strFilePath;

	 
	//Save to csv
	strFilePath.Format(_T("%s%s_OTPDump.csv"),
		strDirPath,
		uts.info.strSN);

	CFile LogFile;
	CString strData;
	CString strTemp;
	if (LogFile.Open(strFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		strData = _T("Addr,Data\r\n");;
		for(int i=0;i<otp_data_len;i++)
		{
			strTemp.Format(_T("0x%04x,0x%02x\r\n"),i,otp_data_in_db[i]);
			strData += strTemp;
		}

		int nCount = strData.GetLength()*2;
		LogFile.Write(strData, nCount);

		LogFile.Close();
		//////////////////////////////////////////////////////////////////////////
	}
#endif
	//-------------------------------------------------
	if (otp_data_len < 0) 
	{
		MessageBox(NULL, _T("Not support prog for proj"), _T("Error"), NULL);
		ret = OTPCALI_ERROR_PROGERR;
		goto out;
	}
	uts.log.Info(_T("otp_data_len=%d"),otp_data_len);

	memset(otp_data_in_sensor,0xFF,otp_data_len);
	uts.log.Info(_T("start get otp from sensor"));

    ret = get_otp_data_from_sensor(otp_data_in_sensor);
	if (ret < 0) 
	{ 
		uts.log.Info(_T("Get otp data from sensor!!"));
		ret = -ret; 
		goto out;
	}

	if (otpDB->lock_otp_data(mid) < 0) 
	{
		ret = OTPCALI_ERROR_DB;
		goto out;
	}

	uts.log.Info(_T("start get otp from db"));
	ret = _get_otp_data_from_db(otp_data_in_db);
	if (ret < 0) { ret = -ret; goto out;}

	//local 20180401
	uts.log.Info(_T("start do prog"));
	int trycnt = 1;
	while (trycnt-- > 0)
	{
		ret = do_prog_otp(); 
		if (check_otp(args) == OTPCALI_ERROR_NO) 
			break;
	}
	if (trycnt < 0)
	{
		ret = OTPCALI_ERROR_PROGERR; 
		goto out;
	}
	if (otpDB->UpdateOtpBurnHistory(mid) < 0)
	{
		ret = OTPCALI_ERROR_DB;
		goto out;
	}

out:
	return SET_ERROR(ret);
}

int BaseOtp::check_otp(void *args)
{
	uts.log.Info(_T("BaseOtp£ºcheck_otp"));

	int ret = OTPCALI_ERROR_NO;

	if (otp_data_len < 0) {
		uts.log.Error(_T("Not support prog for proj"));
		ret = OTPCALI_ERROR_CHKERR; 
		goto out;
	}

	ret = get_otp_data_from_sensor(otp_data_in_sensor);
	if (ret < 0) { ret = -ret; goto out;}

 	ret = _get_otp_data_from_db(otp_data_in_db);
 	if (ret < 0) { ret = -ret; goto out;}
//	memcmp(a, b):1 //×Ö·û´®a>×Ö·û´®b, ·µ»Ø1
// 	memcmp(a, c):-1 // ×Ö·û´®a<×Ö·û´®c, ·µ»Ø£­1
// 	memcmp(a, d):0 //×Ö·û´®a£½×Ö·û´®d, ·µ»Ø0
// 	if (memcmp(otp_data_in_sensor, otp_data_in_db, otp_data_len)) 
// 	{
// 		for(int i=0;i<otp_data_len;i++)
// 		{
// 			uts.log.Error(_T("Sensor : DB [%d] = %d , %d") , i, otp_data_in_sensor[i] , otp_data_in_db[i]);
// 		}
// 		
// 		ret = OTPCALI_ERROR_CHKERR;
// 		uts.log.Error(_T("OtpData different in DB !"));
// 		uts.log.Error(_T("otp_data_in_sensor = %d") , sizeof(otp_data_in_sensor));
// 		uts.log.Error(_T("otp_data_in_db = %d") , sizeof(otp_data_in_db));
// 		uts.log.Error(_T("otp_data_len = %d") , otp_data_len);
// 		
// 		goto out;
// 	}

 	ret = do_check_otp(otp_data_in_sensor,otp_data_in_db);

out:
	return SET_ERROR(ret);
}

OtpDataItem *BaseOtp::get_otp_from_uniform_by_type(int type)
{
	OtpDataItem *item;
	OtpDataHeader *hdr = (OtpDataHeader *)uniform;

	for (int idx = 0; idx < hdr->len; idx += sizeof(OtpDataItem)+item->len) 
	{
		item = (OtpDataItem *)&hdr->data[idx];

		if (item->type == type) return item;
	}
	return NULL;
}

int BaseOtp::check_otp_off_lsc(void *args)
{
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;

	int ret = OTPCALI_ERROR_NO;

	uint8_t *lscbuf = (uint8_t*)dlmalloc(SIZE_K);

	struct LSC_PARAM *lsc = &otp_param.lsc_param;

#if 0
	OtpDataItem *item = get_otp_from_uniform_by_type(OtpDataType_LSC);
	if (!item) {
		ret = SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);
		goto out;
	}

	OtpDataLSC *lsc_otp = (OtpDataLSC*)item->data;
	ret = decode_lsc_data(lsc_otp->lsc, otp_lsc_len, lscbuf, SIZE_K);
	if (ret < 0) {goto out;}

	ret = sensor->lsc_writeback(lscbuf, ret);
	if (ret < 0) {goto out;}
#endif

	//ÀëÏßLSC²âÊÔ yb 20171215-------------------------------------------------
	//¶ÁÈ¡Êý¾Ý
	BYTE pLenCReg[240]={0};
	CString strFileName = _T("otp-lsc-buffer.txt"); 
	CString szTemp;
	CStdioFile file;  
	if (!PathFileExists(strFileName))  
	{  
		return OTPCALI_ERROR_SENSOR;  
	}  

	if (!file.Open(strFileName, CFile::modeRead))  
	{  
		return OTPCALI_ERROR_SENSOR;  
	}  

	for(int i=0; i < 240; i++ )
	{
		file.ReadString(szTemp);
		pLenCReg[i] = wcstol(szTemp,NULL,16);
	}
	file.Close();

	//µ¼ÈëÊý¾Ý
	int temp;
	temp=sensor->dev->i2c_read(BIT16_BIT8,0x5000);
	sensor->dev->i2c_write(BIT16_BIT8,0x5000 ,0x20|temp);

	for(int i=0;i<240;i++)
	{
		sensor->dev->i2c_write(BIT16_BIT8,0x5900+i,pLenCReg[i]);
	}
	//--------------------------------------------------------------------------

	Sleep(200); 
	dev->GetBufferInfo(m_bufferInfo);
	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);

	uts.log.Debug(_T("LSC(after cali): RI=%.2f, Delta=%.2f\n"), lsc->riResult.dRI, lsc->riResult.dRIDelta);

	if (check_lsc(lsc) < 0) {
		ret = SET_ERROR(OTPCALI_ERROR_LSCCALI);
		goto out;
	}
out:
	dlfree(lscbuf);
	return ret;
}

int BaseOtp::wb_check_rawinfo_limit(WB_DATA_UCHAR *wb)
{
	const char *pixel[] = {"R", "Gr", "Gb", "B"};

	uint8_t *gray = (uint8_t*)wb;
	int *v_min = otp_param.wb_param.RGrGbB_min;
	int *v_max = otp_param.wb_param.RGrGbB_max;

	for (int i = 0; i < 4; i++, gray++, v_min++, v_max++) {
		if (*gray < *v_min || *gray > *v_max) {
			uts.log.Error(_T("%s[%d] out of limit [%d,%d]"), pixel[i], *gray, *v_min, *v_max);
			return -1;
		}
	}
	return 0;
}

int BaseOtp::wb_get_rawinfo_from_image(WB_DATA_DOUBLE *dwb, WB_DATA_UCHAR *wb)
{
	using namespace Algorithm;

	WB_PARAM *pwb = &otp_param.wb_param;

	SIZE image_size; 
	dev->GetBufferInfo(m_bufferInfo);

	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;
	ROI roi;
	int nOutMode = 0;
	GetWBROI(&otp_param.wb_param, &image_size, &roi);

	WB_DATA_DOUBLE wb_data_d;
	if (!dwb) dwb = &wb_data_d;

	if (!dev->ReadValue(
		eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
		&nOutMode, sizeof(nOutMode)))
	{
		uts.log.Error(_T("GetSensorOutMode error."));
		SET_ERROR(OTPCALI_ERROR_SENSOR);
	}

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		SET_ERROR(OTPCALI_ERROR_SENSOR);
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	UTS::Algorithm::WB::WBCalibration(m_bufferObj.pRaw8Buffer, image_size.cx, image_size.cy, nOutMode,
		roi.x, roi.y, roi.width, roi.height,
		otp_param.SensorBlack_level, (double*)dwb, 
		(uint8_t*)wb);

	uts.log.Debug(_T("WB: R[%d]Gr[%d]Gb[%d]B[%d]"), wb->R,wb->Gr,wb->Gb,wb->B);

	return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::check_uniform_otp_data(OtpDataHeader *hdr)
{
	int ret = OTPCALI_ERROR_NO;
	OtpDataItem *item;
	OtpOffCheck *off = (OtpOffCheck*)&otp_param.otp_off_param;

	off->result = RESULT_PASS;
	for (int idx = 0; idx < hdr->len; idx += sizeof(OtpDataItem)+item->len) {
		item = (OtpDataItem *)&hdr->data[idx];

		int *result;
		switch (item->type) {
		case OtpDataType_MInfo:
			{
				result = &off->minfo_result;
				*result = RESULT_PASS;
				OtpDataMInfo *minfo = (OtpDataMInfo*)item->data;
				memset(otp_data_in_db, 0, otp_data_len);
				get_minfo_from_db(otp_data_in_db);
				if (minfo->tm) {
					if ((minfo->tm[0] < 10 || minfo->tm[0] > 20) // year in [2010, 2020]
						|| (minfo->tm[1] < 1 || minfo->tm[1] > 12)
						|| (minfo->tm[2] < 1 || minfo->tm[2] > 31)) {
							uts.log.Error(_T("OffChk: Time in ModuleInfo out of spec!"));
							*result = RESULT_FAIL;
					}
					if (memcmp(&otp_data_in_db[minfo->compare_start], 
						&minfo->raw[minfo->compare_start], 
						minfo->tm - minfo->raw - minfo->compare_start)) {
							uts.log.Error(_T("OffChk: ModuleInfo is not accordant!"));
							*result = RESULT_FAIL;
					}
					uint8_t *post = minfo->tm + minfo->tm_len;
					if (memcmp(otp_data_in_db+(post-minfo->raw), post, 
						minfo->raw_len-(post-minfo->raw)
						-item->sum_len-minfo->compare_start)) {
							uts.log.Error(_T("OffChk: ModuleInfo is not accordant!"));
							*result = RESULT_FAIL;
					}
				} else {
					if (memcmp(&otp_data_in_db[minfo->compare_start], 
						&minfo->raw[minfo->compare_start], 
						minfo->raw_len-minfo->compare_start)) {
							uts.log.Error(_T("OffChk: ModuleInfo is not accordant!"));
							*result = RESULT_FAIL;
					}
				}
			}
			break;
		case OtpDataType_AF:
			{
				result = &off->af_result;
				*result = RESULT_PASS;
				OtpDataAF *af = (OtpDataAF*)item->data;
				if (af->af.start > 0 && (af->af.start < off->spec_af_sta_l || af->af.start > off->spec_af_sta_h)) {
					uts.log.Error(_T("OffChk: AF start[%d] dac out of spec!"), af->af.start);
					*result = RESULT_FAIL;
				}
				if (af->af.inf > 0 && (af->af.inf < off->spec_af_inf_l || af->af.inf > off->spec_af_inf_h)) {
					uts.log.Error(_T("OffChk: AF inf[%d] dac out of spec!"), af->af.inf);
					*result = RESULT_FAIL;
				}
				if (af->af.mup > 0 && (af->af.mup < off->spec_af_mac_l || af->af.mup > off->spec_af_mac_h)) {
					uts.log.Error(_T("OffChk: AF mup[%d] dac out of spec!"), af->af.mup);
					*result = RESULT_FAIL;
				}
			}
			break;
		case OtpDataType_WB:
			{
				result = &off->wb_result;
				*result = RESULT_PASS;
				OtpDataWB *wb = (OtpDataWB*)item->data;
				if (wb->gain.RGain != 0 || wb->gain.BGain != 0 || wb->gain.GGain != 0) {
					break;
				}
				WB_RATIO *ratio = &wb->ratio;
				if (ratio->dr_g < off->spec_r_g_l || ratio->dr_g > off->spec_r_g_h) {
					uts.log.Error(_T("OffChk: WB rg ratio[%.3lf] out of spec!"), ratio->dr_g);
					*result = RESULT_FAIL;
				}
				if (ratio->db_g < off->spec_b_g_l || ratio->db_g > off->spec_b_g_h) {
					uts.log.Error(_T("OffChk: WB bg ratio[%.3lf] out of spec!"), ratio->db_g);
					*result = RESULT_FAIL;
				}
				if (wb->has_golden) {
					ratio = &wb->ratio_g;
					if (ratio->dr_g < off->spec_r_g_l || ratio->dr_g > off->spec_r_g_h) {
						uts.log.Error(_T("OffChk: WB golden rg ratio[%.3lf] out of spec!"), ratio->dr_g);
						*result = RESULT_FAIL;
					}
					if (ratio->db_g < off->spec_b_g_l || ratio->db_g > off->spec_b_g_h) {
						uts.log.Error(_T("OffChk: WB golden bg ratio[%.3lf] out of spec!"), ratio->db_g);
						*result = RESULT_FAIL;
					}
				}
			}
			break;
		case OtpDataType_LSC:
			{
				result = &off->lsc_result;
				*result = RESULT_PASS;
				OtpDataLSC *lsc = (OtpDataLSC*)item->data;
				if (is_mem_val(lsc->raw, 0, lsc->raw_len)) {
					uts.log.Error(_T("OffChk: LSC data is all [0]!\n"));
					*result = RESULT_FAIL;
				}
				if (is_mem_val(lsc->raw, 0xFF, lsc->raw_len)) {
					uts.log.Error(_T("OffChk: LSC data is all [FF]!\n"));
					*result = RESULT_FAIL;
				}
			}
			break;
		case OtpDataType_PDAF:
			result = &off->pdaf_result;
			*result = RESULT_PASS;
			/*if (CPdaf::GetInstance().setup_setting_file(PDAF_SETTING_FILE) < 0) {
				uts.log.Error(_T("OffChk: PDAF setup setting file error!"));
				*result = RESULT_FAIL;
			}
			if (pd_cali_verify(item->data) != PD_ERR_OK) {
				uts.log.Error(_T("OffChk: PDAF check error!"));
				*result = RESULT_FAIL;
			}*/
			break;
		}
		if (item->sum_len > 0 && item->type != OtpDataType_MInfo) {
			uint32_t sum = CheckSum(item->sum_sta, item->sum_calc_len) % ((1<<(8*item->sum_len))-1) + 1;
			if (sum != item->sum) {
				uts.log.Error(_T("OffChk: check sum error, type[%d], sum in sensor[%08x], should be[%08x]!"), 
					item->type, item->sum, sum);
				*result = RESULT_FAIL;
			}
		}
		if (*result == RESULT_FAIL) {
			off->result = RESULT_FAIL;
			ret = OTPCALI_ERROR_CHKERR;
		}
	}

	return SET_ERROR(ret);
}
int BaseOtp::otp_off_chk(void *args)
{
	int ret = OTPCALI_ERROR_NO;

	if (otp_param.group_for_use == -1) {
		ret = get_otp_group();
		if (ret < 0 && ret != -OTPCALI_ERROR_OPTNOTSUPPORT) {
			goto out;
		}
		if (ret >= 0) otp_param.group_for_use = ret;
		uts.log.Debug(_T("otp_off_chk: auto get group_for_use = %d"), otp_param.group_for_use);
	}

	ret = get_otp_data_from_sensor(otp_data_in_sensor);
	if (ret < 0) {goto out;}

	memset(uniform, 0, SIZE_K*SIZE_K);
	ret = get_uniform_otp_data(otp_data_in_sensor, uniform, SIZE_K*SIZE_K);
	if (ret < 0) {goto out;}

	ret = check_uniform_otp_data((OtpDataHeader *)uniform);
	if (ret < 0) {goto out;}

out:
	return ret;
}

int BaseOtp::GetAFData(AF_INT *af)
{
	uint8_t tmp[2];
	using namespace DDM;
	const int af_types[] = { OTPDB_OTPTYPE_AFSTART, OTPDB_OTPTYPE_AFINF, OTPDB_OTPTYPE_AFMUP };

	int *dac = (int *)af;
	for (int i = 0; i < ARRAY_SIZE(af_types); i++) {
		int ret = otpDB->GetOtpByType(mid, af_types[i], (char *)tmp, sizeof(tmp));
		if (ret < 0) {
			*dac++ = -1;
			continue;
		}
		*dac++ = get_le_val(tmp, sizeof(tmp));
	}
	return 0;
}
void BaseOtp::do_save_buff(const char * save_path,void *buffer,int buffer_len,int buffer_type)
{
	uint8_t* buffer_uint8_t;
	uint16_t* buffer_uint16_t;
	int16_t* buffer_int16_t;
	int *buffer_int;

	USES_CONVERSION;

	CString str_Temp,str_Content;
	//char str_Path [1024];

	//GetCurrentDirectory(1024,str_Path);
	//CreateDirectoryEx(A2T(str_Path),_T("pdaf_data\\"),NULL);
	//str_Temp.Format(_T("%s\\pdaf_data\\"),A2T(str_Path));
	//SetCurrentDirectory(str_Temp);
	//str_Path.Format(_T("%s%s"),_T("pdaf_data\\",uts.info.strSN));
	//CreateDirectoryEx(_T("pdaf_data\\"),uts.info.strSN,NULL);
	//SetCurrentDirectory(str_Path);


	if(buffer_type==0) {
		buffer_uint8_t = (uint8_t*) buffer;
		for(int i=0;i<buffer_len;i++){
			str_Temp.Format(_T("0x%02x\n"),buffer_uint8_t[i]);
			str_Content += str_Temp;
		}
	}
	if(buffer_type==1) {
		buffer_uint16_t = (uint16_t*) buffer;
		for(int i=0;i<buffer_len;i++){
			str_Temp.Format(_T("0x%04x\n"),buffer_uint16_t[i]);
			str_Content += str_Temp;
		}
	}

	if(buffer_type==2) {
		buffer_int = (int*) buffer;
		for(int i=0;i<buffer_len;i++){
			str_Temp.Format(_T("0x%04x\n"),buffer_int[i]);
			str_Content += str_Temp;
		}
	}
	if(buffer_type==3) {
		buffer_int = (int*) buffer;
		for(int i=0;i<buffer_len;i++){
			str_Temp.Format(_T("%d\n"),buffer_int[i]);
			str_Content += str_Temp;
		}
	}
	if(buffer_type==4) {
		buffer_int16_t = (int16_t*) buffer;
		for(int i=0;i<buffer_len;i++){
			str_Temp.Format(_T("%d\n"),buffer_int16_t[i]);
			str_Content += str_Temp;
		}
	}
	FILE *fp = fopen(save_path,"w");

	if(fp != NULL){ 
		fprintf(fp,T2A(str_Content));
		fclose (fp);
		uts.log.Info(_T("buffer save ok in %s"),CString(save_path));
	}
	else{
		uts.log.Error(_T("buffer save error in %s"),CString(save_path));
	}

}
//---------------------add by wx
int BaseOtp::get_wb_from_raw_data(WB_DATA_USHORT *wb, WB_DATA_USHORT *wb_g,WB_LIGHT_USHORT *wb_light,int WbType)
{

	WB_DATA_LIGHT_USHORT wb_tmp;
	int ret=0;
	///type 1 for 4000k awb,2 for 3100k awb ,other for 5100
	if(WbType==1){  
		uts.log.Debug(_T("get 4000K AWB"));
		int ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB1, &wb_tmp, sizeof(wb_tmp));   
	}
	else if(WbType==2){ 
		uts.log.Debug(_T("get 3100K AWB"));
		int ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB2, &wb_tmp, sizeof(wb_tmp));   
	}
	else{
		uts.log.Debug(_T("get 5100K AWB"));
		int ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB, &wb_tmp, sizeof(wb_tmp));
	}
	if (ret < 0) return SET_ERROR(ret);
	//TODO to check awb range
//check awb range
	WB_DATA_UCHAR w1,w2;
	//otp before
	if (CheckWBLimit(&w1) < 0 || CheckWBLimit(&w2) < 0)
		return SET_ERROR(OTPCALI_ERROR_WBCALI);

	memcpy(wb, &wb_tmp.wb, sizeof(WB_DATA_USHORT));
	memcpy(wb_g, &wb_tmp.wb_g, sizeof(WB_DATA_USHORT));
	memcpy(wb_light, &wb_tmp.wb_light, sizeof(WB_LIGHT_USHORT));

	return SET_ERROR(OTPCALI_ERROR_NO);
}

#pragma endregion

