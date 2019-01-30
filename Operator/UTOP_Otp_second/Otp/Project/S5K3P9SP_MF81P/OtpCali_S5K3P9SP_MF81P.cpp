#include "StdAfx.h"
#include "OtpCali_S5K3P9SP_MF81P.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")

typedef int (/*WINAPI */ *Dllfun1) (char* ini_file_path , short *pRaw10, short *pLRC);
//-----------------------------------------------------------------------------

namespace OtpCali_S5K3P9SP_MF81P {
	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K3P9SP_MF81P::OtpCali_S5K3P9SP_MF81P(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SP_MF81P::do_prog_otp()
	{
		if (eeprom->Write(0x0000, otp_data_in_db, 0x05ad) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);
		if (eeprom->Write(0x1200, otp_data_in_db+0x05ad, 0x0804) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);
		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SP_MF81P::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0x0000, otp, 0x05ad) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if (eeprom->Read(0x1200, otp+0x05ad, 0x0804) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SP_MF81P::LscCali(void *args)
	{
		return do_qulcomm_lsc_awb_cali_RAW10();
	}
	//-------------------------------------------------------------------------------------------------

	int OtpCali_S5K3P9SP_MF81P::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,pRaw10,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout,64);

		/*		USES_CONVERSION;

		int ret = -1;
		FILE *fp;
		HINSTANCE hdll;
		Dllfun1 LensCorrection;

		short * temp_pRaw10 = (short *)pRaw10;
		short * temp_out = (short *)out;

		hdll = LoadLibrary(_T("R_LSCCalibrationDll.dll"));	
		if(hdll==NULL) { FreeLibrary(hdll);return ret;}

		LensCorrection = (Dllfun1)GetProcAddress(hdll,"LensCorrectionLib");

		if(LensCorrection==NULL) { FreeLibrary(hdll);return ret;}

		ret = LensCorrection (T2A(otp_param.pdaf_param.pdafcal_ini_file),temp_pRaw10, temp_out);

		if(hdll!=NULL) { FreeLibrary(hdll);hdll =NULL;}

		return ret;*/	

	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SP_MF81P::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->flag = 0x01;
		m->storage_loc = 0x01;

		m->smartisan_pn[0] = 'E';   //EMCAM0021A00
		m->smartisan_pn[1] = 'M';
		m->smartisan_pn[2] = 'C';
		m->smartisan_pn[3] = 'A';
		m->smartisan_pn[4] = 'M';
		m->smartisan_pn[5] = '0';
		m->smartisan_pn[6] = '0';
		m->smartisan_pn[7] = '2';
		m->smartisan_pn[8] = '1';
		m->smartisan_pn[9] = 'A';
		m->smartisan_pn[10] = '0';
		m->smartisan_pn[11] = '0';

		m->module = 0x09;		//holitech:0x09
		m->sensor = 0x28;
		m->lens   = 0x22;			
		m->vcm    = 0x00;			
		m->vcmdriver = 0x00;	
		m->ir_bg =0x04;
		m->gyro = 0x00;
		m->color_temp =0x02;
		m->oisdriver =0x00;

		m->fpc_version =0x01;
		m->cal_verison =0x01;
		m->ois_fw_version =0x01;

		CString m_sn = uts.info.strSN;

		for (int i= 0; i< 16; i++){
			m->serial_code[i]= atoi(T2A(m_sn.Left(2)));
			m_sn.Delete(0,2);
		}

		m->project_sku = 0x11;
		m->product_fac = 0x01;
		m->product_line = 0x01;

		SYSTEMTIME stime;
		time_t time;
		otpDB->get_otp_data_lock_time(mid, &time);

		if (time < 0)
		{
			GetLocalTime(&stime);
			m->year   = stime.wYear % 100;
			m->month  = (uint8_t)stime.wMonth;
			m->day    = (uint8_t)stime.wDay;
			m->hour   = (uint8_t)stime.wHour;
			m->minute = (uint8_t)stime.wMinute;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			m->year   = today.tm_year % 100;
			m->month  = (uint8_t)today.tm_mon + 1;
			m->day    = (uint8_t)today.tm_mday;
			m->hour   = (uint8_t)stime.wHour;
			m->minute = (uint8_t)stime.wMinute;
		}
		
		m->reserved[0] = 0x00;	
		m->reserved[1] = 0x00;

		uint32_t sum = CheckSum(&m->flag, sizeof(MINFO)-2) % 0xffff;

		put_be_val(sum, m->sum, sizeof(m->sum));

		return sizeof(MINFO);
	}
	int OtpCali_S5K3P9SP_MF81P::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		memset(otp, 0xff, sizeof(OTPData));

		//module info
		uts.log.Debug(_T("Get Minfo"));
		int ret = get_minfo_from_db(&otp->minfo);
		if (ret < 0) return ret;
			
		//AF
		uts.log.Debug(_T("Get AFinfo"));
		otp->af.flag = 0;

		//AWB
		uts.log.Debug(_T("Get WBinfo"));

		otp->awb.flag = 0x01;
		otp->awb.equip_num = 0x01;

		//5100K
		WB_DATA_UCHAR wb[2];
		ret = get_wb_from_raw_data(&wb[0], &wb[1], OTPDB_OTPTYPE_AWB);
		if (ret < 0) { return ret;}

