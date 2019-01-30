#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"


#include <direct.h>

int BaseOtp::do_qulcomm_lsc_awb_cali()
{
	int ret = OTPCALI_ERROR_NO;
	uint8_t lscoutput[1768+8],awbout[6];
	int Lenth_LSC = 0;
	
	WB_DATA_SHORT wb;
	WB_PARAM *pwb = &otp_param.wb_param;
	LSC_PARAM *plsc = &otp_param.lsc_param;

	dev->GetBufferInfo(m_bufferInfo);
	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		ret =  OTPCALI_ERROR_SENSOR;
		goto out;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

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

	using namespace UTS::Algorithm::Image;

	unsigned char *RAW8Image = new unsigned char[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
	memcpy(RAW8Image,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth* m_bufferInfo.nHeight);
//	CString strImg = 
	if (!uts.imgFile.SaveRawFile(_T("AWB_Cali_raw"),RAW8Image,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("LSC: failed to save raw file!"));
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;
	}

	//Cal
	Lenth_LSC = do_qualcomm_LSCAWBCali(nOutMode,RAW8Image,m_bufferInfo.nWidth,m_bufferInfo.nHeight,lscoutput,awbout);
	
	do_save_buff("Qual_LSC_out.txt",lscoutput,Lenth_LSC);
	memcpy(&wb,awbout,sizeof(unsigned short int)*3);

	////multi WB
	//WB_DATA_SHORT *am = &pwb->raw_after_multi2;
	//wb_do_multi(&wb, am);
	//uts.log.Debug(_T("WB(after multi,rgm[%.2lf],bgm[%.2lf]): RG[%d],BG[%d],GrGb[%d]"),pwb->rg_multi,pwb->bg_multi,am->RG,am->BG,am->GbGr);

	//check and update WB with golden
//	if ((ret = upload_wb_with_golden(am)) < 0)
//	{ret = -ret; goto out;}
	;

	if ((ret = check_lsc_with_golden(lscoutput,Lenth_LSC)) < 0)
	{ret = -ret; goto out;}

	memcpy(&plsc->LSCTable,lscoutput,1768);
out:
	RELEASE_ARRAY(RAW8Image);
	return SET_ERROR(ret);
}

int BaseOtp::do_qulcomm_lsc_awb_cali_RAW10()
{
	int ret = OTPCALI_ERROR_NO;
	uint8_t lscoutput[1768+8],awbout[6];
	int Lenth_LSC = 0;

	WB_DATA_SHORT wb;
	WB_PARAM *pwb = &otp_param.wb_param;
	LSC_PARAM *plsc = &otp_param.lsc_param;

	dev->GetBufferInfo(m_bufferInfo);
	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		ret =  OTPCALI_ERROR_SENSOR;
		goto out;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

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

	using namespace UTS::Algorithm::Image;

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	
	//	CString strImg = 
	
	if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_raw"),(BYTE*)m_bufferObj.pFrameBuffer,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("LSC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	//Cal
	Lenth_LSC = do_qualcomm_LSCAWBCali(nOutMode,(unsigned char*) RAW10Image,m_bufferInfo.nWidth,m_bufferInfo.nHeight,lscoutput,awbout);

	do_save_buff("Qual_LSC_out.txt",lscoutput,Lenth_LSC);
	memcpy(&wb,awbout,sizeof(unsigned short int)*3);

	//multi WB
	//WB_DATA_USHORT *am = &pwb->raw_after_multi2;
	//wb_do_multi(&wb, am);
	//uts.log.Debug(_T("WB(after multi,rgm[%.2lf],bgm[%.2lf]): RG[%d],BG[%d],GrGb[%d]"),pwb->rg_multi,pwb->bg_multi,am->RG,am->BG,am->GbGr);

	//check and update WB with golden
	//	if ((ret = upload_wb_with_golden(am)) < 0)
	//	{ret = -ret; goto out;}
	;

	if ((ret = check_lsc_with_golden(lscoutput,Lenth_LSC)) < 0)
	{ret = -ret; goto out;}

	memcpy(&plsc->LSCTable,lscoutput,Lenth_LSC);
out:
	//	RELEASE_ARRAY(RAW10Image);
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

	if (!is_only_upload) {
		USES_CONVERSION;
		int golden_id = otpDB->GetModuleID(T2A(pwbp->goldenSampleName), false);
		if (golden_id < 0) {
			uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
			return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
		}
		if (otpDB->GetOtpByType(golden_id, DDM::OTPDB_OTPTYPE_AWB,
			(char *)&wb_db[1], sizeof(WB_DATA_SHORT), &tm) < 0) {
				uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
				return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
		}
		if (!is_golden_self) {
			time_t delta = time(NULL) - tm; 
			if (delta > pwbp->goldenValidTime * 3600) {
				MessageBox(NULL, _T("请重新校验GoldenSample"), _T("错误"), NULL);
				return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
			} else if (delta > pwbp->goldenWarningTime * 3600) {
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

	if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB,
		(char *)wb_db, sizeof(wb_db)) < 0)
	{
		uts.log.Error(_T("Failed to update WB data to DB!"));
		return SET_ERROR(OTPCALI_ERROR_DB);
	}

	return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::check_wb_qualcommm_with_golden(const WB_DATA_SHORT wb,const WB_DATA_SHORT wb_golden)
{
	otp_param.wb_param.delta_rgVSG = abs((double(wb.RG) / double(wb_golden.RG)) -1.0 );
	otp_param.wb_param.delta_bgVSG = abs((double(wb.BG) / double(wb_golden.BG)) -1.0 );
	otp_param.wb_param.delta_gbgrVSG = abs((double(wb.GbGr) / double(wb_golden.GbGr)) -1.0 );

	double delta_spec = otp_param.wb_param.golden_rate_spec;
	double delta_specgrgb = otp_param.wb_param.golden_rate_spec_gbgr;

	uts.log.Debug(_T("Self:RG[%d],BG[%d],GrGb[%d], Golden:RG[%d],BG[%d],GrGb[%d]"),
		wb.RG, wb.BG, wb.GbGr, 
		wb_golden.RG, wb_golden.BG, wb_golden.GbGr);

	uts.log.Debug(_T("RGDta[%.4lf],BGDta[%.4lf],GrGbDta[%.4lf],Spec[%.4lf] [%.4lf]"),
		otp_param.wb_param.delta_rgVSG, otp_param.wb_param.delta_bgVSG,otp_param.wb_param.delta_gbgrVSG , delta_spec,delta_specgrgb);

	if (otp_param.wb_param.delta_rgVSG > delta_spec || otp_param.wb_param.delta_bgVSG > delta_spec || otp_param.wb_param.delta_gbgrVSG > delta_specgrgb) {
		uts.log.Debug(_T("WB Self vs Golden delta out of sepc!"));
		return SET_ERROR(OTPCALI_ERROR_WBCALI);
	}

	if (_tcscmp(m_szSN, otp_param.wb_param.goldenSampleName)) 
	{
		delta_spec = otp_param.wb_param.golden_be_spec; 

		if (otp_param.wb_param.delta_rgVSG < delta_spec && otp_param.wb_param.delta_bgVSG < delta_spec && otp_param.wb_param.delta_gbgrVSG < delta_spec)
		{
			uts.log.Debug(_T("Should be golden sample!, golden spec[%.4lf]"), delta_spec);
			return SET_ERROR(OTPCALI_ERROR_SHOULDBE_GOLDEN);
		}
	}


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

	if(upload_lsc_cali_data(modulelsc, lsclength) < 0)
	{
		uts.log.Error(_T("Failed to update LSC data to DB!"));
		return SET_ERROR(OTPCALI_ERROR_DB);
	}

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