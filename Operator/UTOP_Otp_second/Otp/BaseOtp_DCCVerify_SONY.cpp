#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "VCM.h"
#include <direct.h>

#include "Focus_MTF.h"

using namespace UTS::Algorithm::Image;

int BaseOtp::sony_dcc_verify()
{
	int ret = OTPCALI_ERROR_NO;

	USES_CONVERSION;

	CString str_path;

	dev->GetBufferInfo(m_bufferInfo);

	int pdaf_sony_tagetdac ;

	pdaf_sony_tagetdac  = 0;
	pdaf_sony_verifydac = 0;

	int *PDTable            = new int[pdaf_pd_len];
	uint8_t *SPCTable       = new uint8_t[otp_spc_len];
	uint8_t *DCCTable       = new uint8_t[otp_dcc_len];
	int *DCCOUT             = new int[otp_dcc_len/2];
	WORD *RAW10Image_input  = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

	memset(PDTable,0,pdaf_pd_len);
	memset(SPCTable,0,otp_spc_len);
	memset(DCCTable,0,otp_dcc_len);
	memset(DCCOUT,0,pdaf_pd_len);


	int dac[48];
	AF_INT af;
	GetAFData(&af);
	uts.log.Info(_T("af.inf = %d ,af.mup = %d."),af.inf,af.mup);

	if (af.inf < 0 || af.mup < 0 || af.inf > af.mup ){
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	if (otpDB->GetOtpByType(mid, 12, (char *)SPCTable, otp_spc_len) < 0){
		uts.log.Error(_T("get dcc table froam db error !"));
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}
	if (otpDB->GetOtpByType(mid, 8, (char *)DCCTable, otp_dcc_len) < 0){
		ret = OTPCALI_ERROR_PDAF_DCC;
		uts.log.Error(_T("get spc_map from db error"));
		goto out;
	}

	////step1:MTF Focus
	//SobelParam sobel;
	//if(otp_param.pdaf_param.pdafverify.cdaf_type==0){
	//	sobel.sta = af.inf;
	//	sobel.end = af.mup;
	//	sobel.step = otp_param.pdaf_param.pdafverify.cdaf_Step;
	//}else{
	//	sobel.sta = af.inf;
	//	sobel.end = af.mup;
	//	sobel.step = otp_param.pdaf_param.pdafverify.cdaf_Step;
	//}


	//SIZE image_size; 
	//dev->GetBufferInfo(m_bufferInfo);
	//image_size.cx = m_bufferInfo.nWidth;
	//image_size.cy = m_bufferInfo.nHeight;

	//GetWBROI(&otp_param.wb_param, &image_size, &sobel.roi);
	//sobel.down_step_cnt = 5;
	//sobel.down_sobel_th = 1.0;
	//SobelResult sobel_rest;

	//if (MTFFocus(dev,&sobel,m_bufferInfo.nWidth,m_bufferInfo.nHeight,&sobel_rest) < 0)
	//{
	//	ret = OTPCALI_ERROR_PDAF_DCC;
	//	goto out;
	//}
	//otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC = sobel_rest.focus_point;

	//Step 2.SPC enable
	
	int ConfidenceLevel;

	ret = do_SPC_writeback(SPCTable);
	if(ret < 0){
		uts.log.Error(_T("do_SPC_writeback error !"));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;
	}

	do_WriteDCCSetting3();
	Algorithm::Sleep(500);

	////get CDAF best pd
	//ret = do_SONY_DCC_stats(HOLD);
	//if(ret<0) { 
	//	ret = -ret;
	//	uts.log.Error(_T("pd inf hold on fail."));
	//	goto out;
	//}

	//Algorithm::Sleep(1000);
	//uts.log.Info(_T("pd inf hold on pass."));

	//ConfidenceLevel = do_SONY_GetPDTable(PDTable);
	//str_path.Format(_T("pdaf_data\\%s_pd_cdafpeak.txt"),uts.info.strSN);
	//do_save_buff(T2A(str_path), PDTable, pdaf_pd_len, 2);

	//ret = do_SONY_DCC_stats(OFF);
	//if(ret<0) { 
	//	ret = -ret;
	//	uts.log.Error(_T("pd inf hold off fail."));
	//	goto out;
	//}

	//Algorithm::Sleep(1000);
	//uts.log.Info(_T("pd inf hold off pass."));

	//step2:VCM Move mid of INF and mup
	int dacmid = (af.mup + af.inf) / 2;

	DLLSetVCM_Move(dev, uts.info.nVCMType, dacmid);
	Sleep(500);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		return -1;	
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	if (!uts.imgFile.SaveRawFile(_T("dcc_verify"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight)){
		uts.log.Error(_T("failed to save dcc_verify file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	ret = do_SONY_DCC_stats(HOLD);
	if(ret<0) { 
		ret = -ret;
		uts.log.Error(_T("pd inf hold on fail."));
		goto out;
	}

	Algorithm::Sleep(1000);
	uts.log.Info(_T("pd inf hold on pass."));

	ConfidenceLevel = do_SONY_GetPDTable(PDTable);
	str_path.Format(_T("pdaf_data\\%s_pd_verify.txt"),uts.info.strSN);
    do_save_buff(T2A(str_path), PDTable, pdaf_pd_len, 2);

	ret = do_SONY_DCC_stats(OFF);
	if(ret<0) { 
		ret = -ret;
		uts.log.Error(_T("pd inf hold off fail."));
		goto out;
	}

	Algorithm::Sleep(1000);
	uts.log.Info(_T("pd inf hold off pass."));

	uint16_t dcctable_apply[48];

	for (int i=0;i<48;i++)
	{
		dcctable_apply[i]=(DCCTable[i*2+1]<<8)+DCCTable[i*2+0];
	}

	str_path.Format(_T("pdaf_data\\%s_pdaf_dcc_verify.txt"),uts.info.strSN);
	do_save_buff(T2A(str_path), dcctable_apply, otp_dcc_len/2, 1);

	ret = do_SONY_DCCVerify(dcctable_apply,PDTable,DCCOUT,&pdaf_sony_tagetdac,0);
	uts.log.Info(_T("dcc verify done."));

	str_path.Format(_T("pdaf_data\\%s_pdaf_dccout.txt"),uts.info.strSN);
	do_save_buff(T2A(str_path), DCCOUT, pdaf_pd_len, 3);

	//step1:MTF Focus
	SobelParam sobel;
	if(otp_param.pdaf_param.pdafverify.cdaf_type==0){
		sobel.sta = af.inf;
		sobel.end = af.mup;
		sobel.step = otp_param.pdaf_param.pdafverify.cdaf_Step;
	}else{
		sobel.sta = dacmid + pdaf_sony_tagetdac-otp_param.pdaf_param.pdafverify.cdaf_serchoffset;
		sobel.end = dacmid + pdaf_sony_tagetdac-otp_param.pdaf_param.pdafverify.cdaf_serchoffset;
		sobel.step = otp_param.pdaf_param.pdafverify.cdaf_Step;
	}

	SIZE image_size; 
	dev->GetBufferInfo(m_bufferInfo);
	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	GetWBROI(&otp_param.wb_param, &image_size, &sobel.roi);
	sobel.down_step_cnt = 5;
	sobel.down_sobel_th = 1.0;
	SobelResult sobel_rest;

	if (MTFFocus(dev,&sobel,m_bufferInfo.nWidth,m_bufferInfo.nHeight,&sobel_rest) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		uts.log.Error(_T("cdaf find peak fail."));
		goto out;
	}

	//get CDAF best pd
	ret = do_SONY_DCC_stats(HOLD);
	if(ret<0) { 
		ret = -ret;
		uts.log.Error(_T("pd inf hold on fail."));
		goto out;
	}

	Algorithm::Sleep(1000);
	uts.log.Info(_T("pd inf hold on pass."));

	ConfidenceLevel = do_SONY_GetPDTable(PDTable);
	str_path.Format(_T("pdaf_data\\%s_pd_cdafpeak.txt"),uts.info.strSN);
	do_save_buff(T2A(str_path), PDTable, pdaf_pd_len, 2);

	ret = do_SONY_DCC_stats(OFF);
	if(ret<0) { 
		ret = -ret;
		uts.log.Error(_T("pd inf hold off fail."));
		goto out;
	}

	Algorithm::Sleep(1000);
	uts.log.Info(_T("pd inf hold off pass."));


	otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC            = sobel_rest.focus_point;
	otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC         = dacmid;
	otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC       = pdaf_sony_tagetdac;
	otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifySonyError = abs(otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC - 
		                                                            (otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC - 
																	 otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC));

	uts.log.Info(_T("SobelDAC         = %d."),otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC);
	uts.log.Info(_T("PDAFInitDAC      = %d."),otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC);
	uts.log.Info(_T("PDAFVerifyDAC    = %d."),otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC);
	uts.log.Info(_T("PDAFVerifyError  = %d."),otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifySonyError);
	uts.log.Info(_T("PDToleranceError = %d."),otp_param.pdaf_param.pdafverify.PDToleranceError);

	dac[0] = af.inf;
	dac[1] = af.mup;
	dac[2] = dacmid;
	dac[3] = otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC;
	dac[4] = pdaf_sony_tagetdac;
	dac[5] = otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifySonyError;
	dac[6] = otp_param.pdaf_param.pdafverify.PDToleranceError;

	str_path.Format(_T("pdaf_data\\%s_pdaf_dac.txt"),uts.info.strSN);
	do_save_buff(T2A(str_path), dac, 6, 3);

	//get PDAF best pd
	DLLSetVCM_Move(dev, uts.info.nVCMType, otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC + 
		                                   otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC);
	Sleep(500);

	ret = do_SONY_DCC_stats(HOLD);
	if(ret<0) { 
		ret = -ret;
		uts.log.Error(_T("pd inf hold on fail."));
		goto out;
	}

	Algorithm::Sleep(1000);
	uts.log.Info(_T("pd inf hold on pass."));

	ConfidenceLevel = do_SONY_GetPDTable(PDTable);
	str_path.Format(_T("pdaf_data\\%s_pd_pdafpeak.txt"),uts.info.strSN);
	do_save_buff(T2A(str_path), PDTable, pdaf_pd_len, 2);

	ret = do_SONY_DCC_stats(OFF);
	if(ret<0) { 
		ret = -ret;
		uts.log.Error(_T("pd inf hold off fail."));
		goto out;
	}

	Algorithm::Sleep(1000);
	uts.log.Info(_T("pd inf hold off pass."));

	//spec judge
	if(otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifySonyError >
	   otp_param.pdaf_param.pdafverify.PDToleranceError)
	{
		uts.log.Error(_T("DCC verify error !"));
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	ret = OTPCALI_ERROR_NO;

out:
    RELEASE_ARRAY(RAW10Image_input);
	RELEASE_ARRAY(PDTable);
	RELEASE_ARRAY(SPCTable);
	RELEASE_ARRAY(DCCTable);
	RELEASE_ARRAY(DCCOUT);


	return SET_ERROR(ret);
}
