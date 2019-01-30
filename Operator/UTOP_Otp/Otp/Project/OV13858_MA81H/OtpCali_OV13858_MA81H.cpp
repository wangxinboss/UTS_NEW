#include "StdAfx.h"
#include "OtpCali_OV13858_MA81H.h"
#include "VCM.h"
#include "algorithm.h"


#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")

#ifndef _Debug
#define _Debug
#endif
//-----------------------------------------------------------------------------

namespace OtpCali_OV13855_MA81H {
	//-------------------------------------------------------------------------------------------------
	OtpCali_OV13855_MA81H::OtpCali_OV13855_MA81H(BaseDevice *dev) : OVOtp(dev)
	{
		otp_type = OTP_TYPE_OTP;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = LSC_LEN_MA81H;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA81H::do_prog_otp()
	{
		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA81H::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA81H::LscCali(void *args)
	{
		using namespace UTS::Algorithm::RI::RI_OpticalCenter;
		using namespace UTS::Algorithm::FixPatternNoise::FixPatternNoise_WD;
		double m_dYvalue;
		FPNInfo m_FPNInfo;
		OTPData *otp = (OTPData*)args;
		int ret = OTPCALI_ERROR_NO;
		struct LSC_PARAM *lsc = &otp_param.lsc_param;
		memset(&m_FPNInfo, 0, sizeof(FPNInfo));

		uint8_t lscoutput[360]={0};
		uint8_t lscinput[186] = {0};

		/*for(int i = 0; i<186; i++)
		{
			lscinput[i] = 0;
		}*/

		dev->GetBufferInfo(m_bufferInfo);

		//////////////////////////////////////////////////////////////////////////
		//Save otp_lsc
		CString strDirPath;
		CString strFilePath;
		SYSTEMTIME st;
		GetLocalTime(&st);

		strDirPath.Format(_T("%sData-%s-%s-%s-%04d%02d%02d_LSCOTPData"),
			uts.info.strResultPath,
			uts.info.strProjectName,
			uts.info.strLineName,
			uts.info.strStationName,
			st.wYear,
			st.wMonth, 
			st.wDay);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		////////////////////////////////////////////////////////////////////////////////
		
		if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));
			ret = OTPCALI_ERROR_SENSOR;
			return SET_ERROR(ret);	
		}

		dev->DisplayImage(m_bufferObj.pBmpBuffer);

		//检查Y值是否在设定区间
		UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
		if (m_dYvalue < otp_param.fpn_param.dltMinY || m_dYvalue > otp_param.fpn_param.dltMaxY)
		{
			ret = uts.errorcode.E_Linumance;
			return SET_ERROR(ret);;
		}

		RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
		uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

