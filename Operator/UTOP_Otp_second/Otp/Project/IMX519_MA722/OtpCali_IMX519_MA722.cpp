#include "StdAfx.h"
#include "OtpCali_IMX519_MA722.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"
//#include "PDAF_QulComm_RevJ.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------
typedef int (/*WINAPI */ *Dllfun1) (char* ini_file_path , short *pRaw10, short *pLRC);
typedef int (/*WINAPI */ *Dllfun2) (char* file_ini_path , short *pRaw10, int *judge);
typedef int (/*WINAPI */ *Dllfun3) (char *sensor, int lens1, int lens2, int *pd1, int *pd2, int h, int v, unsigned short *dcc_out, int calc_mode);
typedef int (/*WINAPI */ *Dllfun4) (char* sensor , unsigned short *dcc, int *pd, int h_num ,int v_num, int *dcc_out, int *pd_out ,int calc_mode);

namespace OtpCali_IMX519_MA722 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_IMX519_MA722::OtpCali_IMX519_MA722(BaseDevice *dev) : SonyOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		otp_spc_len = IMX_519_SPC_LEN;
		otp_dcc_len = IMX_519_DCC_LEN;
		pdaf_pd_len  = IMX_519_PD_LEN;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX519_MA722::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX519_MA722::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX519_MA722::LscCali(void *args)
	{
		return do_qulcomm_lsc_awb_cali();
	}
	//-------------------------------------------------------------------------------------------------

	int OtpCali_IMX519_MA722::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX519_MA722::get_minfo_from_db(void *args)
	{

		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->minfo_flag=0x01;	//0x00: Invalid, 0x01: Valid
		m->minfo_version = 0x01;	//MA702:0x01, MA704:0x02, MA705:0x03
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
		m->sensor_type = 0x01;	//0x01: IMX519
		m->lens = 0x01;			//0x01: 
		m->vcm = 0x01;			//0x01: 
		m->vcmdriver = 0x01;	//0x01: DW9800W

		memset(m->reserved,0xff,sizeof(m->reserved));
		m->sum = CheckSum(&m->minfo_version, sizeof(MINFO)-2) % 0xFF;
		
		return sizeof(MINFO);
	}
	int OtpCali_IMX519_MA722::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		//module info
		uts.log.Debug(_T("Get Minfo"));
		int ret = get_minfo_from_db(&otp->minfo);				

		//WB
		otp->wb.wb_flag=0x01;
		otp->wb.wb_version=0x01;

		uts.log.Debug(_T("Get WBinfo"));

		struct WB_DATA_SHORT wbtemp[2];
		ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB, (unsigned short*)wbtemp, 12);
		if (ret < 0) { return ret;}

		//Golden
