#include "StdAfx.h"
#include "OtpCali_IMX386_MA823.h"
#include "VCM.h"
#include "algorithm.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------
typedef int (/*WINAPI */ *Dllfun1) (char* ini_file_path , short *pRaw10, short *pLRC);
typedef int (/*WINAPI */ *Dllfun2) (char* file_ini_path , short *pRaw10, int *judge);
typedef int (/*WINAPI */ *Dllfun3) (char *sensor, int lens1, int lens2, int *pd1, int *pd2, int h, int v, unsigned short *dcc_out, int calc_mode);
typedef int (/*WINAPI */ *Dllfun4) (char* sensor , unsigned short *dcc, int *pd, int h_num ,int v_num, int *dcc_out, int *pd_out ,int calc_mode);

namespace OtpCali_IMX386_MA823 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_IMX386_MA823::OtpCali_IMX386_MA823(BaseDevice *dev) : SonyOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = MTK_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		otp_spc_len = IMX386_SPC_LEN;
		otp_dcc_len = IMX386_DCC_LEN;
		pdaf_pd_len  = IMX386_PD_LEN;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX386_MA823::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX386_MA823::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX386_MA823::LscCali(void *args)
	{
	    return do_mtk_lsc_cali();
	}
	
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX386_MA823::get_minfo_from_db(void *args)
	{

		USES_CONVERSION;

		MINFO *m = (MINFO *)args;

		m->minfoFlag=0x55;	//0x00: Invalid, 0x01: Valid
		m->ModuleID = 0x42;
		m->LensID = 0x43;
		m->VCMID = 0x01;

		SYSTEMTIME stime;
		time_t time;
		if (otpDB->get_otp_data_lock_time(mid, &time) < 0)
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
		
		m->checkSum = CheckSum(&m->ModuleID, sizeof(MINFO)-2) % 0xFF + 1;
		
		return sizeof(MINFO);
	}
	int OtpCali_IMX386_MA823::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;

		//module info
		uts.log.Debug(_T("Get Minfo"));
		int ret = get_minfo_from_db(&otp->minfo);				

		//WB
		otp->wb.wbFlag=0x55;
		uts.log.Debug(_T("Get WBinfo"));

		WB_DATA_UCHAR wb[2];
		ret = get_wb_from_raw_data(&wb[0], &wb[1]);
		if (ret < 0) { return ret;}

		WB_RATIO ratio;
		get_wb_ratio(&wb[0], &ratio, 512);

		uts.log.Info(_T("r_g=0x%02x,b_g=0x%02x,gbgr=0x%02x"),ratio.r_g,ratio.b_g,ratio.gb_gr);


		put_be_val(ratio.r_g, otp->wb.rg, sizeof(otp->wb.rg));
		put_be_val(ratio.b_g, otp->wb.bg, sizeof(otp->wb.bg));
		put_be_val(ratio.gb_gr, otp->wb.gbgr, sizeof(otp->wb.gbgr));

		otp->wb.checkSum = CheckSum(&otp->wb.rg, sizeof(WB)-2) % 0xFF + 1;

		//AF
		uts.log.Debug(_T("Get AFinfo"));
		int inf = 0, marcro = 0;
		otp->af.afFlag=0x55;

		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0) return ret;

		put_be_val(inf, otp->af.inf, sizeof(otp->af.inf));
		put_be_val(marcro, otp->af.mup, sizeof(otp->af.mup));
		otp->af.checkSum = CheckSum(otp->af.mup, sizeof(AF)-2) % 0xFF + 1;

		//LSC
		uts.log.Debug(_T("Get LSCinfo"));
		otp->lsc.lscFlag = 0x55;
		ret = get_lsc_from_raw_data(otp->lsc.lsc, otp_lsc_len);
		if (ret < 0) return ret;

		otp->lsc.checkSum = CheckSum(otp->lsc.lsc, otp_lsc_len) % 0xFF + 1;

		//pdaf
		uts.log.Debug(_T("Get PDAFinfo"));

		char *spcbuf = new char[otp_spc_len];
		char *dccbuf = new char[otp_dcc_len];

		otp->pdaf.pdafFlag = 0x55;

		ret = otpDB->GetOtpByType(mid, OTPDB_OTPTYPE_SPC, spcbuf,otp_spc_len);
		if (ret < 0) {		
			RELEASE_ARRAY(spcbuf);
		    RELEASE_ARRAY(dccbuf);
			return ret;
		}

		ret = otpDB->GetOtpByType(mid, OTPDB_OTPTYPE_DCC, dccbuf,otp_dcc_len);
		if (ret < 0) {		
			RELEASE_ARRAY(spcbuf);
			RELEASE_ARRAY(dccbuf);
			return ret;
		}

		//copy spc map & dcc map

		memcpy(otp->pdaf.spc_map,spcbuf,otp_spc_len);
		memcpy(otp->pdaf.dcc_map,dccbuf,otp_dcc_len);

		otp->pdaf.checkSum = (CheckSum(otp->pdaf.spc_map, otp_spc_len+otp_dcc_len)) % 0xFF +1;

		RELEASE_ARRAY(spcbuf);
		RELEASE_ARRAY(dccbuf);
		
		return sizeof(OTPData);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX386_MA823:: SPCCali(void *args)
	{
		return SONYSPCCali();
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_IMX386_MA823::DCC0Cali(void *args)
	{
		return  do_sony_dcc_cali();
	}

	int OtpCali_IMX386_MA823::PDAFVerify(void *args)
	{
		return sony_dcc_verify();
	}

	int OtpCali_IMX386_MA823::do_SONY_SPCCali(unsigned short *pRaw10, short *out)
	{
		USES_CONVERSION;

		int ret = -1;
		FILE *fp;
		HINSTANCE hdll;
		Dllfun1 Sony_PDAF_SPC_Gain;

		short * temp_pRaw10 = (short *)pRaw10;
		short * temp_out = (short *)out;

		fp = fopen("spc_gain_386.ini","rt");
		if(fp==NULL) { return ret;}
		fclose(fp);

		hdll = LoadLibrary(_T("Sony_PDAF_SPC_DCC.dll"));	
		if(hdll==NULL) { FreeLibrary(hdll);return ret;}

		Sony_PDAF_SPC_Gain = (Dllfun1)GetProcAddress(hdll,"Sony_PDAF_SPC_Gain");
		if(Sony_PDAF_SPC_Gain==NULL) { FreeLibrary(hdll);return ret;}

		ret = Sony_PDAF_SPC_Gain("spc_gain_386.ini",temp_pRaw10, temp_out);

		//Return Value
		//#define ERR_INI_file_open_fail     0x81
		//#define ERR_LRC_file_open_fail     0x82
		//#define ERR_malloc_memory_fail  0x83
		//#define ERR_malloc_memory_fail  0x84
		//#define EER_No_Suppot_Sensor    0x85
	   //#define Calibration_OK                     0x01


		if(hdll!=NULL) { FreeLibrary(hdll);hdll =NULL;}

		return ret;		
	}
	int OtpCali_IMX386_MA823::do_SONY_SPCVerify(unsigned short *pRaw10,int *judge)
	{
		USES_CONVERSION;

		int ret = -1;
		HINSTANCE hdll;
		Dllfun2 Sony_PDAF_SPC_Judge;

		hdll = LoadLibrary(_T("Sony_PDAF_SPC_DCC.dll"));
		if(hdll==NULL) { FreeLibrary(hdll);return ret;}

		Sony_PDAF_SPC_Judge = (Dllfun2)GetProcAddress(hdll,"Sony_PDAF_SPC_Judge");
		if(Sony_PDAF_SPC_Judge==NULL) { FreeLibrary(hdll);return ret;}

		ret = Sony_PDAF_SPC_Judge("spc_gain_386.ini", (short*)pRaw10, judge);

		FreeLibrary(hdll);
		return ret;
	}
	int OtpCali_IMX386_MA823::do_SONY_DCCCali(int lens1, int lens2, int *pd1, int *pd2,unsigned short *dcc_out, int calc_mode)
	{
		int ret = -1;
		HINSTANCE hdll;
		Dllfun3 Sony_PDAF_DCC;

		hdll = LoadLibrary(_T("Sony_PDAF_SPC_DCC.dll"));
		if(hdll==NULL) { FreeLibrary(hdll);return ret;}

		Sony_PDAF_DCC = (Dllfun3)GetProcAddress(hdll,"Sony_PDAF_DCC");
		if(Sony_PDAF_DCC==NULL) { FreeLibrary(hdll);return ret;}

		ret = Sony_PDAF_DCC("IMX386", lens1,   
			                          lens2, 
			                          pd1, 
									  pd2, 8, 6, 
									  dcc_out,   
									  calc_mode);

		FreeLibrary(hdll);
		return ret;
	}
	int OtpCali_IMX386_MA823::do_SONY_DCCVerify(unsigned short*dcc, int *pd, int *dcc_out, int *pd_out ,int calc_mode)
	{
		int ret = -1;
		HINSTANCE hdll;
		Dllfun4 Sony_PDAF_DCC_verify;

		hdll = LoadLibrary(_T("Sony_PDAF_SPC_DCC.dll"));
		if(hdll==NULL) { FreeLibrary(hdll);return ret;}

		Sony_PDAF_DCC_verify = (Dllfun4)GetProcAddress(hdll,"Sony_PDAF_DCC_verify");
		if(Sony_PDAF_DCC_verify==NULL) { FreeLibrary(hdll);return ret;}

		ret = Sony_PDAF_DCC_verify("IMX386",dcc,
			                                pd,8,6,
											dcc_out,
										    pd_out,
											calc_mode);

		FreeLibrary(hdll);
		return ret;
	}
	int OtpCali_IMX386_MA823::do_WriteSPCSetting1()
	{
		sensor->dev->write_sensor(0x0101 , 0x00);
		sensor->dev->write_sensor(0x0B00 , 0x00);
		sensor->dev->write_sensor(0x3051 , 0x00);
		sensor->dev->write_sensor(0x3052 , 0x00);
		sensor->dev->write_sensor(0x3055 , 0x00);
		sensor->dev->write_sensor(0x3036 , 0x00);
		sensor->dev->write_sensor(0x3047 , 0x01);
		sensor->dev->write_sensor(0x3049 , 0x01);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	int OtpCali_IMX386_MA823::do_WriteSPCSetting2()
	{
		sensor->dev->write_sensor(0x0101 , 0x00);
		sensor->dev->write_sensor(0x0B00 , 0x00);
		sensor->dev->write_sensor(0x3051 , 0x00);
		sensor->dev->write_sensor(0x3052 , 0x00);
		sensor->dev->write_sensor(0x3055 , 0x00);
		sensor->dev->write_sensor(0x3036 , 0x01);
		sensor->dev->write_sensor(0x3047 , 0x01);
		sensor->dev->write_sensor(0x3049 , 0x01);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	int OtpCali_IMX386_MA823::do_WriteDCCSetting3()
	{
		sensor->dev->write_sensor(0x0101 , 0x00);
		sensor->dev->write_sensor(0x0B00 , 0x00);
		sensor->dev->write_sensor(0x3051 , 0x01);
		sensor->dev->write_sensor(0x3052 , 0x01);
		sensor->dev->write_sensor(0x3055 , 0x01);
		sensor->dev->write_sensor(0x3036 , 0x01);
		sensor->dev->write_sensor(0x3047 , 0x01);
		sensor->dev->write_sensor(0x3049 , 0x01);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}

	int OtpCali_IMX386_MA823::do_SPC_writeback(uint8_t *regs)
	{
		uint8_t temp_read[96] = {0};

		sensor->dev->write_sensor(0x0100 , 0x00);
		sensor->dev->write_sensor(0x7d4c,regs,48);
		Sleep(20);
		sensor->dev->write_sensor(0x7d80,regs+48,48);
		Sleep(20);

		sensor->dev->read_sensor(0x7d4c,temp_read,48);
		Sleep(20);
		sensor->dev->read_sensor(0x7d80,temp_read+48,48);
		sensor->dev->write_sensor(0x0100 , 0x01);

		if(memcmp(regs,temp_read,96)){
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		}

		return SET_ERROR(OTPCALI_ERROR_NO);
	}

	int OtpCali_IMX386_MA823::do_SONY_DCC_stats(int mode)
	{

		sensor->dev->write_sensor(0x3046,(uint8_t)(mode&0xff));
		Sleep(5);

		if(mode != sensor->dev->read_sensor(0x3046))
			return SET_ERROR(OTPCALI_ERROR_PDAF_DCC);
		
		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	int OtpCali_IMX386_MA823::do_SONY_GetPDTable(int *pdtable)
	{
		int * tempPDTable = (int*)pdtable;

		uint8_t tempval[1024];
		//uint16_t tempreg[1024];
		uint16_t tempConfidenceLevel[1024];

		memset(tempval,0,1024);
		memset(tempConfidenceLevel,0,1024);
		int cnt = 0;
		for (int i=0;i<192;i++)
		{
			tempval[i*4+0]=sensor->dev->read_sensor(0x8400+i*4+0);
			tempval[i*4+1]=sensor->dev->read_sensor(0x8400+i*4+1);
			tempval[i*4+2]=sensor->dev->read_sensor(0x8400+i*4+2);
			tempval[i*4+3]=sensor->dev->read_sensor(0x8400+i*4+3);

			if((i%16>=0)&&(i%16<=7))
			{
				tempConfidenceLevel[cnt] = ((tempval[i*4+0]&0xff)<<3) + (tempval[i*4+1]>>5);
				tempPDTable[cnt]         = ((tempval[i*4+1]&0x1f)<<6) + (tempval[i*4+2]>>2);
				cnt++;
			}
		}
		
		do_save_buff("pd_register.txt", tempval, 192*4);
		//do_save_buff("pd_reg.txt", tempreg, 192*4, 1);
		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	int OtpCali_IMX386_MA823::do_check_otp(void *args)
	{
		OTPData *otp = (OTPData*)args;

		int ret = OTPCALI_ERROR_CHKERR;

		//minfo
		if(otp->minfo.minfoFlag!=0x55) {
			uts.log.Error(_T("otp_off_chk: minfoFlag =0x%02x error,should be 0x55"),otp->minfo.minfoFlag);
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
		if(otp->minfo.ModuleID!=0x42) {
			uts.log.Error(_T("otp_off_chk: ModuleID =0x%02x error,should be 0x42"),otp->minfo.ModuleID);
			return SET_ERROR(ret);
		}
		if(otp->minfo.LensID!=0x43){
			uts.log.Error(_T("otp_off_chk: LensID =0x%02x error,should be 0x43"),otp->minfo.LensID);
			return SET_ERROR(ret);
		}
		if(otp->minfo.VCMID!=0x01){
			uts.log.Error(_T("otp_off_chk: VCMID =0x%02x error,should be 0x01"),otp->minfo.VCMID);
			return SET_ERROR(ret);
		}
		
		int sum = CheckSum(&otp->minfo.minfoFlag, sizeof(MINFO)-2) % 255 + 1;
		if(otp->minfo.checkSum != sum) {
			uts.log.Error(_T("otp_off_chk: minfo.checkSum =0x%02x error,should be 0x%02x"),otp->minfo.checkSum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: minfo check pass"));

		//wb
		if(otp->wb.wbFlag!=0x55) {
			uts.log.Error(_T("otp_off_chk: wbFlag =0x%02x error,should be 0x55"),otp->wb.wbFlag);
			return SET_ERROR(ret);
		}
		
		/*WB_RATIO ratio;
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
		}*/

		sum = CheckSum(&otp->wb.rg, sizeof(WB)-2) % 255 + 1;
		if(otp->wb.checkSum != sum) {
			uts.log.Error(_T("otp_off_chk: wb.checkSum =0x%02x error,should be 0x%02x"),otp->wb.checkSum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: wb check pass"));


		//lsc
		if(otp->lsc.lscFlag!=0x55) {
			uts.log.Error(_T("otp_off_chk: lsc_flag =0x%02x error,should be 0x55"),otp->lsc.lscFlag);
			return SET_ERROR(ret);
		}
	
		sum = CheckSum(otp->lsc.lsc, sizeof(LSC)-2) % 255 +1;
		if(otp->lsc.checkSum != sum) {
			uts.log.Error(_T("otp_off_chk: lsc.checkSum =0x%02x error,should be 0x%02x"),otp->lsc.checkSum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: af check pass"));


		//af
		

		if(otp->af.afFlag!=0x55){
			uts.log.Error(_T("otp_off_chk: afFlag =0x%02x error,should be 0x55"),otp->af.afFlag);
			return SET_ERROR(ret);
		}

		/*AF_INT af;
		OtpOffCheck *paf = &otp_param.otp_off_param;

		af.inf =otp->af.inf[0]*256 + otp->af.inf[1];
		af.mup =otp->af.mup[0]*256 + otp->af.mup[1];
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
		}*/

		sum = CheckSum(otp->af.mup, sizeof(AF)-2) % 255 + 1;
		if(otp->af.checkSum != sum) {
			uts.log.Error(_T("otp_off_chk: af.checkSum =0x%02x error,should be 0x%02x"),otp->af.checkSum, sum);
			return SET_ERROR(ret);
		}
		uts.log.Info(_T("otp_off_chk: af check pass"));

		//pdaf
		if(otp->pdaf.pdafFlag != 0x55){
			uts.log.Error(_T("otp_off_chk: pdafFlag =0x%02x error,should be 0x55"),otp->pdaf.pdafFlag);
			return SET_ERROR(ret);
		}
		
		sum = CheckSum(otp->pdaf.spc_map, otp_spc_len+otp_dcc_len) % 255 + 1;
		if(otp->pdaf.checkSum != sum) {
			uts.log.Error(_T("otp_off_chk: pdaf.checkSum =0x%02x error,should be 0x%02x"),otp->pdaf.checkSum, sum);
			return SET_ERROR(ret);
		}

		ret = OTPCALI_ERROR_NO;
		return SET_ERROR(ret);

	}
}
