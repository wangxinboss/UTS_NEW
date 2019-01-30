#include "StdAfx.h"
#include "OtpCali_S5K2L7_MA705.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_S5K2L7_MA705 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K2L7_MA705::OtpCali_S5K2L7_MA705(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		otp_spc_len = QULCOMM_SPC_LEN;
		otp_dcc_len = QULCOMM_DCC_LEN;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K2L7_MA705::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K2L7_MA705::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K2L7_MA705::LscCali(void *args)
	{
		//return do_mtk_lsc_cali();
		return do_qulcomm_lsc_awb_cali();
	}
	//-------------------------------------------------------------------------------------------------

	int OtpCali_S5K2L7_MA705::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K2L7_MA705::get_minfo_from_db(void *args)
	{
// 		struct MINFO
// 		{	
// 			uint8_t module;
// 			uint8_t year;
// 			uint8_t month;
// 			uint8_t day;
// 			uint8_t supplier;
// 			uint8_t sensor_type;
// 			uint8_t lens;
// 			uint8_t vcm;
// 			uint8_t driver_ic;
// 			uint8_t ir_bg;
// 			uint8_t mirror_flip;
// 			uint8_t sum[2];
// 		};
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->minfo_flag=0x01;	//0x00: Invalid, 0x01: Valid
		m->module = 0x03;	//MA702:0x01, MA704:0x02, MA705:0x03
		SYSTEMTIME stime;
		time_t time;
		if (otpDB->GetOtpBurnTime(mid, &time) < 0)
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
		m->supplier = 0x08;		//0x08: Holitech
		m->sensor_type = 0x02;	//0x01: IMX362, 0x02: 2L7SA03	
		m->lens = 0x01;			//0x01: 60023A1, 0x02: C5501A
		m->vcm = 0x01;			//0x01: CL-18, 0x02: ATMJ1Z952A
		m->vcmdriver = 0x01;	//0x01: AK7374, 0x02: DW9800W
		m->ir_bg = 0x02;		//0x01: IR, 0x02: BG, 0x03: BMBG
		m->mirror_flip = 0x00;	//0x00: normal, 0x01:mirror, 0x02: flip, 0x03: mirror+flip
		m->reserved[0] = 0xFF;	
		m->reserved[1] = 0xFF;

		//memcpy(&m->sn, T2A(m_szSN), 10);
		m->sum[0] = (CheckSum(&m->minfo_flag, sizeof(MINFO)-2)>>8) & 0xFF;
		m->sum[1] = (CheckSum(&m->minfo_flag, sizeof(MINFO)-2)) & 0xFF;
		

		return sizeof(MINFO);
	}
	int OtpCali_S5K2L7_MA705::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		//module info
		uts.log.Debug(_T("Get Minfo"));
		int ret = get_minfo_from_db(&otp->minfo);
				
		//AF
		uts.log.Debug(_T("Get AFinfo"));
		int inf = 0, marcro = 0;

		otp->af.af_flag=0x01;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0) return ret;
		put_be_val(marcro, otp->af.mup, sizeof(otp->af.mup));
		put_be_val(inf, otp->af.inf, sizeof(otp->af.inf));
		for(int i=0;i<6;i++)
			otp->af.reserved[i]=0xFF;
		otp->af.sum[0] = (CheckSum(&otp->af.af_flag, sizeof(AF)-2)>>8) & 0xFF;
		otp->af.sum[1] = CheckSum(&otp->af.af_flag, sizeof(AF)-2) & 0xFF;

		//Reserved 25 bytes

		for(int i=0;i<25;i++)
			otp->reserved[i]=0xFF;

		//WB
		uts.log.Debug(_T("Get WBinfo"));
		struct WB_DATA_SHORT wbtemp[2];
		ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB, (unsigned short*)wbtemp, 12);
		if (ret < 0) { return ret;}
		ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
			3, (char *)& wbtemp[1], 6);
		//if (ret < 0) return ret;

		otp->wb.wb_flag=0x01;
		//Golden
		put_be_val(wbtemp[0].RG, otp->wb.rg, sizeof(otp->wb.rg));
		put_be_val(wbtemp[0].BG, otp->wb.bg, sizeof(otp->wb.bg));
		put_be_val(wbtemp[0].GbGr, otp->wb.gbgr, sizeof(otp->wb.gbgr));
		put_be_val(wbtemp[1].RG, otp->wb.rg_g, sizeof(otp->wb.rg_g));
		put_be_val(wbtemp[1].BG, otp->wb.bg_g, sizeof(otp->wb.bg_g));
		put_be_val(wbtemp[1].GbGr, otp->wb.gbgr_g, sizeof(otp->wb.gbgr_g));
		otp->wb.reserved[0]=0xFF;
		otp->wb.reserved[1]=0xFF;
		otp->wb.sum[0] = (CheckSum(&otp->wb.wb_flag, sizeof(WB)-2)>>8) & 0xFF;
		otp->wb.sum[1] = CheckSum(&otp->wb.wb_flag, sizeof(WB)-2) & 0xFF;

		//LSC
		uts.log.Debug(_T("Get LSCinfo"));
		uint8_t templsc=0x00;
		otp->lsc.lsc_flag = 0x01;
		//otp->lsc.lscwidth = 17;
		//otp->lsc.lscheight = 13;
		ret = get_lsc_from_raw_data(otp->lsc.lsc, sizeof(otp->lsc.lsc));
		if (ret < 0) return ret;
		for(int i=0;i<sizeof(otp->lsc.lsc);i+=2)
		{
			templsc=otp->lsc.lsc[i];
			otp->lsc.lsc[i]=otp->lsc.lsc[i+1];
			otp->lsc.lsc[i+1]=templsc;
			templsc=0;
		}
		otp->lsc.sum[0] = (CheckSum(&otp->lsc.lsc_flag, sizeof(LSC)-2)>>8) & 0xFF;
		otp->lsc.sum[1] = CheckSum(&otp->lsc.lsc_flag, sizeof(LSC)-2) & 0xFF;

		//pdaf
		uts.log.Debug(_T("Get PDAFinfo"));
		char *pdafbuf = new char[otp_dcc_len];

		otp->pdaf.pdaf_flag = 0x01;
		ret = otpDB->GetOtpByType(mid, 8, pdafbuf,otp_dcc_len);
		if (ret < 0) return ret;
		//copy gain map version, W, H and gain map
		memcpy(&otp->pdaf.cal_version,pdafbuf,6+221*2*2);

		otp->pdaf.gm_sum[0] = (CheckSum(&otp->pdaf.pdaf_flag, 891)>>8) & 0xFF;
		otp->pdaf.gm_sum[1] = (CheckSum(&otp->pdaf.pdaf_flag, 891)) & 0xFF;
		//copy dcc version, W , H and dcc map
		memcpy(&otp->pdaf.dm_q_format,pdafbuf+6+221*2*2,6+48*2);
		otp->pdaf.dm_sum[0] = (CheckSum(&otp->pdaf.dm_q_format, 102)>>8) & 0xFF;
		otp->pdaf.dm_sum[1] = (CheckSum(&otp->pdaf.dm_q_format, 102)) & 0xFF;
		//memcpy(&otp->pdaf.dm_q_format,dccbuf,otp_dcc_len);
		RELEASE_ARRAY(pdafbuf);

