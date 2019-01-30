#include "StdAfx.h"
#include "OtpCali_IMX350_CA225.h"
#include "VCM.h"
#include "algorithm.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_IMX350_CA225 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_IMX350_CA225::OtpCali_IMX350_CA225(BaseDevice *dev) : SonyOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = MTK_LSC_LEN_CA225;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX350_CA225::do_prog_otp()
	{
		otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_MODULE,
			(char *)otp_data_in_db, sizeof(MINFO));

		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX350_CA225::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX350_CA225::LscCali(void *args)
	{
		return do_mtk_lsc_cali_MTK6797_MONO();
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX350_CA225::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->flag = 0x01;
		
		m->caliVer[0] = 0xFF;
		m->caliVer[1] = 0x00;
		m->caliVer[2] = 0x0B;
		m->caliVer[3] = 0x01;

		CString sn = m_szSN;
		sn = sn.Right(2);

		memcpy(&m->sn, T2A(sn), 2);


		m->awbafinfo[0] = 0x01;
		m->awbafinfo[1] = 0x0C;

		return sizeof(MINFO);
	}
	int OtpCali_IMX350_CA225::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;
		USES_CONVERSION;

		//module info
		int ret = get_minfo_from_db(&otp->minfo);

		//WB
		memset(&otp->wb,0,sizeof(otp->wb));

		//AF
		int inf = 0, marcro = 0;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0) return ret;
		put_le_val(marcro, otp->af.mup, sizeof(otp->af.mup));
		put_le_val(inf, otp->af.inf, sizeof(otp->af.inf));

		//LSC
		put_le_val(MTK_LSC_LEN_CA225, otp->lsc.lsclength, sizeof(otp->lsc.lsclength));
		ret = get_lsc_from_raw_data(otp->lsc.lscbuffer, sizeof(otp->lsc.lscbuffer));
		if (ret < 0) return ret;

		//pdaf
		memset(&otp->pdaf,0,sizeof(otp->pdaf));

		//Hall
		memset(&otp->hall,0,sizeof(otp->hall));

		//AF_Distance
		int dis_inf = 5000 , dis_mup = 100;
		put_le_val(dis_mup, otp->af_dis.distance_mup, sizeof(otp->af_dis.distance_mup));
		put_le_val(dis_inf, otp->af_dis.distance_inf, sizeof(otp->af_dis.distance_inf));
		
		//AF_Temperature
		memset(&otp->af_temperature,0,sizeof(otp->af_temperature));

		//Reserve
		memset(&otp->reserve,0,sizeof(otp->reserve));

		//BackecdInfo
		otp->mtkinfo.mtkver = 0x14;
		otp->mtkinfo.mtkflag[0] = 0x4D;
		otp->mtkinfo.mtkflag[1] = 0x54;
		otp->mtkinfo.mtkflag[2] = 0x4B;

		SYSTEMTIME stime;
		time_t time;
		if (otpDB->GetOtpBurnTime(mid, &time) < 0)
		{
			GetLocalTime(&stime);
			otp->mtkinfo.year  = stime.wYear % 100;
			otp->mtkinfo.month = (uint8_t)stime.wMonth;
			otp->mtkinfo.day   = (uint8_t)stime.wDay;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			otp->mtkinfo.year   = today.tm_year % 100;
			otp->mtkinfo.month  = (uint8_t)today.tm_mon + 1;
			otp->mtkinfo.day    = (uint8_t)today.tm_mday;
		}

		int totalchecksum = CheckSum(&otp, 0x0FA2) % 0xFFFFFFFF;

		put_le_val(totalchecksum, otp->checksum, sizeof(otp->checksum));

		return sizeof(OTPData);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX350_CA225::get_uniform_otp_data(void *in, void *out, int maxlen)
	{
		OTPData *otp = (OTPData*)in;
		OtpDataHeader *hdr = (OtpDataHeader*)out;
		hdr->len = 0;

		return sizeof(OtpDataHeader) + hdr->len;
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX350_CA225::get_otp_group(void)
	{
		return 0;
	}
	//-------------------------------------------------------------------------------------------------


	
}
