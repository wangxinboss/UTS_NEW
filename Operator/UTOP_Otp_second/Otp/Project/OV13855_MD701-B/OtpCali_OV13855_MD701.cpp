#include "StdAfx.h"
#include "OtpCali_OV13855_MD701.h"
#include "VCM.h"
#include "algorithm.h"
#include "Otp/Ref/MTK_PDAF/PDAF_MTK.h"


#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_OV13855_MD701 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_OV13855_MD701::OtpCali_OV13855_MD701(BaseDevice *dev) : OVOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = MTK_LSC_LEN_MD701;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		otp_spc_len = MTK_SPC_LEN_OV13855;
		otp_dcc_len = MTK_DCC_LEN_OV13855;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MD701::do_prog_otp()
	{
		//
		char buf[6000];

		memset(buf,0xFF,6000);

		if (eeprom->Write(0, buf, 6000) < 0)
		return SET_ERROR(OTPCALI_ERROR_EEPROM);

		//
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MD701::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MD701::LscCali(void *args)
	{
		return do_mtk_lsc_cali();
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MD701::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->flag = 0x40;
		m->StoregeLoc=0x01;

		CString sn = m_szSN;
		sn = sn.Right(10);
		memcpy(&m->sn, T2A(sn), 10);

		m->IDDef[0] = 0x07;
		m->IDDef[1] = 0x23;
		m->IDDef[2] = 0x2D;
		m->IDDef[3] = 0x29;
		m->IDDef[4] = 0x0C;
		m->IDDef[5] = 0x03;
		m->IDDef[6] = 0x00;
		m->IDDef[7] = 0x01;

		m->Ver[0] = 0x01;
		m->Ver[1] = 0x01;
		m->Ver[2] = 0x01;
		m->Ver[3] = 0x00;

		CString sensorid = uts.info.strSensorId;
		sensorid = sensorid.Right(16);
		memcpy(&m->sensorfuse, T2A(sensorid), 16);

		m->BuildInfo=0x21;
		m->ProData[0]=0x01;
		m->ProData[1]=0x01;

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
			m->minu  = (uint8_t)stime.wMinute;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			m->year  = today.tm_year % 100;
			m->month = (uint8_t)today.tm_mon + 1;
			m->day   = (uint8_t)today.tm_mday;
			m->hour  = (uint8_t)today.tm_hour;
			m->minu  = (uint8_t)today.tm_min;
		}

		m->Reserved[0]=0x00;
		m->Reserved[1]=0x00;
		
		int sum = CheckSum(&m->flag, 50);
		put_be_val(sum% 0xFFFF , m->checksum, sizeof(m->checksum));

		return sizeof(MINFO);
	}
	int OtpCali_OV13855_MD701::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		//module info
		int ret = get_minfo_from_db(&otp->minfo);


		//AF
		otp->af.AFflag=0x40;
		otp->af.Revers[0]=0x00;
		otp->af.Revers[1]=0x00;

		int inf = 0, marcro = 0;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0) return ret;

		put_be_val(marcro, otp->af.mup, sizeof(otp->af.mup));
		put_be_val(inf, otp->af.inf, sizeof(otp->af.inf));

		otp->af.Reser[0]=0x00;
		otp->af.Reser[1]=0x00;
		otp->af.Reser[2]=0x00;
		otp->af.Reser[3]=0x00;
		otp->af.AfDirection=0x01;

		memset(&otp->af.Reserved,0,sizeof(uint8_t)*119);

		int afsum = CheckSum(&otp->af.AFflag, 131);
		put_be_val(afsum % 0xFFFF, otp->af.checksum, sizeof(otp->af.checksum));

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

		int sum = CheckSum(&otp->wb.flag, 35);
		put_be_val(sum % 0xFFFF , otp->wb.checksum, sizeof(otp->wb.checksum));

		//LSC
		otp->lsc.flag=0x40;
		ret = get_lsc_from_raw_data(otp->lsc.lscbuffer, sizeof(otp->lsc.lscbuffer));
		if (ret < 0) return ret;
		memset(&otp->lsc.Reserved,0,sizeof(uint8_t)*452);

		sum = CheckSum(&otp->lsc.flag, 2321);
		put_be_val(sum % 0xFFFF , otp->lsc.checksum, sizeof(otp->lsc.checksum));

		sum = CheckSum(&otp->minfo.flag, 2545);
		put_be_val(sum % 0xFFFF , otp->Totalchecksum, sizeof(otp->Totalchecksum));

		memset(&otp->TotalReserved,0xFF,781*sizeof(uint8_t));

		//pdaf
		otp->pdaf.flag=0x40;
		get_otp_from_raw_data(OTPDB_OTPTYPE_SPC, otp->pdaf.spc, sizeof(otp->pdaf.spc));
		get_otp_from_raw_data(8, otp->pdaf.dcc, sizeof(otp->pdaf.dcc));
		sum = CheckSum(&otp->pdaf.flag, 1373) ;
		put_be_val(sum % 0xFFFF, otp->pdaf.checksum, sizeof(otp->pdaf.checksum));
		
		return sizeof(OTPData);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MD701::get_uniform_otp_data(void *in, void *out, int maxlen)
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
					minfo->tm = minfo->raw + offsetof(MINFO, year);
					minfo->tm_len = 3;
					minfo->raw_len = sizeof(otp->minfo);
					memcpy(minfo->raw, &otp->minfo, minfo->raw_len);
					item->len = sizeof(OtpDataMInfo) + minfo->raw_len;
				}
				break;
			case OtpDataType_WB:
				{
					OtpDataWB *wb = (OtpDataWB*)item->data;
					wb->has_golden = true;
					get_wb_ratio(&otp->wb.module, &wb->ratio, 1.0);
					get_wb_ratio(&otp->wb.module_g, &wb->ratio_g, 1.0);
					wb->raw_len = sizeof(otp->wb);
					memcpy(wb->raw, &otp->wb, wb->raw_len);
					item->len = sizeof(OtpDataWB) + wb->raw_len;
				}
				break;
			case OtpDataType_AF:
				{
					OtpDataAF *af = (OtpDataAF*)item->data;
					af->af.start = -1;
					af->af.inf = get_le_val(otp->af.inf, sizeof(otp->af.inf));
					af->af.mup = get_le_val(otp->af.mup, sizeof(otp->af.mup));
					af->raw_len = sizeof(otp->af);
					memcpy(af->raw, &otp->af, af->raw_len);
					item->len = sizeof(OtpDataAF) + af->raw_len;
				}
				break;
			case OtpDataType_LSC:
				{
					OtpDataLSC *lsc = (OtpDataLSC*)item->data;
					lsc->lsc = lsc->raw + offsetof(LSC, lsclength);
					lsc->raw_len = sizeof(otp->lsc);
					memcpy(lsc->raw, &otp->lsc, lsc->raw_len);
					item->len = sizeof(OtpDataLSC) + lsc->raw_len;
				}
				break;
			case OtpDataType_PDAF:
				{
					memcpy(&item->data[item->len], otp->pdaf.pdaf_empty, sizeof(otp->pdaf.pdaf_empty));
					item->len += sizeof(otp->pdaf.pdaf_empty);
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
	int OtpCali_OV13855_MD701::get_otp_group(void)
	{
		return 0;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MD701::DCC0Cali(void *args)
	{
		return  mtk_dcc_cali("OV13855_v3_mirror_normal_flip_normal_mtk3.0.0.ini");
	}


	int OtpCali_OV13855_MD701::PDAFVerify(void *args)
	{
		return mtk_dcc_verify("OV13855_v3_mirror_normal_flip_normal_mtk3.0.0.ini");
	}

	int OtpCali_OV13855_MD701::do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len)
	{
		return mtk_spc_cali(pRaw10,"OV13855_v3_mirror_normal_flip_normal_mtk3.0.0.ini",out);
	}

	int OtpCali_OV13855_MD701::get_wb_cali_data(WB_GAIN *gain, void *out)
	{
		uint8_t *buf = (uint8_t *)out;
		put_be_val(gain->RGain, buf, 2);
		put_be_val(gain->GGain, buf+2, 2);
		put_be_val(gain->BGain, buf+4, 2);
		return 6;
	}
}