		if (otp_param.nReCapture == 1)
		{
			//LSC RAW file name
			strFilePath.Format(_T("%s\\%s_OV13855_BGGR_%dx%d_RAW8_%04d%02d%02d%02d%02d%02d_LSCCapture"),
				strDirPath,
				uts.info.strSN,
				m_bufferInfo.nHeight,
				m_bufferInfo.nWidth,
				st.wYear,
				st.wMonth, 
				st.wDay,
				st.wHour,
				st.wMinute,
				st.wSecond);

			if (!uts.imgFile.SaveRawFile(strFilePath,(BYTE*)m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
			{
				uts.log.Error(_T("OV LSC: failed to save raw file!"));
				return SET_ERROR(OTPCALI_ERROR_LSCCALI);
			}
		}


		if( LenC_Cal_13850R2A_C(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,
			lsc->cali_target,64,lscinput,&otp_lsc_len))
		{
			if(!(Decode_13850R2A(lscinput, lscoutput))) 
			{
				ret = OTPCALI_ERROR_LSCCALI;
				return SET_ERROR(ret);	
			}
		}
		else
		{
			ret = OTPCALI_ERROR_LSCCALI;
			return SET_ERROR(ret);	
		}
		
		//Apply LSC
		int temp = sensor->dev->i2c_read(BIT16_BIT8,0x5000);
		sensor->dev->i2c_write(BIT16_BIT8,0x5000, 0x04 | temp);

		for(int i=0;i<360 ;i++) 
		{
			sensor->dev->write_sensor(0x5500 + i, lscoutput[i]);			
		}
		
		Sleep(1000);

		if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));
			ret =  OTPCALI_ERROR_SENSOR;
			return SET_ERROR(ret);
		}

		dev->DisplayImage(m_bufferObj.pBmpBuffer);
		RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
		uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

		if (otp_param.nReCapture == 1)
		{
			//LSC RAW file name
			strFilePath.Format(_T("%s\\%s_OV13855_BGGR_%dx%d_RAW8_%04d%02d%02d%02d%02d%02d_LSCCapture_Apply"),
				strDirPath,
				uts.info.strSN,
				m_bufferInfo.nHeight,
				m_bufferInfo.nWidth,
				st.wYear,
				st.wMonth, 
				st.wDay,
				st.wHour,
				st.wMinute,
				st.wSecond);

			if (!uts.imgFile.SaveRawFile(strFilePath,(BYTE*)m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
			{
				uts.log.Error(_T("OV LSC: failed to save raw file before apply lsc!"));
				return SET_ERROR(OTPCALI_ERROR_LSCCALI);
			}
		}
		
		//check FPN
		FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
		if (m_FPNInfo.m_dRowDifMax > otp_param.fpn_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.fpn_param.dFPNSpec)
		{
			ret = uts.errorcode.E_FixPatternNoise;
			uts.log.Error(_T("check FPN spec NG   m_dRowDifMax= %f    m_dColDifMax= %f    dThreshold=%f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax, otp_param.fpn_param.dFPNSpec);
			return SET_ERROR(ret);
		}

		if (check_lsc(lsc) < 0) 
		{
			ret = OTPCALI_ERROR_LSCCALI;
			return SET_ERROR(ret);
		}
		
		//upload_lsc:
		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC,
			(char *)lscinput, otp_lsc_len) < 0) 
		{
			uts.log.Error(_T("Failed to update LSC Calibration data to DB!!"));
			return SET_ERROR(OTPCALI_ERROR_DB);
		}

		ret= OTPCALI_ERROR_NO;
		return SET_ERROR(ret);
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA81H::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;

		MINFO *m = (MINFO *)args;
		m->flag = 0x40;	//0x40:Group1 valid, 0xD0:Group2 valid
		m->minfo1.mid = 0x42;	//0x42:BYD

		SYSTEMTIME stime;
		time_t time;
		if (otpDB->GetOtpBurnTime(mid, &time) < 0)
		{
			GetLocalTime(&stime);
			m->minfo1.year  = stime.wYear % 100;
			m->minfo1.month = (uint8_t)stime.wMonth;
			m->minfo1.day   = (uint8_t)stime.wDay;
		}else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			m->minfo1.year = today.tm_year % 100;
			m->minfo1.month = (uint8_t)today.tm_mon + 1;
			m->minfo1.day = (uint8_t)today.tm_mday;
		}

		return sizeof(MINFO);
	}

	int OtpCali_OV13855_MA81H::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;
		WB_PARAM *pwb = &otp_param.wb_param;
		USES_CONVERSION;

		//module info
		int ret = get_minfo_from_db(&otp->minfo);	
		//WB
		WB_DATA_UCHAR wb[2];		
		ret = get_wb_from_raw_data(&wb[0], &wb[1]);
		if (ret < 0) { return ret;}

		WB_RATIO ratio;
		get_wb_ratio(&wb[0], &ratio, 512);

		uts.log.Info(_T("r_g=0x%02x,b_g=0x%02x"),ratio.r_g,ratio.b_g);

		otp->minfo.minfo1.wb.r_g_msb=(BYTE)(ratio.r_g>>2);
		otp->minfo.minfo1.wb.b_g_msb=(BYTE)(ratio.b_g>>2);
		otp->minfo.minfo1.wb.r_g_b_g_lsb=(BYTE)(((ratio.r_g&0x03)<<6)+((ratio.b_g&0x03)<<4));
		
		if(otp->minfo.minfo1.wb.r_g_msb == 0 || otp->minfo.minfo1.wb.b_g_msb == 0)
		{
			ret = OTPCALI_ERROR_NODATA;
			return ret;
		}

		//LSC
		uint8_t DecodeBuf[360];
		int checksumOTP = 0;
		for(int i = 0;i<360; i++)
		{
			DecodeBuf[i] = 0;
		}
		ret = get_lsc_from_raw_data(otp->lsc.lsc1.lsc, sizeof(otp->lsc.lsc1.lsc));
		if (ret < 0) return ret;

		if(Decode_13850R2A(otp->lsc.lsc1.lsc,DecodeBuf))
		{
			for(int i=0; i<360; i++)
			{
				checksumOTP += DecodeBuf[i];
			}
		}
		else
		{
			return OTPCALI_ERROR_LSCCALI;
		}

		otp->lsc.lsc1.lsc_sum = (CheckSum(&otp->lsc.lsc1.lsc, 186)) % 0xFF + 1;
		otp->lsc.lsc1.lsc_decode_sum = checksumOTP % 0xFF + 1;
		otp->lsc.lsc1.total_sum = otp->lsc.lsc1.lsc_sum ^ otp->lsc.lsc1.lsc_decode_sum;

		//AF
		int inf = 0, marcro = 0;
		ret = get_af_from_raw_data(NULL, &inf, &marcro);
		if (ret < 0) return ret;

		//计算姿势差--------------------
		/*marcro = marcro - OFFSETMACRO;
		if(marcro < 0) 
		{
		ret = OTPCALI_ERROR_NODATA;
		return SET_ERROR(ret);
		}

		inf = inf - OFFSETINF;
		if(inf < 0)
		{
		ret = OTPCALI_ERROR_NODATA;
		return SET_ERROR(ret);
		}*/
		//-------------------------------
		otp->af.flag = 0x40;
		otp->af.af1.inf = (BYTE)(inf>>2);
		otp->af.af1.mup = (BYTE)(marcro>>2);
		//01: Upward  10: Horizon  11: Downward 
		otp->af.af1.vcmdir = BYTE(((inf&0x03)<<6)+((marcro&0x03)<<4) + 0x01);
		
		return sizeof(OTPData);
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_OV13855_MA81H::get_wb_cali_data(WB_GAIN *gain, void *out)
	{
		uint8_t *buf = (uint8_t *)out;
		put_be_val(gain->BGain, buf, 2);
		put_be_val(gain->GGain, buf+2, 2);
		put_be_val(gain->RGain, buf+4, 2);
		return 6;
	}

	int OtpCali_OV13855_MA81H::do_check_otp()
	{

		/*uint8_t cData[199];
		uint8_t cCheckSum[186];
		memset(cData,0,199);
		memset(cCheckSum,0,186);

		for(int i=0;i<199;i++){
			cData[i] = otp_data_in_sensor[i];
		}

		for(int i=0;i<186;i++){
			cCheckSum[i] = cData[i+10];
		}
	
		uint8_t _CheckSum[3];
		memset(_CheckSum,0,3);
		_CheckSum[0] = CheckSum(cCheckSum,186) % 0xff +1;

		if((cData[6] == 0) || (cData[7] ==0) || (cData[1] != 0x08) || (cData[2] != 0x03)) {
			return OTPCALI_ERROR_CHKERR;
		}

		if(_CheckSum[0] !=  cData[196]){
			return OTPCALI_ERROR_CHKERR;
		}*/

 		return OTPCALI_ERROR_NO;
	}
}
