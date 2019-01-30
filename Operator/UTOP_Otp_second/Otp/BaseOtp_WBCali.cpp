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

static int wb_check_ratio(const WB_RATIO *ratio, const WB_RATIO *ratio_g, 
	double *delta_rg, double *delta_bg, 
	double spec_rg, double spec_bg)
{
	*delta_rg = abs(ratio_g->dr_gr - ratio->dr_gr);
	*delta_bg = abs(ratio_g->db_gb - ratio->db_gb);
	//*delta_rg = abs(ratio->dr_g - ratio_g->dr_g) / ratio_g->dr_g;
	//*delta_bg = abs(ratio->db_g - ratio_g->db_g) / ratio_g->db_g;

	uts.log.Debug(_T("Self:RG[%.4lf],BG[%.4lf],VS:RG[%.4lf],BG[%.4lf];")
		_T("RGDta[%.4lf],BGDta[%.4lf],SpecRG[%.4lf],SpecBG[%.4lf]"),
		ratio->dr_gr, ratio->db_gb, ratio_g->dr_gr, ratio_g->db_gb,
		*delta_rg, *delta_bg, spec_rg, spec_bg);

	if (*delta_rg > spec_rg || *delta_bg > spec_bg) {
		return -1;
	}

	return 0;
}

int BaseOtp::WBCali(void *args)
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
	WB_DATA_UCHAR *wb_data_c = &pwb->wb_data_c;

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

	//check FPN
	FPNInfo m_FPNInfo;
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.dFPNSpec)
	{
		uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);
		ret = uts.errorcode.E_FixPatternNoise;;
		goto out;
	}
	uts.log.Info(_T("check FPN : m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);

	//获取单通道值
	UTS::Algorithm::WB::WBCalibration(m_bufferObj.pRaw8Buffer, image_size.cx, image_size.cy, nOutMode,
		roi.x, roi.y, roi.width, roi.height,
		otp_param.SensorBlack_level, (double*)&wb_data_d, 
		(uint8_t*)wb_data_c);

	WB_RATIO ratio, ratio_target;
	ratio_target.dr_g = otp_param.wb_param.rg_target;
	ratio_target.db_g = otp_param.wb_param.bg_target;
	get_wb_ratio(wb_data_c, &ratio, 1.0);
	otp_param.wb_param.ratio = ratio;

	uts.log.Debug(_T("WB(before cali):R[%d], Gr[%d], Gb[%d], B[%d], RG[%.3lf], BG[%.3lf]"),wb_data_c->R,wb_data_c->Gr,wb_data_c->Gb,wb_data_c->B,ratio.dr_gr,ratio.db_gb);
	if (CheckWBLimit(wb_data_c) < 0) 
	{
		ret = OTPCALI_ERROR_WBCALI;
		goto out;
	}

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

	WB_DATA_UCHAR *am = &pwb->raw_after_multi;
	wb_do_multi(wb_data_c, am);
	
	
	if(uts.info.nOtpDBType>=0)
	{
		if ((ret = upload_wb_with_golden(am)) < 0)
			return ret;
	}
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

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

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

	//check FPN
	FPNInfo m_FPNInfo;
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.dFPNSpec)
	{
		uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);
		ret = uts.errorcode.E_FixPatternNoise;;
		goto out;
	}
	uts.log.Info(_T("check FPN : m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);

	
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
//	memcpy(RAW10Image,m_bufferObj.pFrameBuffer,m_bufferInfo.nWidth* m_bufferInfo.nHeight*2);

	UTS::Algorithm::WB::WBCalibration(RAW10Image, image_size.cx, image_size.cy, nOutMode,
		roi.x, roi.y, roi.width, roi.height,
		otp_param.SensorBlack_level, (double*)&wb_data_d, 
		(unsigned short*)wb_data_c2);

	uts.log.Debug(_T("WB(before cali):R[%d], Gr[%d], Gb[%d], B[%d]"),wb_data_c2->R,wb_data_c2->Gr,wb_data_c2->Gb,wb_data_c2->B);


	//if (CheckWBLimit(wb_data_c2) < 0) 
	//{
	//	ret = OTPCALI_ERROR_WBCALI;
	//	goto out;
	//}

	WB_RATIO ratio, ratio_target;
	ratio_target.dr_gr = otp_param.wb_param.rg_target;
	ratio_target.db_gb = otp_param.wb_param.bg_target;
	get_wb_ratio(wb_data_c2, &ratio, 1.0);
	otp_param.wb_param.ratio = ratio;


	uts.log.Debug(_T("WB(before mutil): ratio.r_gr[%d], ratio.b_gr[%d]"),ratio.r_gr,ratio.b_gr );
	ratio.r_gr = int(ratio.r_gr * pwb->rg_multi);
	ratio.b_gb = int(ratio.b_gb * pwb->bg_multi);

	uts.log.Debug(_T("WB(before mutil): ratio.r_gr[%d], ratio.b_gr[%d]"),ratio.r_gr,ratio.b_gb );
	uts.log.Debug(_T("WB(mutil): pwb->rg_multi[%04f], pwb->bg_multi[%04f]"),pwb->rg_multi,pwb->bg_multi );

	ret = wb_check_ratio(&ratio, &ratio_target, &pwb->rg_delta_after_cali, &pwb->bg_delta_after_cali, 
		pwb->rg_delta_spec, pwb->bg_delta_spec);
	if(ret < 0) {
		uts.log.Error(_T("CheckWBResult Fail."));
		ret = OTPCALI_ERROR_WBCALI;
		goto out;
	}
////////////////////////////////Akon-2018-4-6
	/*
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

		CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

		if (CheckWBResult_RAW10(
			RAW10Image,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight,
			nOutMode) < 0)
		{
			uts.log.Error(_T("CheckWBResult Fail."));
			ret = OTPCALI_ERROR_WBCALI;
			goto out;
		}
	}
	*/
	
	//WB_DATA_USHORT *am = &pwb->raw_after_multi2;
	//wb_do_multi(wb_data_c2, am);
	//uts.log.Debug(_T("WB(after multi,rgm[%.2lf],bgm[%.2lf]): RG[%d],BG[%d],GrGb[%d]"),pwb->rg_multi,pwb->bg_multi,am->RG,am->BG,am->GbGr);
	

	if ((ret = upload_wb_with_golden(wb_data_c2)) < 0)
		goto out;

	
	ret = OTPCALI_ERROR_NO;

out:
	RELEASE_ARRAY(RAW10Image);

	return SET_ERROR(ret);
}

void BaseOtp::get_gain_by_wb_ratio(const WB_RATIO *ratio, 
	const WB_RATIO *ratio_target, 
	double base, WB_GAIN *gain)
{
	gain->RGain = int(base * ratio_target->dr_gr / ratio->dr_gr + 0.5);
	gain->BGain = int(base * ratio_target->db_gb / ratio->db_gb + 0.5);
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
		int golden_id = otpDB->GetModuleID(T2A(pwbp->goldenSampleName), false);
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
			if (CheckWBLimit(&wb_db[1]) < 0) return SET_ERROR(OTPCALI_ERROR_WBCALI);
		}

		WB_RATIO ratio, ratio_g;
		get_wb_ratio(&wb_db[0], &ratio, 1.0);
		get_wb_ratio(&wb_db[1], &ratio_g, 1.0);
		int ret = check_wb_with_golden(&ratio, &ratio_g); 
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

	ratio->db_gr = multi * wb->B / wb->Gr;
	ratio->b_gr = int(ratio->db_gr + 0.5);

	ratio->dgb_gr= multi * wb->Gb / wb->Gr;
	ratio->gb_gr= int(ratio->dgb_gr + 0.5);

	ratio->dgr_gb= multi * wb->Gr / wb->Gb;
	ratio->gr_gb= int(ratio->dgr_gb + 0.5);

	ratio->db_gb= multi * wb->B / wb->Gb;
	ratio->b_gb= int(ratio->db_gb + 0.5);
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

	ratio->db_gr = multi * wb->B / wb->Gr;
	ratio->b_gr = int(ratio->db_gr + 0.5);

	ratio->dgb_gr= multi * wb->Gb / wb->Gr;
	ratio->gb_gr= int(ratio->dgb_gr + 0.5);

	ratio->dgr_gb= multi * wb->Gr / wb->Gb;
	ratio->gr_gb= int(ratio->dgr_gb + 0.5);

	ratio->db_gb= multi * wb->B / wb->Gb;
	ratio->b_gb= int(ratio->db_gb + 0.5);
}

