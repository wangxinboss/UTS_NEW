#include "StdAfx.h"
#include "OtpCali_S5K3L8_MA706.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"
//#include "PDAF_QulComm_RevJ.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_S5K3L8_MA706 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K3L8_MA706::OtpCali_S5K3L8_MA706(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		otp_spc_len = QULCOMM_SPC_LEN;
		otp_dcc_len = QULCOMM_DCC_LEN;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA706::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA706::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA706::LscCali(void *args)
	{
		return do_qulcomm_lsc_awb_cali();
	}
	//-------------------------------------------------------------------------------------------------

	int OtpCali_S5K3L8_MA706::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA706::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->module = 0x06;		//MA706:0x06
		m->lens = 0x01;			
		m->vcm = 0x01;			
		m->vcmdriver = 0x01;	
		SYSTEMTIME stime;
		time_t time;
		otpDB->get_otp_data_lock_time(mid, &time);

		if (time < 0)
		{
			GetLocalTime(&stime);
			m->year  = stime.wYear % 100;
			m->month = (uint8_t)stime.wMonth;
			m->day   = (uint8_t)stime.wDay;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			m->year   = today.tm_year % 100;
			m->month  = (uint8_t)today.tm_mon + 1;
			m->day    = (uint8_t)today.tm_mday;
		}
		

		//memcpy(&m->sn, T2A(m_szSN), 10);
		m->sum = (CheckSum(&m->module, sizeof(MINFO)-4)) % 256;
		m->reserved[0] = 0xFF;	
		m->reserved[1] = 0xFF;
		m->reserved[2] = 0xFF;
		return sizeof(MINFO);
	}
	int OtpCali_S5K3L8_MA706::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		//module info
		uts.log.Debug(_T("Get Minfo"));
		otp->minfo_flag=0;
		int ret = get_minfo_from_db(&otp->minfo);
		if (ret < 0) return ret;
		otp->minfo_flag=1;
				
		//AF
		uts.log.Debug(_T("Get AFinfo"));
		int start = 0,inf = 0, marcro = 0;

		otp->af_flag=0;
		ret = get_af_from_raw_data(&start, &inf, &marcro);
		if (ret < 0) return ret;
		otp->af_flag=1;
		put_be_val(inf, otp->af.inf, sizeof(otp->af.inf));
		put_be_val(marcro, otp->af.mup, sizeof(otp->af.mup));
		put_be_val(start, otp->af.start, sizeof(otp->af.start));
		otp->af.sum = (CheckSum(&otp->af, sizeof(AF)-7)) % 256;	
		for(int i=0;i<6;i++)
			otp->af.reserved[i]=0xFF;	

		//AWB
		uts.log.Debug(_T("Get WBinfo"));
		struct WB_DATA_SHORT wbtemp[2];
		ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB, (char *)& wbtemp[0], 12);
		if (ret < 0) { return ret;}