// 		char *pdafbuf = new char[sizeof(PDAF)];
// 
// 		ret = otpDB->GetOtpByType(mid, 8, pdafbuf,sizeof(PDAF));
// 		if (ret < 0) return ret;
// 		memcpy(&otp->pdaf,pdafbuf,sizeof(PDAF));
// 		RELEASE_ARRAY(pdafbuf);
		
		return sizeof(OTPData);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K2L7_MA705::DCC0Cali(void *args)
	{
		return  get_qulcomm_2pd_REVL();
	}


	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K2L7_MA705::PDAFVerify(void *args)
	{
		return 0;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K2L7_MA705:: SPCCali(void *args)
	{
		return SPCCali_2PD();
	}
	
	
	int OtpCali_S5K2L7_MA705::do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len)
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_2L7();
		return pdaf.SPCCali_2PD(pRaw10,width,height,out);
	}

	int OtpCali_S5K2L7_MA705::do_DCCCali(uint8_t *SPCTable,uint16_t **imagebuf, 
		int16_t *dcc_stack_lenspos, uint8_t out[])
	{
		PDAF_QulComm_RevL pdaf;

		pdaf.get_sensor_cfg_2L7();
		return pdaf.DCCCali_2PD(SPCTable,imagebuf,dcc_stack_lenspos,out);
	}

}
