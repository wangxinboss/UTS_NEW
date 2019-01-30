#include "StdAfx.h"
#include "OtpCali_Hi846_MF82K.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"
//#include "PDAF_QulComm_RevJ.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------
//typedef struct CalibrationDataStruct
//{
//	unsigned short int * R_LSC;
//	unsigned short int * Gr_LSC;
//	unsigned short int * Gb_LSC;
//	unsigned short int * B_LSC;
//	unsigned short int * AWB;
//	unsigned short int   R_Max;
//	unsigned short int   Gr_Max;
//	unsigned short int   Gb_Max;
//	unsigned short int   B_Max;
//} CalibrationDataStruct;

//#define RGGB_PATTERN	0
//#define GRBG_PATTERN	1
//#define BGGR_PATTERN	2
//#define GBRG_PATTERN	3

typedef unsigned char BYTE;

namespace OtpCali_Hi846_MF82K {
	//-------------------------------------------------------------------------------------------------
	OtpCali_Hi846_MF82K::OtpCali_Hi846_MF82K(BaseDevice *dev) : HynixOtp(dev)
	{
		otp_type = OTP_TYPE_OTP;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_Hi846_MF82K::do_prog_otp()
	{

		if(!sensor->do_prog_otp(0x0,0x0201,otp_data_in_db,sizeof(MINFO)))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		if(!sensor->do_prog_otp(0x0,0x0235,otp_data_in_db+sizeof(MINFO),sizeof(WB)))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		if(!sensor->do_prog_otp(0x0,0x0290,otp_data_in_db+sizeof(MINFO)+sizeof(WB),sizeof(LSC)))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_Hi846_MF82K::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if(!sensor->do_read_otp(0x0,0x0201,&otp->minfo.basi_flag,sizeof(MINFO)))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		if(!sensor->do_read_otp(0x0,0x0235,&otp->wb.wb_flag,sizeof(WB)))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		if(!sensor->do_read_otp(0x0,0x0290,&otp->lsc.lsc_flag,sizeof(LSC)))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_Hi846_MF82K::LscCali(void *args)
	{
		return do_qulcomm_lsc_awb_cali();
	}
	//-------------------------------------------------------------------------------------------------

	int OtpCali_Hi846_MF82K::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
	
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_Hi846_MF82K::get_minfo_from_db(void *args)
	{

		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->basi_flag=0x01;	//
		m->mid = 0x42;	//
		m->af_ff_flag = 0x00;

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

		m->sensor = 0x00;	    //0x00
		m->lens = 0x00;			//0x00 
		m->vcm = 0x00;			//0x00 
		m->vcmdriver = 0x00;	//0x00
		m->version =0x01;

		memset(m->reserved,0x00,sizeof(m->reserved));
		m->sum = CheckSum(&m->mid, sizeof(MINFO)-8) % 0xFF + 1;
		
		return sizeof(MINFO);
	}
	int OtpCali_Hi846_MF82K::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;
		//WB_PARAM *pwb = &otp_param.wb_param;
		USES_CONVERSION;

		//module info
		uts.log.Debug(_T("Get Minfo"));
		int ret = get_minfo_from_db(&otp->minfo);				

		//WB

		uts.log.Debug(_T("Get WBinfo"));
		otp->wb.wb_flag=0x01;

		struct WB_DATA_USHORT wbtemp[2];
		ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB, (unsigned short*)wbtemp, 16);
		if (ret < 0) { return ret;}

		
		//Golden
/*		ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
			3, (char *)& wbtemp[1], 6);
		if (ret < 0) return ret;*/

		WB_RATIO ratio, ratio_g;
		get_wb_ratio(&wbtemp[0], &ratio, 1023);

		uts.log.Info(_T("5100K:r_gr=0x%04x,b_gb=0x%04x,gb_gr=0x%04x"),ratio.r_gr,ratio.b_gb,ratio.gb_gr);

		ratio.r_gr = int(ratio.r_gr * otp_param.wb_param.rg_multi);
		ratio.b_gb = int(ratio.b_gb * otp_param.wb_param.bg_multi);

		uts.log.Info(_T("otp_param.wb_param.multi:r_grmutil=%04f,br_grmutil=%04f"),otp_param.wb_param.rg_multi,otp_param.wb_param.bg_multi);
		uts.log.Info(_T("Mutil_After:r_gr=0x%04x,b_gb=0x%04x,gb_gr=0x%04x"),ratio.r_gr,ratio.b_gb,ratio.gb_gr);

		put_be_val(ratio.r_gr, otp->wb.r_gr, sizeof(otp->wb.r_gr));
		put_be_val(ratio.b_gb, otp->wb.b_gb, sizeof(otp->wb.b_gb));
		put_be_val(ratio.gb_gr, otp->wb.gb_gr, sizeof(otp->wb.gb_gr));

		put_be_val(519, otp->wb.r_gr_g, sizeof(otp->wb.r_gr_g));
		put_be_val(636, otp->wb.b_gb_g, sizeof(otp->wb.b_gb_g));
		put_be_val(1023, otp->wb.gb_gr_g, sizeof(otp->wb.gb_gr_g));

		memset(otp->wb.reserved,0x00,sizeof(otp->wb.reserved));

		otp->wb.sum = CheckSum(otp->wb.r_gr, sizeof(WB)-19) % 0xFF + 1;

		//LSC
		uts.log.Debug(_T("Get LSCinfo"));

		otp->lsc.lsc_flag = 0x01;
		
		uint8_t *lscDB = new uint8_t[QULCOMM_LSC_LEN+8];
		ret = get_lsc_from_raw_data(lscDB, QULCOMM_LSC_LEN+8);
		if (ret < 0) return ret;

		//ret = get_lsc_from_raw_data(otp->lsc.lsc, sizeof(otp->lsc.lsc));
		//if (ret < 0) return ret;

		for(int i=0;i<221;i+=1)
		{
			otp->lsc.lsc[221*2*0+2*i]	 =lscDB[221*2*1+2*i+1];	//R_H
			otp->lsc.lsc[221*2*0+2*i+1]  =lscDB[221*2*1+2*i];		//R_L
			otp->lsc.lsc[221*2*1+2*i]    =lscDB[221*2*0+2*i+1];	//Gr_H
			otp->lsc.lsc[221*2*1+2*i+1]  =lscDB[221*2*0+2*i];		//Gr_L
			otp->lsc.lsc[221*2*2+2*i]    =lscDB[221*2*2+2*i+1];	//Gb_H
			otp->lsc.lsc[221*2*2+2*i+1]  =lscDB[221*2*2+2*i];		//Gb_L
			otp->lsc.lsc[221*2*3+2*i]    =lscDB[221*2*3+2*i+1];	//B_H
			otp->lsc.lsc[221*2*3+2*i+1]  =lscDB[221*2*3+2*i];		//B_L
		}

		otp->lsc.sum = CheckSum(otp->lsc.lsc, otp_lsc_len) % 0xFF + 1;

		RELEASE_ARRAY(lscDB);
		
		return sizeof(OTPData);
	}

