#include "StdAfx.h"
#include "OtpCali_S5K5E2_MD701.h"
#include "VCM.h"
#include "algorithm.h"


#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_S5K5E2_MD701 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K5E2_MD701::OtpCali_S5K5E2_MD701(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_OTP;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = LSC_LEN_MD701;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K5E2_MD701::do_prog_otp()
	{
		//info
		sensor->do_prog_otp(2,0x0A04,otp_data_in_db + 0,64);

		//WB
		sensor->do_prog_otp(4,0x0A04,otp_data_in_db + 64,64);

		//LSC
		sensor->do_prog_otp( 9,0x0A04,otp_data_in_db +128 + 0,64);
		sensor->do_prog_otp(10,0x0A04,otp_data_in_db +128 + 64,64);
		sensor->do_prog_otp(11,0x0A04,otp_data_in_db +128 + 128,64);
		sensor->do_prog_otp(12,0x0A04,otp_data_in_db +128 + 192,64);
		sensor->do_prog_otp(13,0x0A04,otp_data_in_db +128 + 256,64);
		sensor->do_prog_otp(14,0x0A04,otp_data_in_db +128 + 320,40);
		sensor->do_prog_otp(15,0x0A04,otp_data_in_db +128 + 360,64);
		
		
		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K5E2_MD701::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		char *buf = new char[64] ;
		sensor->do_read_otp(2,0x0A04,buf + 0,64);
		memcpy(&otp->minfo,buf,64);
		sensor->do_read_otp(4,0x0A04,buf + 0,64);
		memcpy(&otp->wb,buf,64);

		RELEASE_ARRAY(buf);

		char *lscbuf = new char[sizeof(otp->lsc)] ;
		sensor->do_read_otp(9,0x0A04,lscbuf + 0,64);
		sensor->do_read_otp(10,0x0A04,lscbuf + 64,64);
		sensor->do_read_otp(11,0x0A04,lscbuf + 128,64);
		sensor->do_read_otp(12,0x0A04,lscbuf + 192,64);
		sensor->do_read_otp(13,0x0A04,lscbuf + 256,64);
		sensor->do_read_otp(14,0x0A04,lscbuf + 320,40);
		sensor->do_read_otp(15,0x0A04,lscbuf + 360,64);
		
		memcpy(&otp->lsc,lscbuf,sizeof(otp->lsc));

		RELEASE_ARRAY(lscbuf);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K5E2_MD701::LscCali(void *args)
	{
		return SamsungLscCali();
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K5E2_MD701::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->flag = 0x40;
		m->storagelocation = 0x00;

		CString sn = m_szSN;
		sn = sn.Right(10);
		memcpy(&m->sn, T2A(sn), 10);
		
		m->venderid    = 0x07;
		m->sensorid    = 0x24;
		m->lensid      = 0x33;
		m->vcmid       = 0x00;
		m->drivericid  = 0x00;
		m->irid        = 0x00;
		m->gyroid      = 0x00;
		m->colortempid = 0x01;

		m->fpcver      = 0x01;
		m->caliver     = 0x01;
		m->lensver     = 0x01;
		m->oisver      = 0x00;

		memcpy(&m->sensorfuse, T2A(uts.info.strSensorId), 16);
		
		m->camearastage = 0x21;
		m->factory      = 0x01;
		m->line         = 0x01;
		
		SYSTEMTIME stime;
		time_t time;
		otpDB->get_otp_data_lock_time(mid, &time);
		if (time < 0)
		{
			GetLocalTime(&stime);
			m->year  = stime.wYear % 100;
			m->month = (uint8_t)stime.wMonth;
			m->day   = (uint8_t)stime.wDay;
			m->hour  = (uint8_t)stime.wHour;
			m->minute= (uint8_t)stime.wMinute;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			m->year  = today.tm_year % 100;
			m->month = (uint8_t)today.tm_mon + 1;
			m->day   = (uint8_t)today.tm_mday;
			m->hour  = (uint8_t)today.tm_hour;
			m->minute= (uint8_t)today.tm_min;
		}

		memset(&m->reserved,0,14);

		int sum = CheckSum(&m->flag, 62);
		put_be_val(sum % 0xFFFF , m->checksum, sizeof(m->checksum));

		return sizeof(MINFO);
	}
	int OtpCali_S5K5E2_MD701::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		//module info
		int ret = get_minfo_from_db(&otp->minfo);

		//WB
		WB_DATA_USHORT wb[2];
		ret = otpDB->GetOtpByType(mid, 3, (char *)& wb[0], 8);
		if (ret < 0) { return ret;}

		//Golden
// 		ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
// 			3, (char *)& wb[1], 8);
// 		if (ret < 0) return ret;

		wb[1].R = 0;
		wb[1].Gr = 0;
		wb[1].Gb = 0;
		wb[1].B = 0;

		otp->wb.flag = 0x40;

		ret = otpDB->GetOtpByType(mid, 14,(char *)&otp->wb.oc, 16);
		if (ret < 0) return ret;

		otp->wb.lightsource_ev = 2696;
		otp->wb.lightsource_u  = 19960;
		otp->wb.lightsource_v  = 48360;

		otp->wb.r  = wb[0].R;
		otp->wb.gr = wb[0].Gr;
		otp->wb.gb = wb[0].Gb;
		otp->wb.b  = wb[0].B;

		otp->wb.rg = int((16384.0*(wb[0].R- 64)/((wb[0].Gr + wb[0].Gb)/2.0 - 64))+0.5);
		otp->wb.bg = int((16384.0*(wb[0].B- 64)/((wb[0].Gr + wb[0].Gb)/2.0 - 64))+0.5);
		otp->wb.grgb = int((16384.0*wb[0].Gr / wb[0].Gb) + 0.5);

		otp->wb.r_g  = wb[1].R;
		otp->wb.gr_g = wb[1].Gr;
		otp->wb.gb_g = wb[1].Gb;
		otp->wb.b_g  = wb[1].B;

		//otp->wb.rg_g = int((16384.0*(wb[1].R- 64)/((wb[1].Gr + wb[1].Gb)/2.0 - 64))+0.5);
		//otp->wb.bg_g = int((16384.0*(wb[1].B- 64)/((wb[1].Gr + wb[1].Gb)/2.0 - 64))+0.5);
		//otp->wb.grgb_g = int((16384.0*wb[1].Gr / wb[1].Gb) + 0.5);
		otp->wb.rg_g = 0;
		otp->wb.bg_g = 0;
		otp->wb.grgb_g = 0;

		memset(&otp->wb.reserved,0,11);

		int sum = CheckSum(&otp->wb.flag, 62);
		put_be_val(sum % 0xFFFF , otp->wb.checksum, sizeof(otp->wb.checksum));

		//LSC
		ret = get_lsc_from_raw_data(otp->lsc.lscbuffer, sizeof(otp->lsc.lscbuffer));
		if (ret < 0) return ret;

		memset(&otp->lsc.reserved,0,5);
		otp->lsc.flag = 0x40;
		sum = CheckSum(&otp->lsc.lscbuffer, 422) % 0xFFFF + 1;
		put_be_val(sum, otp->lsc.checksum, sizeof(otp->lsc.checksum));


		return sizeof(OTPData);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K5E2_MD701::get_uniform_otp_data(void *in, void *out, int maxlen)
	{
		OTPData *otp = (OTPData*)in;
		OtpDataHeader *hdr = (OtpDataHeader*)out;
		hdr->len = 0;
		/*
		for (int i = 0; i < OtpDataType_Num; i++) {
			OtpDataItem *item = (OtpDataItem *)&hdr->data[hdr->len];
			item->type = i;
			item->len = 0;

			switch (item->type) {
			case OtpDataType_MInfo:
				{
					OtpDataMInfo *minfo = (OtpDataMInfo*)item->data;
					minfo->tm = minfo->raw + offsetof(MINFO, y);
					minfo->tm_len = 3;
					minfo->raw_len = sizeof(otp->minfo);
					memcpy(minfo->raw, &otp->minfo, minfo->raw_len);
					item->len = sizeof(OtpDataMInfo) + minfo->raw_len;
					item->sum_len = sizeof(otp->minfo.sum);
					item->sum = get_be_val(otp->minfo.sum, sizeof(otp->minfo.sum));
					item->sum_sta = minfo->raw + offsetof(MINFO, mid);
					item->sum_calc_len = 19;
				}
				break;
			case OtpDataType_WB:
				{
					OtpDataWB *wb = (OtpDataWB*)item->data;
					wb->has_golden = true;
					wb->ratio.dr_g = get_be_val(otp->wb.r_g, sizeof(otp->wb.r_g))/1024.0;
					wb->ratio.db_g = get_be_val(otp->wb.b_g, sizeof(otp->wb.b_g))/1024.0;
					wb->ratio_g.dr_g = get_be_val(otp->wb.r_g_g, sizeof(otp->wb.r_g_g))/1024.0;
					wb->ratio_g.db_g = get_be_val(otp->wb.b_g_g, sizeof(otp->wb.b_g_g))/1024.0;
					wb->raw_len = sizeof(otp->wb);
					memcpy(wb->raw, &otp->wb, wb->raw_len);
					item->len = sizeof(OtpDataWB) + wb->raw_len;
					item->sum_len = sizeof(otp->wb.sum);
					item->sum = get_be_val(otp->wb.sum, sizeof(otp->wb.sum));
					item->sum_sta = wb->raw + offsetof(WB, r_g);
					item->sum_calc_len = 12;
				}
				break;
			case OtpDataType_AF:
				{
					OtpDataAF *af = (OtpDataAF*)item->data;
					af->af.start = -1;
					af->af.inf = get_be_val(otp->af.inf, sizeof(otp->af.inf));
					af->af.mup = get_be_val(otp->af.mup, sizeof(otp->af.mup));
					af->raw_len = sizeof(otp->af);
					memcpy(af->raw, &otp->af, af->raw_len);
					item->len = sizeof(OtpDataAF) + af->raw_len;
					item->sum_len = sizeof(otp->af.sum);
					item->sum = get_be_val(otp->af.sum, sizeof(otp->af.sum));
					item->sum_sta = af->raw + offsetof(AF, lens);
					item->sum_calc_len = 6;
				}
				break;
			case OtpDataType_LSC:
				{
					OtpDataLSC *lsc = (OtpDataLSC*)item->data;
					lsc->lsc = lsc->raw + offsetof(LSC, lsc);
					lsc->raw_len = sizeof(otp->lsc);
					memcpy(lsc->raw, &otp->lsc, lsc->raw_len);
					item->len = sizeof(OtpDataLSC) + lsc->raw_len;
					
					item->sum = get_be_val(otp->lsc.sum, sizeof(otp->lsc.sum));
					item->sum_sta = lsc->lsc;
					item->sum_calc_len = otp_lsc_len;
				}
				break;
			default:
				continue;
			}
			hdr->len += sizeof(OtpDataItem) + item->len;
			if (hdr->len + (int)sizeof(OtpDataHeader) > maxlen) {
				return -1;
			}
		}
		*/
		return sizeof(OtpDataHeader) + hdr->len;
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K5E2_MD701::get_otp_group(void)
	{
		return 0;
	}
	               
	int OtpCali_S5K5E2_MD701::do_SamsungLSCCali(uint16_t *pRaw10,int width, int height, char *otp, int &otp_len,char *sram, int &sram_len)
	{
		doSamsungLSCCali_5E2(pRaw10,width,height,otp,otp_len,sram,sram_len);


		return 0;
	}

	

}
