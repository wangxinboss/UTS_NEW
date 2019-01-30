#include "StdAfx.h"
#include "OtpCali_OV16885_MA84F.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_OV16885_MA84F {
	//-------------------------------------------------------------------------------------------------
	OtpCali_OV16885_MA84F::OtpCali_OV16885_MA84F(BaseDevice *dev) : OVOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		otp_spc_len = QULCOMM_SPC_LEN;
		otp_dcc_len = QULCOMM_DCC_LEN;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV16885_MA84F::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV16885_MA84F::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV16885_MA84F::LscCali(void *args)
	{
		return do_qulcomm_lsc_awb_cali();
	}
	//-------------------------------------------------------------------------------------------------

	int OtpCali_OV16885_MA84F::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV16885_MA84F::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;
		
		m->mInfoFlag = 0x01;
        m->moduleID = 0x42;
		m->LensID = 0x43;
		
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

		m->checkSum = (CheckSum(&m->moduleID, sizeof(MINFO)-2)%255);

		return sizeof(MINFO);
	}

	int OtpCali_OV16885_MA84F::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		//module info
		uts.log.Debug(_T("Get Minfo"));
		int ret = get_minfo_from_db(&otp->minfo);
		if (ret < 0) return ret;

		//AWB
		otp->awb.awbFlag = 0x01;

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
	
		otp->awb.checkSum = (CheckSum(&otp->awb.rg, 6)%255);

		//AF
		uts.log.Debug(_T("Get AFinfo"));
		otp->af.afFlag = 0x01;

		int inf = 0, marcro = 0;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0)
		{
			uts.log.Error(_T("Get AF Data Error"));
			return SET_ERROR(OTPCALI_ERROR_NODATA);
		}
		uts.log.Debug(_T("Get AF inf:%d mup:%d"),inf,marcro);

		otp->af.afCaliDir = 0x02;
		put_le_val(inf, otp->af.inf, sizeof(otp->af.inf));
		put_le_val(marcro, otp->af.mup, sizeof(otp->af.mup));

		otp->af.checkSum = (CheckSum(&otp->af.afCaliDir, 5)%255);
		//LSC
		//////////////////////////////////////////////////
		//          (need check data format)
		/////////////////////////////////////////////////
		uts.log.Debug(_T("Get LSCinfo"));
		otp->lsc.lscFlag = 0x01;
		uint8_t *lscDB = new uint8_t[QULCOMM_LSC_LEN];
		ret = get_lsc_from_raw_data(lscDB, QULCOMM_LSC_LEN);
		if (ret < 0) 
		{
			RELEASE_ARRAY(lscDB);
			uts.log.Error(_T("Get LSC Data Error"));
			return SET_ERROR(OTPCALI_ERROR_LSCCALI);
		}
	
		for(int i=0; i<221; i++)
		{
			otp->lsc.lsc[8*i]     = lscDB[221*2*1 + 2*i];     //R_L 
			otp->lsc.lsc[8*i+1]   = lscDB[221*2*1 + 2*i + 1]; //R_H
			otp->lsc.lsc[8*i+2]   = lscDB[221*2*0 + 2*i];     //Gr_L
			otp->lsc.lsc[8*i+3]   = lscDB[221*2*0 + 2*i + 1]; //Gr_H
			otp->lsc.lsc[8*i+4]   = lscDB[221*2*2 + 2*i];     //GB_L
			otp->lsc.lsc[8*i+5]   = lscDB[221*2*2 + 2*i + 1]; //GB_H
			otp->lsc.lsc[8*i+6]	  = lscDB[221*2*3 + 2*i];     //B_L
			otp->lsc.lsc[8*i+7]   = lscDB[221*2*3 + 2*i + 1]; //B_H
		}

		otp->lsc.checkSum = (CheckSum(&otp->lsc.lsc, 1768)%255);
		RELEASE_ARRAY(lscDB);		

		//pdaf 
		uts.log.Debug(_T("Get PDAFinfo"));
		char *pdafbuf = new char[otp_dcc_len];
		ret = otpDB->GetOtpByType(mid, 8, pdafbuf,otp_dcc_len);
		if (ret < 0) 
		{
			RELEASE_ARRAY(pdafbuf);
			uts.log.Error(_T("Get PDAF Data Error"));
			return SET_ERROR(OTPCALI_ERROR_PDAF_DCC);
		}
		//GainMap
		otp->pdafGM.gmFlag  = 0x01;
		//copy gain map version, W, H and gain map 2+2+2+442+442
		memcpy(&otp->pdafGM.version,pdafbuf,890);

		otp->pdafGM.checkSum = (CheckSum(&otp->pdafGM.gm_l,884)%255);

		//DCC
		otp->pdafDCC.dccFlag = 0x01;
		//copy Q Format W,H and DCC 2+2+2+48*2
		memcpy(&otp->pdafDCC.dcc_q_format,pdafbuf+890,102);
		otp->pdafDCC.dccFlag = (CheckSum(&otp->pdafDCC.dcc, 96)%255);
		
		RELEASE_ARRAY(pdafbuf);

		return sizeof(OTPData);
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV16885_MA84F::DCC0Cali(void *args)
	{
		return  get_qulcomm_pd_REVL();
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV16885_MA84F::do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len)
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_OV16885();
		return pdaf.SPCCali(pRaw10,width,height,out);
	}

	int OtpCali_OV16885_MA84F::do_DCCCali(uint8_t *SPCTable,uint16_t **imagebuf, 
		int16_t *dcc_stack_lenspos, uint8_t out[])
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_OV16885();
		return pdaf.DCCCali(SPCTable,imagebuf,dcc_stack_lenspos,out);
	}

	int OtpCali_OV16885_MA84F::PDAFVerify(void *args)
	{
		return qulcomm_dcc_verify_REL();
	}

	int OtpCali_OV16885_MA84F::do_qulcomm_DCCVerify_REV(uint8_t *DCCTable,uint16_t *img,int& DAC)
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_OV16885();
		return pdaf.DCCVerify(img,DCCTable,DAC);

	}

	int OtpCali_OV16885_MA84F::get_wb_cali_data(WB_GAIN *gain, void *out)
	{
		uint8_t *buf = (uint8_t *)out;
		put_be_val(gain->BGain, buf, 2);
		put_be_val(gain->GGain, buf+2, 2);
		put_be_val(gain->GGain, buf+4, 2);
		put_be_val(gain->RGain, buf+6, 2);
		return 6;
	}

}
