#include "StdAfx.h"
#include "OtpCali_Hi846_MA80D.h"
#include "VCM.h"
#include "algorithm.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")

namespace OtpCali_Hi846_MA80D 
{
	OtpCali_Hi846_MA80D::OtpCali_Hi846_MA80D(BaseDevice *dev):HiOtp(dev)
	{
		otp_type = OTP_TYPE_OTP;
		otp_data_len =  sizeof(OTPData);
		otp_lsc_len = MTK_LSC_LEN_MA80D;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
	}

	int OtpCali_Hi846_MA80D::do_prog_otp()
	{
		//烧录数据测试-------------------------------------------------------------------------------------------------------
		/*FILE *input = fopen("otp-prog_buffer.txt","w");

		if(input != NULL)
		{
		for(int i=0;i<otp_data_len;i++)
		{
		fprintf(input,"0x%02x\n",otp_data_in_db[i]);
		}
		}
		fclose(input);
		input=NULL;
		return SET_ERROR(OTPCALI_ERROR_NO);*/
		//-------------------------------------------------------------------------------------------------------
		int m_starAddr,l_starAddr,wb_startAddr,af_startAddr;

		m_starAddr = 0x0202;
		l_starAddr = 0x0236;
		wb_startAddr = 0x0C60;
		af_startAddr = 0x0CBB;
		
		//Moudle Info flag
		if(sensor->do_prog_otp(0,0x0201,otp_data_in_db+0,1) < 0)
			return SET_ERROR(OTPCALI_ERROR_PROGERR);
		//Moudle Info 17*3 =51
		if(sensor->do_prog_otp(0,m_starAddr,otp_data_in_db+1,sizeof(MINFO_Group)) < 0)
			return SET_ERROR(OTPCALI_ERROR_PROGERR);

		//LSC Flag 
		if(sensor->do_prog_otp(0,0x0235,otp_data_in_db+52,1) < 0)
			return SET_ERROR(OTPCALI_ERROR_PROGERR);
		//LSC 867*3 =2601
		if(sensor->do_prog_otp(0,l_starAddr,otp_data_in_db+53,sizeof(LSC_Group)) < 0)
			return SET_ERROR(OTPCALI_ERROR_PROGERR);

		//WB Flag
		if(sensor->do_prog_otp(0,0x0C5F,otp_data_in_db+2654,1) < 0)
			return SET_ERROR(OTPCALI_ERROR_PROGERR);
		//WB 30*3 =90
		if(sensor->do_prog_otp(0,wb_startAddr,otp_data_in_db+2655,sizeof(Hi846_WB_Group)) < 0)
			return SET_ERROR(OTPCALI_ERROR_PROGERR);
		//AF Flag
		if(sensor->do_prog_otp(0,0x0CBA,otp_data_in_db+2745,1) < 0)
			return SET_ERROR(OTPCALI_ERROR_PROGERR);

		//AF 6*3 =18
		if(sensor->do_prog_otp(0,af_startAddr,otp_data_in_db+2746,sizeof(Hi846_AF_Group)) < 0)
			return SET_ERROR(OTPCALI_ERROR_PROGERR);

		//return
		return SET_ERROR(OTPCALI_ERROR_NO);
	}

	int OtpCali_Hi846_MA80D::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;
		int m_addr,l_addr,wb_addr, af_addr;

		char otpbuf[3072]={0};
		memset(otpbuf,0,3072);

		m_addr = 0x0202;
		l_addr = 0x0236;
		wb_addr = 0x0C60;
		af_addr = 0x0CBB;

