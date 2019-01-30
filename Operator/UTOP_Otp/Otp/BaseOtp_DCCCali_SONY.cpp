#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "VCM.h"
#include <direct.h>
//#include "PDAFCalibrationTools.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")

using namespace UTS::Algorithm::Image;

int BaseOtp::do_sony_dcc_cali()
{
	int ret = OTPCALI_ERROR_NO;

	USES_CONVERSION;

	dev->GetBufferInfo(m_bufferInfo);

	CString str_path;

	int *PDTable_INF             = new int[pdaf_pd_len];
	int *PDTable_Macro           = new int[pdaf_pd_len];
	uint8_t *SPCTable            = new uint8_t[otp_spc_len];
	uint16_t *DCCTable            = new uint16_t [otp_dcc_len/2];
	unsigned short *tempDCCTable = new unsigned short [otp_dcc_len];
	WORD *RAW10Image_input       = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

	memset(PDTable_INF,   0, pdaf_pd_len);
	memset(PDTable_Macro, 0, pdaf_pd_len);
	memset(SPCTable,      0, otp_spc_len);
	memset(DCCTable,      0, otp_dcc_len);
	memset(tempDCCTable,  0, otp_dcc_len);

	int ConfidenceLevel_INF,ConfidenceLevel_Macro;

	PDAF_PAPRAM *pwb = &otp_param.pdaf_param;

	DLLSetVCM_Initial(dev, uts.info.nVCMType, NULL);
	Algorithm::Sleep(500);

	AF_INT af;
	GetAFData(&af);
	uts.log.Info(_T("af.inf = %d ,af.mup = %d."),af.inf,af.mup);

	if (af.inf >= af.mup ){
		ret = OTPCALI_ERROR_PDAF_DCC;
		uts.log.Error(_T("af.inf = %d ,af.mup = %d."),af.inf,af.mup);
		goto out;
	}

	int Inf_LensPosition   = unsigned short((double(af.inf) + double(af.mup))/2.0 - ((double(af.mup) - double(af.inf))/2.0*0.8));
	int Macro_LensPosition = unsigned short((double(af.inf) + double(af.mup))/2.0 + ((double(af.mup) - double(af.inf))/2.0*0.8));

	//Step 1.SPC enable
	if (otpDB->GetOtpByType(mid, OTPDB_OTPTYPE_SPC, (char *)SPCTable,
		otp_spc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	str_path.Format(_T("pdaf_data\\%s_pdaf_spc.txt"),uts.info.strSN);
	do_save_buff(T2A(str_path),SPCTable,otp_spc_len);

	do_WriteSPCSetting2();
	Algorithm::Sleep(500);

	dev->GetBufferInfo(m_bufferInfo);
	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame)){
		uts.log.Error(_T("Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out; 
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);


	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	if (!uts.imgFile.SaveRawFile(_T("dcc_spc_before"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight)){
		uts.log.Error(_T("failed to save dcc_spc_before file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	do_SPC_writeback(SPCTable);
	Algorithm::Sleep(1000);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame)){
		uts.log.Error(_T("Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out; 
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	if (!uts.imgFile.SaveRawFile(_T("dcc_spc_after"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight)){
		uts.log.Error(_T("failed to save dcc_spc_after file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}	

	do_WriteDCCSetting3();

	//step2:VCM Move INF 
	DLLSetVCM_Move(dev, uts.info.nVCMType, Inf_LensPosition);
	Algorithm::Sleep(500);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out; 
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	if (!uts.imgFile.SaveRawFile(_T("dcc_inf"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight)){
		uts.log.Error(_T("failed to save dcc_inf file."));
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

	ConfidenceLevel_INF = do_SONY_GetPDTable(PDTable_INF);

	str_path.Format(_T("pdaf_data\\%s_pd_inf.txt"),uts.info.strSN);
	do_save_buff(T2A(str_path), PDTable_INF, pdaf_pd_len, 2);

	ret = do_SONY_DCC_stats(OFF);
	if(ret<0) { 
		ret = -ret;
		uts.log.Error(_T("pd inf hold off fail."));
		goto out;
	}

	Algorithm::Sleep(1000);
	uts.log.Info(_T("pd inf hold off pass."));

	//step2:VCM Move Macro
	DLLSetVCM_Move(dev, uts.info.nVCMType, Macro_LensPosition);
	Algorithm::Sleep(500);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame)){
		uts.log.Error(_T("Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out; 
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	if (!uts.imgFile.SaveRawFile(_T("dcc_macro"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight)){
		uts.log.Error(_T("failed to save dcc_macro file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	ret = do_SONY_DCC_stats(HOLD);
	if(ret<0) { 
		ret = -ret;
		uts.log.Error(_T("pd macro hold on fail."));
		goto out;
	}
	Algorithm::Sleep(1000);
	uts.log.Info(_T("pd macro hold on pass."));

	ConfidenceLevel_Macro = do_SONY_GetPDTable(PDTable_Macro);
	str_path.Format(_T("pdaf_data\\%s_pd_macro.txt"),uts.info.strSN);
	do_save_buff(T2A(str_path), PDTable_Macro, pdaf_pd_len, 2);

	ret = do_SONY_DCC_stats(OFF);
	if(ret<0) { 
		ret = -ret;
		uts.log.Error(_T("pd macro hold off fail."));
		goto out;
	}

	Algorithm::Sleep(1000);
	uts.log.Info(_T("pd macro hold off pass."));

	//Step3:
	ret = do_SONY_DCCCali(Inf_LensPosition,Macro_LensPosition,PDTable_INF,PDTable_Macro,DCCTable,0);
	str_path.Format(_T("pdaf_data\\%s_pdaf_dcc.txt"),uts.info.strSN);
	do_save_buff(T2A(str_path), DCCTable, otp_dcc_len/2, 1);
	uts.log.Info(_T("dcc Cali done"));

	if(ret < 0){
		uts.log.Error(_T("Dcc cali fail!"));
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out; 
	}

	if (otpDB->UpdateOtpByType(mid, OTPDB_OTPTYPE_DCC, (char *)DCCTable, otp_dcc_len) < 0)
	{
		uts.log.Error(_T("Failed to update coef data to DB!"));
		ret = OTPCALI_ERROR_DB;
		goto out; 
	}

	ret = OTPCALI_ERROR_NO;

out:
	RELEASE_ARRAY(RAW10Image_input);
	RELEASE_ARRAY(PDTable_INF);
	RELEASE_ARRAY(PDTable_Macro);
	RELEASE_ARRAY(SPCTable);
	RELEASE_ARRAY(DCCTable);
	RELEASE_ARRAY(tempDCCTable);

	return SET_ERROR(ret);
}