		put_be_val(16384, otp->awb._5100kr_gr,  sizeof(otp->awb._5100kr_gr));
		put_be_val(16384, otp->awb._5100kb_gr,  sizeof(otp->awb._5100kb_gr));
		put_be_val(16384, otp->awb._5100kgb_gr, sizeof(otp->awb._5100kgb_gr));

		WB_RATIO ratio, ratio_g;
		get_wb_ratio(&wb[0], &ratio, 1024);

		uts.log.Info(_T("5100K:r_gr=0x%04x,b_gr=0x%04x,gb_gr=0x%04x"),ratio.r_gr,ratio.b_gr,ratio.gb_gr);

		put_be_val(ratio.r_gr*16384,  otp->awb.ratio_5100k_r_gr,  sizeof(otp->awb.ratio_5100k_r_gr));
		put_be_val(ratio.b_gr*16384,  otp->awb.ratio_5100k_b_gr,  sizeof(otp->awb.ratio_5100k_b_gr));
		put_be_val(ratio.gb_gr*16384, otp->awb.ratio_5100k_gb_gr, sizeof(otp->awb.ratio_5100k_gb_gr));


		//3100K
		ret = get_wb_from_raw_data(&wb[0], &wb[1], 9);
		if (ret < 0) { return ret;}

		put_be_val(16384, otp->awb._3100kr_gr,  sizeof(otp->awb._3100kr_gr));
		put_be_val(16384, otp->awb._3100kb_gr,  sizeof(otp->awb._3100kb_gr));
		put_be_val(16384, otp->awb._3100kgb_gr, sizeof(otp->awb._3100kgb_gr));

		get_wb_ratio(&wb[0], &ratio, 1024);

		uts.log.Info(_T("3100K:r_gr=0x%04x,b_gr=0x%04x,gb_gr=0x%04x"),ratio.r_gr,ratio.b_gr,ratio.gb_gr);

		put_be_val(ratio.r_gr*16384,  otp->awb.ratio_3100k_r_gr,  sizeof(otp->awb.ratio_3100k_r_gr));
		put_be_val(ratio.b_gr*16384,  otp->awb.ratio_3100k_b_gr,  sizeof(otp->awb.ratio_3100k_b_gr));
		put_be_val(ratio.gb_gr*16384, otp->awb.ratio_3100k_gb_gr, sizeof(otp->awb.ratio_3100k_gb_gr));

		uint32_t sum = CheckSum(&otp->awb.flag, sizeof(AWB)-2) % 0xffff;

		put_be_val(sum, otp->awb.sum, sizeof(otp->awb.sum));

		//LSC
		//////////////////////////////////////////////////
		//          (need check data format)
		/////////////////////////////////////////////////
		uts.log.Debug(_T("Get LSCinfo"));

		uint8_t *lscDB = new uint8_t[QULCOMM_LSC_LEN+8];
		ret = get_lsc_from_raw_data(lscDB, QULCOMM_LSC_LEN+8);
		if (ret < 0) return ret;
		otp->lsc.flag = 0x01;
		otp->lsc.flag = 0x01;

		int cnt = 0;
		for(int i=0;i<QULCOMM_LSC_LEN;i+=8)
		{
			
			otp->lsc.lsc[cnt++] = lscDB[221*2*1+i/4];		//R_L
			otp->lsc.lsc[cnt++] = lscDB[221*2*0+i/4];		//Gr_L
			otp->lsc.lsc[cnt++] = lscDB[221*2*2+i/4];		//Gb_L
			otp->lsc.lsc[cnt++] = lscDB[221*2*3+i/4];		//B_L
			otp->lsc.lsc[cnt++]	= lscDB[221*2*1+i/4+1]*64 +   //R_H
				                  lscDB[221*2*0+i/4+1]*16 +   //Gr_H
								  lscDB[221*2*2+i/4+1]*4  +   //Gb_H
								  lscDB[221*2*3+i/4+1] ;	  //B_H
		}

		otp->lsc.r_max[0]  = lscDB[QULCOMM_LSC_LEN+1];
		otp->lsc.r_max[1]  = lscDB[QULCOMM_LSC_LEN+0];
		otp->lsc.gr_max[0] = lscDB[QULCOMM_LSC_LEN+3];
		otp->lsc.gr_max[1] = lscDB[QULCOMM_LSC_LEN+2];
		otp->lsc.gb_max[0] = lscDB[QULCOMM_LSC_LEN+5];
		otp->lsc.gb_max[1] = lscDB[QULCOMM_LSC_LEN+4];
		otp->lsc.b_max[0]  = lscDB[QULCOMM_LSC_LEN+7];
		otp->lsc.b_max[1]  = lscDB[QULCOMM_LSC_LEN+6];

		sum = CheckSum(&otp->lsc, sizeof(LSC)-2) % 0xffff;

		put_be_val(sum, otp->lsc.sum, sizeof(otp->lsc.sum));

		RELEASE_ARRAY(lscDB);

		//Total CheckSum
		sum = CheckSum(&otp->minfo.flag, sizeof(OTPData)-sizeof(Cross_Talk)) % 0xffff;
		//crosstalk
		uts.log.Debug(_T("Get Crosstalkinfo"));
		
	//	uint8_t *lscDB = new uint8_t[QULCOMM_LSC_LEN];
	//	ret = get_lsc_from_raw_data(lscDB, QULCOMM_LSC_LEN);
	//	if (ret < 0) return ret;

		
		return sizeof(OTPData);
	}

	//-------------------------------------------------------------------------------------------------//

}
