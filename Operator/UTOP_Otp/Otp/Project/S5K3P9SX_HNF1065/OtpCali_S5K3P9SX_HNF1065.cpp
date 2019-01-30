#include "StdAfx.h"
#include "OtpCali_S5K3P9SX_HNF1065.h"
#include "VCM.h"
#include "algorithm.h"
#include "REVL/PDAF_QulComm_RevL.h"
#include "dlmalloc.h"
#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_S5K3P9SX_HNF1065 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K3P9SX_HNF1065::OtpCali_S5K3P9SX_HNF1065(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_lsc_mtk_len=MTK_LSC_LEN;
		otp_cst_len=CROSS_TALK_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
		
		//otp_spc_len = QULCOMM_SPC_LEN;
		//otp_dcc_len = QULCOMM_DCC_LEN;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_HNF1065::do_prog_otp()
	{
		#if 0
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
		#endif
		#if 1
		uts.log.Debug(_T("to dump otp data from db no prog"));
		CFile fp_bin;
		CString strDirPath;
		CString strFilePath;
	 
		SYSTEMTIME st;
		GetLocalTime(&st);
		fp_bin.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		fp_bin.Write(otp_data_in_db,otp_data_len);
		fp_bin.Close();

	//Save to csv
		strFilePath.Format(_T("%s%s_OTPDump.csv"),
		strDirPath,
		uts.info.strSN);

		CFile LogFile;
		CString strData;
		CString strTemp;
		if (LogFile.Open(strFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			strData = _T("Addr,Data\r\n");;
			for(int i=0;i<otp_data_len;i++)
			{
				strTemp.Format(_T("%d,0x%04x,0x%02x\r\n"),i,otp_data_in_db[i]);
				strData += strTemp;
			}

			int nCount = strData.GetLength()*2;
			LogFile.Write(strData, nCount);

			LogFile.Close();
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			MessageBox(NULL, _T("Can't write data to the dump data file!"), _T("Save Log Error"), MB_OK | MB_ICONERROR);
		}

		#endif
		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_HNF1065::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	int OtpCali_S5K3P9SX_HNF1065::WBCali(void *args)
	{
		return  do_qulcomm_cali(1);
	}
	int OtpCali_S5K3P9SX_HNF1065::CrossTalk(void *args)
	{

		sensor->dev->i2c_write(BIT16_BIT8,0x0b06 ,0x01);
		sensor->dev->i2c_write(BIT16_BIT8,0x0b09 ,0x00);

		return do_crosstalk();
	}
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_HNF1065::LscCali(void *args)
	{
		uts.log.Debug(_T("OtpCali_S5K3P9SX_HNF------1065LscCali"));
		return do_qulcomm_cali(0);
	}
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_HNF1065::LSCMTK_Cail(void *args)
	{
	    uts.log.Debug(_T("OtpCali_S5K3P9SX_HNF1065 LSCMTK_Cail"));
		return do_mtk_lsc_cali_MTK051017_RGB();
	}
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_HNF1065::do_qualcomm_Type_Cali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],int type)
	{
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCOrAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,type);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P9SX_HNF1065::get_minfo_from_db(void *args)
	{	 
		MINFO *m = (MINFO *)args;
		m->moudule_id[0]= 0x09;
		m->moudule_id[1]=0x00;
		m->lens_id[0] = 0x72;
		m->lens_id[1] = 0x00;
		m->sensor_id[0] = 0x62;
		m->sensor_id[1] = 0x00;
		memset(&m->vcm_id,0x00,2);
// 		SYSTEMTIME stime;
// 		time_t time;
// 
// 		if (otpDB->get_otp_data_lock_time(mid, &time) < 0)
// 		{
// 			uts.log.Debug(_T("get_otp_data_lock_time NG!"));
// 			GetLocalTime(&stime);
// 			 
// 			m->year[0]= stime.wYear % 100;
// 			m->year[1]= stime.wYear / 100;
// 			m->month = (uint8_t)stime.wMonth;
// 			m->day   = (uint8_t)stime.wDay;
// 		}
// 		else
// 		{
// 			uts.log.Debug(_T("get_otp_data_lock_time pass!"));
// 			struct tm today;
// 			_localtime64_s( &today, &time );
// 
// 			m->year[0]= today.tm_year % 100;
// 			m->year[1]= today.tm_year / 100;
// 			m->month  = (uint8_t)today.tm_mon + 1;
// 			m->day    = (uint8_t)today.tm_mday;
// 		}
		SYSTEMTIME stime;
		time_t time;
		otpDB->get_otp_data_lock_time(mid, &time);
		if (time < 0)
		{
			GetLocalTime(&stime);
			m->year[0]  = stime.wYear % 100;
			m->year[1]=stime.wYear / 100;
			m->month = (uint8_t)stime.wMonth;
			m->day   = (uint8_t)stime.wDay;
// 			m->hour  = (uint8_t)stime.wHour;
// 			m->minu  = (uint8_t)stime.wMinute;
// 			m->sec =  (uint8_t)stime.wSecond;
		}
		else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			m->year [0] = today.tm_year % 100;
			m->year [1] = today.tm_year / 100;
			m->month = (uint8_t)today.tm_mon + 1;
			m->day   = (uint8_t)today.tm_mday;
// 			m->hour  = (uint8_t)today.tm_hour;
// 			m->minu  = (uint8_t)today.tm_min;
// 			m->sec =  (uint8_t)today.tm_sec;
		}

		return sizeof(MINFO);
	}

	int OtpCali_S5K3P9SX_HNF1065::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		USES_CONVERSION;
		uts.log.Debug(_T("Get Minfo"));
		int ret=0;
		 ret = get_minfo_from_db(&otp->minfo);
		if (ret < 0) return ret;

		//AWB
		//上传的数据的要求要扣住OB//5100k
		uts.log.Debug(_T("Get WBinfo"));
		//TODO setwbparam()
		WB_DATA_USHORT wb[2];
		WB_LIGHT_USHORT wl;
		 ret = get_wb_from_raw_data(&wb[0], &wb[1],&wl,0);
		if (ret < 0) 
		{ 
			uts.log.Error(_T("Get AWB 5100k Data Error"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}
		
		//WB_RATIO ratio;
		//get_wb_ratio(&wb[0], &ratio, 1024);
	/*	int r_g,gr_g,gb_g,b_g;
		r_g=otp_param.wb_param.RGrGbB_g[0];
		gr_g=otp_param.wb_param.RGrGbB_g[1];
		gb_g=otp_param.wb_param.RGrGbB_g[2];
		b_g=otp_param.wb_param.RGrGbB_g[3];
	*/
		uts.log.Info(_T("HEX:5100K:R=0x%02x,B=0x%02x,Gr =0x%02x,Gb=0x%02x"),wb[0].R,wb[0].B,wb[0].Gr,wb[0].Gb);
		uts.log.Info(_T("DEC:5100K:R_g=%d,B_g=0x%d,Gr_g =%d,Gb_g=%d"),wb[1].R,wb[1].B,wb[1].Gr,wb[1].Gb);
		uts.log.Info(_T("DEC:5100K:RG=%d,BG=0x%d"),wl.RG,wl.BG);
		put_le_val(wb[0].R, otp->awb5100.r, sizeof(otp->awb5100.r));
		put_le_val(wb[0].Gr, otp->awb5100.gr, sizeof(otp->awb5100.gr));
		put_le_val(wb[0].Gb, otp->awb5100.gb, sizeof(otp->awb5100.gb));
		put_le_val(wb[0].B, otp->awb5100.b, sizeof(otp->awb5100.b));
		//golden数据
		put_le_val(wb[1].R, otp->awb5100.r_g, sizeof(otp->awb5100.r_g));
		put_le_val(wb[1].Gr, otp->awb5100.gr_g, sizeof(otp->awb5100.gr_g));
		put_le_val(wb[1].Gb, otp->awb5100.gb_g, sizeof(otp->awb5100.gb_g));
		put_le_val(wb[1].B, otp->awb5100.b_g, sizeof(otp->awb5100.b_g));
		//light 数据
		put_le_val(wl.RG,otp->light5100.lightsource_rg, sizeof(otp->light5100.lightsource_rg));
		put_le_val(wl.BG,otp->light5100.lightsource_bg, sizeof(otp->light5100.lightsource_bg));
		//af	
		
		otp->af.flaglsc=0x04;
		//otp->af.reversed1=0xff;
		//awb 3100k
		ret = get_wb_from_raw_data(&wb[0], &wb[1],&wl,2);
		if (ret < 0) 
		{ 
			uts.log.Error(_T("Get AWB 3100k Data Error"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}
		//
		uts.log.Info(_T("HEX:3100K:R=0x%02x,B=0x%02x,Gr =0x%02x,Gb=0x%02x"),wb[0].R,wb[0].B,wb[0].Gr,wb[0].Gb);
		uts.log.Info(_T("DEC:3100K:R_g=%d,B_g=0x%d,Gr_g =%d,Gb_g=%d"),wb[1].R,wb[1].B,wb[1].Gr,wb[1].Gb); 
		uts.log.Info(_T("DEC:4000K:RG=%d,BG=0x%d"),wl.RG,wl.BG);
		put_le_val(wb[0].R, otp->awb3100.r, sizeof(otp->awb3100.r));
		put_le_val(wb[0].Gr, otp->awb3100.gr, sizeof(otp->awb3100.gr));
		put_le_val(wb[0].Gb, otp->awb3100.gb, sizeof(otp->awb3100.gb));
		put_le_val(wb[0].B, otp->awb3100.b, sizeof(otp->awb3100.b));
		//golden数据
		put_le_val(wb[1].R, otp->awb3100.r_g, sizeof(otp->awb3100.r_g));
		put_le_val(wb[1].Gr, otp->awb3100.gr_g, sizeof(otp->awb3100.gr_g));
		put_le_val(wb[1].Gb, otp->awb3100.gb_g, sizeof(otp->awb3100.gb_g));
		put_le_val(wb[1].B, otp->awb3100.b_g, sizeof(otp->awb3100.b_g));
		// 3100k light
		put_le_val(wl.RG,otp->light3100.lightsource_rg, sizeof(otp->light3100.lightsource_rg));
		put_le_val(wl.BG,otp->light3100.lightsource_bg, sizeof(otp->light3100.lightsource_bg));
		//flag and check for 5100
		otp->awb_fc5100.flag=0x01;
		otp->awb_fc5100.check=(CheckSum(&otp->minfo.moudule_id[0], 28)%255);
		//memset(&otp->reversed,0xff,2);
		//flag and check for 31000k
		otp->awb_fc3100.flag=0x01;
		otp->awb_fc3100.check=(CheckSum(&otp->awb3100.r[0], 16)%255);
		//光源系数  int 4个字节，只需要两个字节，待验证
	//在上面已经赋值
		//awb 4000K
		ret = get_wb_from_raw_data(&wb[0], &wb[1],&wl,1);
		if (ret < 0) 
		{ 
			uts.log.Error(_T("Get AWB 4000k Data Error"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}
		//
		uts.log.Info(_T("HEX:4000K:R=0x%02x,B=0x%02x,Gr =0x%02x,Gb=0x%02x"),wb[0].R,wb[0].B,wb[0].Gr,wb[0].Gb);
		uts.log.Info(_T("DEC:4000K:R_g=%d,B_g=0x%d,Gr_g =%d,Gb_g=%d"),wb[1].R,wb[1].B,wb[1].Gr,wb[1].Gb); 
		uts.log.Info(_T("DEC:4000K:RG=%d,BG=0x%d"),wl.RG,wl.BG);

		put_le_val(wb[0].R, otp->awb4000.r, sizeof(otp->awb4000.r));
		put_le_val(wb[0].Gr, otp->awb4000.gr, sizeof(otp->awb4000.r));
		put_le_val(wb[0].Gb, otp->awb4000.gb, sizeof(otp->awb4000.r));
		put_le_val(wb[0].B, otp->awb4000.b, sizeof(otp->awb4000.r));
		//golden数据
		put_le_val(wb[1].R, otp->awb4000.r_g, sizeof(otp->awb4000.r));
		put_le_val(wb[1].Gr, otp->awb4000.gr_g, sizeof(otp->awb4000.r));
		put_le_val(wb[1].Gb, otp->awb4000.gb_g, sizeof(otp->awb4000.r));
		put_le_val(wb[1].B, otp->awb4000.b_g, sizeof(otp->awb4000.r));
		// 4000k light
		put_le_val(wl.RG,otp->light4000.lightsource_rg, sizeof(otp->light4000.lightsource_rg));
		put_le_val(wl.BG,otp->light4000.lightsource_bg, sizeof(otp->light4000.lightsource_bg));
		//flag and check for 4000K
		otp->awb_fc4000.flag=0x01;
		otp->awb_fc4000.check=(CheckSum(&otp->awb4000.r[0], 16)%255);
		//light check
		otp->light_fc5100.flag=0x01;
		otp->light_fc5100.check=CheckSum(&otp->light5100.lightsource_rg[0], sizeof(otp->light5100))%255;
		otp->light_fc4000.flag=0x01;
		otp->light_fc4000.check=CheckSum(&otp->light4000.lightsource_rg[0], sizeof(otp->light4000))%255;
		otp->light_fc3100.flag=0x01;
		otp->light_fc3100.check=CheckSum(&otp->light3100.lightsource_rg[0], sizeof(otp->light3100))%255;
		//qrcode
		// 2D Barcode
	CString Barcode = uts.info.strSN;//uts.info.strSensorId;
	uts.log.Error(_T("Get Barcode = %s") , Barcode);
	Barcode = Barcode.Left(16);
	memcpy(&otp->QrCode, T2A(Barcode), 16);
	otp->qrcode.flag = 0x01;
	otp->qrcode.check=CheckSum(&otp->QrCode , sizeof(otp->QrCode));
	 //qualcomm lsc
	uts.log.Debug(_T("start get qualcomm lsc form db"));
	uint8_t *qualcommlscdb=new uint8_t[QULCOMM_LSC_LEN];

	  ret = get_lsc_from_raw_data(qualcommlscdb, QULCOMM_LSC_LEN);
		if (ret < 0) 
		{ 
			uts.log.Error(_T("Get qualcomm lsc error"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}
		memset(&otp->lsc_qual,0x00,sizeof(otp->lsc_qual));
		//for(int i=0;i<QULCOMM_LSC_LEN/4;i+=2)
		//{
		//	otp->lsc_qual[221*2*0 +i]     = qualcommlscdb[221*2*1 + i];     //R_L
		//	otp->lsc_qual[221*2*0 +i+1]   = qualcommlscdb[221*2*1 + i + 1]; //R_H
		//	otp->lsc_qual[221*2*1 +i]     = qualcommlscdb[221*2*0 + i];     //Gr_L
		//	otp->lsc_qual[221*2*1 +i+1]   = qualcommlscdb[221*2*0 + i + 1]; //Gr_H
		//	otp->lsc_qual[221*2*2 +i]     = qualcommlscdb[221*2*2 + i];     //GB_L
		//	otp->lsc_qual[221*2*2 +i+1]   = qualcommlscdb[221*2*2 + i + 1]; //GB_H
		//	otp->lsc_qual[221*2*3 +i]     = qualcommlscdb[221*2*3 + i];     //B_L
		//	otp->lsc_qual[221*2*3 +i+1]   = qualcommlscdb[221*2*3 + i + 1]; //B_H
		//}

		//TODO 验证我的RGGB是否正确
		for(int i=0,k=0;i<QULCOMM_LSC_LEN/4;i+=2,k+=5)
		{
			if(k==255||k==511||k==767||k==1023){
				k=k+1;
				} 
			otp->lsc_qual[k]     = qualcommlscdb[221*2*0 + i];     //R_L
			otp->lsc_qual[k+1]   = qualcommlscdb[221*2*1 + i];     //Gr_L
			otp->lsc_qual[k+2]   = qualcommlscdb[221*2*2 + i];     //GB_L
			otp->lsc_qual[k+3]   = qualcommlscdb[221*2*3 + i];     //B_L
			otp->lsc_qual[k+4]   =   ((0x30&&qualcommlscdb[221*2*0 + i + 1])<<6)   //R_H
									+((0x30&&qualcommlscdb[221*2*1 + i + 1])<<4 ) //Gr_H
									+((0x30&&qualcommlscdb[221*2*2 + i + 1])<<2)  //GB_H
									+(0x30&&qualcommlscdb[221*2*3 + i + 1]);     //B_H
		}
		otp->lscqual.flag=0x01;
		otp->lscqual.check=CheckSum(&otp->lsc_qual[0] , 1109)%255;
	  //lsc_mtk
		uts.log.Debug(_T("start get mtk lsc form db"));
		ret=get_otp_from_raw_data(OTPDB_OTPTYPE_LSCMTK, &otp->lsc_mtk, MTK_LSC_LEN);
		if (ret < 0) 
		{ 
			uts.log.Error(_T("Get or set  lsc data error"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}


		otp->lscmtk.flag=0x01;
		otp->lscmtk.check=CheckSum(&otp->lsc_mtk ,sizeof(otp->lsc_mtk))%255;
		//Crosstalk data
		//要求先地位后高位存储
		uts.log.Debug(_T("start get cross talk data form db"));
		uint8_t *cross=new uint8_t[2048];
		//ret = get_lsc_from_raw_data(&cross , sizeof(otp->crosstalk));
		ret= get_otp_from_raw_data(OTPDB_OTPTYPE_CrossTalk, cross, CROSS_TALK_LEN);
		if (ret < 0) 
		{ 
			uts.log.Error(_T("Get   Crosstalk data error"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}
	//	memset(&otp->cross_talk,0,2048);
		for(int i=0;i<2048;i=i+2)
		{
			otp->cross_talk[i]=cross[i+1];
			otp->cross_talk[i+1]=cross[i];
		}
 		otp->crosstalk.flag=0x01;
		otp->crosstalk.check=CheckSum(&otp->cross_talk ,sizeof(otp->cross_talk))%255;
	 
	
		RELEASE_ARRAY(cross);
		RELEASE_ARRAY(qualcommlscdb);
		return sizeof(OTPData);
	}

	int OtpCali_S5K3P9SX_HNF1065::do_check_otp(void *args,void *args1)
	{
		OTPData *otp_sensor = (OTPData*)args;
		OTPData *otp_db = (OTPData*)args1;
		 
		uts.log.Debug(_T("otp_data check of hnf1065"));
		if(memcmp(otp_sensor->minfo.sensor_id, otp_db->minfo.sensor_id, (otp_data_len-6))!=0){
			uts.log.Debug(_T("hnf1065 sensor data different db data"));
			for(int i=6;i<otp_data_len;i++)
			{
				uts.log.Error(_T("Sensor : DB [%d] = %d , %d") , i, otp_data_in_sensor[i] , otp_data_in_db[i]);
			}
			uts.log.Error(_T("OtpData different in DB !"));
			uts.log.Error(_T("otp_data_in_sensor = %d") , sizeof(otp_data_in_sensor));
			uts.log.Error(_T("otp_data_in_db = %d") , sizeof(otp_data_in_db));
			uts.log.Error(_T("otp_data_len = %d") , sizeof(otp_data_in_db));
			return OTPCALI_ERROR_CHKERR;
		}
		if(memcmp(otp_sensor,otp_db,2)!=0){
			uts.log.Error(_T("OtpData different in DB !"));
			uts.log.Error(_T("otp_data_in_db = %d") ,otp_data_in_sensor[0]);
			uts.log.Error(_T("otp_data_len = %d") , otp_data_in_sensor[0]);
			return OTPCALI_ERROR_CHKERR;
		}
		if (otp_sensor->minfo.year[0]<17 || otp_sensor->minfo.year[0]>21
			||otp_sensor->minfo.year[1]!=20
			||otp_sensor->minfo.month < 1 || otp_sensor->minfo.month > 13 
			|| otp_sensor->minfo.day < 1 || otp_sensor->minfo.day > 31)
		{
			uts.log.Error(_T("year month day error!!!"));
			return OTPCALI_ERROR_NODATA;
		}

		/*
		OTPData *otp = (OTPData*)args;
		//ModuleInfo
		//检查

		if (otp->minfo.flag != 0x01 || otp->minfo.version != 0x01 || otp->minfo.moduleID != 0x02 
			|| otp->minfo.lensID != 0x01 || otp->minfo.vcmID != 0x00 || otp->minfo.vcmDriverID != 0x00 
			|| otp->minfo.colorTemp != 0x01 || otp->minfo.mirFlipFlag != 0x00)
		{
			uts.log.Error(_T("ModuleInfo Data error!!!"));
			return OTPCALI_ERROR_NODATA;
		}

		//检查year month day
		if (otp->minfo.year <10 || otp->minfo.year >25
			||otp->minfo.month < 1 || otp->minfo.month > 13 
			|| otp->minfo.day < 1 || otp->minfo.day > 31)
		{
			uts.log.Error(_T("year month day error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//检查CheckSum
		uint8_t mInfo_checkSum = (CheckSum(&otp->minfo.version, sizeof(MINFO)-2)%255);
		if (otp->minfo.checkSum != mInfo_checkSum)
		{
			uts.log.Error(_T("minfo CheckSum  error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//AWB
		if (otp->awb.flag != 0x01)
		{
			uts.log.Error(_T("AWB Flag error!!!"));
			return OTPCALI_ERROR_WBCALI;
		}

		//检查WB的Data
		int RGValue = get_le_val(otp->awb.rg, 2);
		int BGValue = get_le_val(otp->awb.bg, 2);
		int GGValue = get_le_val(otp->awb.gbgr, 2);
		int RgGoldenValue = get_le_val(otp->awb.rg_golden,2);
		int BgGoldenValue = get_le_val(otp->awb.bg_golden,2);
		int GgGoldenValue = get_le_val(otp->awb.gbgr_golden,2);

		if (RGValue > 612 || RGValue < 502 
			|| BGValue >795 || BGValue < 651
			|| GGValue > 1125 || GGValue < 950)
		{
			uts.log.Error(_T("wb data  error!!!"));
			return OTPCALI_ERROR_WBCALI;
		}

		//检查WB的Golden
		if (RgGoldenValue != 557 || BgGoldenValue != 723 || GgGoldenValue != 1024)
		{
			uts.log.Error(_T("wb Golden data  error!!!"));
			return OTPCALI_ERROR_WBCALI;
		}
		//检查Wb的CheckSum
		uint8_t	wb_checkSum = (CheckSum(&otp->awb.rg, 12)%255);
		if (wb_checkSum != otp->awb.checkSum)
		{
			uts.log.Error(_T("AWB CheckSum  error!!!"));
			return OTPCALI_ERROR_WBCALI;
		}

		//LSC
		if (otp->lsc.flag != 0x01)
		{
			uts.log.Error(_T("lsc Flag error!!!"));
			return OTPCALI_ERROR_LSCCALI;
		}
		//检查LSC的CheckSUM
		uint8_t lsc_checkSum = (CheckSum(&otp->lsc.lsc, QULCOMM_LSC_LEN)) % 0xff ;
		if (otp->lsc.checkSum != lsc_checkSum)
		{
			uts.log.Error(_T("LSC CheckSum  error!!!"));
			return OTPCALI_ERROR_LSCCALI;
		}
		*/
		return OTPCALI_ERROR_NO;
	}
	int OtpCali_S5K3P9SX_HNF1065::do_dump_otp()
{
	uts.log.Debug(_T("HNF1065 dump"));
	int len = otp_data_len;
    unsigned char *databuf = (unsigned char *)dlmalloc(len);
	int ret=get_otp_data_from_sensor(databuf);

	CFile fp_bin;
	CString strDirPath;
	CString strFilePath;
	 
	SYSTEMTIME st;
	GetLocalTime(&st);

	strDirPath.Format(_T("%sData-%s-%s-%s-%04d%02d%02d_OTPDump\\"),
		uts.info.strResultPath,
		uts.info.strProjectName,
		uts.info.strLineName,
		uts.info.strStationName,
		st.wYear,
		st.wMonth, 
		st.wDay);

	UTS::OSUtil::CreateMultipleDirectory(strDirPath);

	//bin file name
	strFilePath.Format(_T("%s%s_OTPDump.bin"),
		strDirPath,
		uts.info.strSN);

	fp_bin.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	fp_bin.Write(databuf,otp_data_len);
	fp_bin.Close();


	//Save to csv
	strFilePath.Format(_T("%s%s_OTPDump.csv"),
		strDirPath,
		uts.info.strSN);

	CFile LogFile;
	CString strData;
	CString strTemp;
	int page=0;
	if (LogFile.Open(strFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		strData = _T("Addr,Data\r\n");;
		for(int i=0;i<otp_data_len;i++)
		{
			strTemp.Format(_T("%d,0x%04x,0x%02x\r\n"),i,databuf[i]);
			strData += strTemp;
		}

		int nCount = strData.GetLength()*2;
		LogFile.Write(strData, nCount);

		LogFile.Close();
		//////////////////////////////////////////////////////////////////////////
	}
	else
	{
		MessageBox(NULL, _T("Can't write data to the dump data file!"), _T("Save Log Error"), MB_OK | MB_ICONERROR);
	}
    uts.log.Debug(_T("OtpData in Sensor:"));
	for(int i=0;i<otp_data_len/16;i++)
	{
		PrintDebugArray(databuf+i*16, 16);
		uts.log.Debug(_T(""));
	}
	PrintDebugArray(databuf+(otp_data_len/16)*16, otp_data_len-(otp_data_len/16)*16);

    dlfree(databuf);
    return 0;
}
}