/*		ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
			3, (char *)& wbtemp[1], 6);
		if (ret < 0) return ret;*/


		put_be_val(wbtemp[0].RG, otp->wb.rg, sizeof(otp->wb.rg));
		put_be_val(wbtemp[0].BG, otp->wb.bg, sizeof(otp->wb.bg));
		put_be_val(wbtemp[0].GbGr, otp->wb.gbgr, sizeof(otp->wb.gbgr));

		memset(otp->wb.reserved,0xff,sizeof(otp->wb.reserved));

		otp->wb.sum = CheckSum(&otp->wb.rg, sizeof(WB)-3) % 0xFF;

		//LSC
		uts.log.Debug(_T("Get LSCinfo"));
		uint8_t templsc=0x00;
		otp->lsc.lsc_flag = 0x01;
		otp->lsc.lsc_version=0x01;
		ret = get_lsc_from_raw_data(otp->lsc.lsc, sizeof(otp->lsc.lsc));
		if (ret < 0) return ret;

		otp->lsc.sum = CheckSum(otp->lsc.lsc, otp_lsc_len) % 0xFF;

		//AF
		uts.log.Debug(_T("Get AFinfo"));
		int inf = 0, marcro = 0;

		otp->af.af_flag=0x01;

		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0) return ret;

		put_be_val(inf, otp->af.inf, sizeof(otp->af.inf));
		put_be_val(marcro, otp->af.mup, sizeof(otp->af.mup));
		
		memset(otp->af.reserved,0xff,sizeof(otp->af.reserved));

		otp->af.sum = CheckSum(otp->af.inf, sizeof(AF)-2) % 0xFF;

		//pdaf
		uts.log.Debug(_T("Get PDAFinfo"));

		char *spcbuf = new char[otp_spc_len];
		char *dccbuf = new char[otp_dcc_len];

		otp->pdaf.spc_flag = 0x01;
		otp->pdaf.dcc_flag = 0x01;

		ret = otpDB->GetOtpByType(mid, 12, spcbuf,otp_spc_len);
		if (ret < 0) {		
			RELEASE_ARRAY(spcbuf);
		    RELEASE_ARRAY(dccbuf);
			return ret;
		}

		ret = otpDB->GetOtpByType(mid, 8, dccbuf,otp_dcc_len);
		if (ret < 0) {		
			RELEASE_ARRAY(spcbuf);
			RELEASE_ARRAY(dccbuf);
			return ret;
		}

		//copy spc map & dcc map

		memcpy(otp->pdaf.spc_map,spcbuf,otp_spc_len);
		memcpy(otp->pdaf.dcc_map,dccbuf,otp_dcc_len);

		otp->pdaf.spc_sum = (CheckSum(otp->pdaf.spc_map, otp_spc_len)) % 0xFF;
		otp->pdaf.dcc_sum = (CheckSum(otp->pdaf.dcc_map, otp_dcc_len)) % 0xFF;

		RELEASE_ARRAY(spcbuf);
		RELEASE_ARRAY(dccbuf);
		
		return sizeof(OTPData);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX519_MA722:: SPCCali(void *args)
	{
		return SONYSPCCali();
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX519_MA722::DCC0Cali(void *args)
	{
		return  do_sony_dcc_cali();
	}

	int OtpCali_IMX519_MA722::PDAFVerify(void *args)
	{
		return sony_dcc_verify();
	}

	int OtpCali_IMX519_MA722::do_SONY_SPCCali(unsigned short *pRaw10, short *out)
	{
		USES_CONVERSION;

		int ret = -1;
		FILE *fp;
		HINSTANCE hdll;
		Dllfun1 Sony_PDAF_LRC;

		short * temp_pRaw10 = (short *)pRaw10;
		short * temp_out = (short *)out;

		fp = fopen(T2A(otp_param.pdaf_param.pdafcal_ini_file),"rt");
		if(fp==NULL) { return ret;}
		fclose(fp);

		hdll = LoadLibrary(_T("Sony_PDAF_LRC.dll"));	
		if(hdll==NULL) { FreeLibrary(hdll);return ret;}

		Sony_PDAF_LRC = (Dllfun1)GetProcAddress(hdll,"Sony_PDAF_LRC");
		if(Sony_PDAF_LRC==NULL) { FreeLibrary(hdll);return ret;}

		ret = Sony_PDAF_LRC (T2A(otp_param.pdaf_param.pdafcal_ini_file),temp_pRaw10, temp_out);

		if(hdll!=NULL) { FreeLibrary(hdll);hdll =NULL;}

		return ret;		
	}
	int OtpCali_IMX519_MA722::do_SONY_SPCVerify(unsigned short *pRaw10,int *judge)
	{
		USES_CONVERSION;

		int ret = -1;
		HINSTANCE hdll;
		Dllfun2 Sony_PDAF_LRC_Judge;

		hdll = LoadLibrary(_T("Sony_PDAF_LRC.dll"));
		if(hdll==NULL) { FreeLibrary(hdll);return ret;}

		Sony_PDAF_LRC_Judge = (Dllfun2)GetProcAddress(hdll,"Sony_PDAF_LRC_Judge");
		if(Sony_PDAF_LRC_Judge==NULL) { FreeLibrary(hdll);return ret;}

		ret = Sony_PDAF_LRC_Judge (T2A(otp_param.pdaf_param.pdafcal_ini_file), (short*)pRaw10, judge);

		FreeLibrary(hdll);
		return ret;
	}
	int OtpCali_IMX519_MA722::do_SONY_DCCCali(int lens1, int lens2, int *pd1, int *pd2,unsigned short *dcc_out, int calc_mode)
	{
		int ret = -1;
		HINSTANCE hdll;
		Dllfun3 Sony_PDAF_DCC;

		hdll = LoadLibrary(_T("Sony_PDAF_LRC.dll"));
		if(hdll==NULL) { FreeLibrary(hdll);return ret;}

		Sony_PDAF_DCC = (Dllfun3)GetProcAddress(hdll,"Sony_PDAF_DCC");
		if(Sony_PDAF_DCC==NULL) { FreeLibrary(hdll);return ret;}

		ret = Sony_PDAF_DCC("IMX519", lens1,   
			                          lens2, 
			                          pd1, 
									  pd2, 8, 6, 
									  dcc_out,   
									  calc_mode);

		FreeLibrary(hdll);
		return ret;
	}
	int OtpCali_IMX519_MA722::do_SONY_DCCVerify(unsigned short*dcc, int *pd, int *dcc_out, int *pd_out ,int calc_mode)
	{
		int ret = -1;
		HINSTANCE hdll;
		Dllfun4 Sony_PDAF_DCC_verify;

		hdll = LoadLibrary(_T("Sony_PDAF_LRC.dll"));
		if(hdll==NULL) { FreeLibrary(hdll);return ret;}

		Sony_PDAF_DCC_verify = (Dllfun4)GetProcAddress(hdll,"Sony_PDAF_DCC_verify");
		if(Sony_PDAF_DCC_verify==NULL) { FreeLibrary(hdll);return ret;}

		ret = Sony_PDAF_DCC_verify("IMX519",dcc,
			                                pd,8,6,
											dcc_out,
										    pd_out,
											calc_mode);

		FreeLibrary(hdll);
		return ret;
	}
	int OtpCali_IMX519_MA722::do_WriteSPCSetting1()
	{
		sensor->dev->write_sensor(0x0101 , 0x00);
		sensor->dev->write_sensor(0x0B00 , 0x00);
		sensor->dev->write_sensor(0x3606 , 0x00);
		sensor->dev->write_sensor(0x3E36 , 0x00);
		sensor->dev->write_sensor(0x3E35 , 0x00);
		sensor->dev->write_sensor(0x3E37 , 0x01);
		sensor->dev->write_sensor(0x3D0D , 0x01);
		sensor->dev->write_sensor(0x3D10 , 0x00);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	int OtpCali_IMX519_MA722::do_WriteSPCSetting2()
	{
		sensor->dev->write_sensor(0x0101 , 0x00);
		sensor->dev->write_sensor(0x0B00 , 0x00);
		sensor->dev->write_sensor(0x3606 , 0x00);
		sensor->dev->write_sensor(0x3E36 , 0x00);
		sensor->dev->write_sensor(0x3E35 , 0x01);
		sensor->dev->write_sensor(0x3E37 , 0x01);
		sensor->dev->write_sensor(0x3D0D , 0x01);
		sensor->dev->write_sensor(0x3D10 , 0x00);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	int OtpCali_IMX519_MA722::do_WriteDCCSetting3()
	{
		sensor->dev->write_sensor(0x0101 , 0x00);
		sensor->dev->write_sensor(0x0B00 , 0x00);
		sensor->dev->write_sensor(0x3606 , 0x01);
		sensor->dev->write_sensor(0x3E36 , 0x01);
		sensor->dev->write_sensor(0x3E35 , 0x01);
		sensor->dev->write_sensor(0x3E37 , 0x01);
		sensor->dev->write_sensor(0x3D0D , 0x01);
		sensor->dev->write_sensor(0x3D10 , 0x00);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}

	int OtpCali_IMX519_MA722::do_SPC_writeback(uint8_t *regs)
	{
		uint8_t temp_read[140] = {0};

		sensor->dev->write_sensor(0x0100 , 0x00);
		sensor->dev->write_sensor(0x7520,regs,70);
		Sleep(20);
		sensor->dev->write_sensor(0x7568,regs+70,70);
		Sleep(20);

		sensor->dev->read_sensor(0x7520,temp_read,70);
		Sleep(20);
		sensor->dev->read_sensor(0x7568,temp_read+70,70);
		sensor->dev->write_sensor(0x0100 , 0x01);

		if(memcmp(regs,temp_read,140)){
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		}

		return SET_ERROR(OTPCALI_ERROR_NO);
	}

	int OtpCali_IMX519_MA722::do_SONY_DCC_stats(int mode)
	{

		sensor->dev->write_sensor(0xe20f,(uint8_t)(mode&0xff));
		Sleep(5);

		if(mode!=sensor->dev->read_sensor(0xe20f))
			return SET_ERROR(OTPCALI_ERROR_PDAF_DCC);
		
		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	int OtpCali_IMX519_MA722::do_SONY_GetPDTable(int *pdtable)
	{
		int * tempPDTable = (int*)pdtable;

		uint8_t tempval[1024];
		uint16_t tempreg[1024];
		uint16_t tempConfidenceLevel[1024];

		memset(tempval,0,1024);
		memset(tempConfidenceLevel,0,1024);
		int cnt = 0;
		for (int i=0;i<96;i++){
			tempval[i*4+0]=sensor->dev->read_sensor(0xc808+i*4+0);
			tempval[i*4+1]=sensor->dev->read_sensor(0xc808+i*4+1);
			tempval[i*4+2]=sensor->dev->read_sensor(0xc808+i*4+2);
			tempval[i*4+3]=sensor->dev->read_sensor(0xc808+i*4+3);

			if((i%16>=0)&&(i%16<=7)){
				tempConfidenceLevel[cnt] = ((tempval[i*4+0]&0xff)<<3) + (tempval[i*4+1]>>5);
				tempPDTable[cnt]         = ((tempval[i*4+1]&0x1f)<<6) + (tempval[i*4+2]>>2);
				cnt++;

				//if(tempConfidenceLevel[i]<0x400)   return SET_ERROR(OTPCALI_ERROR_PDAF_DCC);
			}
			}
		
		do_save_buff("pd_register.txt", tempval, 96*4);
		//do_save_buff("pd_reg.txt", tempreg, 192*4, 1);
		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	int OtpCali_IMX519_MA722::do_check_otp(void *args)
	{
		OTPData *otp = (OTPData*)args;

		int ret = OTPCALI_ERROR_CHKERR;

		//minfo
		if(otp->minfo.minfo_flag!=0x01) {
			uts.log.Error(_T("otp_off_chk: info_flag =0x%02x error,should be 0x01"),otp->minfo.minfo_flag);
			return SET_ERROR(ret);
		}
		if(otp->minfo.minfo_version!=0x01) {
			uts.log.Error(_T("otp_off_chk: info_version =0x%02x error,should be 0x01"),otp->minfo.minfo_version);
			return SET_ERROR(ret);
		}
		if(otp->minfo.year<0x11 || otp->minfo.year>0x16) {
			uts.log.Error(_T("otp_off_chk: year =0x%02x error,should be in [0x11,0x16]"),otp->minfo.year);
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
		if(otp->minfo.supplier!=0x08) {
			uts.log.Error(_T("otp_off_chk: supplier =0x%02x error,should be 0x08"),otp->minfo.supplier);
			return SET_ERROR(ret);
		}
		if(otp->minfo.sensor_type!=0x01){
			uts.log.Error(_T("otp_off_chk: sensor_id =0x%02x error,should be 0x01"),otp->minfo.sensor_type);
			return SET_ERROR(ret);
		}
		if(otp->minfo.lens!=0x01){
			uts.log.Error(_T("otp_off_chk: lens_id =0x%02x error,should be 0x01"),otp->minfo.lens);
			return SET_ERROR(ret);
		}
		if(otp->minfo.vcm!=0x01){
			uts.log.Error(_T("otp_off_chk: vcm_id =0x%02x error,should be 0x01"),otp->minfo.vcm);
			return SET_ERROR(ret);
		}
		if(otp->minfo.vcmdriver!=0x01){
			uts.log.Error(_T("otp_off_chk: driveric_id =0x%02x error,should be 0x01"),otp->minfo.vcmdriver);
			return SET_ERROR(ret);
		}
		if(!is_mem_val(otp->minfo.reserved,0xff,sizeof(otp->minfo.reserved))) {
			uts.log.Error(_T("otp_off_chk: minfo.Reserve should be 0xff "));
			return SET_ERROR(ret);
		}

		int sum = CheckSum(&otp->minfo.minfo_version, sizeof(MINFO)-2) % 255 ;
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
		if(otp->wb.wb_version!=0x01) {
			uts.log.Error(_T("otp_off_chk: wb_version =0x%02x error,should be 0x01"),otp->wb.wb_version);
			return SET_ERROR(ret);
		}
		WB_RATIO ratio;
		WB_PARAM *pwb = &otp_param.wb_param;

		ratio.dr_g = (otp->wb.rg[0]*256+otp->wb.rg[1])/pwb->rg_multi;
		ratio.db_g = (otp->wb.bg[0]*256+otp->wb.bg[1])/pwb->bg_multi;

		pwb->rg_delta = abs(ratio.dr_g - pwb->rg_target);
		pwb->bg_delta = abs(ratio.db_g - pwb->bg_target);

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
		if(!is_mem_val(otp->wb.reserved,0xff,sizeof(otp->wb.reserved))) {
			uts.log.Error(_T("otp_off_chk: wb.Reserve should be 0xff "));
			return SET_ERROR(ret);
		}

		sum = CheckSum(&otp->wb.rg, sizeof(WB)-3) % 255 ;
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
		if(otp->lsc.lsc_version!=0x01) {
			uts.log.Error(_T("otp_off_chk: lsc_version =0x%02x error,should be 0x01"),otp->lsc.lsc_version);
			return SET_ERROR(ret);
		}

		sum = CheckSum(otp->lsc.lsc, sizeof(LSC)-3) % 255;
		if(otp->lsc.sum != sum) {
			uts.log.Error(_T("otp_off_chk: lsc.sum =0x%02x error,should be 0x%02x"),otp->lsc.sum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: af check pass"));


		//af
		AF_INT af;
		OtpOffCheck *paf = &otp_param.otp_off_param;

		af.inf =otp->af.inf[0]*256 + otp->af.inf[1];
		af.mup =otp->af.mup[0]*256 + otp->af.mup[1];

		if(otp->af.af_flag!=0x01){
			uts.log.Error(_T("otp_off_chk: af_flag =0x%02x error,should be 0x01"),otp->af.af_flag);
			return SET_ERROR(ret);
		}
		if(af.inf < paf->spec_af_inf_l || af.inf > paf->spec_af_inf_h) {
			uts.log.Error(_T("otp_off_chk: inf =%d error,should be in [%d,%d]"),af.inf,paf->spec_af_inf_l,paf->spec_af_inf_h);
			return SET_ERROR(ret);
		}
		if(af.mup < paf->spec_af_mac_l || af.inf > paf->spec_af_mac_h) {
			uts.log.Error(_T("otp_off_chk: mup =%d error,should be in [%d,%d]"),af.mup,paf->spec_af_mac_l,paf->spec_af_mac_h);
			return SET_ERROR(ret);
		}
		if(!is_mem_val(otp->af.reserved,0xff,sizeof(otp->af.reserved))) {
			uts.log.Error(_T("otp_off_chk: af.Reserve should be 0xff "));
			return SET_ERROR(ret);
		}

		sum = CheckSum(otp->af.inf, sizeof(AF)-2) % 255 ;
		if(otp->af.sum != sum) {
			uts.log.Error(_T("otp_off_chk: af.sum =0x%02x error,should be 0x%02x"),otp->af.sum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: af check pass"));

		//pdaf
		if(otp->pdaf.spc_flag!=0x01){
			uts.log.Error(_T("otp_off_chk: spc_flag =0x%02x error,should be 0x01"),otp->pdaf.spc_flag);
			return SET_ERROR(ret);
		}
		if(otp->pdaf.dcc_flag!=0x01){
			uts.log.Error(_T("otp_off_chk: dcc_flag =0x%02x error,should be 0x01"),otp->pdaf.dcc_flag);
			return SET_ERROR(ret);
		}
		
		sum = CheckSum(otp->pdaf.spc_map, sizeof(otp->pdaf.spc_map)) % 255 ;
		if(otp->pdaf.spc_sum != sum) {
			uts.log.Error(_T("otp_off_chk: spc.sum =0x%02x error,should be 0x%02x"),otp->pdaf.spc_sum, sum);
			return SET_ERROR(ret);
		}

		sum = CheckSum(otp->pdaf.dcc_map, sizeof(otp->pdaf.dcc_map)) % 255 ;
		if(otp->pdaf.dcc_sum != sum) {
			uts.log.Error(_T("otp_off_chk: dcc.sum =0x%02x error,should be 0x%02x"),otp->pdaf.dcc_sum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: pdaf check pass"));
		

		ret = OTPCALI_ERROR_NO;
		return SET_ERROR(ret);

	}
	
}
