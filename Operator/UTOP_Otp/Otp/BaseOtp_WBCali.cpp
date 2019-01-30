#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"

#include <direct.h>

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::FixPatternNoise::FixPatternNoise_WD;

#pragma region WB

int BaseOtp::WBCali(void *args)
{
	int ret = OTPCALI_ERROR_NO;

	WB_PARAM *pwb = &otp_param.wb_param;
	SIZE image_size; 
	double m_dYvalue;
	FPNInfo m_FPNInfo;
	memset(&m_FPNInfo, 0, sizeof(FPNInfo));

	dev->GetBufferInfo(m_bufferInfo);

	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	//////////////////////////////////////////////////////////////////////////
	//Save otp_lsc?
	CString strDirPath;
	CString strFilePath;
	SYSTEMTIME st;
	GetLocalTime(&st);

	strDirPath.Format(_T("%sImage-%s-%s-%s-%04d%02d%02d_AWBOTPData"),
		uts.info.strResultPath,
		uts.info.strProjectName,
		uts.info.strLineName,
		uts.info.strStationName,
		st.wYear,
		st.wMonth, 
		st.wDay);

	UTS::OSUtil::CreateMultipleDirectory(strDirPath);
	////////////////////////////////////////////////////////////////////////////////

	ROI roi;
	GetWBROI(&otp_param.wb_param, &image_size, &roi);
	uts.log.Debug(_T("GetWBROI:%d %d"),roi.width,roi.height);

	WB_DATA_DOUBLE wb_data_d;
	WB_DATA_UCHAR *wb_data_c = &pwb->wb_data_c;

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		ret = OTPCALI_ERROR_SENSOR;
		goto out;
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	//检查Y值是否在设定区间
	UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
	if (m_dYvalue < otp_param.fpn_param.dltMinY || m_dYvalue > otp_param.fpn_param.dltMaxY)
	{
		ret = uts.errorcode.E_Linumance;
		uts.log.Debug(_T("Now YValue = %d , fpn Min Y : Max Y = %d : %d"),m_dYvalue,otp_param.fpn_param.dltMinY,otp_param.fpn_param.dltMaxY);
		return SET_ERROR(ret);
	}

	//check FPN
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.fpn_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.fpn_param.dFPNSpec)
	{
		ret = uts.errorcode.E_FixPatternNoise;
		uts.log.Error(_T("check FPN spec NG   m_dRowDifMax= %f    m_dColDifMax= %f    dThreshold=%f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax, otp_param.fpn_param.dFPNSpec);
		return SET_ERROR(ret);
	}


	//////////////////////////////获取 sensor mode?////////////////////////////////////////
	int nOutMode = 0;

	if (!dev->ReadValue(
		eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
		&nOutMode, sizeof(nOutMode)))
	{
		uts.log.Error(_T("GetSensorOutMode error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;
	}
	//////////////////////////////////////////////////////////////////////////

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
			uts.log.Error(_T("AWB: failed to save raw file before apply AWB!"));
			return SET_ERROR(OTPCALI_ERROR_LSCCALI);
		}
		if (!uts.imgFile.SaveBmpFile(strFilePath,(BYTE*)m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
		{
			uts.log.Error(_T("AWB: failed to save bmp file before apply AWB!"));
			return SET_ERROR(OTPCALI_ERROR_LSCCALI);
		}
	}
	
	////////////////////////////////减BLC/////////////////////////////////
	//获取单通道值
	UTS::Algorithm::WB::WBCalibration(m_bufferObj.pRaw8Buffer, image_size.cx, image_size.cy, nOutMode,
		roi.x, roi.y, roi.width, roi.height,
		otp_param.SensorBlack_level, (double*)&wb_data_d, 
		(uint8_t*)wb_data_c);

	uts.log.Debug(_T("WB(before cali):R[%d], Gr[%d], Gb[%d], B[%d]"),wb_data_c->R,wb_data_c->Gr,wb_data_c->Gb,wb_data_c->B);
	//////////////////////////////////////////////////////////////////////////

	if (CheckWBLimit(wb_data_c) < 0) 
	{
		ret = OTPCALI_ERROR_WBCALI;
		goto out;
	}

	//////////////////////////////////////////////////////////////////////////
	WB_RATIO ratio, ratio_target;
	ratio_target.dr_g = otp_param.wb_param.rg_target;
	ratio_target.db_g = otp_param.wb_param.bg_target;
	get_wb_ratio(wb_data_c, &ratio, 1.0);
	otp_param.wb_param.ratio = ratio;

	WB_GAIN wb_gain;
	ret = get_wb_gain(&ratio, &ratio_target,&wb_gain);
	uts.log.Debug(_T("get_wb_gain : %d"),ret);

	if (ret != -OTPCALI_ERROR_OPTNOTSUPPORT) 
	{
		uint8_t buf[8];
		ret = get_wb_cali_data(&wb_gain, buf);
		uts.log.Debug(_T("get_wb_cali_data : %d"),ret);
		
		uts.log.Debug(_T("--Finish wb_apply "));
		Sleep(200);

		if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));

			ret = OTPCALI_ERROR_SENSOR;
			goto out;
		}

		dev->DisplayImage(m_bufferObj.pBmpBuffer);
		
		if (otp_param.nReCapture == 1)
		{
			//AWB RAW file name
			strFilePath.Format(_T("%s\\%s_S5K3P9SX_GRBG_%dx%d_RAW8_%04d%02d%02d%02d%02d%02d_AWBCapture_Apply"),
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
				uts.log.Error(_T("AWB: failed to save raw file after apply AWB!"));
				return SET_ERROR(OTPCALI_ERROR_LSCCALI);
			}
			if (!uts.imgFile.SaveBmpFile(strFilePath,(BYTE*)m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
			{
				uts.log.Error(_T("AWB: failed to save bmp file after apply AWB!"));
				return SET_ERROR(OTPCALI_ERROR_LSCCALI);
			}
		}

		if (CheckWBResult(
			m_bufferObj.pRaw8Buffer,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight,
			nOutMode) < 0)
		{
			uts.log.Error(_T("CheckWBResult Fail."));
			ret = OTPCALI_ERROR_WBCALI;
			goto out;
		}
	}

	WB_DATA_UCHAR *am = &pwb->raw_after_multi;
	wb_do_multi(wb_data_c, am);
	uts.log.Debug(_T("WB(after multi,rgm[%.2lf],bgm[%.2lf]): R[%d], Gr[%d], Gb[%d], B[%d]"),pwb->rg_multi,pwb->bg_multi,am->R,am->Gr,am->Gb,am->B);
	
	if ((ret = upload_wb_with_golden(am)) < 0)
		return ret;


	ret = OTPCALI_ERROR_NO;
out:
	return SET_ERROR(ret);
}

int BaseOtp::WBCali_RAW10(void *args)
{
	int ret = OTPCALI_ERROR_NO;

	WB_PARAM *pwb = &otp_param.wb_param;

	SIZE image_size; 
	dev->GetBufferInfo(m_bufferInfo);

	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	ROI roi;
	GetWBROI(&otp_param.wb_param, &image_size, &roi);
	uts.log.Debug(_T("GetWBROI:%d %d"),roi.width,roi.height);

	WB_DATA_DOUBLE wb_data_d;
	WB_DATA_USHORT *wb_data_c2 = &pwb->wb_data_c2;

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		ret = OTPCALI_ERROR_SENSOR;
		goto out;
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	int nOutMode = 0;

	if (!dev->ReadValue(
		eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
		&nOutMode, sizeof(nOutMode)))
	{
		uts.log.Error(_T("GetSensorOutMode error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;
	}

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	//memcpy(RAW10Image,m_bufferObj.pFrameBuffer,m_bufferInfo.nWidth* m_bufferInfo.nHeight*2);

	UTS::Algorithm::WB::WBCalibration(RAW10Image, image_size.cx, image_size.cy, nOutMode,
		roi.x, roi.y, roi.width, roi.height,
		otp_param.SensorBlack_level, (double*)&wb_data_d, 
		(unsigned short*)wb_data_c2);

	uts.log.Debug(_T("WB(before cali):R[%d], Gr[%d], Gb[%d], B[%d]"),wb_data_c2->R,wb_data_c2->Gr,wb_data_c2->Gb,wb_data_c2->B);

	/*
	if (CheckWBLimit(wb_data_c2) < 0) 
	{
		ret = OTPCALI_ERROR_WBCALI;
		goto out;
	}

	WB_RATIO ratio, ratio_target;
	ratio_target.dr_g = otp_param.wb_param.rg_target;
	ratio_target.db_g = otp_param.wb_param.bg_target;
	get_wb_ratio(wb_data_c, &ratio, 1.0);
	otp_param.wb_param.ratio = ratio;


	WB_GAIN wb_gain;
	ret = get_wb_gain(&ratio, &ratio_target,&wb_gain);

	uts.log.Debug(_T("get_wb_gain : %d"),ret);

	if (ret != -OTPCALI_ERROR_OPTNOTSUPPORT) {
		uint8_t buf[8];
		ret = get_wb_cali_data(&wb_gain, buf);
		uts.log.Debug(_T("get_wb_cali_data : %d"),ret);
		if (sensor->wb_writeback(buf, ret) < 0) {
			ret = OTPCALI_ERROR_EEPROM;
			uts.log.Error(_T("wb_writeback Error!!!"));
			goto out;
		}

		uts.log.Debug(_T("--Finish wb_writeback "));
		Sleep(200);

		if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));

			ret = OTPCALI_ERROR_SENSOR;
			goto out;
		}

		dev->DisplayImage(m_bufferObj.pBmpBuffer);

		if (CheckWBResult(
			m_bufferObj.pRaw8Buffer,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight,
			nOutMode) < 0)
		{
			uts.log.Error(_T("CheckWBResult Fail."));
			ret = OTPCALI_ERROR_WBCALI;
			goto out;
		}
	}
	

	WB_DATA_SHORT *am = &pwb->raw_after_multi2;
	wb_do_multi(&wb, am);
	uts.log.Debug(_T("WB(after multi,rgm[%.2lf],bgm[%.2lf]): RG[%d],BG[%d],GrGb[%d]"),pwb->rg_multi,pwb->bg_multi,am->RG,am->BG,am->GbGr);
	*/

	if ((ret = upload_wb_with_golden(wb_data_c2)) < 0)
		return ret;
	
	ret = OTPCALI_ERROR_NO;

out:
	RELEASE_ARRAY(RAW10Image);

	return SET_ERROR(ret);
}

void BaseOtp::get_gain_by_wb_ratio(const WB_RATIO *ratio, 
	const WB_RATIO *ratio_target, 
	double base, WB_GAIN *gain)
{
	gain->RGain = int(base * ratio_target->dr_g / ratio->dr_g + 0.5);
	gain->BGain = int(base * ratio_target->db_g / ratio->db_g + 0.5);
	gain->GGain = int(base + 0.5);
}

int BaseOtp::check_wb_with_golden(const WB_RATIO *ratio, const WB_RATIO *ratio_g)
{
	otp_param.wb_param.delta_rgVSG = abs(ratio->dr_g - ratio_g->dr_g) / ratio_g->dr_g;
	otp_param.wb_param.delta_bgVSG = abs(ratio->db_g - ratio_g->db_g) / ratio_g->db_g;
	double delta_spec = otp_param.wb_param.golden_rate_spec;

	uts.log.Debug(_T("Self:RG[%.4lf],BG[%.4lf],Golden:RG[%.4lf],BG[%.4lf];")
		_T("RGDta[%.4lf],BGDta[%.4lf],Spec[%.4lf]"),
		ratio->dr_g, ratio->db_g, ratio_g->dr_g, ratio_g->db_g,
		otp_param.wb_param.delta_rgVSG, otp_param.wb_param.delta_bgVSG, delta_spec);

	if (otp_param.wb_param.delta_rgVSG > delta_spec || 
		otp_param.wb_param.delta_bgVSG > delta_spec) {
		uts.log.Debug(_T("WB Self vs Golden delta out of sepc!"));
		return SET_ERROR(OTPCALI_ERROR_WBCALI);
	}
	/*
	if (_tcscmp(m_szSN, otp_param.wb_param.goldenSampleName)== 0) {
		delta_spec = otp_param.wb_param.golden_be_spec; 
		if (otp_param.wb_param.delta_rgVSG < delta_spec && 
			otp_param.wb_param.delta_bgVSG < delta_spec) {
			uts.log.Debug(_T("Should be golden sample!, golden spec[%.4lf]"), delta_spec);
			return SET_ERROR(OTPCALI_ERROR_SHOULDBE_GOLDEN);
		}
	}
	*/
	return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::upload_wb_with_golden(const WB_DATA_UCHAR *wb)
{
	WB_PARAM *pwbp = &otp_param.wb_param;

	time_t tm = 0;
	int is_golden_self = !_tcscmp(m_szSN, pwbp->goldenSampleName);
	int is_only_upload = !_tcscmp(pwbp->goldenSampleName, EMPTY_STR);

	WB_DATA_UCHAR wb_db[2];
	memset(wb_db, 0, sizeof(wb_db));
	memcpy(&wb_db[0], wb, sizeof(WB_DATA_UCHAR));

	if (!is_only_upload) {
		USES_CONVERSION;
		/*int golden_id = otpDB->GetModuleID(T2A(pwbp->goldenSampleName), false);
		if (golden_id < 0) {
			uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
			return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
		}
		if (otpDB->GetOtpByType(golden_id, DDM::OTPDB_OTPTYPE_AWB,
			(char *)&wb_db[1], sizeof(WB_DATA_UCHAR), &tm) < 0) {
				uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
				return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
		}
		if (!is_golden_self) {
			/*
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
			*/
		/*	if (CheckWBLimit(&wb_db[1]) < 0) return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}
		*/
		WB_RATIO ratio, ratio_g;
		get_wb_ratio(&wb_db[0], &ratio, 1.0);
		get_wb_ratio(&wb_db[1], &ratio_g, 1.0);
		//int ret = check_wb_with_golden(&ratio, &ratio_g); 
		//if (ret < 0) return ret;
	}

	//int ret = check_wb_with_data(&wb_db[0]); 
	//if (ret < 0) 
	//	return SET_ERROR(OTPCALI_ERROR_WBCALI);

	if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB,
		(char *)wb_db, sizeof(wb_db)) < 0)
	{
		uts.log.Error(_T("Failed to update WB data to DB!"));
		return SET_ERROR(OTPCALI_ERROR_DB);
	}

	return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::check_wb_with_data(const WB_DATA_UCHAR *wb)
{
	//AWB 规格
	//golden data RG=557 BG=723 GG=1024
	int RGainHigh=612;
	int RGainlow=502;
	int	BGainHigh=795;
	int	BGainlow=651;
	int	GrGbGainHig=1125;
	int	GrGbGainlow=950;

	/*int	R_HighLimit=80;
	int	R_LowLimit=66;
	int	B_HighLimit=103;
	int	B_LowLimit=85;
	int	Gr_HighLimit=167;
	int	Gr_LowLimit=137;
	int	Gb_HighLimit=168;
	int	Gb_LowLimit=138;*/


	WB_DATA_UCHAR wb_db[2];
	memset(wb_db, 0, sizeof(wb_db));
	memcpy(&wb_db[0], wb, sizeof(WB_DATA_UCHAR));

	WB_RATIO ratio;
	get_wb_ratio(&wb_db[0], &ratio, 1023);

	
	uts.log.Debug(_T("Self:R[%d],Gr[%d],Gb[%d],B[%d],\
		RG[%d],BG[%d],GrGb[%d], Golden:RG[557],BG[723],GbGr[1024]"),
		wb_db[0].R,wb_db[0].Gr,wb_db[0].Gb,wb_db[0].B,
		ratio.r_gr, ratio.b_gb, ratio.gb_gr);

	if (/*wb_db[0].R > R_HighLimit || wb_db[0].R < R_LowLimit || wb_db[0].Gr > Gr_HighLimit
		|| wb_db[0].Gr < Gr_LowLimit || wb_db[0].Gb > Gb_HighLimit || wb_db[0].Gb < Gb_LowLimit
		|| wb_db[0].B > B_HighLimit || wb_db[0].B < B_LowLimit ||*/ 
		ratio.r_gr > RGainHigh || ratio.r_gr < RGainlow 
		|| ratio.b_gb >BGainHigh || ratio.b_gb < BGainlow
		|| ratio.gb_gr > GrGbGainHig || ratio.gb_gr < GrGbGainlow) 
	{
		uts.log.Debug(_T("WB Self vs Golden delta out of sepc!"));
		return SET_ERROR(OTPCALI_ERROR_WBCALI);
	}

	return SET_ERROR(OTPCALI_ERROR_NO);
}

void BaseOtp::get_wb_ratio(const WB_DATA_UCHAR *wb, WB_RATIO *ratio, double multi)
{
	ratio->multi = multi;

	ratio->dg_a = (wb->Gr + wb->Gb) / 2.0;
	ratio->g_a = int(ratio->dg_a + 0.5);

	ratio->dr_g = multi * wb->R / ratio->dg_a;
	ratio->r_g = int(ratio->dr_g+0.5);

	ratio->db_g = multi * wb->B / ratio->dg_a;
	ratio->b_g = int(ratio->db_g + 0.5);

	ratio->dr_gr = multi * wb->R / wb->Gr;
	ratio->r_gr = int(ratio->dr_gr + 0.5);

	ratio->db_gb = multi * wb->B / wb->Gb;
	ratio->b_gb = int(ratio->db_gb + 0.5);

	ratio->dgb_gr= multi * wb->Gb / wb->Gr;
	ratio->gb_gr= int(ratio->dgb_gr + 0.5);

	ratio->dgr_gb= multi * wb->Gr / wb->Gb;
	ratio->gr_gb= int(ratio->dgr_gb + 0.5);
}
void BaseOtp::get_wb_ratio(const WB_DATA_USHORT *wb, WB_RATIO *ratio, double multi)
{
	ratio->multi = multi;

	ratio->dg_a = (wb->Gr + wb->Gb) / 2.0;
	ratio->g_a = int(ratio->dg_a + 0.5);

	ratio->dr_g = multi * wb->R / ratio->dg_a;
	ratio->r_g = int(ratio->dr_g+0.5);

	ratio->db_g = multi * wb->B / ratio->dg_a;
	ratio->b_g = int(ratio->db_g + 0.5);

	ratio->dr_gr = multi * wb->R / wb->Gr;
	ratio->r_gr = int(ratio->dr_gr + 0.5);

	ratio->db_gb = multi * wb->B / wb->Gb;
	ratio->b_gb = int(ratio->db_gb + 0.5);

	ratio->dgb_gr= multi * wb->Gb / wb->Gr;
	ratio->gb_gr= int(ratio->dgb_gr + 0.5);

	ratio->dgr_gb= multi * wb->Gr / wb->Gb;
	ratio->gr_gb= int(ratio->dgr_gb + 0.5);
}
void BaseOtp::GetWBROI(const WB_PARAM *pwb_param, const SIZE *imagesz, ROI *roi)
{
	if (pwb_param->roi.cx < 0 || pwb_param->roi.cy < 0) {
		roi->width = imagesz->cx / 5;
		roi->height = imagesz->cy / 5;
	} 
	else 
	{
		roi->width = pwb_param->roi.cx;
		roi->height = pwb_param->roi.cy;
	}
	roi->x = (imagesz->cx - roi->width) / 2;
	roi->y = (imagesz->cy - roi->height) / 2;
}

int BaseOtp::CheckWBLimit(WB_DATA_UCHAR *wb)
{
	const char *pixel[] = {"R", "Gr", "Gb", "B"};

	uint8_t *gray = (uint8_t*)wb;
	int *v_min = otp_param.wb_param.RGrGbB_min;
	int *v_max = otp_param.wb_param.RGrGbB_max;

	for (int i = 0; i < 4; i++, gray++, v_min++, v_max++) {
		if (*gray < *v_min || *gray > *v_max) {
			uts.log.Debug(_T("%s[%d] out of limit [%d,%d]"), pixel[i], *gray, *v_min, *v_max);
			return -1;
		}
	}
	
	return 0;
}

int BaseOtp::CheckWBLimit_Ushort(WB_DATA_USHORT*wb)
{
	const char *pixel[] = {"R", "Gr", "Gb", "B"};

	unsigned short  *gray = (unsigned short*)wb;
	int *v_min = otp_param.wb_param.RGrGbB_min;
	int *v_max = otp_param.wb_param.RGrGbB_max;

	for (int i = 0; i < 4; i++, gray++, v_min++, v_max++) {
		if (*gray < *v_min || *gray > *v_max) {
			uts.log.Debug(_T("%s[%d] out of limit [%d,%d]"), pixel[i], *gray, *v_min, *v_max);
			return -1;
		}
	}
	return 0;
}

int BaseOtp::wb_do_multi(const WB_DATA_UCHAR *in, WB_DATA_UCHAR *out)
{
	WB_PARAM *pwb = &otp_param.wb_param;

	out->R = int(in->R * pwb->rg_multi+0.5) ;
	out->Gr = in->Gr;
	out->Gb = in->Gb;
	out->B = int(in->B * pwb->bg_multi+0.5);
	return 0;
}

int BaseOtp::CheckWBResult(uint8_t* pRaw8, int width, int height, int rawBayerType)
{
	int ret = 0;

	struct WB_DATA_DOUBLE wb_data;

	WB_PARAM *pwb = &otp_param.wb_param;
	ROI roi;
	SIZE image_size;
	sensor->dev->GetImageSize(image_size);
	GetWBROI(pwb, &image_size, &roi);

	WB_DATA_UCHAR *wb_data_c = &pwb->wb_data_c_aftercali;
	memset(wb_data_c, 0x00, sizeof(*wb_data_c));

	Algorithm::WB::WBCalibration(pRaw8, width, height, rawBayerType,
		roi.x, roi.y, roi.width, roi.height,
		otp_param.SensorBlack_level, (double *)&wb_data, (uint8_t *)wb_data_c);
	
	if (CheckWBLimit(wb_data_c) < 0) return -1;

	WB_RATIO ratio;
	get_wb_ratio(wb_data_c, &ratio, 1.0);
	pwb->rg_delta = abs(ratio.dr_g - pwb->rg_target);
	pwb->bg_delta = abs(ratio.db_g - pwb->bg_target);

	uts.log.Debug(_T("WB(after cali):R[%d]Gr[%d]Gb[%d]B[%d],Self:RG[%.3lf]BG[%.3lf],")
		_T("Tgt:[%.3lf]BG[%.3lf],Dlt:[%.3lf]BG[%.3lf]"),
		wb_data_c->R, wb_data_c->Gr, wb_data_c->Gb, wb_data_c->B,
		ratio.dr_g, ratio.db_g, pwb->rg_target, pwb->bg_target,
		pwb->rg_delta, pwb->bg_delta);
	if (pwb->rg_delta > pwb->rg_delta_spec || pwb->bg_delta > pwb->bg_delta_spec) {
		return -1;
	}
	return 0;
}

int BaseOtp::do_wb_cali_before(uint8_t* pRaw8, int width, int height, int rawBayerType)
{
	int ret = OTPCALI_ERROR_NO;

	WB_PARAM *pwb = &otp_param.wb_param;
	ROI roi;
	SIZE image_size;
	sensor->dev->GetImageSize(image_size);
	GetWBROI(&otp_param.wb_param, &image_size, &roi);

	WB_DATA_DOUBLE wb_data_d;
	WB_DATA_UCHAR *wb_data_c = &pwb->wb_data_c;

	Algorithm::WB::WBCalibration(pRaw8, width, height, rawBayerType,
		roi.x, roi.y, roi.width, roi.height,
		otp_param.SensorBlack_level, (double*)&wb_data_d, 
		(uint8_t*)wb_data_c);
	uts.log.Debug(_T("WB(before cali): R[%d], Gr[%d], Gb[%d], B[%d]"),
		wb_data_c->R,wb_data_c->Gr,wb_data_c->Gb,wb_data_c->B);

	if (CheckWBLimit(wb_data_c) < 0) {
		ret = OTPCALI_ERROR_WBCALI;
		goto out;
	}

out:
	return SET_ERROR(ret);
}

int BaseOtp::do_wb_cali()
{
	int ret = OTPCALI_ERROR_WBCALI;
	WB_PARAM *pwb = &otp_param.wb_param;
	WB_DATA_UCHAR *wb_data_c = &pwb->wb_data_c;
	WB_RATIO ratio, ratio_target;
	ratio_target.dr_g = otp_param.wb_param.rg_target;
	ratio_target.db_g = otp_param.wb_param.bg_target;
	get_wb_ratio(wb_data_c, &ratio, 1.0);

	WB_GAIN wb_gain;
	ret = get_wb_gain(&ratio, &ratio_target, &wb_gain);
	if (ret != -OTPCALI_ERROR_OPTNOTSUPPORT) {
		uint8_t buf[8];
		ret = get_wb_cali_data(&wb_gain, buf);
		if (sensor->wb_writeback(buf, ret) < 0) {
			ret = OTPCALI_ERROR_SENSOR;
			goto out;
		}
		Sleep(200);
	}
out:
	return ret;
}

static int wb_check_ratio(const WB_RATIO *ratio, const WB_RATIO *ratio_g, 
	double *delta_rg, double *delta_bg, 
	double spec_rg, double spec_bg)
{
	*delta_rg = abs(ratio->dr_g - ratio_g->dr_g) / ratio_g->dr_g;
	*delta_bg = abs(ratio->db_g - ratio_g->db_g) / ratio_g->db_g;

	uts.log.Debug(_T("Self:RG[%.4lf],BG[%.4lf],VS:RG[%.4lf],BG[%.4lf];")
		_T("RGDta[%.4lf],BGDta[%.4lf],SpecRG[%.4lf],SpecBG[%.4lf]"),
		ratio->dr_g, ratio->db_g, ratio_g->dr_g, ratio_g->db_g,
		*delta_rg, *delta_bg, spec_rg, spec_bg);

	if (*delta_rg > spec_rg || *delta_bg > spec_bg) {
		return -1;
	}

	return 0;
}

int BaseOtp::wb_check_result_after_calibration(void)
{
	WB_PARAM *wb = &otp_param.wb_param;
	WB_DATA_UCHAR *raw = &wb->wb_data_c_aftercali;

	wb_get_rawinfo_from_image(NULL, raw);

	if (wb_check_rawinfo_limit(raw) < 0) return -1;

	WB_RATIO ratio, ratio_t;
	get_wb_ratio(raw, &ratio, 1.0);

	ratio_t.dr_g = wb->rg_target;
	ratio_t.db_g = wb->bg_target;
	return wb_check_ratio(&ratio, &ratio_t, &wb->rg_delta_after_cali, &wb->bg_delta_after_cali, 
		wb->rg_delta_spec, wb->bg_delta_spec);
}


//-------------------------------------------------------------------------------------------------
int BaseOtp::check_otp_off_wb(void *args)
{
	int ret = OTPCALI_ERROR_NO;
	WB_PARAM *pwb = &otp_param.wb_param;

	OtpDataItem *item = get_otp_from_uniform_by_type(OtpDataType_WB);
	if (!item) {
		ret = SET_ERROR(OTPCALI_ERROR_NODATA);
		goto out;
	}
	OtpDataWB *wb = (OtpDataWB*)item->data;
	if (wb->has_golden && pwb->rg_multi < 0) {
		if (wb_check_rawinfo_limit(&pwb->off_golden) < 0) {
			uts.log.Error(_T("WB OffWriteBack: golden in this light is out of limit, ")
				_T("please check parameter[WB_GOLDEN_OFFLINE_RGrGbB]"));
			ret = SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
			goto out;
		}
		WB_RATIO ratio_g;
		get_wb_ratio(&pwb->off_golden, &ratio_g, 1.0);
		pwb->rg_multi = wb->ratio_g.dr_g / ratio_g.dr_g;
		pwb->bg_multi = wb->ratio_g.db_g / ratio_g.db_g;
		uts.log.Debug(_T("WB OffWriteBack: reasigned rg_multi[%.3lf],bg_multi[%.3lf]"), 
			pwb->rg_multi, pwb->bg_multi);
	}
	WB_RATIO *ratio = &wb->ratio;
	WB_GAIN gain;
	if (wb->gain.RGain == 0 && wb->gain.GGain == 0 && wb->gain.BGain == 0) {
		WB_RATIO ratio_target; 
		ratio_target.dr_g = pwb->rg_target;
		ratio_target.db_g = pwb->bg_target;
		pwb->ratio_m.dr_g = ratio->dr_g / pwb->rg_multi;
		pwb->ratio_m.db_g = ratio->db_g / pwb->bg_multi;
		ret = get_wb_gain(&pwb->ratio_m, &ratio_target, &gain);
		if (ret < 0) {goto out;}
	} else {
		memcpy(&gain, &wb->gain, sizeof(WB_GAIN));
		gain.RGain = int(gain.RGain / pwb->rg_multi + 0.5);
		gain.BGain = int(gain.BGain / pwb->bg_multi + 0.5);
	}

	uint8_t buf[8];
	ret = get_wb_cali_data(&gain, buf);
	if (ret < 0) {goto out;}

	ret = sensor->wb_writeback(buf, ret);
	if (ret < 0) {goto out;}

	Sleep(200);
	if (wb_check_result_after_calibration() < 0) {
		ret = SET_ERROR(OTPCALI_ERROR_WBCALI);
		goto out;
	}
	double rg_delta, bg_delta;
	if (wb->has_golden && wb_check_ratio(ratio, &wb->ratio_g, &rg_delta, &bg_delta,
		pwb->golden_rate_spec, pwb->golden_rate_spec) < 0) {
			ret = SET_ERROR(OTPCALI_ERROR_WBCALI);
			goto out;
	}
out:
	return ret;
}

// int BaseOtp::upload_wb_with_golden(const WB_DATA_USHORT *wb)
// {
// 	WB_PARAM *pwbp = &otp_param.wb_param;
// 
// 	time_t tm = 0;
// 	int is_golden_self = !_tcscmp(m_szSN, pwbp->goldenSampleName);
// 	int is_only_upload = !_tcscmp(pwbp->goldenSampleName, EMPTY_STR);
// 
// 	WB_DATA_USHORT wb_db[2];
// 	memset(wb_db, 0, sizeof(wb_db));
// 	memcpy(&wb_db[0], wb, sizeof(WB_DATA_USHORT));
// 
// 	if (!is_only_upload) {
// 		USES_CONVERSION;
// 		int golden_id = otpDB->GetModuleID(T2A(pwbp->goldenSampleName), false);
// 		if (golden_id < 0) {
// 			uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
// 			return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
// 		}
// 
// 		if (otpDB->GetOtpByType(golden_id, DDM::OTPDB_OTPTYPE_AWB,
// 			(char *)&wb_db[1], sizeof(WB_DATA_SHORT), &tm) < 0) {
// 				uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
// 				return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
// 		}
// 		
// 		if (!is_golden_self) {
// 			time_t delta = time(NULL) - tm; 
// 			if (delta > pwbp->goldenValidTime * 3600) {
// 				MessageBox(NULL, _T("请重新校验GoldenSample"), _T("错误"), NULL);
// 				return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
// 			} else if (delta > pwbp->goldenWarningTime * 3600) {
// 				time_t remaining_tm = pwbp->goldenValidTime*3600 - delta;
// 				TCHAR buf[0x100];
// 				_stprintf(buf, _T("GoldenSample剩余有效时间：%lld小时, %lld秒"),
// 					remaining_tm / 3600, remaining_tm % 3600);
// 				MessageBox(NULL, buf, _T("警告"), NULL);
// 			}
// 		}
// 
// 		// wb_db[0] : module  ;  wb_db[1] : golden
// 		//int ret = check_wb_qualcommm_with_golden(wb_db[0],wb_db[1]); 
// 		//if (ret < 0) return ret;
// 	}
// 
// 	/*if(m_param.nWB_Type == 2)
// 	{
// 		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB2,
// 			(char *)wb_db, sizeof(wb_db)) < 0)
// 		{
// 			uts.log.Error(_T("Failed to update WB data to DB!"));
// 			return SET_ERROR(OTPCALI_ERROR_DB);
// 		}
// 	}
// 	else if(m_param.nWB_Type == 3)
// 	{
// 		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB3,
// 			(char *)wb_db, sizeof(wb_db)) < 0)
// 		{
// 			uts.log.Error(_T("Failed to update WB data to DB!"));
// 			return SET_ERROR(OTPCALI_ERROR_DB);
// 		}
// 	}
// 	else
// 	{*/
// 	if(otp_param.wb_param.WBType == 2)
// 	{
// // 		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB2,
// // 			(char *)wb_db, sizeof(wb_db)) < 0)
// // 		{
// // 			uts.log.Error(_T("Failed to update WB data to DB!"));
// // 			return SET_ERROR(OTPCALI_ERROR_DB);
// // 		}
// 	}
// 	else if(otp_param.wb_param.WBType == 3)
// 	{
// // 		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB3,
// // 			(char *)wb_db, sizeof(wb_db)) < 0)
// // 		{
// // 			uts.log.Error(_T("Failed to update WB data to DB!"));
// // 			return SET_ERROR(OTPCALI_ERROR_DB);
// // 		}
// 	}
// 	else
// 	{
// 		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB,
// 			(char *)wb_db, sizeof(wb_db)) < 0)
// 		{
// 			uts.log.Error(_T("Failed to update WB data to DB!"));
// 			return SET_ERROR(OTPCALI_ERROR_DB);
// 		}
// 	}
// 	//}
// 	return SET_ERROR(OTPCALI_ERROR_NO);
// 
// }
int BaseOtp::upload_wb_with_golden(const WB_DATA_USHORT *wb)
{
	
	WB_PARAM *pwbp = &otp_param.wb_param;
	uts.log.Debug(_T("upload golden to database"));
	time_t tm = 0;
	int is_golden_self = !_tcscmp(m_szSN, pwbp->goldenSampleName);
	int is_only_upload = !_tcscmp(pwbp->goldenSampleName, EMPTY_STR);
	
	WB_DATA_USHORT  wb_g;
	WB_DATA_LIGHT_USHORT  wb_dbwx;
	WB_LIGHT_USHORT wb_li;
	USES_CONVERSION;

	wb_g.R=otp_param.wb_param.RGrGbB_g[0];
	wb_g.Gr=otp_param.wb_param.RGrGbB_g[1];
	wb_g.Gb=otp_param.wb_param.RGrGbB_g[2];
	wb_g.B=otp_param.wb_param.RGrGbB_g[3];
	
	wb_li.RG=otp_param.LightConfident_RG;
	wb_li.BG=otp_param.LightConfident_BG;

	uts.log.Info(_T("awb to db::R=0x%02x,B=0x%02x,Gr =0x%02x,Gb=0x%02x"),wb[0].R,wb[0].B,wb[0].Gr,wb[0].Gb);
	uts.log.Info(_T("golden to db::R_g=%d,B_g=0x%d,Gr_g =%d,Gb_g=%d"),wb_g.R,wb_g.B,wb_g.Gr,wb_g.Gb); 
	uts.log.Info(_T("rg to db::RG=%d,BG=0x%d"),wb_li.RG,wb_li.BG);
	//-------check r gain bgain----------------
	if(wb_g.R>0&&wb_g.B>0&&wb_g.Gb>0&&wb_g.Gr>0)
	{
		WB_RATIO ratio, ratio_g;
		
		
		double g=(wb->Gb+wb->Gr)/2;
		double golden_g=(wb_g.Gr+wb_g.Gb)/2;
		ratio.dr_g=wb->R/g;
		ratio.db_g=wb->B/g;
		ratio_g.db_g=wb_g.B/golden_g;
		ratio_g.dr_g=wb_g.R/golden_g;
		check_wb_with_golden(&ratio,&ratio_g);
// 		if(RGain > GoldenR_Gain*(1+otp_param.wb_param.) || RGain < GoldenB_Gain*0.9)
// 		{
// 
// 		}
// 
// 		if(BGain > GoldenB_Gain*1.1 || BGain < GoldenB_Gain*0.9)
// 		{
// 
// 		}
	}
	//-----------------------

	memset(&wb_dbwx, 0, sizeof(wb_dbwx));
	memcpy(&wb_dbwx.wb, wb, sizeof(WB_DATA_USHORT)); 
	memcpy(&wb_dbwx.wb_g, &wb_g, sizeof(WB_DATA_USHORT));
	memcpy(&wb_dbwx.wb_light, &wb_li, sizeof(WB_LIGHT_USHORT));

	int GoldenR,GoldenGr,GoldenGb,GoldenB;
	double NormalRG,NormalBG,GoldenRG,GoldenBG;
	double NormalD,GoldenD;
	if (is_golden_self) {
		if(wb_g.R>0&&wb_g.B>0&&wb_g.Gb>0&&wb_g.Gr>0)
		{
			NormalRG = (double)(wb_dbwx.wb.R) / (double)((wb_dbwx.wb.Gr+wb_dbwx.wb.Gb)/2);
			NormalBG = (double)(wb_dbwx.wb.B) /(double) ((wb_dbwx.wb.Gr+wb_dbwx.wb.Gb)/2);
			GoldenRG = (double) wb_dbwx.wb_g.R / (double)((wb_dbwx.wb_g.Gr+wb_dbwx.wb_g.Gb)/2);
			GoldenBG = (double)wb_dbwx.wb_g.B / (double)((wb_dbwx.wb_g.Gr+wb_dbwx.wb_g.Gb)/2);

			double arfa=((GoldenRG-NormalRG)*(GoldenRG-NormalRG))/(GoldenRG*GoldenRG)+((GoldenBG-NormalBG)*(GoldenBG-NormalBG))/(GoldenBG*GoldenBG);
			if(arfa<0.09){
				uts.log.Warning(_T("wx Cailucate light confident"));
				uts.log.Debug(_T("iRG=%d,iBG=%d"),1000*GoldenRG/NormalRG+0.5,1000*GoldenBG/NormalBG+0.5);

				//更新数据库光源系数 uts.dbCof.GetOperatorSingleSpec(strSection, _T("strWB_LightConfident"), m_param.strWB_LightConfident, _T("1000,1000"), _T("iRG,iBG"));
				return -1;
			}
			else{
				uts.log.Debug(_T("Please to Check Light source"));
				return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
			}
		}
		
		//USES_CONVERSION;
	/*	int golden_id = otpDB->GetModuleID(T2A(pwbp->goldenSampleName), false);
		if (golden_id < 0) {
			uts.log.Debug(_T("Failed to Get Golden[%s] data from DB!"), pwbp->goldenSampleName);
			return SET_ERROR(OTPCALI_ERROR_NO_GOLDEN);
		}
		if (otpDB->GetOtpByType(golden_id, DDM::OTPDB_OTPTYPE_AWB,
			(char *)&wb_dbwx.wb_g, sizeof(WB_DATA_SHORT), &tm) < 0) {
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
		}*/

		// wb_db[0] : module  ;  wb_db[1] : golden
		//int ret = check_wb_qualcommm_with_golden(wb_db[0],wb_db[1]); 
		//if (ret < 0) return ret;
	}
	if(otp_param.nWB_Type==2){
		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB2,
			(char *)&wb_dbwx, sizeof(wb_dbwx)) < 0)
		{
			uts.log.Error(_T("Failed to update WB data to DB!"));
			return SET_ERROR(OTPCALI_ERROR_DB);
		}
	}
	else if(otp_param.nWB_Type==1){
		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB1,
		(char *)&wb_dbwx, sizeof(wb_dbwx)) < 0)
		{
			uts.log.Error(_T("Failed to update WB data to DB!"));
			return SET_ERROR(OTPCALI_ERROR_DB);
		}
	}
	else{
		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB,
		(char *)&wb_dbwx, sizeof(wb_dbwx)) < 0)
		{
			uts.log.Error(_T("Failed to update WB data to DB!"));
			return SET_ERROR(OTPCALI_ERROR_DB);
		}
	}
	return (OTPCALI_ERROR_NO);

}



#pragma endregion