#include "StdAfx.h"
#include "OtpCali_S5K3P9SX_MA841.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_S5K3P9SX_MA841 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K3P9SX_MA841::OtpCali_S5K3P9SX_MA841(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		otp_spc_len = QULCOMM_SPC_LEN;
		otp_dcc_len = QULCOMM_DCC_LEN;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MA841::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MA841::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MA841::LscCali(void *args)
	{
		return do_qulcomm_lsc_awb_cali();
	}
	//-------------------------------------------------------------------------------------------------

	int OtpCali_S5K3P9SX_MA841::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MA841::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;
		
		m->flag = 0x01;
		m->version = 0x01;
        m->moduleID = 0x02;
		m->lensID = 0x01;
		m->vcmID = 0x01;
		m->vcmDriverID = 0x01;
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

	int OtpCali_S5K3P9SX_MA841::get_otp_data_from_db(void *args)
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
	
		//goldenÊý¾Ý
		put_le_val(551, otp->awb.rg_golden, sizeof(otp->awb.rg_golden));
		put_le_val(652, otp->awb.bg_golden, sizeof(otp->awb.bg_golden));
		put_le_val(1023, otp->awb.gbgr_golden, sizeof(otp->awb.gbgr_golden));

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
		memset(&otp->lsc.reserved,0x00,246);

		//AF
		int inf = 0, marcro = 0;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0)
		{
			uts.log.Error(_T("Get AF Data Error"));
			return SET_ERROR(OTPCALI_ERROR_NODATA);
		}
		uts.log.Debug(_T("Get AF inf:%d mup:%d"),inf,marcro);

		put_le_val(inf, otp->af.inf, sizeof(otp->af.inf));
		put_le_val(marcro, otp->af.mup, sizeof(otp->af.mup));

		otp->af.checkSum = (CheckSum(&otp->af.inf, 4)%255);
		memset(&otp->af.reserved,0x00,2);


		//pdaf
		uts.log.Debug(_T("Get PDAFinfo"));
		char *pdafbuf = new char[otp_dcc_len];
		
		ret = otpDB->GetOtpByType(mid, 8, pdafbuf,otp_dcc_len);
		if (ret < 0) 
		{
			uts.log.Error(_T("Get PDAF Data Error"));
			return SET_ERROR(OTPCALI_ERROR_PDAF_DCC);
		}

		otp->pdafGainmap.flag = 0x01;
		//copy gain map version, W, H and gain map
		memcpy(&otp->pdafGainmap.version,pdafbuf,890);
		otp->pdafGainmap.checkSum = (CheckSum(&otp->pdafGainmap.version, 890)%255);

		memset(&otp->pdafGainmap.reserved,0x00,4);

		otp->pdafDCC.flag = 0x01;
		//copy DCC
		memcpy(&otp->pdafDCC.dcc_q_format,pdafbuf+890,102);
		otp->pdafDCC.checkSum = (CheckSum(&otp->pdafDCC.dcc_q_format, 102)%255);

		memset(&otp->pdafDCC.reserved,0x00,16);

		RELEASE_ARRAY(pdafbuf);

		return sizeof(OTPData);
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MA841::DCC0Cali(void *args)
	{
		return  get_qulcomm_pd_REVL();
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_MA841::do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len)
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_3P9SX();
		return pdaf.SPCCali(pRaw10,width,height,out);
	}

	int OtpCali_S5K3P9SX_MA841::do_DCCCali(uint8_t *SPCTable,uint16_t **imagebuf, 
		int16_t *dcc_stack_lenspos, uint8_t out[])
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_3P9SX();
		return pdaf.DCCCali(SPCTable,imagebuf,dcc_stack_lenspos,out);
	}

	int OtpCali_S5K3P9SX_MA841::PDAFVerify(void *args)
	{
		return qulcomm_dcc_verify_REL();
	}

	int OtpCali_S5K3P9SX_MA841::do_qulcomm_DCCVerify_REV(uint8_t *DCCTable,uint16_t *img,int& DAC)
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_3P9SX();
		return pdaf.DCCVerify(img,DCCTable,DAC);

	}

}