		//Moudle flag
		if(sensor->do_read_otp(0,0x0201,otpbuf,1))
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		//Moudle Info 17*3 =51
		if(sensor->do_read_otp(0,m_addr,otpbuf+1,sizeof(MINFO)))
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		//LSC flag
		if(sensor->do_read_otp(0,0x0235,otpbuf+52,1))
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		//LSC 867*3 =2601
		if(sensor->do_read_otp(0,l_addr,otpbuf+53,sizeof(LSC)))
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		//WB Flag
		if(sensor->do_read_otp(0,0x0C5F,otpbuf+2654,1))
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		//WB 30*3 =90
		if(sensor->do_read_otp(0,wb_addr,otpbuf+2655,sizeof(Hi846_WB)))
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		//AF Flag
		if(sensor->do_read_otp(0,0x0CBA,otpbuf+2745,1) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		//AF 
		if(sensor->do_read_otp(0,af_addr,otpbuf+2746,sizeof(Hi846_AF)) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		memcpy(otp,otpbuf,otp_data_len);

		return sizeof(OTPData);
	}

	int OtpCali_Hi846_MA80D::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		//module info
		otp->mInfoFlag = 0x01;
		int ret = get_minfo_from_db(&otp->mInfo);

		//LSC
		otp->lscFlag = 0x01;
		ret = get_lsc_from_raw_data(otp->lsc.LSCGroup1.lscbuffer, otp_lsc_len);
		if (ret < 0) return ret;

		otp->lsc.LSCGroup1.lscchecksum = (CheckSum(&otp->lsc.LSCGroup1.lscbuffer, MTK_LSC_LEN_MA80D)) % 255 + 1;

		//WB
		otp->wb_flag = 0x01;
		
		
		WB_DATA_UCHAR wb[2];
		ret = get_wb_from_raw_data(&wb[0], &wb[1]);
		if (ret < 0) { return ret;}

		WB_RATIO ratio;
		get_wb_ratio(&wb[0], &ratio, 512);

		uts.log.Info(_T("r_g=0x%02x,b_g=0x%02x"),ratio.r_g,ratio.b_g);

		otp->wb.wbGroup1.r[0] = 0;
		otp->wb.wbGroup1.r[1] = wb[0].R ;
		otp->wb.wbGroup1.gb[0] = 0;
		otp->wb.wbGroup1.gb[1] = wb[0].Gb ;
		otp->wb.wbGroup1.gr[0] = 0;
		otp->wb.wbGroup1.gr[1] = wb[0].Gr ;
		otp->wb.wbGroup1.b[0] = 0;
		otp->wb.wbGroup1.b[1] =  wb[0].B;
		

		otp->wb.wbGroup1.rg[0] = ratio.r_g >> 8;
		otp->wb.wbGroup1.rg[1] = ratio.r_g & 0xFF ;
		otp->wb.wbGroup1.bg[0] = ratio.b_g >> 8;
		otp->wb.wbGroup1.bg[1] = ratio.b_g & 0xFF ;
		otp->wb.wbGroup1.gbgr[0] = ratio.gb_gr >>8;
		otp->wb.wbGroup1.gbgr[1] = ratio.gb_gr & 0xFF;

		otp->wb.wbGroup1.reserved = 0x00;
		otp->wb.wbGroup1.wb_CheckSum = (CheckSum(&otp->wb.wbGroup1.rg[0], 28)) % 255 + 1;

		//AF 
		int inf = 0, marcro = 0;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0) return ret;
		otp->afFlag = 0x01;
		//otp->af.afGroup1.Af_Cal = 0x01;
		otp->af.afGroup1.Af_Inf[0] = inf >> 8;
		otp->af.afGroup1.Af_Inf[1] = inf & 0xFF;
		otp->af.afGroup1.Af_Mup[0] = marcro >> 8;
		otp->af.afGroup1.Af_Mup[1] = marcro & 0xFF;

		otp->af.afGroup1.afchecksum = (CheckSum(&otp->af.afGroup1.Af_Cal,5))%255 +1;

