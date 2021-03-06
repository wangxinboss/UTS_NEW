#include "StdAfx.h"
#include "OtpCali_S5K3L6_MA85V.h"
#include "VCM.h"
#include "algorithm.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")

namespace OtpCali_S5K3L6_MA85V 
{
	OtpCali_S5K3L6_MA85V::OtpCali_S5K3L6_MA85V(BaseDevice *dev):SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len =  sizeof(OTPData);
		otp_lsc_len = MTK_LSC_LEN;
		otp_spc_len = MTK_SPC_LEN;
		otp_dcc_len = MTK_DCC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
	}

	int OtpCali_S5K3L6_MA85V::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
		//烧录数据测试-------------------------------------------------------------------------------------------------------

	}

	int OtpCali_S5K3L6_MA85V::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}

	int OtpCali_S5K3L6_MA85V::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

		//module info
		otp->mInfoFlag = 0x01;
		int ret = get_minfo_from_db(&otp->mInfo);

		//AF 
		int inf = 0, marcro = 0;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0) return ret;
		otp->afFlag = 0x01;
		//otp->af.afGroup1.Af_Cal = 0x01;
		put_be_val(inf, otp->af.Af_Inf, sizeof(otp->af.Af_Inf));
		put_be_val(marcro, otp->af.Af_Mup, sizeof(otp->af.Af_Mup)); //酚蔼逼function

		otp->af.afchecksum = (CheckSum(&otp->af.Af_Inf[0],4))%256;

		//WB
		otp->wb_flag = 0x01;
		
		
		WB_DATA_UCHAR wb[2];
		ret = get_wb_from_raw_data(&wb[0], &wb[1]);
		if (ret < 0) { return ret;}

		otp->wb.r = wb[0].R ;
		otp->wb.gb = wb[0].Gb ;
		otp->wb.gr = wb[0].Gr ;
		otp->wb.b=  wb[0].B;
		

		otp->wb.g_r = 0;
		otp->wb.g_gr = 0 ;
		otp->wb.g_gb = 0;
		otp->wb.g_b = 0 ;

		otp->wb.wb_CheckSum = (CheckSum(&otp->wb.r, 8)) % 256;

		//LSC
		otp->lscFlag = 0x01;
		ret = get_lsc_from_raw_data(otp->lsc.lscbuffer, otp_lsc_len);
		if (ret < 0) return ret;
		otp->lsc.lscchecksum = (CheckSum(&otp->lsc.lscbuffer, MTK_LSC_LEN)) % 256;

		//pdaf
		otp->pdafflag=0x01;
		get_otp_from_raw_data(OTPDB_OTPTYPE_SPC, otp->pdaf.spc, sizeof(otp->pdaf.spc));
		get_otp_from_raw_data(8, otp->pdaf.dcc, sizeof(otp->pdaf.dcc));
		otp->pdaf.checksum = (CheckSum(&otp->pdafflag, 1405)-1)%256 ;

		return sizeof(OTPData);
	}

	int OtpCali_S5K3L6_MA85V::LscCali(void *args)
	{
		return do_mtk_lsc_cali();
	}

	int OtpCali_S5K3L6_MA85V::get_minfo_from_db(void *args)
	{
		MINFO_Group *m = (MINFO_Group *)args;
		m->moudleID = 0x42;
		m->LensID = 0x47;
		m->vcmID=0x5E;
		m->driverICID=0x13;

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

		int sum = CheckSum(&m->moudleID, 7);

		m->mInfoCheckSum = sum % 256;

		return sizeof(MINFO_Group);
	}

	/*
	int OtpCali_S5K3L6_MA85V::do_check_otp(void *args)
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
		uint8_t lsc_checkSum = (CheckSum(&(otp->lsc.LSCGroup1.lscbuffer), MTK_LSC_LEN_MA85V)) % 255 + 1;
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
	*/
	int OtpCali_S5K3L6_MA85V::DCC0Cali(void *args)
	{
		return  mtk_dcc_cali("S5K3L8_MirrorFlip_mtk.ini");
	}


	int OtpCali_S5K3L6_MA85V::PDAFVerify(void *args)
	{
		return mtk_dcc_verify("S5K3L8_MirrorFlip_mtk.ini");
	}

	int OtpCali_S5K3L6_MA85V::do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len)
	{
		return mtk_spc_cali(pRaw10,"S5K3L8_MirrorFlip_mtk.ini",out);
	}
}

