#include "StdAfx.h"
#include "OtpCali_OV13855_MA82L.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_OV13855_MA82L {
	//-------------------------------------------------------------------------------------------------
	OtpCali_OV13855_MA82L::OtpCali_OV13855_MA82L(BaseDevice *dev) : OVOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		otp_spc_len = QULCOMM_SPC_LEN;
		otp_dcc_len = QULCOMM_DCC_LEN;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA82L::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA82L::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA82L::LscCali(void *args)
	{
		//上传CCT数据 
		OTP_CCT *pCCT = &otp_param.cct;
		int Ev= int(pCCT->EV+0.5);
		int	cie_x = int(65535* pCCT->Cie_x+0.5);
		int cie_y = int(65535* pCCT->Cie_y+0.5);

		//添加CCT规格卡控：
		/*	Ev_CCT_Max=550
		Ev_CCT_Min=450
		x_CCT_Max=60000
		x_CCT_Min=20000
		y_CCT_Max=60000
		y_CCT_Min=22000*/
		/*if (Ev < 450 || Ev> 550 || cie_x <20000 || cie_x>60000 ||
		cie_y < 22000 || cie_y >60000)*/
		if (Ev < 450 || Ev> 550 || cie_x <22000 || cie_x>23500 ||
			cie_y < 22500 || cie_y >24500)
		{
			uts.log.Error(_T("CCT data OUT Range Ev=%d,cie_x =%d,cie_y = %d"),Ev,cie_x,cie_y);
			return SET_ERROR(OTPCALI_ERROR_CCT);
		}

		CCT cctInfo;
		put_be_val(Ev,cctInfo.Ev,2);
		put_be_val(cie_x,cctInfo.cie_x,2);
		put_be_val(cie_y,cctInfo.cie_y,2);

		int sum = (CheckSum(cctInfo.Ev, 6)%65535);
		put_be_val(sum,cctInfo.checkSum,2);

		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_CCT, (char *)(cctInfo.Ev), sizeof(cctInfo)) < 0)
		{
			uts.log.Error(_T("Failed to update CCT data to DB!"));
			return SET_ERROR(OTPCALI_ERROR_DB);
		}

		return do_qulcomm_lsc_awb_cali();
	}
	//-------------------------------------------------------------------------------------------------

	int OtpCali_OV13855_MA82L::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA82L::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;
		
        m->moduleCode = 0x01;
		m->supplierCode = 0x05;
		m->moduleVer = 0x02;
		m->reserved = 0xFF;

		//Sensor ID烧录
		memset(&m->SN,0xFF,30);
		CString sn = uts.info.strSensorId;
		for (int i = 0; i< 16; i++)
		{
			m->SN[i] = (BYTE)(wcstol((sn.Mid(2*i,2)),NULL,16));
		}
		
		//将数组从ASCII转成对应数字 20180401
		
		SYSTEMTIME stime;
		time_t time;
		otpDB->get_otp_data_lock_time(mid, &time);

		if (time < 0)
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

		//checkSum MOD(SUM(0x00:0x23),65535)
		int sum = (CheckSum(&m->year, sizeof(MINFO)-3)%65535);
		put_be_val(sum,m->checkSum,2);
	

		return sizeof(MINFO);
	}

	int OtpCali_OV13855_MA82L::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		//module info
		uts.log.Debug(_T("Get Minfo"));
		int ret = get_minfo_from_db(&otp->minfo);
		if (ret < 0) return ret;

		//CCT
		ret = otpDB->GetOtpByType(mid, DDM::OTPDB_OTPTYPE_CCT, (char *)(otp->cct.Ev), 8);
		if (ret < 0)
		{
			uts.log.Error(_T("Get CCT Data Error"));
			return SET_ERROR(OTPCALI_ERROR_CCT);
		}

		int Ev= otp->cct.Ev[0]*256+otp->cct.Ev[1];
		int	cie_x = otp->cct.cie_x[0]*256+otp->cct.cie_x[1];
		int cie_y = otp->cct.cie_y[0]*256+otp->cct.cie_y[1];
		if (Ev < 450 || Ev> 550 || cie_x <22000 || cie_x>23500 ||
			cie_y < 22500 || cie_y >24500)
		{
			uts.log.Error(_T("CCT data OUT Range Ev=%d,cie_x =%d,cie_y = %d"),Ev,cie_x,cie_y);
			return SET_ERROR(OTPCALI_ERROR_CCT);
		}

		//AWB
		uts.log.Debug(_T("Get WBinfo"));
		struct WB_DATA_SHORT wbtemp;
		ret = otpDB->GetOtpByType(mid, 3, (char *)& wbtemp, 6);
		if (ret < 0)
		{
			uts.log.Error(_T("Get AWB Data Error"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}

		put_be_val(wbtemp.RG, otp->awb.rg, sizeof(otp->awb.rg));
		put_be_val(wbtemp.BG, otp->awb.bg, sizeof(otp->awb.bg));
		put_be_val(wbtemp.GbGr, otp->awb.gbgr, sizeof(otp->awb.gbgr));
	
		int sum = (CheckSum(&otp->awb.rg, 6)%65535);
		put_be_val(sum,otp->awb.checkSum,2);

		//AWB station
		SYSTEMTIME stime;
		time_t time;
		otpDB->get_otp_data_lock_time(mid, &time);

		if (time < 0)
		{
			GetLocalTime(&stime);
			otp->awbststion.year  = stime.wYear % 100;
			otp->awbststion.month = (uint8_t)stime.wMonth;
			otp->awbststion.day   = (uint8_t)stime.wDay;
			otp->awbststion.hour   = (uint8_t)stime.wHour;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			otp->awbststion.year   = today.tm_year % 100;
			otp->awbststion.month  = (uint8_t)today.tm_mon + 1;
			otp->awbststion.day    = (uint8_t)today.tm_mday;
			otp->awbststion.hour   = (uint8_t)today.tm_hour;
		}

		otp->awbststion.ver = 0x01;
		otp->awbststion.number = 0x01;

		sum = (CheckSum(&otp->awbststion.year, 6)%65535);
		put_be_val(sum,otp->awbststion.checkSum,2);

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
	
		for(int i=0;i<QULCOMM_LSC_LEN/4;i+=2)
		{
			otp->lsc.lsc[221*2*0 +i]     = lscDB[221*2*1 + i + 1]; //R_H
			otp->lsc.lsc[221*2*0 +i+1]   = lscDB[221*2*1 + i];     //R_L
			otp->lsc.lsc[221*2*1 +i]     = lscDB[221*2*0 + i + 1]; //Gr_H
			otp->lsc.lsc[221*2*1 +i+1]   = lscDB[221*2*0 + i];     //Gr_L
			otp->lsc.lsc[221*2*2 +i]     = lscDB[221*2*2 + i + 1]; //GB_H
			otp->lsc.lsc[221*2*2 +i+1]   = lscDB[221*2*2 + i];     //GB_L
			otp->lsc.lsc[221*2*3 +i]     = lscDB[221*2*3 + i + 1]; //B_H
			otp->lsc.lsc[221*2*3 +i+1]   = lscDB[221*2*3 + i];     //B_L
		}

		sum = (CheckSum(&otp->lsc.lsc, 1768)%65535);
		put_be_val(sum,otp->lsc.checkSum,2);
		RELEASE_ARRAY(lscDB);		

		//LSC Station
		if (time < 0)
		{
			GetLocalTime(&stime);
			otp->lscstation.year  = stime.wYear % 100;
			otp->lscstation.month = (uint8_t)stime.wMonth;
			otp->lscstation.day   = (uint8_t)stime.wDay;
			otp->lscstation.hour   = (uint8_t)stime.wHour;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			otp->lscstation.year   = today.tm_year % 100;
			otp->lscstation.month  = (uint8_t)today.tm_mon + 1;
			otp->lscstation.day    = (uint8_t)today.tm_mday;
			otp->lscstation.hour   = (uint8_t)today.tm_hour;
		}

		otp->lscstation.ver = 0x01;
		otp->lscstation.number = 0x01;

		sum = (CheckSum(&otp->lscstation.year, 6)%65535);
		put_be_val(sum,otp->lscstation.checkSum,2);

		//pdaf
		uts.log.Debug(_T("Get PDAFinfo"));
		char *pdafbuf = new char[otp_dcc_len];
		
		ret = otpDB->GetOtpByType(mid, 8, pdafbuf,otp_dcc_len);
		if (ret < 0) 
		{
			uts.log.Error(_T("Get PDAF Data Error"));
			return SET_ERROR(OTPCALI_ERROR_PDAF_DCC);
		}

		otp->pdaf_flag  = 0x80;
		//copy gain map version, W, H and gain map
		memcpy(&otp->pdaf.version,pdafbuf,992);

		sum = (CheckSum(&otp->pdaf_flag, 993)%65535);
		put_be_val(sum,otp->pdaf.checkSum,2);
		RELEASE_ARRAY(pdafbuf);

		//PDAF Station
		if (time < 0)
		{
			GetLocalTime(&stime);
			otp->pdafstation.year  = stime.wYear % 100;
			otp->pdafstation.month = (uint8_t)stime.wMonth;
			otp->pdafstation.day   = (uint8_t)stime.wDay;
			otp->pdafstation.hour   = (uint8_t)stime.wHour;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			otp->pdafstation.year   = today.tm_year % 100;
			otp->pdafstation.month  = (uint8_t)today.tm_mon + 1;
			otp->pdafstation.day    = (uint8_t)today.tm_mday;
			otp->pdafstation.hour   = (uint8_t)today.tm_hour;
		}

		otp->pdafstation.ver = 0x01;
		otp->pdafstation.number = 0x01;

		sum = (CheckSum(&otp->pdafstation.year, 6)%65535);
		put_be_val(sum,otp->pdafstation.checkSum,2);

		//AF
		int inf = 0, marcro = 0;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0)
		{
			uts.log.Error(_T("Get AF Data Error"));
			return SET_ERROR(OTPCALI_ERROR_NODATA);
		}
		uts.log.Debug(_T("Get AF inf:%d mup:%d"),inf,marcro);
		/*AF_PARAM *paf = &otp_param.af;
		inf -= paf->off_inf;
		marcro -= paf->off_mac;*/

		memset(&otp->af.reserved1,0xFF,4);
		put_be_val(inf, otp->af.inf, sizeof(otp->af.inf));
		put_be_val(marcro, otp->af.mup, sizeof(otp->af.mup));
		memset(&otp->af.reserved2,0xFF,4);

		sum = (CheckSum(&otp->af.inf, 4)%65535);
		put_be_val(sum,otp->af.checksum,2);

		//AF Station
		if (time < 0)
		{
			GetLocalTime(&stime);
			otp->afstation.year  = stime.wYear % 100;
			otp->afstation.month = (uint8_t)stime.wMonth;
			otp->afstation.day   = (uint8_t)stime.wDay;
			otp->afstation.hour   = (uint8_t)stime.wHour;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			otp->afstation.year   = today.tm_year % 100;
			otp->afstation.month  = (uint8_t)today.tm_mon + 1;
			otp->afstation.day    = (uint8_t)today.tm_mday;
			otp->afstation.hour   = (uint8_t)today.tm_hour;
		}

		otp->afstation.ver = 0x01;
		otp->afstation.number = 0x01;

		sum = (CheckSum(&otp->afstation.year, 6)%65535);
		put_be_val(sum,otp->afstation.checkSum,2);

		//
		memset(otp->reserved,0xFF,4135);

		//Total Check sum
		sum = (CheckSum(&otp->minfo.year, 2866)%65535);
		put_be_val(sum,otp->totalchecksum,2);

		return sizeof(OTPData);
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA82L::DCC0Cali(void *args)
	{
		return  get_qulcomm_pd_REVL();
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA82L::do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len)
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_OV13855();
		return pdaf.SPCCali(pRaw10,width,height,out);
	}

	int OtpCali_OV13855_MA82L::do_DCCCali(uint8_t *SPCTable,uint16_t **imagebuf, 
		int16_t *dcc_stack_lenspos, uint8_t out[])
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_OV13855();
		return pdaf.DCCCali(SPCTable,imagebuf,dcc_stack_lenspos,out);
	}

	int OtpCali_OV13855_MA82L::PDAFVerify(void *args)
	{
		return qulcomm_dcc_verify_REL();
	}

	int OtpCali_OV13855_MA82L::do_qulcomm_DCCVerify_REV(uint8_t *DCCTable,uint16_t *img,int& DAC)
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_OV13855();
		return pdaf.DCCVerify(img,DCCTable,DAC);

	}

}
