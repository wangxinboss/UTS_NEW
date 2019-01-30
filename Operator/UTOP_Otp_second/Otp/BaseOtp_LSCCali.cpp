#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"

#include <direct.h>
#include "SamsungOtp.h"
#include "VCM.h"

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::FixPatternNoise::FixPatternNoise_WD;
// LscCali
int BaseOtp::LscCali(void *args)
{
	int ret = OTPCALI_ERROR_NO;
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	if (otp_lsc_len <= 0) {
		uts.log.Error(_T("no lsc defination !"));
		return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);
	}

	dev->GetBufferInfo(m_bufferInfo);

	struct LSC_PARAM *lsc = &otp_param.lsc_param;

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}
	//check FPN
	FPNInfo m_FPNInfo;
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.dFPNSpec)
	{
		uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);
		return uts.errorcode.E_FixPatternNoise;;
	}
	uts.log.Info(_T("check FPN : m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("LSC(before cali): RI=%.2f, Delta=%.2f"),lsc->riResult.dRI, lsc->riResult.dRIDelta);

	if (do_lsc_cali() < 0) return SET_ERROR(OTPCALI_ERROR_LSCCALI);

	uint8_t *decode = (uint8_t*)dlmalloc(SIZE_K*10);
	int len = decode_lsc_data(otp_lsc_data, otp_lsc_len, decode, SIZE_K*10);

	ret = sensor->lsc_writeback(decode, len);
	dlfree(decode);
	if (ret == -SENSORDRIVER_ERROR_NOTSUPPORT) 
		goto upload_lsc;

	if (ret < 0) return SET_ERROR(OTPCALI_ERROR_SENSOR);

	Sleep(200); 

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("LSC(after cali): RI=%.2f, Delta=%.2f"),lsc->riResult.dRI, lsc->riResult.dRIDelta);

	if (check_lsc(lsc) < 0) return SET_ERROR(OTPCALI_ERROR_LSCCALI);

upload_lsc:
	if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC,
		(char *)otp_lsc_data, otp_lsc_len) < 0) {
			uts.log.Error(_T("Failed to update LSC Calibration data to DB!!"));
			return SET_ERROR(OTPCALI_ERROR_DB);
	}
	
	return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::upload_lsc_cali_data(void *raw, int len)
{
	if (is_mem_val(raw, 0, len)) {
		uts.log.Error(_T("OffChk: LSC data is all [0]!!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}
	if (is_mem_val(raw, 0xFF, len)) {
		uts.log.Error(_T("OffChk: LSC data is all [FF]!\n"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC, (char *)raw, len) < 0) {
		uts.log.Error(_T("Failed to update coef data to DB!"));
		return SET_ERROR(OTPCALI_ERROR_DB);
	}
	return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::check_lsc(struct LSC_PARAM *lsc)
{
    if (lsc->riResult.dRI < lsc->ri_target_l
        || lsc->riResult.dRI > lsc->ri_target_h
        || lsc->riResult.dRIDelta > lsc->ri_delta_target)
    {
        uts.log.Debug(_T("lsc out of sepc!"));
        return -1;
    }
    return 0;
}

int BaseOtp::upload_lsc()
{
    if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC,
        (char *)otp_lsc_data, otp_lsc_len) < 0)
    {
        uts.log.Error(_T("Failed to update LSC Calibration data to DB!"));
        return SET_ERROR(OTPCALI_ERROR_DB);
    }
    return SET_ERROR(OTPCALI_ERROR_NO);
}

int BaseOtp::do_mtk_lsc_cali()
{
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	int ret = OTPCALI_ERROR_NO;

	TCHAR cmd[0x100];

	struct LSC_PARAM *lsc = &otp_param.lsc_param;

	_stprintf(cmd, _T("%s\\%s\\"), uts.info.strRootPath, _T("slim_cal_correction_v1.4"));

	if (_wchdir(cmd) != 0) {
		 uts.log.Error(_T("Can not change to path[%s]!"), cmd);
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	//OSUtil::ExcuteCommand(_T("cmd.exe /C \"del /F result.txt eeprom_slim.txt\""), TRUE);
	remove("eeprom_slim.txt");
	remove("result.txt");

	uts.log.Info(_T("TK LSC: change tp path[%s]!"),cmd);

	dev->GetBufferInfo(m_bufferInfo);
	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}

	if (!uts.imgFile.SaveRawFile(_T("Calibration_MTK"),m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("TK LSC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	uts.log.Info(_T("TK LSC: succeed to save raw file!"));

	
	//check FPN

	FPNInfo m_FPNInfo;
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.dFPNSpec)
	{
		uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);
		return uts.errorcode.E_FixPatternNoise;;
	}
	uts.log.Info(_T("check FPN : m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

//	OSUtil::ExcuteCommand(_T("1_ShadingCalibrationSample.exe"), TRUE);
//	OSUtil::ExcuteCommand(_T("2_ShadingCorrectionSample.exe"), TRUE);

	ShellExecute (NULL,L"open",_T("1_ShadingCalibrationSample.exe"),NULL,NULL,SW_HIDE);
	Sleep(1500);
	ShellExecute (NULL,L"open",_T("2_ShadingCorrectionSample.exe"),NULL,NULL,SW_HIDE);
	Sleep(500);

	uts.log.Info(_T("TK LSC: succeed to run lsc tool!"));

	char tmp[0x10];
	/*if (file_read("result.txt", 1, tmp, 4) < 0) {
		uts.log.Error(_T("Read file error. filename = result.txt"));
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;
	}*/

	int _errcount = 0;

	uts.log.Info(_T("TK LSC: start to read result.txt!"));

	lsc->bLSCTable = TRUE;
	while(file_read("result.txt", 1, tmp, 4) < 0)
	{
		_errcount ++;
		Sleep(100);

		if (_errcount > 200)
		{
			uts.log.Error(_T("Read file error. filename = result.txt"));
			ret = OTPCALI_ERROR_LSCCALI;
			lsc->riResult.dRI = 0 ;
			lsc->riResult.dRIDelta = 0;
			lsc->bLSCTable = FALSE;
			goto out;
		}
	}

	lsc->bLSCVerify = TRUE;

	tmp[4] = '\0';
	if (strcmp(tmp, "pass")) {
		uts.log.Error(_T("LSC Cal error!!"));
		ret = OTPCALI_ERROR_LSCCALI;
		lsc->riResult.dRI = 0 ;
		lsc->riResult.dRIDelta = 0;
		lsc->bLSCVerify = FALSE;
		goto out;
	}

	uts.log.Info(_T("TK LSC: start to read eeprom_slim.txt!"));
	if (file_read("eeprom_slim.txt", 0, otp_lsc_data, otp_lsc_len) != otp_lsc_len) {
		uts.log.Error(_T("Read file error. filename = eeprom_slim.txt"));
		ret = OTPCALI_ERROR_LSCCALI;
		lsc->riResult.dRI = 0 ;
		lsc->riResult.dRIDelta = 0;
		goto out;
	}

	int rewrsz =  m_bufferInfo.nWidth * m_bufferInfo.nHeight / 4;
	uint8_t *raw = (uint8_t *)dlmalloc(rewrsz);

	uts.log.Info(_T("TK LSC: start to read after_ob_correct_lo.raw!"));
	if (file_read("after_ob_correct_lo.raw", 0, raw, rewrsz) != rewrsz) {
		uts.log.Error(_T("Read file error. filename = after_ob_correct_lo.raw"));
		ret = OTPCALI_ERROR_LSCCALI;
		lsc->riResult.dRI = 0 ;
		lsc->riResult.dRIDelta = 0;
		dlfree(raw);
		goto out;
	}
	uint8_t *bmp = (uint8_t *)dlmalloc(m_bufferInfo.nWidth * m_bufferInfo.nHeight / 4 * 3);

	int nOutMode = 0;

	if (!dev->ReadValue(
		eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
		&nOutMode, sizeof(nOutMode)))
	{
		uts.log.Error(_T("GetSensorOutMode error."));
		ret = uts.errorcode.E_NoImage;
		goto out;
	}

	UTS::Algorithm::Image::CImageProc::GetInstance().RawToBmp(nOutMode, raw, bmp, m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight / 2);
	RI_RGB(bmp, m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight / 2, lsc->roi.cx / 2, lsc->roi.cy / 2, lsc->riResult);
	uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI, lsc->riResult.dRIDelta);

	if (check_lsc(lsc) < 0) {
		dlfree(raw);
		dlfree(bmp);
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;
	}
	dlfree(raw);
	dlfree(bmp);

	uts.log.Debug(_T("get lsc data:"));
	PrintDebugArray(otp_lsc_data, otp_lsc_len);

	ret = upload_lsc_cali_data(otp_lsc_data, otp_lsc_len);
	if (ret < 0) {ret = -ret; goto out;}

	ret = OTPCALI_ERROR_NO;
out:
	return SET_ERROR(ret);
}

int BaseOtp::do_mtk_lsc_cali_MTK6797_MONO()
{
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	int ret = OTPCALI_ERROR_NO;

	TCHAR cmd[0x100];

	struct LSC_PARAM *lsc = &otp_param.lsc_param;

	_stprintf(cmd, _T("%s\\%s\\"), uts.info.strRootPath, _T("OTP_Tool_MONO_041416"));

	if (_wchdir(cmd) != 0) {
		 uts.log.Error(_T("Can not change to path[%s]!"), cmd);
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	//OSUtil::ExcuteCommand(_T("cmd.exe /C \"del /F result.txt eeprom_slim.txt\""), TRUE);
	remove("eeprom_slim.txt");
	remove("result.txt");

	dev->GetBufferInfo(m_bufferInfo);
	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}

	//check FPN

	FPNInfo m_FPNInfo;
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.dFPNSpec)
	{
		uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);
		return uts.errorcode.E_FixPatternNoise;;
	}
	uts.log.Info(_T("check FPN : m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	if (!uts.imgFile.SaveRawFile(_T("Calibration_MTK"),(BYTE*)RAW10Image,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("TK LSC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	RELEASE_ARRAY(RAW10Image);

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

//	OSUtil::ExcuteCommand(_T("ShadingCalibrationSample.exe"), TRUE);
//	OSUtil::ExcuteCommand(_T("ShadingCorrectionSample.exe"), TRUE);

	ShellExecute (NULL,L"open",_T("ShadingCalibrationSample.exe"),NULL,NULL,SW_HIDE);

	int _errcount = 0;
	while(file_read("eeprom_slim.txt", 0, otp_lsc_data, otp_lsc_len) != otp_lsc_len)
	{
		_errcount ++;
		Algorithm::Sleep(100);

		if (_errcount > 200)
		{
			uts.log.Error(_T("Read file error. filename = eeprom_slim.txt"));
			ret = OTPCALI_ERROR_LSCCALI;
			lsc->riResult.dRI = 0 ;
			lsc->riResult.dRIDelta = 0;
			goto out;
		}
	}

	ShellExecute (NULL,L"open",_T("ShadingCorrectionSample.bat"),NULL,NULL,SW_HIDE);

	char tmp[0x10]={0},tmp1[0x10]={0},tmp2[0x10]={0};

	_errcount = 0;

	lsc->bLSCTable = TRUE;
	while(file_read("result.txt", 1, tmp, 10) < 0)
	{
		_errcount ++;
		Algorithm::Sleep(100);

		if (_errcount > 200)
		{
			uts.log.Error(_T("Read file error. filename = result.txt"));
			ret = OTPCALI_ERROR_LSCCALI;
			lsc->riResult.dRI = 0 ;
			lsc->riResult.dRIDelta = 0;
			lsc->bLSCTable = FALSE;
			goto out;
		}
	}

	lsc->bLSCVerify = TRUE;

	memcpy(tmp1,tmp,4);
	memcpy(tmp2,tmp+5,4);

	tmp1[4] = '\0';
	tmp2[4] = '\0';

	if (strcmp(tmp1, "pass") || strcmp(tmp2, "pass") ) 
	{
		uts.log.Error(_T("LSC Cal error!!"));
		ret = OTPCALI_ERROR_LSCCALI;
		lsc->riResult.dRI = 0 ;
		lsc->riResult.dRIDelta = 0;
		lsc->bLSCVerify = FALSE;
		goto out;
	}
	
	int rewrsz =  m_bufferInfo.nWidth * m_bufferInfo.nHeight / 4;
	uint8_t *raw = (uint8_t *)dlmalloc(rewrsz);
	if (file_read("after_ob_lo.raw", 0, raw, rewrsz) != rewrsz) {
		uts.log.Error(_T("Read file error. filename = after_ob_correct_lo.raw"));
		ret = OTPCALI_ERROR_LSCCALI;
		lsc->riResult.dRI = 0 ;
		lsc->riResult.dRIDelta = 0;
		dlfree(raw);
		goto out;
	}
	uint8_t *bmp = (uint8_t *)dlmalloc(m_bufferInfo.nWidth * m_bufferInfo.nHeight / 4 * 3);

	int nOutMode = 0;

	if (!dev->ReadValue(
		eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
		&nOutMode, sizeof(nOutMode)))
	{
		uts.log.Error(_T("GetSensorOutMode error."));
		ret = uts.errorcode.E_NoImage;
		goto out;
	}

	UTS::Algorithm::Image::CImageProc::GetInstance().RawToBmp(nOutMode, raw, bmp, m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight / 2);
	RI_RGB(bmp, m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight / 2, lsc->roi.cx / 2, lsc->roi.cy / 2, lsc->riResult);
	uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI, lsc->riResult.dRIDelta);

	RGBTRIPLE rgb;
	RECT rect_Avg;
	rect_Avg.left = m_bufferInfo.nWidth / 4 - 200;
	rect_Avg.top = m_bufferInfo.nHeight / 4 - 200;
	rect_Avg.right = m_bufferInfo.nWidth / 4 + 200;
	rect_Avg.bottom = m_bufferInfo.nHeight / 4 + 200;

	UTS::Algorithm::GetROIAvgRGB(bmp, m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight / 2,32, rect_Avg, rgb);
	lsc->dAvgCenterG = rgb.rgbtGreen; 
	uts.log.Debug(_T("After LSC Calibration,(R,G,B) = (%d,%d,%d) "),rgb.rgbtRed,rgb.rgbtGreen,rgb.rgbtBlue);

	if (check_lsc(lsc) < 0) {
		dlfree(raw);
		dlfree(bmp);
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;
	}
	dlfree(raw);
	dlfree(bmp);

	uts.log.Debug(_T("get lsc data:"));
	PrintDebugArray(otp_lsc_data, otp_lsc_len);

	ret = upload_lsc_cali_data(otp_lsc_data, otp_lsc_len);
	if (ret < 0) {ret = -ret; goto out;}

	ret = OTPCALI_ERROR_NO;
out:
	return SET_ERROR(ret);
}

int BaseOtp::do_mtk_lsc_cali_MTK6797_RGB()
{
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	int ret = OTPCALI_ERROR_NO;

	TCHAR cmd[0x100];

	struct LSC_PARAM *lsc = &otp_param.lsc_param;

	_stprintf(cmd, _T("%s\\%s\\"), uts.info.strRootPath, _T("OTP_Tool_RGB_041416"));

	if (_wchdir(cmd) != 0) {
		uts.log.Error(_T("Can not change to path[%s]!"), cmd);
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	//OSUtil::ExcuteCommand(_T("cmd.exe /C \"del /F result.txt eeprom_slim.txt\""), TRUE);
	remove("eeprom_slim.txt");
	remove("result.txt");

	dev->GetBufferInfo(m_bufferInfo);
	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}
	/*
    WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	if (!uts.imgFile.SaveRawFile(_T("Calibration_MTK"),(BYTE*)RAW10Image,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("TK LSC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	RELEASE_ARRAY(RAW10Image);
	*/

	//check FPN

	FPNInfo m_FPNInfo;
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.dFPNSpec)
	{
		uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);
		return uts.errorcode.E_FixPatternNoise;;
	}
	uts.log.Info(_T("check FPN : m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);

	if (!uts.imgFile.SaveRawFile(_T("Calibration_MTK"),(BYTE*)m_bufferObj.pFrameBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("TK LSC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	ShellExecute (NULL,L"open",_T("ShadingCalibrationSample.exe"),NULL,NULL,SW_HIDE);

	int _errcount = 0;
	while(file_read("eeprom_slim.txt", 0, otp_lsc_data, otp_lsc_len) != otp_lsc_len)
	{
		_errcount ++;
		Algorithm::Sleep(100);

		if (_errcount > 50)
		{
			uts.log.Error(_T("Read file error. filename = eeprom_slim.txt"));
			ret = OTPCALI_ERROR_LSCCALI;
			lsc->riResult.dRI = 0 ;
			lsc->riResult.dRIDelta = 0;
			goto out;
		}
	}

	ShellExecute (NULL,L"open",_T("ShadingCorrectionSample.bat"),NULL,NULL,SW_HIDE);

	char tmp[0x10]={0},tmp1[0x10]={0},tmp2[0x10]={0};

	_errcount = 0;

	lsc->bLSCTable = TRUE;
	while(file_read("result.txt", 1, tmp, 10) < 0)
	{
		_errcount ++;
		Algorithm::Sleep(100);

		if (_errcount > 50)
		{
			uts.log.Error(_T("Read file error. filename = result.txt"));
			ret = OTPCALI_ERROR_LSCCALI;
			lsc->riResult.dRI = 0 ;
			lsc->riResult.dRIDelta = 0;
			lsc->bLSCTable = FALSE;
			goto out;
		}
	}

	lsc->bLSCVerify = TRUE;

	memcpy(tmp1,tmp,4);
	memcpy(tmp2,tmp+5,4);

	tmp1[4] = '\0';
	tmp2[4] = '\0';
	
	if (strcmp(tmp1, "pass") || strcmp(tmp2, "pass")) 
	{
		uts.log.Error(_T("LSC Cal error - No pass!!"));
		ret = OTPCALI_ERROR_LSCCALI;
		lsc->riResult.dRI = 0 ;
		lsc->riResult.dRIDelta = 0;
		lsc->bLSCVerify = FALSE;
		goto out;
	}
	

	int rewrsz =  m_bufferInfo.nWidth * m_bufferInfo.nHeight / 4;
	uint8_t *raw = (uint8_t *)dlmalloc(rewrsz);
	if (file_read("after_ob_lo.raw", 0, raw, rewrsz) != rewrsz) {
		uts.log.Error(_T("Read file error. filename = after_ob_correct_lo.raw"));
		ret = OTPCALI_ERROR_LSCCALI;
		lsc->riResult.dRI = 0 ;
		lsc->riResult.dRIDelta = 0;
		dlfree(raw);
		goto out;
	}
	uint8_t *bmp = (uint8_t *)dlmalloc(m_bufferInfo.nWidth * m_bufferInfo.nHeight / 4 * 3);

	int nOutMode = 0;

	if (!dev->ReadValue(
		eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
		&nOutMode, sizeof(nOutMode)))
	{
		uts.log.Error(_T("GetSensorOutMode error."));
		ret = uts.errorcode.E_NoImage;
		goto out;
	}

	UTS::Algorithm::Image::CImageProc::GetInstance().RawToBmp(nOutMode, raw, bmp, m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight / 2);
	RI_RGB(bmp, m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight / 2, lsc->roi.cx / 2, lsc->roi.cy / 2, lsc->riResult);
	uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI, lsc->riResult.dRIDelta);

	RGBTRIPLE rgb;
	RECT rect_Avg;
	rect_Avg.left = m_bufferInfo.nWidth / 4 - 200;
	rect_Avg.top = m_bufferInfo.nHeight / 4 - 200;
	rect_Avg.right = m_bufferInfo.nWidth / 4 + 200;
	rect_Avg.bottom = m_bufferInfo.nHeight / 4 + 200;

	UTS::Algorithm::GetROIAvgRGB(bmp, m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight / 2,32, rect_Avg, rgb);
	lsc->dAvgCenterG = rgb.rgbtGreen; 
	uts.log.Debug(_T("After LSC Calibration,(R,G,B) = (%d,%d,%d) "),rgb.rgbtRed,rgb.rgbtGreen,rgb.rgbtBlue);


	if (check_lsc(lsc) < 0) {
		dlfree(raw);
		dlfree(bmp);
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;
	}
	dlfree(raw);
	dlfree(bmp);

	uts.log.Debug(_T("get lsc data:"));
	PrintDebugArray(otp_lsc_data, otp_lsc_len);

	ret = upload_lsc_cali_data(otp_lsc_data, otp_lsc_len);
	if (ret < 0) {ret = -ret; goto out;}

	ret = OTPCALI_ERROR_NO;
out:
	return SET_ERROR(ret);
}
int BaseOtp::do_lsc_cali()
{
/*	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	FILE* fp = fopen("LensRI.txt","rt");
	int m_nWidth = 3264;
	int m_nHeight = 2448;
	int m_nGr=60,m_nR=60,m_nB=60,m_nGb=60;
	double m_dbAlpha = 0.3, m_dbSeed = 0.01;
	BOOL m_bScooby = FALSE;
	int i;
	struct LSC_PARAM *lsc = &otp_param.lsc_param;

	//struct LSC_PARAM *lsc = &otp_param.lsc_param;
	//////////////////////////////////////////////////////////////////////////
	AF_INT af;
	GetAFData(&af);
	if (af.inf < 0 || af.mup < 0)
	{
		uts.log.Error(_T("GetAFData"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	DLLSetVCM_Initial(dev,uts.info.nVCMType, NULL);
	//VCM Move INF 
	DLLSetVCM_Move(dev, uts.info.nVCMType, af.inf);
	Sleep(300);

	//////////////////////////////////////////////////////////////////////////
	if(fp==NULL)
	{
		printk(_T("File Open Error."));
		return SET_ERROR(SENSORDRIVER_ERROR_LSCCALI);
	}
	else
	{
		double* pLensRI = new double[11];
		char strData[256];
		for(i=0;i<11;i++)
		{
			fscanf(fp,"%s",strData);
			sscanf(strData,"%1f",pLensRI+1);
		}
		pLensRI[0]=27;
		double crmax = 100*(1-(100-1.4*pLensRI[0])/(100-pLensRI[0]));
		m_nGr = (int) (m_nGr > crmax ? crmax : m_nGr); //m_nGr ªì©l­È?
		m_nR = (int) (m_nR > crmax ? crmax : m_nR);
		m_nB = (int) (m_nB > crmax ? crmax : m_nB);
		m_nGb = (int) (m_nGb > crmax ? crmax : m_nGb);
	}
	dev->GetBufferInfo(m_bufferInfo);
	SetParameter(64, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_nGr,m_nR, m_nB, m_nGb, 0.3, 0.01, TRUE, FALSE);

	CString strRawFile;
	CString strOTPSetFile;
	CString strSRAMSetfile;
	CString strWorkPath;

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_V24"),(BYTE*)RAW10Image,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("LSC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}
	int x,y;
	for(y=0;y<m_bufferInfo.nHeight;y+=2)
	{
		for(x=0;x<m_bufferInfo.nWidth;x+=2)
		{
			RAW10Image[y*m_bufferInfo.nWidth+x+1]=0x03FF;
		}
	}


	CString str;
	str = "D:\\";
	BOOL is_SINGULAR = FALSE;
	BOOL bSEED_overflow = FALSE;
	BOOL b2ndSet = FALSE;
	Run((BYTE *)RAW10Image,(BYTE *)RAW10Image, FALSE, str,FALSE,&is_SINGULAR,&bSEED_overflow);
	//Run(pRawBuffer,pRawBuffer, FALSE, str,FALSE,FALSE,FALSE);
	SaveOTPSetfile("LSCOTPData.txt");
	uts.log.Debug(_T("LSC: LSCOTPData file!"));
	SaveSRAMSetfile("LSCSRAMData.txt");
	uts.log.Debug(_T("LSC: LSCSRAMData file!"));

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);


	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	int nSRAM = GetSRAMSetfileSize();
	if(nSRAM >0)
	{
		char *szSRAM = new char[nSRAM];
		GetSRAMSetfile(szSRAM);
		Sleep(10);
		//write_reg(szSRAM, nSRAM);
	}

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	sensor->dev->write_sensor(0x0B00, 0x01);

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	int nOTP = GetOTPSetfileSize();
	if(nOTP > 0)
	{
		char *szOTP = new char[nOTP];
		uint8_t *otp_lsc = new uint8_t[360];
		char	addr_buf[5];
		addr_buf[4] = 0;
		char reg_buf[5] = { 0 };
		int reg;

		GetOTPSetfile(szOTP);
		if(0)// use 2nd Set
		{
			for(i=0;i<40;i++)
			{
				memcpy(reg_buf, &szOTP[78+(24+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i]=reg;
				//otp_lsc[i]=(szOTP[80+51*2+i]-0x30)*0x10+(szOTP[80+51*2+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+40]=reg;
				//otp_lsc[i+17]=(szOTP[80+170+i]-0x30)*0x10+(szOTP[80+170+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85*2+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+40+64]=reg;
				//otp_lsc[i+17+64]=(szOTP[80+170*2+i]-0x30)*0x10+(szOTP[80+170*2+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85*3+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+40+64*2]=reg;
				//otp_lsc[i+17+64*2]=(szOTP[80+170*3+i]-0x30)*0x10+(szOTP[80+170*3+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85*4+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+40+64*3]=reg;
				//otp_lsc[i+17+64*3]=(szOTP[80+170*4+i]-0x30)*0x10+(szOTP[80+170*4+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85*5+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+40+64*4]=reg;
				//otp_lsc[i+17+64*4]=(szOTP[80+170*5+i]-0x30)*0x10+(szOTP[80+170*5+i+1]-0x30);
			}
		}
		else
		{
			for(i=0;i<17;i++)
			{
				memcpy(reg_buf, &szOTP[78+(47+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i]=reg;
				//otp_lsc[i]=(szOTP[80+51*2+i]-0x30)*0x10+(szOTP[80+51*2+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+17]=reg;
				//otp_lsc[i+17]=(szOTP[80+170+i]-0x30)*0x10+(szOTP[80+170+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85*2+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+17+64]=reg;
				//otp_lsc[i+17+64]=(szOTP[80+170*2+i]-0x30)*0x10+(szOTP[80+170*2+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85*3+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+17+64*2]=reg;
				//otp_lsc[i+17+64*2]=(szOTP[80+170*3+i]-0x30)*0x10+(szOTP[80+170*3+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85*4+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+17+64*3]=reg;
				//otp_lsc[i+17+64*3]=(szOTP[80+170*4+i]-0x30)*0x10+(szOTP[80+170*4+i+1]-0x30);
			}
			for(i=0;i<64;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85*5+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+17+64*4]=reg;
				//otp_lsc[i+17+64*4]=(szOTP[80+170*5+i]-0x30)*0x10+(szOTP[80+170*5+i+1]-0x30);
			}
			for(i=0;i<23;i++)
			{
				memcpy(reg_buf, &szOTP[78+(85*6+i)*2], 2);
				reg = strtoul(reg_buf, NULL, 16);
				otp_lsc[i+17+64*5]=reg;
				//otp_lsc[i+17+64*5]=(szOTP[80+170*6+i]-0x30)*0x10+(szOTP[80+170*6+i+1]-0x30);
			}
		}

		//upload_lsc:
		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC,
			(char *)otp_lsc, 360) < 0) {
				uts.log.Error(_T("Failed to update LSC Calibration data to DB!!"));
				return SET_ERROR(OTPCALI_ERROR_DB);
		}
		get_otp_data_from_sensor(otp_data_in_sensor);
		uts.log.Debug(_T("OtpData in Sensor:"));
		for(int i=0;i<otp_data_len/16;i++)
		{
			PrintDebugArray(otp_data_in_sensor+i*16, 16);
			uts.log.Debug(_T(""));
		}
		PrintDebugArray(otp_data_in_sensor+(otp_data_len/16)*16, otp_data_len-(otp_data_len/16)*16);

		if(1)// use 2nd Set
		{
			//LSC set2
			//page9 0x0A1C~0x0A43 LenC Setting 2
			if(sensor->do_prog_otp(0x9,0x0A1C,otp_lsc,40))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//pageA 0x0A04~0x0A43 LenC Setting 2
			if(sensor->do_prog_otp(0xA,0x0A04,otp_lsc+40,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//pageB 0x0A04~0x0A43 LenC Setting 2
			if(sensor->do_prog_otp(0xB,0x0A04,otp_lsc+40+64,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//pageC 0x0A04~0x0A43 LenC Setting 2
			if(sensor->do_prog_otp(0xC,0x0A04,otp_lsc+40+64*2,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//pageD 0x0A04~0x0A43 LenC Setting 2
			if(sensor->do_prog_otp(0xD,0x0A04,otp_lsc+40+64*3,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//pageE 0x0A04~0x0A43 LenC Setting 2
			if(sensor->do_prog_otp(0xE,0x0A04,otp_lsc+40+64*4,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
		}
		else
		{
			//LSC set1
			//page3 0x0A33~0x0A43 LenC Setting 1
			if(sensor->do_prog_otp(0x3,0x0A33,otp_lsc,17))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//page4 0x0A04~0x0A43 LenC Setting 1
			if(sensor->do_prog_otp(0x4,0x0A04,otp_lsc+17,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//page5 0x0A04~0x0A43 LenC Setting 1
			if(sensor->do_prog_otp(0x5,0x0A04,otp_lsc+17+64,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//page6 0x0A04~0x0A43 LenC Setting 1
			if(sensor->do_prog_otp(0x6,0x0A04,otp_lsc+17+64*2,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//page7 0x0A04~0x0A43 LenC Setting 1
			if(sensor->do_prog_otp(0x7,0x0A04,otp_lsc+17+64*3,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//page8 0x0A04~0x0A43 LenC Setting 1
			if(sensor->do_prog_otp(0x8,0x0A04,otp_lsc+17+64*4,64))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);
			//page9 0x0A04~0x0A43 LenC Setting 1
			if(sensor->do_prog_otp(0x9,0x0A04,otp_lsc+17+64*5,23))
				return SET_ERROR(OTPCALI_ERROR_SENSOR);

		}

		CString strRegName = _T("OTP_WBLSC_TEST");

		if (!dev->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
			strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
		{
			uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		}

		//temp[0]=0x1C;
		//sensor->do_prog_otp(0xF,0x0A05,temp,1);
		// 		sensor->do_read_otp(0xF,0x0A04,temp_otp,64);
		// 		Sleep(100);
		// 
		// 		//LSC
		// 		//page3 0x0A33~0x0A43 LenC Setting 1
		// 		if(sensor->do_prog_otp(0x3,0x0A33,otp_lsc,17))
		// 			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		// 		//page4 0x0A04~0x0A43 LenC Setting 1
		// 		if(sensor->do_prog_otp(0x4,0x0A04,otp_lsc+17,64))
		// 			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		// 		//page5 0x0A04~0x0A43 LenC Setting 1
		// 		if(sensor->do_prog_otp(0x5,0x0A04,otp_lsc+17+64*2,64))
		// 			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		// 		//page6 0x0A04~0x0A43 LenC Setting 1
		// 		if(sensor->do_prog_otp(0x6,0x0A04,otp_lsc+17+64*3,64))
		// 			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		// 		//page7 0x0A04~0x0A43 LenC Setting 1
		// 		if(sensor->do_prog_otp(0x7,0x0A04,otp_lsc+17+64*4,64))
		// 			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		// 		//page8 0x0A04~0x0A43 LenC Setting 1
		// 		if(sensor->do_prog_otp(0x8,0x0A04,otp_lsc+17+64*5,64))
		// 			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		// 		//page9 0x0A04~0x0A43 LenC Setting 1
		// 		if(sensor->do_prog_otp(0x9,0x0A04,otp_lsc+17+64*6,23))
		// 			return SET_ERROR(OTPCALI_ERROR_SENSOR);
	}
	else
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);*/

	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------

int BaseOtp::chagetexttohex(char *buf, char *output, int len)
{
	int reg;
	int count = 0;
	char	addr_buf[5];
	addr_buf[4] = 0;
	char reg_buf[5] = { 0 };

	int ret = 0;
	for (int i = 0; i < len;) {
		if (buf[i] == '/') {
			for (; i < len;) {
				i++;
				if (buf[i] == '\n') break;
			}
		} else if (buf[i] == 'm' || buf[i] == 'M') {
			i += 5;
			for (int m = 0; m < 70; m++) {
				memcpy(reg_buf, &buf[i], 2);
				i += 2;
				reg = strtoul(reg_buf, NULL, 16);
				*(output + count) = reg;
				count++;
				if (buf[i] == 'p' || buf[i] == 'P') break;
				if (buf[i] == '\r' || buf[i] == '\n') break;
			}
		} else i++;
	}
	return count;
}

int BaseOtp::write_reg(char *buf, int len)
{
	unsigned long addr, reg;
	char addr_buf[5];
	addr_buf[4] = 0;
	char reg_buf[5];
	reg_buf[2] = 0;
	reg_buf[3] = 0;
	int ret = 0;
	for (int i = 0; i < len;) {
		if (buf[i] == '/') {
			for (; i < len;) {
				i++;
				if (buf[i] == '\n') break;
			}
		} else if (buf[i] == 's') {
			i++;
			for (int j = 0; j < 4; j++) {
				addr_buf[j] = buf[i];
				i++;
			}
			addr = strtoul(addr_buf, NULL, 16);
			for (int j = 0; j < 2; j++) {
				reg_buf[j] = buf[i];
				i++;
			}
			reg = strtoul(reg_buf, NULL, 16);
			sensor->dev->write_sensor((WORD)addr, (WORD)reg);
		} else if (buf[i] == 'p') {
			Sleep(10); i++;
		} else i++;
	}
	return 0;
}


int BaseOtp::SamsungLscCali()
{
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;

	int ret = OTPCALI_ERROR_NO;
	char *OTPSetting,*SRAMSetting;
	int LSCLen_OTP = 0,LSCLen_SRAM = 0;

	OTPSetting = new char[1200];
	SRAMSetting = new char[8500];


	dev->GetBufferInfo(m_bufferInfo);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;	
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	//check FPN
	FPNInfo m_FPNInfo;
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.dFPNSpec)
	{
		uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);
		return uts.errorcode.E_FixPatternNoise;;
	}
	uts.log.Info(_T("check FPN : m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.dFPNSpec);

	struct LSC_PARAM *lsc = &otp_param.lsc_param;

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*4];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth*2, m_bufferInfo.nHeight);

	if (!uts.imgFile.SaveRawFile(_T("Capture_RAW10"),(BYTE*)RAW10Image,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("TK LSC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	ret = do_SamsungLSCCali(RAW10Image,m_bufferInfo.nWidth,m_bufferInfo.nHeight,
							OTPSetting,LSCLen_OTP,
							SRAMSetting,LSCLen_SRAM);
	
	//Load SRAM check
	write_reg(SRAMSetting,LSCLen_SRAM);
	sensor->dev->write_sensor(0x3400 ,0x00);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;		
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	if (check_lsc(lsc) < 0) {
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;
	}


	if (chagetexttohex(OTPSetting, (char *)otp_lsc_data, LSCLen_OTP) != otp_lsc_len) {
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;	
	}

	if(otp_lsc_len != 0)
	{
		if (otpDB->UpdateOtpByType(mid, OTPDB_OTPTYPE_LSC, (char*)otp_lsc_data, otp_lsc_len) < 0) {
			uts.log.Error(_T("Failed to update LSC data to DB!"));
			ret  = OTPCALI_ERROR_DB;
		}
	}else
	{
		ret = OTPCALI_ERROR_LSCCALI;
	}

out:
	RELEASE_ARRAY(RAW10Image);
	RELEASE_ARRAY(OTPSetting);
	RELEASE_ARRAY(SRAMSetting);
	
	return SET_ERROR(ret);
}