// 		ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
// 			3, (char *)& wbtemp[1], 6);
// 		if (ret < 0) return ret;

		wbtemp[1].BG = 0;
		wbtemp[1].RG = 0;
		wbtemp[1].GbGr = 0;


		otp->awb_flag=0x01;
		put_be_val(wbtemp[0].RG, otp->awb.rg, sizeof(otp->awb.rg));
		put_be_val(wbtemp[0].BG, otp->awb.bg, sizeof(otp->awb.bg));
		put_be_val(wbtemp[0].GbGr, otp->awb.gbgr, sizeof(otp->awb.gbgr));
		//Golden
		put_be_val(wbtemp[1].RG, otp->awb.rg_g, sizeof(otp->awb.rg_g));
		put_be_val(wbtemp[1].BG, otp->awb.bg_g, sizeof(otp->awb.bg_g));
		put_be_val(wbtemp[1].GbGr, otp->awb.gbgr_g, sizeof(otp->awb.gbgr_g));
		otp->awb.sum = (CheckSum(&otp->awb, sizeof(AWB)-1)) % 256;

		//LSC
		//////////////////////////////////////////////////
		//          (need check data format)
		/////////////////////////////////////////////////
		uts.log.Debug(_T("Get LSCinfo"));
		uint8_t templsc=0x00;
		uint8_t *lscDB = new uint8_t[QULCOMM_LSC_LEN];
		ret = get_lsc_from_raw_data(lscDB, QULCOMM_LSC_LEN);
		if (ret < 0) return ret;
		otp->lsc_flag = 0x01;
		//DB struct = Gr[0:220] B[0:220] Gb[0:220] B[0:220] [L,H]
		//MA706 struct = R[0]Gr[0]Gb[0]B[0]...R[220]Gr[220]Gb[220]B[220] [H,L]
		for(int i=0;i<QULCOMM_LSC_LEN;i+=8)
		{
			otp->lsc.lsc[i]	 =lscDB[221*2*1+i/4+1];	//R_H
			otp->lsc.lsc[i+1]=lscDB[221*2*1+i/4];		//R_L
			otp->lsc.lsc[i+2]=lscDB[221*2*0+i/4+1];	//Gr_H
			otp->lsc.lsc[i+3]=lscDB[221*2*0+i/4];		//Gr_L
			otp->lsc.lsc[i+4]=lscDB[221*2*2+i/4+1];	//Gb_H
			otp->lsc.lsc[i+5]=lscDB[221*2*2+i/4];		//Gb_L
			otp->lsc.lsc[i+6]=lscDB[221*2*3+i/4+1];	//B_H
			otp->lsc.lsc[i+7]=lscDB[221*2*3+i/4];		//B_L
		}
// 		for(int i=0;i<sizeof(otp->lsc.lsc);i+=2)
// 		{
// 			templsc=otp->lsc.lsc[i];
// 			otp->lsc.lsc[i]=otp->lsc.lsc[i+1];
// 			otp->lsc.lsc[i+1]=templsc;
// 			templsc=0;
// 		}
		otp->lsc.sum = (CheckSum(&otp->lsc, sizeof(LSC)-1)) % 256;
		RELEASE_ARRAY(lscDB);


		//pdaf
		uts.log.Debug(_T("Get PDAFinfo"));
		char *pdafbuf = new char[otp_dcc_len];

		ret = otpDB->GetOtpByType(mid, 8, pdafbuf,otp_dcc_len);
		if (ret < 0) return ret;
		otp->pdaf_gm_flag = 0x01;
		//copy gain map version, W, H and gain map
		memcpy(&otp->q_pdaf_gm.version,pdafbuf,6+221*2*2);
		otp->q_pdaf_gm.sum = (CheckSum(&otp->q_pdaf_gm.version, 890)) % 256;

		otp->pdaf_dcc_flag = 0x01;
		//copy dcc version, W , H and dcc map
		memcpy(&otp->q_pdaf_dcc.dcc_q_format,pdafbuf+6+221*2*2,6+48*2);
		otp->q_pdaf_dcc.sum = (CheckSum(&otp->q_pdaf_dcc.dcc_q_format, 102)) % 256;
		for(int i=0;i<2556;i++)
			otp->q_pdaf_dcc.reserved[i]=0xFF;	
		//memcpy(&otp->pdaf.dm_q_format,dccbuf,otp_dcc_len);
		RELEASE_ARRAY(pdafbuf);
		

		//IQ_Master
		//IQ_Slave
		//Dual_Camera
		//Total CheckSum
		return sizeof(OTPData);
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA706::DCC0Cali(void *args)
	{
		return  get_qulcomm_pd_REVL();
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3L8_MA706::do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len)
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_3L8();
		return pdaf.SPCCali(pRaw10,width,height,out);
	}

	int OtpCali_S5K3L8_MA706::do_DCCCali(uint8_t *SPCTable,uint16_t **imagebuf, 
		int16_t *dcc_stack_lenspos, uint8_t out[])
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_3L8();
		return pdaf.DCCCali(SPCTable,imagebuf,dcc_stack_lenspos,out);
	}

// 	bool OtpCali_S5K3L8_MA706::do_qulcomm_DCCVerify_REV(uint8_t *DCCTable,uint16_t *img)
// 	{
// 		PDAF_QulComm_RevL pdaf;
// 
// 		pdaf.get_sensor_cfg_3L8();
// 		return pdaf.DCCVerify(img,DCCTable);
// 	}

}
