#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "VCM.h"
#include <direct.h>
//#include "PDAFCalibrationTools.h"
#include "Focus_MTF.h"

#pragma comment(lib, "UTS_VCM.lib")

#pragma comment(lib, "UTSAlgorithm.lib")

using namespace UTS::Algorithm::Image;

int BaseOtp::qulcomm_dcc_verify()
{
	int ret = OTPCALI_ERROR_NO;

	u8 tmp[8];

	if (otpDB->GetOtpByType(mid, 8, (char *)&tmp[0], 4) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}
	if (otpDB->GetOtpByType(mid, 9, (char *)&tmp[4], 4) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	u16 coef[4];
	coef[0] = get_le_val(tmp + 0, 2);
	coef[1] = get_le_val(tmp + 2, 2);
	coef[2] = get_le_val(tmp + 4, 2);
	coef[3] = get_le_val(tmp + 6, 2);
	int coef_max = max(max(coef[0] , coef[1]) ,max(coef[2] , coef[3])) ;
	int coef_min = min(min(coef[0] , coef[1]) ,min(coef[2] , coef[3])) ;
	int coef_total = coef[0] + coef[1] + coef[2] + coef[3];
	u16 coef_mid = ( coef_total - coef_max - coef_min) / 2;

	otp_param.pdaf_param.pdafverify.pdafResult.PD = coef_mid;

	uts.log.Debug(_T("PD: %d %d %d %d Verifty PD:%d"),coef[0],coef[1],coef[2],coef[3],coef_mid);

	AF_INT af;
	GetAFData(&af);
	if (af.inf < 0 || af.mup < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	uts.log.Debug(_T("AF inf:%d macro:%d"),af.inf,af.mup);

	uint8_t SPCTable[0x1200];

	if (otpDB->GetOtpByType(mid, OTPDB_OTPTYPE_SPC, (char *)SPCTable,
		otp_spc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	//MTF Focus
	SobelParam sobel;
	sobel.sta = af.inf;
	sobel.end = af.mup;
	sobel.step = 4;

	SIZE image_size; 
	dev->GetBufferInfo(m_bufferInfo);

	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	GetWBROI(&otp_param.wb_param, &image_size, &sobel.roi);
	sobel.down_step_cnt = 5;
	sobel.down_sobel_th = 4.0;
	SobelResult sobel_rest;

	if (MTFFocus(dev,&sobel,m_bufferInfo.nWidth,m_bufferInfo.nHeight,&sobel_rest) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}
	otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC = sobel_rest.focus_point;

	dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame);
	SaveImage(_T("MTFFocus"));

	int dacmid = (af.mup + af.inf) / 2;
	otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC = dacmid;

	DLLSetVCM_Move(dev, uts.info.nVCMType, otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC);
	Algorithm::Sleep(500);

	if (!dev->Recapture(
		m_bufferObj,
		uts.info.nLTDD_DummyFrame,
		uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("Recapture error. "));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	bool verifyret = do_qulcomm_DCCVerify(SPCTable,RAW10Image,
		m_bufferInfo.nWidth, m_bufferInfo.nHeight,
		coef_mid,
		otp_param.pdaf_param.pdafverify.PDToleranceError,
		otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC,
		sobel_rest.focus_point,
		otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC,
		otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyError);

	RELEASE_ARRAY(RAW10Image);

	if(!verifyret) 
	{
		uts.log.Error(_T("DCC verify error !"));
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	//OK -> update to DB
	put_le_val(coef_mid, tmp, 2);
	if (otpDB->UpdateOtpByType(mid, 10, (char *)tmp, 2) < 0)
	{
		uts.log.Error(_T("Failed to update coef data to DB!"));
		ret = OTPCALI_ERROR_DB;
		goto out;
	}

out:
	return SET_ERROR(ret);
}

int BaseOtp::qulcomm_dcc_verify_REL()
{
	int ret = OTPCALI_ERROR_NO;
	uint8_t DCCTable[0x1200];

	if (otpDB->GetOtpByType(mid, 8,(char *) DCCTable,otp_dcc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	AF_INT af;
	GetAFData(&af);
	if (af.inf < 0 || af.mup < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	uts.log.Debug(_T("AF inf:%d macro:%d"),af.inf,af.mup);

	//MTF Focus
	SobelParam sobel;
	sobel.sta = af.inf;
	sobel.end = af.mup;
	//sobel.step = 4;
	sobel.step = 16;

	SIZE image_size; 
	dev->GetBufferInfo(m_bufferInfo);

	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	GetWBROI(&otp_param.wb_param, &image_size, &sobel.roi);
	sobel.down_step_cnt = 5;
	sobel.down_sobel_th = 4.0;
	SobelResult sobel_rest;

	DLLSetVCM_Initial(dev,uts.info.nVCMType, NULL);

	if (MTFFocus(dev,&sobel,m_bufferInfo.nWidth,m_bufferInfo.nHeight,&sobel_rest) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}
	otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC = sobel_rest.focus_point;

	dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame);
	SaveImage(_T("MTFFocus"));

	int dacmid = (af.mup + af.inf) / 2;
	otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC = dacmid;

	DLLSetVCM_Move(dev, uts.info.nVCMType, otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC);
	Algorithm::Sleep(500);

	if (!dev->Recapture(
		m_bufferObj,
		uts.info.nLTDD_DummyFrame,
		uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("Recapture error. "));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	SaveImage(_T("PDAFVerifyDAC"));
	uts.log.Debug(_T("PDAF init DAC:%d"),otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC);

	bool verifyret = true;
	int MoveDAC;
	do_qulcomm_DCCVerify_REV(DCCTable,RAW10Image,MoveDAC);

	RELEASE_ARRAY(RAW10Image);

	otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC = MoveDAC;
	otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyError = abs(otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC - 
		(dacmid - otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC));
	if(otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyError >
		otp_param.pdaf_param.pdafverify.PDToleranceError)
	{
		uts.log.Error(_T("DCC verify error !"));
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}



out:
	return SET_ERROR(ret);
}

int BaseOtp::qulcomm_dcc_verify_2pd_REVL()
{
	int ret = OTPCALI_ERROR_NO;
	uint8_t DCCTable[0x1200];

	if (otpDB->GetOtpByType(mid, 8,(char *) DCCTable,otp_dcc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	AF_INT af;
	GetAFData(&af);
	if (af.inf < 0 || af.mup < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	uts.log.Debug(_T("AF inf:%d macro:%d"),af.inf,af.mup);

	//MTF Focus
	SobelParam sobel;
	sobel.sta = af.inf;
	sobel.end = af.mup;
	//sobel.step = 4;
	sobel.step = 16;

	SIZE image_size; 
	dev->GetBufferInfo(m_bufferInfo);

	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	GetWBROI(&otp_param.wb_param, &image_size, &sobel.roi);
	sobel.down_step_cnt = 5;
	sobel.down_sobel_th = 4.0;
	SobelResult sobel_rest;

	DLLSetVCM_Initial(dev,uts.info.nVCMType, NULL);

	if (MTFFocus(dev,&sobel,m_bufferInfo.nWidth,m_bufferInfo.nHeight,&sobel_rest) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}
	otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC = sobel_rest.focus_point;

	dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame);
	SaveImage(_T("MTFFocus"));

	int dacmid = (af.mup + af.inf) / 2;
	otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC = dacmid;

	DLLSetVCM_Move(dev, uts.info.nVCMType, otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC);
	Algorithm::Sleep(500);

	if (!dev->Recapture(
		m_bufferObj,
		uts.info.nLTDD_DummyFrame,
		uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("Recapture error. "));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth*2, m_bufferInfo.nHeight);
	SaveImage(_T("PDAFVerifyDAC"));
	uts.log.Debug(_T("PDAF init DAC:%d"),otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC);

	bool verifyret = true;
	int MoveDAC;
	do_qulcomm_DCCVerify_REV(DCCTable,RAW10Image,MoveDAC);

	RELEASE_ARRAY(RAW10Image);

	otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC = MoveDAC;
	otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyError = abs(otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC - 
		(dacmid - otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC));
	if(otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyError >
		otp_param.pdaf_param.pdafverify.PDToleranceError)
	{
		uts.log.Error(_T("DCC verify error !"));
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}



out:
	return SET_ERROR(ret);
}
