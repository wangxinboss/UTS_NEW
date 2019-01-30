#include "StdAfx.h"
#include "OtpCali_S5K3P9SX_MF83Y.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_S5K3P9SX_MF83Y {
	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K3P9SX_MF83Y::OtpCali_S5K3P9SX_MF83Y(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		//otp_spc_len = QULCOMM_SPC_LEN;
		//otp_dcc_len = QULCOMM_DCC_LEN;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MF83Y::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MF83Y::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MF83Y::LscCali(void *args)
	{
		return do_qulcomm_lsc_awb_cali();
	}
	//-------------------------------------------------------------------------------------------------

	int OtpCali_S5K3P9SX_MF83Y::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MF83Y::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;
		
		m->flag = 0x01;
		m->version = 0x01;
        m->moduleID = 0x02;
		m->lensID = 0x01;
		m->vcmID = 0x00;
		m->vcmDriverID = 0x00;
		m->colorTemp = 0x01;
		m->mirFlipFlag = 0x00;

		memset(&m->qrCode,0x00,4);
		
		SYSTEMTIME stime;
		time_t time;

		if (otpDB->get_otp_data_lock_time(mid, &time) < 0)
		{
			uts.log.Debug(_T("get_otp_data_lock_time NG!"));
			GetLocalTime(&stime);
			m->year  = stime.wYear % 100;
			m->month = (uint8_t)stime.wMonth;
			m->day   = (uint8_t)stime.wDay;
		}
		else
		{
			uts.log.Debug(_T("get_otp_data_lock_time pass!"));
			struct tm today;
			_localtime64_s( &today, &time );

			m->year   = today.tm_year % 100;
			m->month  = (uint8_t)today.tm_mon + 1;
			m->day    = (uint8_t)today.tm_mday;
		}

		m->checkSum = (CheckSum(&m->version, sizeof(MINFO)-2)%255);

		return sizeof(MINFO);
	}

	int OtpCali_S5K3P9SX_MF83Y::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		//module info
		uts.log.Debug(_T("Get Minfo"));
		int ret = get_minfo_from_db(&otp->minfo);
		if (ret < 0) return ret;

		//AWB
		uts.log.Debug(_T("Get WBinfo"));
		WB_DATA_UCHAR wb[2];		
		ret = get_wb_from_raw_data(&wb[0], &wb[1]);
		if (ret < 0) 
		{ 
			uts.log.Error(_T("Get AWB Data Error"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}

		WB_RATIO ratio;
		get_wb_ratio(&wb[0], &ratio, 1023);

		uts.log.Info(_T("r_gr=0x%04x,b_gb=0x%04x Gb_Gr =0x%04x"),ratio.r_gr,ratio.b_gb,ratio.gb_gr);

		put_le_val(ratio.r_gr, otp->awb.rg, sizeof(otp->awb.rg));
		put_le_val(ratio.b_gb, otp->awb.bg, sizeof(otp->awb.bg));
		put_le_val(ratio.gb_gr, otp->awb.gbgr, sizeof(otp->awb.gbgr));
	
		//golden数据
		put_le_val(557, otp->awb.rg_golden, sizeof(otp->awb.rg_golden));
		put_le_val(723, otp->awb.bg_golden, sizeof(otp->awb.bg_golden));
		put_le_val(1024, otp->awb.gbgr_golden, sizeof(otp->awb.gbgr_golden));

		otp->awb.checkSum = (CheckSum(&otp->awb.rg, 12)%255);
		memset(&otp->awb.reserved,0x00,2);

		//LSC
		//////////////////////////////////////////////////
		//          (need check data format)
		/////////////////////////////////////////////////
		uts.log.Debug(_T("Get LSCinfo"));
		uint8_t *lscDB = new uint8_t[QULCOMM_LSC_LEN];
		ret = get_lsc_from_raw_data(lscDB, QULCOMM_LSC_LEN);
		if (ret < 0) 
		{
			uts.log.Error(_T("Get LSC Data Error"));
			return SET_ERROR(OTPCALI_ERROR_LSCCALI);
		}
	    otp->lsc.flag = 0x01;

		for(int i=0;i<QULCOMM_LSC_LEN/4;i+=2)
		{
			otp->lsc.lsc[221*2*0 +i]     = lscDB[221*2*1 + i];     //R_L
			otp->lsc.lsc[221*2*0 +i+1]   = lscDB[221*2*1 + i + 1]; //R_H
			otp->lsc.lsc[221*2*1 +i]     = lscDB[221*2*0 + i];     //Gr_L
			otp->lsc.lsc[221*2*1 +i+1]   = lscDB[221*2*0 + i + 1]; //Gr_H
			otp->lsc.lsc[221*2*2 +i]     = lscDB[221*2*2 + i];     //GB_L
			otp->lsc.lsc[221*2*2 +i+1]   = lscDB[221*2*2 + i + 1]; //GB_H
			otp->lsc.lsc[221*2*3 +i]     = lscDB[221*2*3 + i];     //B_L
			otp->lsc.lsc[221*2*3 +i+1]   = lscDB[221*2*3 + i + 1]; //B_H
		}

		otp->lsc.checkSum = (CheckSum(&otp->lsc.lsc, 1768)%255);
		RELEASE_ARRAY(lscDB);		

		return sizeof(OTPData);
	}

	int OtpCali_S5K3P9SX_MF83Y::do_check_otp(void *args)
	{
		OTPData *otp = (OTPData*)args;
		//ModuleInfo
		//检查

		if (otp->minfo.flag != 0x01 || otp->minfo.version != 0x01 || otp->minfo.moduleID != 0x02 
			|| otp->minfo.lensID != 0x01 || otp->minfo.vcmID != 0x00 || otp->minfo.vcmDriverID != 0x00 
			|| otp->minfo.colorTemp != 0x01 || otp->minfo.mirFlipFlag != 0x00)
		{
			uts.log.Error(_T("ModuleInfo Data error!!!"));
			return OTPCALI_ERROR_NODATA;
		}

		//检查year month day
		if (otp->minfo.year <10 || otp->minfo.year >25
			||otp->minfo.month < 1 || otp->minfo.month > 13 
			|| otp->minfo.day < 1 || otp->minfo.day > 31)
		{
			uts.log.Error(_T("year month day error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//检查CheckSum
		uint8_t mInfo_checkSum = (CheckSum(&otp->minfo.version, sizeof(MINFO)-2)%255);
		if (otp->minfo.checkSum != mInfo_checkSum)
		{
			uts.log.Error(_T("minfo CheckSum  error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//AWB
		if (otp->awb.flag != 0x01)
		{
			uts.log.Error(_T("AWB Flag error!!!"));
			return OTPCALI_ERROR_WBCALI;
		}

		//检查WB的Data
		int RGValue = get_le_val(otp->awb.rg, 2);
		int BGValue = get_le_val(otp->awb.bg, 2);
		int GGValue = get_le_val(otp->awb.gbgr, 2);
		int RgGoldenValue = get_le_val(otp->awb.rg_golden,2);
		int BgGoldenValue = get_le_val(otp->awb.bg_golden,2);
		int GgGoldenValue = get_le_val(otp->awb.gbgr_golden,2);

		if (RGValue > 612 || RGValue < 502 
			|| BGValue >795 || BGValue < 651
			|| GGValue > 1125 || GGValue < 950)
		{
			uts.log.Error(_T("wb data  error!!!"));
			return OTPCALI_ERROR_WBCALI;
		}

		//检查WB的Golden
		if (RgGoldenValue != 557 || BgGoldenValue != 723 || GgGoldenValue != 1024)
		{
			uts.log.Error(_T("wb Golden data  error!!!"));
			return OTPCALI_ERROR_WBCALI;
		}
		//检查Wb的CheckSum
		uint8_t	wb_checkSum = (CheckSum(&otp->awb.rg, 12)%255);
		if (wb_checkSum != otp->awb.checkSum)
		{
			uts.log.Error(_T("AWB CheckSum  error!!!"));
			return OTPCALI_ERROR_WBCALI;
		}

		//LSC
		if (otp->lsc.flag != 0x01)
		{
			uts.log.Error(_T("lsc Flag error!!!"));
			return OTPCALI_ERROR_LSCCALI;
		}
		//检查LSC的CheckSUM
		uint8_t lsc_checkSum = (CheckSum(&otp->lsc.lsc, QULCOMM_LSC_LEN)) % 0xff ;
		if (otp->lsc.checkSum != lsc_checkSum)
		{
			uts.log.Error(_T("LSC CheckSum  error!!!"));
			return OTPCALI_ERROR_LSCCALI;
		}

		return OTPCALI_ERROR_NO;
	}
}
