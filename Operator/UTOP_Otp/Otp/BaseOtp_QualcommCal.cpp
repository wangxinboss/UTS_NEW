#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "malloc.h"
#include "UTS.h"
#include <direct.h>

using namespace UTS::Algorithm::FixPatternNoise::FixPatternNoise_WD;
using namespace UTS::Algorithm::RI::RI_OpticalCenter;

int BaseOtp::do_qulcomm_lsc_awb_cali()
{
	uts.log.Info(_T("BaseOtp_QualcommCal:do_qulcomm_lsc_awb_cali"));

	int ret = OTPCALI_ERROR_NO;
	uint8_t lscoutput[1768],awbout[6];
	int Lenth_LSC = 0;
	
	//WB_DATA_SHORT wb;
	WB_PARAM *pwb = &otp_param.wb_param;
	LSC_PARAM *plsc = &otp_param.lsc_param;

	double m_dYvalue;
	FPNInfo m_FPNInfo;
	memset(&m_FPNInfo, 0, sizeof(FPNInfo));
	dev->GetBufferInfo(m_bufferInfo);

	//////////////////////////////////////////////////////////////////////////
	//Save otp_lsc
	CString strDirPath;
	CString strFilePath;
	SYSTEMTIME st;
	GetLocalTime(&st);

	strDirPath.Format(_T("%sImage-%s-%s-%s-%04d%02d%02d_AWBLSCOTPData"),
		uts.info.strResultPath,
		uts.info.strProjectName,
		uts.info.strLineName,
		uts.info.strStationName,
		st.wYear,
		st.wMonth, 
		st.wDay);

	UTS::OSUtil::CreateMultipleDirectory(strDirPath);
	////////////////////////////////////////////////////////////////////////////////
	
	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		ret =  OTPCALI_ERROR_SENSOR;
		goto out;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	//检查Y值是否在设定区间
// 	UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
// 	if (m_dYvalue < otp_param.fpn_param.dltMinY || m_dYvalue > otp_param.fpn_param.dltMaxY)
// 	{
// 		uts.log.Error(_T("m_dYvalue error: %.f"),m_dYvalue);
// 		ret = OTPCALI_ERROR_WBCALI;
// 		goto out;
// 	}

	//check FPN
// 	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
// 	if (m_FPNInfo.m_dRowDifMax > otp_param.fpn_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.fpn_param.dFPNSpec)
// 	{
// 		uts.log.Error(_T("check FPN spec NG   m_dRowDifMax= %f    m_dColDifMax= %f    dThreshold=%f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax, otp_param.fpn_param.dFPNSpec);
// 		ret = OTPCALI_ERROR_WBCALI;
// 		goto out;
// 	}

	if (otp_param.nReCapture == 1)
	{
		//AWB RAW file name
		strFilePath.Format(_T("%s\\%s_S5K4H7SX_GRBG_%dx%d_RAW8_%04d%02d%02d%02d%02d%02d_AWBCapture"),
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
			uts.log.Error(_T("OV AWB: failed to save raw file!"));
			ret = OTPCALI_ERROR_WBCALI;
			goto out;
		}

		if (!uts.imgFile.SaveBmpFile(strFilePath,(BYTE*)m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
		{
			uts.log.Error(_T("OV AWB: failed to save bmp file!"));
			ret = OTPCALI_ERROR_WBCALI;
			goto out;
		}
	}

	int nOutMode = 0;

	if (!dev->ReadValue(
		eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
		&nOutMode, sizeof(nOutMode)))
	{
		uts.log.Error(_T("GetSensorOutMode error."));
		ret = uts.errorcode.E_NoImage;
	}

	//Check Light 
	RECT rect;
	RGBTRIPLE rgb;
	WB_DATA_UCHAR wb_tmp;
	SIZE image_size; 
	
	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	ROI roi;
	GetWBROI(&otp_param.wb_param, &image_size, &roi);
	uts.log.Debug(_T("GetWBROI:%d %d"),roi.width,roi.height);

	rect.left = roi.x;
	rect.top =  roi.y;
	rect.right = roi.x + roi.width;
	rect.bottom = roi.y + roi.height;

	UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer, 
		m_bufferInfo.nWidth, m_bufferInfo.nHeight, 
		0, rect, rgb);

	wb_tmp.B = rgb.rgbtBlue;
	wb_tmp.Gb = rgb.rgbtGreen;
	wb_tmp.Gr = rgb.rgbtGreen;
	wb_tmp.R = rgb.rgbtRed;

	if (CheckWBLimit(&wb_tmp) < 0) {
		ret = OTPCALI_ERROR_WBCALI;
		goto out;
	}



	//Cal
	Lenth_LSC = do_qualcomm_LSCAWBCali(nOutMode,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,lscoutput,awbout);
	
	//memcpy(&wb,awbout,sizeof(unsigned short int)*3);

	////multi WB
	//WB_DATA_SHORT *am = &pwb->raw_after_multi2;
	//wb_do_multi(&wb, am);
	//uts.log.Debug(_T("WB(after multi,rgm[%.2lf],bgm[%.2lf]): RG[%d],BG[%d],GrGb[%d]"),pwb->rg_multi,pwb->bg_multi,am->RG,am->BG,am->GbGr);

	////check and update WB with golden
	//if ((ret = upload_wb_with_golden(am)) < 0)
	//{ret = -ret; goto out;}

	memcpy(&plsc->LSCTable,lscoutput,Lenth_LSC);
	if ((ret = check_lsc_with_golden(lscoutput,Lenth_LSC)) < 0)
	{ret = -ret; goto out;}

out:
	return SET_ERROR(ret);
}

int BaseOtp::wb_do_multi(const WB_DATA_SHORT *in, WB_DATA_SHORT *out)
{
	WB_PARAM *pwb = &otp_param.wb_param;

	out->RG = int(in->RG * pwb->rg_multi + 0.5);
	out->GbGr = in->GbGr;
	out->BG = int(in->BG * pwb->bg_multi + 0.5);
	return 0;
}

int BaseOtp::upload_wb_with_golden(const WB_DATA_SHORT *wb)
{
	WB_PARAM *pwbp = &otp_param.wb_param;

	time_t tm = 0;
	int is_golden_self = !_tcscmp(m_szSN, pwbp->goldenSampleName);
	int is_only_upload = !_tcscmp(pwbp->goldenSampleName, EMPTY_STR);

	WB_DATA_SHORT wb_db[2];
	memset(wb_db, 0, sizeof(wb_db));
	memcpy(&wb_db[0], wb, sizeof(WB_DATA_SHORT));

	if (!is_only_upload) 
	{
		USES_CONVERSION;
		int golden_id = otpDB->GetModuleID(T2A(pwbp->goldenSampleName), false);
		if (golden_id < 0) 
		{
			uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
			return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
		}
		if (otpDB->GetOtpByType(golden_id, DDM::OTPDB_OTPTYPE_AWB,
			(char *)&wb_db[1], sizeof(WB_DATA_SHORT), &tm) < 0)
		{
				uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
				return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
		}
		if (!is_golden_self) 
		{
			time_t delta = time(NULL) - tm; 
			if (delta > pwbp->goldenValidTime * 3600) 
			{
				MessageBox(NULL, _T("请重新校验GoldenSample"), _T("错误"), NULL);
				return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
			} 
			else if (delta > pwbp->goldenWarningTime * 3600) 
			{
				time_t remaining_tm = pwbp->goldenValidTime*3600 - delta;
				TCHAR buf[0x100];
				_stprintf(buf, _T("GoldenSample剩余有效时间：%lld小时, %lld秒"),
					remaining_tm / 3600, remaining_tm % 3600);
				MessageBox(NULL, buf, _T("警告"), NULL);
			}
		}

		// wb_db[0] : module  ;  wb_db[1] : golden
		int ret = check_wb_qualcommm_with_golden(wb_db[0],wb_db[1]); 
		if (ret < 0) return ret;
	}
	//未导入Golden前的 AWB卡控 by杨兵20180412
	int ret = check_wb_qualcommm_with_golden(wb_db[0],wb_db[1]); 
	if (ret < 0) return ret;

	if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB,
		(char *)wb_db, sizeof(wb_db)) < 0)
	{
		uts.log.Error(_T("Failed to update WB data to DB!"));
		return SET_ERROR(OTPCALI_ERROR_DB);
	}
	free(wb_db);
	return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::check_wb_qualcommm_with_golden(const WB_DATA_SHORT wb,const WB_DATA_SHORT wb_golden)
{
	WB_DATA_SHORT get_wb_golden;
	get_wb_golden.RG = 533;
	get_wb_golden.BG = 698;
	get_wb_golden.GbGr = 1023;

	otp_param.wb_param.delta_rgVSG = abs((double(wb.RG) / double(get_wb_golden.RG)) -1.0 );
	otp_param.wb_param.delta_bgVSG = abs((double(wb.BG) / double(get_wb_golden.BG)) -1.0 );
	otp_param.wb_param.delta_gbgrVSG = abs((double(wb.GbGr) / double(get_wb_golden.GbGr)) -1.0 );

	double delta_spec = otp_param.wb_param.golden_rate_spec;
	double delta_specgrgb = otp_param.wb_param.golden_rate_spec_gbgr;

	uts.log.Debug(_T("Self:RG[%d],BG[%d],GrGb[%d], Golden:RG[%d],BG[%d],GrGb[%d]"),
		wb.RG, wb.BG, wb.GbGr, 
		get_wb_golden.RG, get_wb_golden.BG, get_wb_golden.GbGr);

	uts.log.Debug(_T("RGDta[%.4lf],BGDta[%.4lf],GrGbDta[%.4lf],Spec[%.4lf] [%.4lf]"),
		otp_param.wb_param.delta_rgVSG, otp_param.wb_param.delta_bgVSG,otp_param.wb_param.delta_gbgrVSG , delta_spec,delta_specgrgb);

	if (otp_param.wb_param.delta_rgVSG > delta_spec || otp_param.wb_param.delta_bgVSG > delta_spec 
		|| (wb.GbGr< 1000 || wb.GbGr > 1200)) 
	{
		uts.log.Debug(_T("WB Self vs Golden delta out of sepc!"));
		return SET_ERROR(OTPCALI_ERROR_WBCALI);
	}

	/*
	//未导入Golden前的 AWB卡控 by杨兵20180412
	if (_tcscmp(m_szSN, otp_param.wb_param.goldenSampleName)) 
	{
	delta_spec = otp_param.wb_param.golden_be_spec; 

	if (otp_param.wb_param.delta_rgVSG < delta_spec && otp_param.wb_param.delta_bgVSG < delta_spec && otp_param.wb_param.delta_gbgrVSG < delta_spec)
	{
	uts.log.Debug(_T("Should be golden sample!, golden spec[%.4lf]"), delta_spec);
	return SET_ERROR(OTPCALI_ERROR_SHOULDBE_GOLDEN);
	}
	}*/


	return SET_ERROR(OTPCALI_ERROR_NO);
}