void BaseOtp::GetWBROI(const WB_PARAM *pwb_param, const SIZE *imagesz, ROI *roi)
{
	if (pwb_param->roi.cx < 0 || pwb_param->roi.cy < 0) {
		roi->width = imagesz->cx / 5;
		roi->height = imagesz->cy / 5;
	} else {
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

int BaseOtp::wb_do_multi(const WB_DATA_UCHAR *in, WB_DATA_UCHAR *out)
{
	WB_PARAM *pwb = &otp_param.wb_param;

	out->R = in->R ;
	out->Gr = in->Gr;
	out->Gb = in->Gb;
	out->B = in->B ;
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
		ratio.dr_gr, ratio.db_gb, pwb->rg_target, pwb->bg_target,
		pwb->rg_delta, pwb->bg_delta);
	if (pwb->rg_delta > pwb->rg_delta_spec || pwb->bg_delta > pwb->bg_delta_spec) {
		return -1;
	}
	return 0;
}

int BaseOtp::CheckWBResult_RAW10(WORD* pRaw10, int width, int height, int rawBayerType)
{
	int ret = 0;

	struct WB_DATA_DOUBLE wb_data;

	WB_PARAM *pwb = &otp_param.wb_param;
	ROI roi;
	SIZE image_size;
	sensor->dev->GetImageSize(image_size);
	GetWBROI(pwb, &image_size, &roi);

	WB_DATA_DOUBLE wb_data_d;
	WB_DATA_USHORT *wb_data_c2 = &pwb->wb_data_c_aftercali2;

	memset(wb_data_c2, 0x00, sizeof(*wb_data_c2));

	int nOutMode = 0;

	if (!dev->ReadValue(
		eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
		&nOutMode, sizeof(nOutMode)))
	{
		uts.log.Error(_T("GetSensorOutMode error."));
		ret = OTPCALI_ERROR_SENSOR;
		return -1;
	}

	UTS::Algorithm::WB::WBCalibration(pRaw10, image_size.cx, image_size.cy, nOutMode,
		roi.x, roi.y, roi.width, roi.height,
		otp_param.SensorBlack_level, (double*)&wb_data_d, 
		(unsigned short*)wb_data_c2);

	WB_RATIO ratio;
	get_wb_ratio(wb_data_c2, &ratio, 1.0);
	pwb->rg_delta = abs(ratio.dr_gr - pwb->rg_target);
	pwb->bg_delta = abs(ratio.db_gb - pwb->bg_target);

	uts.log.Debug(_T("WB(after cali):R[%d]Gr[%d]Gb[%d]B[%d],Self:RG[%.3lf]BG[%.3lf],")
		_T("Tgt:[%.3lf]BG[%.3lf],Dlt:[%.3lf]BG[%.3lf]"),
		wb_data_c2->R, wb_data_c2->Gr, wb_data_c2->Gb, wb_data_c2->B,
		ratio.dr_gr, ratio.db_gb, pwb->rg_target, pwb->bg_target,
		pwb->rg_delta, pwb->bg_delta);
	if (pwb->rg_delta > (pwb->rg_delta_spec/2.0) || pwb->bg_delta > (pwb->bg_delta_spec/2.0)) {
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

int BaseOtp::upload_wb_with_golden(const WB_DATA_USHORT *wb)
{
	WB_PARAM *pwbp = &otp_param.wb_param;

	time_t tm = 0;
	int is_golden_self = !_tcscmp(m_szSN, pwbp->goldenSampleName);
	int is_only_upload = !_tcscmp(pwbp->goldenSampleName, EMPTY_STR);

	WB_DATA_USHORT wb_db[2];
	memset(wb_db, 0, sizeof(wb_db));
	memcpy(&wb_db[0], wb, sizeof(WB_DATA_USHORT));

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
		//int ret = check_wb_qualcommm_with_golden(wb_db[0],wb_db[1]); 
		//if (ret < 0) return ret;
	}

	if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_AWB,
		(char *)wb_db, sizeof(wb_db)) < 0)
	{
		uts.log.Error(_T("Failed to update WB data to DB!"));
		return SET_ERROR(OTPCALI_ERROR_DB);
	}

	return SET_ERROR(OTPCALI_ERROR_NO);

}



#pragma endregion