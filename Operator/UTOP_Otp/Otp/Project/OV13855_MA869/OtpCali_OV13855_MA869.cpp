#include "StdAfx.h"
#include "OtpCali_OV13855_MA869.h"
#include "VCM.h"
#include "algorithm.h"
#include "Otp/Ref/MTK_PDAF/PDAF_MTK.h"


#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_OV13855_MA869 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_OV13855_MA869::OtpCali_OV13855_MA869(BaseDevice *dev) : OVOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = MTK_LSC_LEN_MA869;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		otp_spc_len = MTK_SPC_LEN_OV13855;
		otp_dcc_len = MTK_DCC_LEN_OV13855;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA869::do_prog_otp()
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
	int OtpCali_OV13855_MA869::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA869::LscCali(void *args)
	{
		return do_mtk_lsc_cali();
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA869::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->flag = 0x01;
		m->VendorID = 0x08;

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
			m->sec =  (uint8_t)stime.wSecond;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			m->year  = today.tm_year % 100;
			m->month = (uint8_t)today.tm_mon + 1;
			m->day   = (uint8_t)today.tm_mday;
			m->hour  = (uint8_t)today.tm_hour;
			m->minu  = (uint8_t)today.tm_min;
			m->sec =  (uint8_t)today.tm_sec;
		}

		m->LnesID = 0x23;
		m->VCMID = 0x16;
		m->DriverICID = 0x0A;
		m->OIS_FW_VER = 0xFF;
		m->SensorID = 0x1A;
		m->ProductID = 0x1C;
		m->VersionID = 0x04;
		m->HistoryID = 0x01;

		CString sensorid = uts.info.strSensorId;
		uts.log.Error(_T("Get SensorID = %s") , sensorid);
		//sensorid = sensorid.Right(16);
		//memcpy(&m->sensorfuse, T2A(sensorid), 16);
		//memcpy(&m->sensorfuse, uts.info.strSensorId, 16);

		//CString sn = uts.info.strSensorId;
		for (int i = 0; i< 16; i++)
		{
			m->sensorfuse[i] = (BYTE)(wcstol((sensorid.Mid(2*i,2)),NULL,16));
			uts.log.Info(_T("sensorfuse[%d] = %d "), i, m->sensorfuse[i] );
		}
		
		m->Reserved[0]=0xff;
		m->Reserved[1]=0xff;
		m->Reserved[2]=0xff;

		m->EepromorOTP = 0x01;

		int sum = CheckSum(&m->flag, 35);
		uts.log.Error(_T("Mininfo sum = %d") , sum);
		m->checksum = sum% 0xff+1;

		return sizeof(MINFO);
	}

	int OtpCali_OV13855_MA869::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;
		int sum;
		USES_CONVERSION;

		//module info
		int ret = get_minfo_from_db(&otp->minfo);

		//Segment Information
		otp->SI.flag = 0x01;
		otp->SI.SegCount = 0x04;
		otp->SI.SegAFOffset_H = 0x00;
		otp->SI.SegAFOffset_L = 0x40;
		otp->SI.SegAWBOffset_H = 0x00;
		otp->SI.SegAWBOffset_L = 0x54;
		otp->SI.SegLSCOffset_H = 0x00;
		otp->SI.SegLSCOffset_L = 0x80;
		otp->SI.SegAECOffset_H = 0xdc;
		otp->SI.SegAECOffset_L = 0xdc;
		otp->SI.SegPDAFOffset_H = 0x0e;
		otp->SI.SegPDAFOffset_L = 0x70;
		memset(&otp->SI.Reserved,0xff,sizeof(uint8_t)*12);
		otp->SI.SegChecksumOffset_H = 0x1B;
		otp->SI.SegChecksumOffset_L = 0xF3;
		sum = CheckSum(&otp->SI.flag+1, 25);
		otp->SI.checksum = sum%0xff + 1;
		uts.log.Error(_T("SI sum = %d") , sum);
		
		//AF
		otp->af.SegAF_TagID_H = 0x00;
		otp->af.SegAF_TagID_L = 0x01;
		otp->af.flag = 0x01;
		otp->af.TypeNum = 0x01;

		int inf = 0, marcro = 0;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0) return ret;
		put_be_val(inf, otp->af.inf, sizeof(otp->af.inf));
		put_be_val(marcro, otp->af.mup, sizeof(otp->af.mup));

		otp->af.Reser[0] = 0xff;
		otp->af.Reser[1] = 0xff;
		otp->af.Reser[2] = 0xff;
		otp->af.Reser[3] = 0xff;

		//姿勢差差異的值
		otp->af.PosDiffA_H = 0x00; //姿勢差 水平與向下
		otp->af.PosDiffA_L = 0x4A;
		otp->af.PosDiffB_H = 0x00; //姿勢差 水平與向上
		otp->af.PosDiffB_L = 0x46;
		otp->af.PosDiffC_H = 0x00; //姿勢差 向上與向下
		otp->af.PosDiffC_L = 0x90;
		sum = CheckSum(&otp->af.flag+1, 15);
		uts.log.Error(_T("af sum = %d") , sum);
		otp->af.checksum = sum%0xff + 1;

		otp->Reserved = 0xff;

		//memset(&otp->af.Reserved,0,sizeof(uint8_t)*119);

		//WB
		WB_DATA_USHORT wb[2];
		//ret = otpDB->GetOtpByType(mid, 3, (char *)& wb[0], 8);
		//if (ret < 0) { return ret;}
		ret = get_wb_from_raw_data(&wb[0], &wb[1]);
		if (ret < 0) 
		{ 
			uts.log.Info(_T("get wb fail"));
			return ret;
		}
		uts.log.Info(_T("get wb success"));
	
		uts.log.Error(_T("Get WB[0].R = %d") ,  wb[0].R);
		uts.log.Error(_T("Get WB[0].Gr = %d") ,  wb[0].Gr);
		uts.log.Error(_T("Get WB[0].Gb = %d") ,  wb[0].Gb);
		uts.log.Error(_T("Get WB[0].B = %d") ,  wb[0].B);
		
		

		otp->wb.SegAWBTabID_H = 0x00;
		otp->wb.SegAWBTabID_L = 0x02;
		otp->wb.flag = 0x01;
		otp->wb.TypeNum = 0x02;
		otp->wb.mirror = 0x00;
		otp->wb.filp = 0x00;
		otp->wb.r_H = (wb[0].R -64) /256;
		otp->wb.r_L = (wb[0].R -64) %256;
		otp->wb.gr_H = (wb[0].Gr -64)/256;
		otp->wb.gr_L = (wb[0].Gr -64)%256;
		otp->wb.gb_H = (wb[0].Gb -64)/256;
		otp->wb.gb_L = (wb[0].Gb -64)%256;
		otp->wb.b_H  = (wb[0].B -64)/256;
		otp->wb.b_L  = (wb[0].B -64)%256;
		memset(&otp->wb.Reserved,0xff,sizeof(uint8_t)*8);
		sum = CheckSum(&otp->wb.flag+1, 19);
		uts.log.Error(_T("wb sum = %d") , sum);
		otp->wb.checksum = sum%0xff + 1;

		memset(&otp->Reserved2,0xff,sizeof(uint8_t)*21);

		//LSC
		otp->lsc.SegLSCTagID_H = 0x00;
		otp->lsc.SegLSCTagID_L = 0x03;
		otp->lsc.flag = 0x01;
		otp->lsc.TypeNum = 0x02;
		otp->lsc.VerNum = 0x0E;
		otp->lsc.mirror = 0x00;
		otp->lsc.filp = 0x00;
		
		ret = get_lsc_from_raw_data(otp->lsc.lscbuffer, sizeof(otp->lsc.lscbuffer));
		if (ret < 0) return ret;
	

		sum = CheckSum(&otp->lsc.flag+1, 1872);
		uts.log.Error(_T("lsc sum = %d") , sum);
		otp->lsc.checksum = sum%0xff + 1;

		memset(&otp->Reserved3,0xFF,1692*sizeof(uint8_t));

		//pdaf
		otp->pdaf.SegPDAFTagID_H = 0x00;
		otp->pdaf.SegPDAFTagID_L = 0x04;
		otp->pdaf.flag = 0x01;
		otp->pdaf.TypeNum = 0x02;
		memset(&otp->pdaf.Reser,0xff,sizeof(uint8_t)*6);
		get_otp_from_raw_data(OTPDB_OTPTYPE_SPC, otp->pdaf.spc, sizeof(otp->pdaf.spc));
		memset(&otp->pdaf.Reser2,0xff,sizeof(uint8_t)*388);
		get_otp_from_raw_data(8, otp->pdaf.dcc, sizeof(otp->pdaf.dcc));
		sum = CheckSum(&otp->pdaf.flag+1, 1767) ;
		otp->pdaf.checksum = sum%0xff + 1;
		uts.log.Error(_T("pdaf sum = %d") , sum);
		
		//checksum
		memset(&otp->Reserved4,0xff,sizeof(uint8_t)*1688);
		otp->SegChecksumTagID_H = 0x80;
		otp->SegChecksumTagID_L = 0x00;
		sum = CheckSum(&otp->minfo.flag, 7157);
		uts.log.Error(_T("all sum = %d") , sum);
		otp->Totalchecksum =  sum%0xff + 1;
		
		return sizeof(OTPData);
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA869::get_uniform_otp_data(void *in, void *out, int maxlen)
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
	int OtpCali_OV13855_MA869::get_otp_group(void)
	{
		return 0;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA869::DCC0Cali(void *args)
	{
		return  mtk_dcc_cali("OV13855_v3_mirror_normal_flip_normal_mtk3.0.0.ini");
	}


	int OtpCali_OV13855_MA869::PDAFVerify(void *args)
	{
		return mtk_dcc_verify("OV13855_v3_mirror_normal_flip_normal_mtk3.0.0.ini");
	}

	int OtpCali_OV13855_MA869::do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len)
	{
		return mtk_spc_cali(pRaw10,"OV13855_v3_mirror_normal_flip_normal_mtk3.0.0.ini",out);
	}

	int OtpCali_OV13855_MA869::get_wb_cali_data(WB_GAIN *gain, void *out)
	{
		uint8_t *buf = (uint8_t *)out;
		put_be_val(gain->RGain, buf, 2);
		put_be_val(gain->GGain, buf+2, 2);
		put_be_val(gain->BGain, buf+4, 2);
		return 6;
	}
}