int BaseOtp::check_lsc_with_golden(uint8_t *modulelsc,int lsclength)
{
	LSC_PARAM *pwbp = &otp_param.lsc_param;

	time_t tm = 0;
	int is_only_upload = !_tcscmp(pwbp->goldenSampleName, EMPTY_STR);

	uint8_t goldenlsc[0x1200];

	if (!is_only_upload) {
		USES_CONVERSION;
		int golden_id = otpDB->GetModuleID(T2A(pwbp->goldenSampleName), false);
		if (golden_id < 0) {
			uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
			return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
		}
		if (otpDB->GetOtpByType(golden_id, DDM::OTPDB_OTPTYPE_LSC,
			(char *)goldenlsc, lsclength, &tm) < 0) {
				uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
				return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
		}
	
		int ret = check_lsc_qualcommm_with_golden(modulelsc,goldenlsc,lsclength); 
		if (ret < 0) return ret;
	}

	//杨兵 2018/04/18
	int ret = check_lsc_qualcommm(modulelsc,lsclength); 
	if (ret < 0) return ret;

	////////////////上传DB////////////////////////////////////////////////////
	if(upload_lsc_cali_data(modulelsc, lsclength) < 0)
	{
		uts.log.Error(_T("Failed to update LSC data to DB!"));
		return SET_ERROR(OTPCALI_ERROR_DB);
	}

	return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::check_lsc_qualcommm(uint8_t *module,int lsclength)
{
	uts.log.Debug(_T("check_lsc_qualcommm Start"));
	LSC_PARAM *plsc = &otp_param.lsc_param;
	//卡控规格：
	int LSC_R_Max = 1023;
	int LSC_Gr_Max= 1023;
	int LSC_Gb_Max= 1023;
	int LSC_B_Max = 1023;
	int LSC_R_Min=  50;
	int LSC_Gr_Min= 100;
	int LSC_Gb_Min= 100;
	int LSC_B_Min=  50;
	int RGRatio_H=120;		//R/G
	int	RGRatio_L=75;
	int	BGRatio_H=120;		//B/G
	int	BGRatio_L=75;
	int LSCDistanceSpec = 120;

	int LSCData_R_Max,LSCData_Gr_Max,LSCData_Gb_Max,LSCData_B_Max;
	LSCData_R_Max=LSCData_Gr_Max=LSCData_Gb_Max=LSCData_B_Max=0;
	int LSCData_R_Min,LSCData_Gr_Min,LSCData_Gb_Min,LSCData_B_Min;
	LSCData_R_Min=LSCData_Gr_Min=LSCData_Gb_Min=LSCData_B_Min=1023;

	CString sfilename = _T("Standard.ini");
	CString sTmp      = _T("");

	FILE *fp = NULL;
	fp = fopen("Standard.ini", "a");
	fclose(fp);

	CStdioFile MyFile(sfilename, CFile::modeRead | CFile::typeText);  

	int StandardBuf[17*13][4] = {0};

	for( int i = 0; i < 221; i++ )
	{
		MyFile.ReadString( sTmp);
		StandardBuf[i][0]=_ttoi( sTmp);

		MyFile.ReadString( sTmp);
		StandardBuf[i][1]=_ttoi( sTmp);

		MyFile.ReadString( sTmp);
		StandardBuf[i][2]=_ttoi( sTmp);

		MyFile.ReadString( sTmp);
		StandardBuf[i][3]=_ttoi( sTmp);
	}

	MyFile.Close();

	int tempBuf[17*13][4] = {0};
	for( int i = 0; i < 221; i++ )
	{
		tempBuf[i][0] = plsc->LSCTable.R[i];//R
		tempBuf[i][1] = plsc->LSCTable.GR[i];//GR
		tempBuf[i][2] = plsc->LSCTable.Gb[i];//GB
		tempBuf[i][3] = plsc->LSCTable.B[i];//B


		if (tempBuf[i][0]<LSCData_R_Min)
		{
			LSCData_R_Min=tempBuf[i][0];
		}
		if (tempBuf[i][0]>LSCData_R_Max)
		{
			LSCData_R_Max=tempBuf[i][0];
		}
		if (tempBuf[i][1]<LSCData_Gr_Min)
		{
			LSCData_Gr_Min=tempBuf[i][1];
		}
		if (tempBuf[i][1]>LSCData_Gr_Max)
		{
			LSCData_Gr_Max=tempBuf[i][1];
		}
		if (tempBuf[i][2]<LSCData_Gb_Min)
		{
			LSCData_Gb_Min=tempBuf[i][2];
		}
		if (tempBuf[i][2]>LSCData_Gb_Max)
		{
			LSCData_Gb_Max=tempBuf[i][2];
		}
		if(tempBuf[i][3]<LSCData_B_Min)
		{
			LSCData_B_Min=tempBuf[i][3];
		}
		if (tempBuf[i][3]>LSCData_B_Max)
		{
			LSCData_B_Max=tempBuf[i][3];
		}
	}

	int tempMax = 0;
	int tempDistance = 0;

	float CenterRG=(float)tempBuf[111][0]*2/(tempBuf[111][1]+tempBuf[111][2]);
	float CenterBG=(float)tempBuf[111][3]*2/(tempBuf[111][1]+tempBuf[111][2]);
	uts.log.Debug(_T("CenterRG=%0.3f,CenterBG=%0.3f"), CenterRG,CenterBG);
	float tempRG[221]={0};
	float tempBG[221]={0};

	for(int i = 0; i < 17*13; i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			tempDistance = abs(tempBuf[i][j] - StandardBuf[i][j]);

			if( tempDistance > tempMax )
			{
				tempMax = tempDistance;
			}		 
		}
		tempRG[i]=(float)tempBuf[i][0]*2/(float)(tempBuf[i][1]+tempBuf[i][2])/CenterRG;
		tempBG[i]=(float)tempBuf[i][3]*2/(float)(tempBuf[i][1]+tempBuf[i][2])/CenterBG;
	}

	int tempG = 0;
	for(int i = 0; i < 221; i++ )
	{
		tempG = tempRG[i]*100;
		if (tempG > RGRatio_H || tempG < RGRatio_L)
		{
			uts.log.Error(_T("%d Block RG Out Range"),i);
			return SET_ERROR(OTPCALI_ERROR_LSCCALI);
		}
		tempG=tempBG[i]*100;
		if (tempG > BGRatio_H || tempG < BGRatio_L)
		{
			uts.log.Error(_T("%d Block BG Out Range"),i);
			return SET_ERROR(OTPCALI_ERROR_LSCCALI);
		}
	}

	
	uts.log.Debug(_T("LSCData_R_Max=%d,LSCData_R_Min=%d,LSCData_Gr_Max=%d,LSCData_Gr_Min=%d,\
					 LSCData_Gb_Max=%d,LSCData_Gb_Min=%d,LSCData_B_Max=%d,LSCData_B_Min=%d"),
					 LSCData_R_Max,LSCData_R_Min,
					 LSCData_Gr_Max,LSCData_Gr_Min,
					 LSCData_Gb_Max,LSCData_Gb_Min,
					 LSCData_B_Max,LSCData_B_Min);
	if (LSCData_R_Max != LSC_R_Max|| LSCData_R_Min<LSC_R_Min ||LSCData_Gr_Max != LSC_Gr_Max || LSCData_Gr_Min<LSC_Gr_Min
		|| LSCData_Gb_Max != LSC_Gb_Max || LSCData_Gb_Min<LSC_Gb_Min || LSCData_B_Max != LSC_B_Max || LSCData_B_Min<LSC_B_Min)
	{	
		uts.log.Error(_T("LSCData Out Range"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	uts.log.Debug(_T("LSCDistanceSpec: %d,DeltMax:%d"), LSCDistanceSpec,tempMax);
	/*if( tempMax > LSCDistanceSpec )
	{
	uts.log.Error(_T("LSCDistanceSpec Out Range"));
	return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	} */
	uts.log.Debug(_T("check_lsc_qualcommm End"));

	return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::check_lsc_qualcommm_with_golden(uint8_t *module,uint8_t *golden,int lsclength)
{
	LSC_PARAM *pwbp = &otp_param.lsc_param;
	unsigned short *modulelsc,*goldenlsc;
	double delta_spec = otp_param.lsc_param.golden_rate_spec;
	double delta = 0.0;

	pwbp->maxdeltarate = 0;

	modulelsc = new unsigned short[int(lsclength/2)];
	goldenlsc = new unsigned short[int(lsclength/2)];

	memcpy(modulelsc,module,sizeof(uint8_t)*lsclength);
	memcpy(goldenlsc,golden,sizeof(uint8_t)*lsclength);

	for (int i= 0 ;i< lsclength/2 ; i++)
	{
		if(goldenlsc[i] == 0) //avoid divid 0
		{
			delta = abs((double(modulelsc[i]+1) / double(goldenlsc[i]+1)) -1.0 );
		}
		else
		{
			delta = abs((double(modulelsc[i]) / double(goldenlsc[i])) -1.0 );
		}

		if(delta > pwbp->maxdeltarate) pwbp->maxdeltarate = delta;

		if (delta > delta_spec) 
		{
			uts.log.Debug(_T("LSC Self vs Golden delta out of sepc!delta[%.2f] spec[%.2f]"),delta,delta_spec);
			free(modulelsc);
			free(goldenlsc);
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}
	}

	free(modulelsc);
	free(goldenlsc);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-----------------------------add by wx
int BaseOtp::do_qulcomm_cali(int type)
{
	int ret = OTPCALI_ERROR_NO;
	uint8_t lscoutput[1768],awbout[8];
	int Lenth_LSC = 0;

	//WB_DATA_SHORT wb;
	WB_PARAM *pwb = &otp_param.wb_param;
	LSC_PARAM *plsc = &otp_param.lsc_param;

	double m_dYvalue;
	FPNInfo m_FPNInfo;
	memset(&m_FPNInfo, 0, sizeof(FPNInfo));
	dev->GetBufferInfo(m_bufferInfo);
	//////////////////////////////////////////////////////////////////////////
	//Save otp_lsc
	CString strDirPath;
	CString strFilePath;
	SYSTEMTIME st;
	GetLocalTime(&st);

	strDirPath.Format(_T("%sImage-%s-%s-%s-%04d%02d%02d_AWBLSCOTPData"),
		uts.info.strResultPath,
		uts.info.strProjectName,
		uts.info.strLineName,
		uts.info.strStationName,
		st.wYear,
		st.wMonth, 
		st.wDay);

	UTS::OSUtil::CreateMultipleDirectory(strDirPath);
	////////////////////////////////////////////////////////////////////////////////

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		ret =  OTPCALI_ERROR_SENSOR;
		goto out;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	//检查Y值是否在设定区间
	UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
	if (m_dYvalue < otp_param.fpn_param.dltMinY || m_dYvalue > otp_param.fpn_param.dltMaxY)
	{
		ret = OTPCALI_ERROR_WBCALI;
		goto out;
	}
// 
// 	//check FPN
// 	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
// 	if (m_FPNInfo.m_dRowDifMax > otp_param.fpn_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.fpn_param.dFPNSpec)
// 	{
// 		uts.log.Error(_T("check FPN spec NG   m_dRowDifMax= %f    m_dColDifMax= %f    dThreshold=%f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax, otp_param.fpn_param.dFPNSpec);
// 		ret = OTPCALI_ERROR_WBCALI;
// 		goto out;
// 	}

	if (otp_param.nReCapture == 1)
	{
		//AWB RAW file name
		strFilePath.Format(_T("%s\\%s_S5K3P9SX_GRBG_%dx%d_RAW8_%04d%02d%02d%02d%02d%02d_AWBCapture"),
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
			uts.log.Error(_T("OV AWB: failed to save raw file!"));
			ret = OTPCALI_ERROR_WBCALI;
			goto out;
		}

		if (!uts.imgFile.SaveBmpFile(strFilePath,(BYTE*)m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
		{
			uts.log.Error(_T("OV AWB: failed to save bmp file!"));
			ret = OTPCALI_ERROR_WBCALI;
			goto out;
		}
	}

	int nOutMode = 0;

	if (!dev->ReadValue(
		eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
		&nOutMode, sizeof(nOutMode)))
	{
		uts.log.Error(_T("GetSensorOutMode error."));
		ret = uts.errorcode.E_NoImage;
	}

	//Check Light 
	RECT rect;
	RGBTRIPLE rgb;
	WB_DATA_UCHAR wb_tmp;
	SIZE image_size; 

	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	ROI roi;
	GetWBROI(&otp_param.wb_param, &image_size, &roi);
	uts.log.Debug(_T("GetWBROI:%d %d"),roi.width,roi.height);

	rect.left = roi.x;
	rect.top =  roi.y;
	rect.right = roi.x + roi.width;
	rect.bottom = roi.y + roi.height;

	UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer, 
		m_bufferInfo.nWidth, m_bufferInfo.nHeight, 
		0, rect, rgb);

	wb_tmp.B = rgb.rgbtBlue;
	wb_tmp.Gb = rgb.rgbtGreen;
	wb_tmp.Gr = rgb.rgbtGreen;
	wb_tmp.R = rgb.rgbtRed;
	// TODO 三种色温要求的g值不一样，需要根据type 判断
	if (CheckWBLimit(&wb_tmp) < 0) {
		uts.log.Debug(_T("R,Gr,Gb,B out of spec,R=%d,Gr=%d,Gb=%d,B=%d"),wb_tmp.R,wb_tmp.Gr,wb_tmp.Gb,wb_tmp.B);
		uts.log.Debug(_T("maybe you  should check light source or change auto expouse"));
		ret = OTPCALI_ERROR_WBCALI;
		goto out;
	}

	if(type==0){
		//------------------
		//管控apply前LSC规格
		struct LSC_PARAM *lsc = &otp_param.lsc_param;
		dev->GetBufferInfo(m_bufferInfo);
		if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));

			return OTPCALI_ERROR_SENSOR;
		}
		dev->DisplayImage(m_bufferObj.pBmpBuffer);

		RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);

		uts.log.Debug(_T("LSC(before cali): RI=%.2f, Delta=%.2f\n"), lsc->riResult.dRI, lsc->riResult.dRIDelta);

		if (check_lsc(lsc) < 0) {
			ret = SET_ERROR(OTPCALI_ERROR_LSCCALI);
			goto out;
		}
		//------------------
		Lenth_LSC = do_qualcomm_Type_Cali(nOutMode,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,lscoutput,type);
		memcpy(&plsc->LSCTable,lscoutput,Lenth_LSC);
		if ((ret = check_lsc_with_golden(lscoutput,Lenth_LSC)) < 0)
		{ret = -ret; goto out;}
	}else{
		
		Lenth_LSC = do_qualcomm_Type_Cali(nOutMode,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,awbout,type);
		memcpy(&pwb->wb_data_c2,awbout,Lenth_LSC);
		//do_wb_cali();还原后管控百分之三TODO
		WB_DATA_USHORT  wb_g,wb;
		WB_RATIO ratio, ratio_g;
		wb_g.R=otp_param.wb_param.RGrGbB_g[0];
		wb_g.Gr=otp_param.wb_param.RGrGbB_g[1];
		wb_g.Gb=otp_param.wb_param.RGrGbB_g[2];
		wb_g.B=otp_param.wb_param.RGrGbB_g[3];
		memcpy(&wb, &pwb->wb_data_c2, sizeof(WB_DATA_USHORT)); 

		get_wb_ratio(&wb, &ratio, 1.0);
		//do wb Vs golden wx
		uts.log.Debug(_T("gr=%d"),wb_g.R);
		if(wb_g.R>0&&wb_g.B>0&&wb_g.Gb>0&&wb_g.Gr>0)
		{
			uts.log.Info(_T("start to check awb with golden"));
			
			get_wb_ratio(&wb_g, &ratio_g, 1.0);
			int ret = check_wb_with_golden(&ratio, &ratio_g); 
			if (ret < 0) return ret;
		}
		//do wb VS wbTarget
		pwb->rg_delta = abs(ratio.dr_g - pwb->rg_target);
		pwb->bg_delta = abs(ratio.db_g - pwb->bg_target);
		if (pwb->rg_delta > pwb->rg_delta_spec || pwb->bg_delta > pwb->bg_delta_spec) {
			uts.log.Debug(_T("R=%d,Gr=%d,Gb=%d,B=%d"),wb.R,wb.Gr,wb.Gb,wb.B);
			uts.log.Debug(_T("WB Self vs target delta out of sepc!"));
			return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}

		if ((ret = upload_wb_with_golden(&pwb->wb_data_c2)) < 0)
		{ret = -ret; goto out;}
	}
	//Cal
	//memcpy(&wb,awbout,sizeof(unsigned short int)*3);
	////multi WB
	//WB_DATA_SHORT *am = &pwb->raw_after_multi2;
	//wb_do_multi(&wb, am);
	//uts.log.Debug(_T("WB(after multi,rgm[%.2lf],bgm[%.2lf]): RG[%d],BG[%d],GrGb[%d]"),pwb->rg_multi,pwb->bg_multi,am->RG,am->BG,am->GbGr);

	////check and update WB with golden
	//if ((ret = upload_wb_with_golden(am)) < 0)
	//{ret = -ret; goto out;}
out: 
	return SET_ERROR(ret);
}
