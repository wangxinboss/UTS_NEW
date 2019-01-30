#include "StdAfx.h"
#include "OtpCali_S5K3L8_MA83F.h"
#include "VCM.h"
#include "algorithm.h"
#include "Otp/Ref/MTK_PDAF/PDAF_MTK.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")


namespace OtpCali_S5K3L8_MA83F {
	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K3L8_MA83F::OtpCali_S5K3L8_MA83F(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = MTK_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA83F::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA83F::get_otp_data_from_sensor(void *args)
	{
		uint8_t *otp = (uint8_t*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA83F::LscCali(void *args)
	{
		return do_mtk_lsc_cali();
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA83F::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->flag = 0x01;	//0x01
		m->firstPixel = 0x02;
		m->MirFlipFlag = 0x00;
		m->width_Low = 0x70;
		m->width_High = 0x10;
		m->Height_Low = 0x30;
		m->Height_High = 0x0c;
		m->LscVersionLow = 0x0E;
		m->LscVersionHigh = 0x00;
		m->PDAFVersionLow = 0xff;
		m->PDAFVersionHigh = 0x03;

		m->ModuleID = 0x42;	//0x42:BYD
		m->LensID = 0x00;


		SYSTEMTIME stime;
		time_t time;
		if (otpDB->get_otp_data_lock_time(mid, &time) < 0)
		{
			uts.log.Debug(_T("get_otp_data_lock_time NG!"));
			GetLocalTime(&stime);
			m->Year  = stime.wYear % 100;
			m->Month = (uint8_t)stime.wMonth;
			m->Day   = (uint8_t)stime.wDay;
		}else
		{
			uts.log.Debug(_T("get_otp_data_lock_time pass!"));
			struct tm today;
			_localtime64_s( &today, &time );

			m->Year = today.tm_year % 100;
			m->Month = (uint8_t)today.tm_mon + 1;
			m->Day = (uint8_t)today.tm_mday;
		}

		return sizeof(MINFO);
	}

	int OtpCali_S5K3L8_MA83F::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;
		WB_PARAM *pwb = &otp_param.wb_param;
		USES_CONVERSION;

		//module info
		int ret = get_minfo_from_db(&otp->minfo);

		//WB
		otp->wb.version = 0x01;
		otp->wb.enFlag = 0x03;

		WB_DATA_UCHAR wb[2];		
		ret = get_wb_from_raw_data(&wb[0], &wb[1]);
		if (ret < 0) 
		{ 
			uts.log.Error(_T("Get AWB Data Error"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}

		uts.log.Info(_T("r=%d,gr=%d,gb=%d,b=%d"),
			wb[0].R,wb[0].Gr,wb[0].Gb,wb[0].B);

		otp->wb.r    = wb[0].R;
		otp->wb.gr   = wb[0].Gr;
		otp->wb.gb   = wb[0].Gb;
		otp->wb.b    = wb[0].B;
        otp->wb.r_g  = 73;
		otp->wb.gr_g = 152;
		otp->wb.gb_g = 153;
		otp->wb.b_g  = 94;

		//LSC
		ret = get_lsc_from_raw_data(otp->lsc.lscbuffer, MTK_LSC_LEN);
		if (ret < 0) 
		{
			uts.log.Error(_T("Get LSC Data Error"));
			return SET_ERROR(OTPCALI_ERROR_LSCCALI);
		}

		otp->lsc.checkSum = (CheckSum(&otp->lsc.lscbuffer, MTK_LSC_LEN)) % 0xff;
		return sizeof(OTPData);
	}

	int OtpCali_S5K3L8_MA83F::do_check_otp(void *args)
	{
		OTPData *otp = (OTPData*)args;

		//检查ModuleInfo
		if (otp->minfo.flag != 0x01 || otp->minfo.firstPixel != 0x02 || otp->minfo.firstPixel != 0x00 
			|| otp->minfo.width_Low != 0x70 || otp->minfo.width_High != 0x10 
			|| otp->minfo.Height_Low != 0x30 || otp->minfo.Height_High != 0x0c 
			|| otp->minfo.LscVersionLow != 0x0E || otp->minfo.LscVersionHigh != 0x00 
			|| otp->minfo.PDAFVersionLow != 0xff || otp->minfo.PDAFVersionHigh != 0x03 
			|| otp->minfo.ModuleID != 0x42 || otp->minfo.LensID != 0x00)
		{
			uts.log.Info(_T("ModuleInfo Data error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//检查year month day
		if (otp->minfo.Year <10 || otp->minfo.Year >25
			||otp->minfo.Month < 1 || otp->minfo.Month > 13 
			|| otp->minfo.Day < 1 || otp->minfo.Day > 31)
		{
			uts.log.Info(_T("year month day error!!!"));
			return OTPCALI_ERROR_NODATA;
		}

		//检查WB Version和enFlag
		if (otp->wb.version != 0x01 || otp->wb.enFlag != 0x03)
		{
			uts.log.Info(_T("WB Version and enFlag data error!!!"));
			return OTPCALI_ERROR_NODATA;
		}

		//检查WB的Data
		if (otp->wb.r < 66 || otp->wb.r >80
			|| otp->wb.b <85 ||otp->wb.b >103
			|| otp->wb.gr <137 ||otp->wb.gr >167
			|| otp->wb.gb <138 ||otp->wb.gb >168)
		{
			uts.log.Info(_T("wb data  error!!!"));
			return OTPCALI_ERROR_NODATA;
		}

		//检查WB的Golden
		if (otp->wb.r_g != 73 || otp->wb.gr_g != 152
			|| otp->wb.gb_g != 153 || otp->wb.b_g != 94)
		{
			uts.log.Info(_T("wb Golden data  error!!!"));
			return OTPCALI_ERROR_NODATA;
		}

		//检查LSC的CheckSUM
		uint8_t lsc_checkSum = (CheckSum(&otp->lsc.lscbuffer, MTK_LSC_LEN)) % 0xff + 1;
		if (otp->lsc.checkSum != lsc_checkSum)
		{
			uts.log.Info(_T("LSC CheckSum  error!!!"));
			return OTPCALI_ERROR_NODATA;
		}

 		return OTPCALI_ERROR_NO;
	}
}