		return sizeof(OTPData);
	}

	int OtpCali_Hi846_MA80D::LscCali(void *args)
	{
		return do_Hi846_lsc_cali();
	}

	int OtpCali_Hi846_MA80D::get_minfo_from_db(void *args)
	{
		MINFO *m = (MINFO *)args;
		m->mInfoGroup1.moudleID = 0x42;
		m->mInfoGroup1.afID = 0x01;

		SYSTEMTIME stime;
		time_t time;
		if (otpDB->GetOtpBurnTime(mid, &time) < 0)
		{
			GetLocalTime(&stime);
			m->mInfoGroup1.year  = stime.wYear % 100;
			m->mInfoGroup1.month = (uint8_t)stime.wMonth;
			m->mInfoGroup1.day   = (uint8_t)stime.wDay;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			m->mInfoGroup1.year   = today.tm_year % 100;
			m->mInfoGroup1.month  = (uint8_t)today.tm_mon + 1;
			m->mInfoGroup1.day    = (uint8_t)today.tm_mday;
		}

		int sum = CheckSum(&m->mInfoGroup1.moudleID, 10);

		m->mInfoGroup1.mInfoCheckSum = sum % 255 +1;

		return sizeof(MINFO);
	}


	int OtpCali_Hi846_MA80D::do_check_otp(void *args)
	{
		OTPData *otp = (OTPData*)args;

		//检查flag
		if (otp->mInfoFlag != 0x01 || otp->lscFlag != 0x01 || otp->afFlag != 0x01 || otp->wb_flag != 0x01)
		{
			uts.log.Info(_T("OTP Flag Data error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//检查Sensor ID AF ID
		if (otp->mInfo.mInfoGroup1.moudleID != 0x42 || otp->mInfo.mInfoGroup1.afID != 0x01)
		{
			uts.log.Info(_T("SensorsID or AF ID error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//检查ModuleInfo的check Sum
		uint8_t mInfo_checkSum = CheckSum(&(otp->mInfo.mInfoGroup1.moudleID), 10) % 255 +1;
		if (otp->mInfo.mInfoGroup1.mInfoCheckSum != mInfo_checkSum)
		{
			uts.log.Info(_T("module Info CheckSum  error!!!"));
			return OTPCALI_ERROR_NODATA;
		}

		//检查LSC的CheckSUM
		uint8_t lsc_checkSum = (CheckSum(&(otp->lsc.LSCGroup1.lscbuffer), MTK_LSC_LEN_MA80D)) % 255 + 1;
		if (otp->lsc.LSCGroup1.lscchecksum != lsc_checkSum)
		{
			uts.log.Info(_T("LSC CheckSum  error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//检查WB的CheckSum
		uint8_t wb_checkSum = (CheckSum(&(otp->wb.wbGroup1.rg[0]), 28)) % 255 + 1;
		if (otp->wb.wbGroup1.wb_CheckSum != wb_checkSum)
		{
			uts.log.Info(_T("wb CheckSum  error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//检查af的CheckSum
		uint8_t af_checkSum = (CheckSum(&otp->af.afGroup1.Af_Cal,5))%255 +1;
		if (otp->af.afGroup1.afchecksum != af_checkSum)
		{
			uts.log.Info(_T("af CheckSum  error!!!"));
			return OTPCALI_ERROR_NODATA;
		}
		//LSC 应用
		using namespace UTS::Algorithm::RI::RI_OpticalCenter;

		dev->GetBufferInfo(m_bufferInfo);
		if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));
			return OTPCALI_ERROR_SENSOR;	
		}

		dev->DisplayImage(m_bufferObj.pBmpBuffer);
		struct LSC_PARAM *lsc = &otp_param.lsc_param;

		RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
		uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

		int GbB = 0, GrR = 429;
		WORD wSramLscParaGain_R_0x611e[143]={0};
		WORD wSramLscParaGain_B_0x6000[143]={0};
		WORD wSramLscParaGain_Gr_0x511e[143]={0};
		WORD wSramLscParaGain_Gb_0x5000[143]={0};
		for (int i = 0; i< 143; i++)
		{
			GbB = i*3;
			GrR = 429 + i*3;
			wSramLscParaGain_Gb_0x5000[i] = (otp->lsc.LSCGroup1.lscbuffer[GbB] & 0xff)|
				((otp->lsc.LSCGroup1.lscbuffer[GbB+1] & 0x0f)<<8);			//Gb
			wSramLscParaGain_B_0x6000[i] = ((otp->lsc.LSCGroup1.lscbuffer[GbB+2] & 0xff)<<4)|
				((otp->lsc.LSCGroup1.lscbuffer[GbB+1] >>4) & 0x0f);			//B

			wSramLscParaGain_Gr_0x511e[i] = (otp->lsc.LSCGroup1.lscbuffer[GrR] & 0xff)|
				((otp->lsc.LSCGroup1.lscbuffer[GrR+1] & 0x0f)<<8);			//Gr
			wSramLscParaGain_R_0x611e[i] = ((otp->lsc.LSCGroup1.lscbuffer[GrR+2] & 0xff)<<4)|
				((otp->lsc.LSCGroup1.lscbuffer[GrR+1] >>4) & 0x0f);			//R
		}

		//Gb gain
		for (int i = 0; i<143; i++)
		{
			sensor->dev->write_sensor(0x5000 + 2 * i, wSramLscParaGain_Gb_0x5000[i]);
		}
		//Gr gain
		for (int i = 0; i<143; i++)
		{
			sensor->dev->write_sensor(0x511e + 2 * i, wSramLscParaGain_Gr_0x511e[i]);
		}
		//B gain
		for (int i = 0; i<143; i++)
		{
			sensor->dev->write_sensor(0x6000 + 2 * i, wSramLscParaGain_B_0x6000[i]);
		}
		//R gain
		for (int i = 0; i<143; i++)
		{
			sensor->dev->write_sensor(0x611e + 2 * i, wSramLscParaGain_R_0x611e[i]);
		}
		//LSC enable  0x0a05  bit[4]=1
		sensor->dev->write_sensor(0x0a04, 0x015a);
		Sleep(1000);


		if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));
			return OTPCALI_ERROR_SENSOR;		
		}

		dev->DisplayImage(m_bufferObj.pBmpBuffer);

		RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
		uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);
		if (check_lsc(lsc) < 0) 
		{
			return OTPCALI_ERROR_LSCCALI;
		}


		return OTPCALI_ERROR_NO;
	}
}