	int OtpCali_Hi846_MF82K::do_check_otp(void *args)
	{
		OTPData *otp = (OTPData*)args;

		int ret = OTPCALI_ERROR_CHKERR;

		//minfo
		if(otp->minfo.basi_flag!=0x01) {
			uts.log.Error(_T("otp_off_chk: basi_flag =0x%02x error,should be 0x01"),otp->minfo.basi_flag);
			return SET_ERROR(ret);
		}
		if(otp->minfo.mid!=0x42) {
			uts.log.Error(_T("otp_off_chk: module id =0x%02x error,should be 0x01"),otp->minfo.mid);
			return SET_ERROR(ret);
		}
		if(otp->minfo.year<0x11 || otp->minfo.year>0x17) {
			uts.log.Error(_T("otp_off_chk: year =0x%02x error,should be in [0x11,0x17]"),otp->minfo.year);
			return SET_ERROR(ret);
		}
		if(otp->minfo.month<0x01 || otp->minfo.month>0x0c) {
			uts.log.Error(_T("otp_off_chk: month =0x%02x error,should be in [0x01,0x0c]"),otp->minfo.month);
			return SET_ERROR(ret);
		}
		if(otp->minfo.day<0x01 || otp->minfo.day>0x1f) {
			uts.log.Error(_T("otp_off_chk: day =0x%02x error,should be in [0x01,0x1f]"),otp->minfo.day);
			return SET_ERROR(ret);
		}
		if(otp->minfo.sensor!=0x00){
			uts.log.Error(_T("otp_off_chk: sensor_id =0x%02x error,should be 0x01"),otp->minfo.sensor);
			return SET_ERROR(ret);
		}
		if(otp->minfo.lens!=0x00){
			uts.log.Error(_T("otp_off_chk: lens_id =0x%02x error,should be 0x01"),otp->minfo.lens);
			return SET_ERROR(ret);
		}
		if(otp->minfo.vcm!=0x00){
			uts.log.Error(_T("otp_off_chk: vcm_id =0x%02x error,should be 0x01"),otp->minfo.vcm);
			return SET_ERROR(ret);
		}
		if(otp->minfo.vcmdriver!=0x00){
			uts.log.Error(_T("otp_off_chk: driveric_id =0x%02x error,should be 0x01"),otp->minfo.vcmdriver);
			return SET_ERROR(ret);
		}
		if(otp->minfo.version!=0x01){
			uts.log.Error(_T("otp_off_chk: version_id =0x%02x error,should be 0x01"),otp->minfo.version);
			return SET_ERROR(ret);
		}
		if(!is_mem_val(otp->minfo.reserved,0x00,sizeof(otp->minfo.reserved))) {
			uts.log.Error(_T("otp_off_chk: minfo.Reserve should be 0x00 "));
			return SET_ERROR(ret);
		}

		int sum = CheckSum(&otp->minfo.mid, sizeof(MINFO)-2) % 255 + 1;
		if(otp->minfo.sum != sum) {
			uts.log.Error(_T("otp_off_chk: minfo.sum =0x%02x error,should be 0x%02x"),otp->minfo.sum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: minfo check pass"));

		//wb
		if(otp->wb.wb_flag!=0x01) {
			uts.log.Error(_T("otp_off_chk: wb_flag =0x%02x error,should be 0x01"),otp->wb.wb_flag);
			return SET_ERROR(ret);
		}

		WB_RATIO ratio , ratio_g;
		WB_PARAM *pwb = &otp_param.wb_param;

		ratio.r_gr = (otp->wb.r_gr[0]*256+otp->wb.r_gr[1]);
		ratio.b_gb = (otp->wb.b_gb[0]*256+otp->wb.b_gb[1]);

		ratio_g.r_gr = (otp->wb.r_gr_g[0]*256+otp->wb.r_gr_g[1]);
		ratio_g.b_gb = (otp->wb.b_gb_g[0]*256+otp->wb.b_gb_g[1]);
		ratio_g.gb_gr = (otp->wb.gb_gr_g[0]*256+otp->wb.gb_gr_g[1]);

		pwb->rg_delta = abs(ratio.r_gr - ratio_g.r_gr)/1023;
		pwb->bg_delta = abs(ratio.b_gb - ratio_g.b_gb)/1023;

		if (pwb->rg_delta > pwb->rg_delta_spec) {
			uts.log.Error(_T("otp_off_chk: ratio.dr_g =%.3f rg_target =%.3f"), ratio.dr_g, pwb->rg_target);
			uts.log.Error(_T("otp_off_chk: rg_delta =%.3f error,should be in [%.3f,%.3f]"), pwb->rg_delta, 0.0, pwb->rg_delta_spec);
			return SET_ERROR(ret);
		}
		if (pwb->bg_delta > pwb->bg_delta_spec) {
			uts.log.Error(_T("otp_off_chk: ratio.db_g =%.3f bg_target =%.3f"), ratio.db_g, pwb->bg_target);
			uts.log.Error(_T("otp_off_chk: bg_delta =%.3f error,should be in [%.3f,%.3f]"), pwb->bg_delta, 0.0, pwb->bg_delta_spec);
			return SET_ERROR(ret);
		}

		if(ratio_g.r_gr!= 519) {
			uts.log.Error(_T("otp_off_chk: r_gr_golden =0x%04x error,should be 0x%04x"),ratio_g.r_gr,519);
			return SET_ERROR(ret);
		}
		if(ratio_g.b_gb!= 636) {
			uts.log.Error(_T("otp_off_chk: b_gb_golden =0x%04x error,should be 0x%04x"),ratio_g.b_gb,636);
			return SET_ERROR(ret);
		}
		if(ratio_g.gb_gr!= 1023) {
			uts.log.Error(_T("otp_off_chk: gb_gr_golden =0x%04x error,should be 0x%04x"),ratio_g.gb_gr,1023);
			return SET_ERROR(ret);
		}

		if(!is_mem_val(otp->wb.reserved,0x00,sizeof(otp->wb.reserved))) {
			uts.log.Error(_T("otp_off_chk: wb.Reserve should be 0xff "));
			return SET_ERROR(ret);
		}

		sum = CheckSum(&otp->wb.r_gr, sizeof(WB)-2) % 255 + 1;
		if(otp->wb.sum != sum) {
			uts.log.Error(_T("otp_off_chk: wb.sum =0x%02x error,should be 0x%02x"),otp->wb.sum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: wb check pass"));


		//lsc
		if(otp->lsc.lsc_flag!=0x01) {
			uts.log.Error(_T("otp_off_chk: lsc_flag =0x%02x error,should be 0x01"),otp->lsc.lsc_flag);
			return SET_ERROR(ret);
		}

		sum = CheckSum(otp->lsc.lsc, sizeof(LSC)-2) % 255 + 1;
		if(otp->lsc.sum != sum) {
			uts.log.Error(_T("otp_off_chk: lsc.sum =0x%02x error,should be 0x%02x"),otp->lsc.sum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: lsc check pass"));
		

		ret = OTPCALI_ERROR_NO;
		return SET_ERROR(ret);

	}
	
}
