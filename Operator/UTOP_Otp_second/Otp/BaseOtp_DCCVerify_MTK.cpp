#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "VCM.h"
#include <direct.h>
#include "Focus_MTF.h"
#include "Otp/Ref/MTK_PDAF/PDAF_MTK.h"

#pragma comment(lib, "UTSAlgorithm.lib")

using namespace UTS::Algorithm::Image;

int BaseOtp::mtk_dcc_verify(char *filename)
{
	int ret = OTPCALI_ERROR_NO;
	
	dev->GetBufferInfo(m_bufferInfo);

	// ============================================================
	// pdaf_init - set the ini file and get capture num, max output size
	// ============================================================
	int Res = 0;
	int cap_num = -1;
	int max_size = -1;
	Res |= pdaf_init(filename, cap_num, max_size);
	uts.log.Debug(_T("Pdaf_init : %d ,Cap_num : %d ,Max_Size : %d"),Res,cap_num,max_size);

	uint8_t *SPCTable = new uint8_t[otp_spc_len];
	uint8_t *DCCTable = new uint8_t[otp_dcc_len];

	//Get AF
	AF_INT af;
	GetAFData(&af);
	if (af.inf < 0 || af.mup < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto end;
	}
	uts.log.Debug(_T("AF inf:%d macro:%d"),af.inf,af.mup);

	//Get SPC
	if (otpDB->GetOtpByType(mid, OTPDB_OTPTYPE_SPC, (char *)SPCTable,
		otp_spc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto end;
	}

	//Get DCC
	if (otpDB->GetOtpByType(mid, 8, (char *)DCCTable,
		otp_dcc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto end;
	}

	//MTF Focus
	SobelParam sobel;
	sobel.sta = af.inf;
	sobel.end = af.mup;
	sobel.step = 4;

	SIZE image_size; 
	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	GetWBROI(&otp_param.wb_param, &image_size, &sobel.roi);
	sobel.down_step_cnt = 5;
	sobel.down_sobel_th = 4.0;
	SobelResult sobel_rest;

	if (MTFFocus(dev,&sobel,m_bufferInfo.nWidth,m_bufferInfo.nHeight,&sobel_rest) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto end;
	}
	otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC = sobel_rest.focus_point;

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto end; 
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	int dacmid = (af.mup + af.inf) / 2;
	otp_param.pdaf_param.pdafverify.pdafResult.PDAFInitDAC = dacmid;

	DLLSetVCM_Move(dev, uts.info.nVCMType,dacmid);
	Algorithm::Sleep(500);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto end; 
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	pdaf_get_DAC_dcc(RAW10Image,SPCTable,DCCTable,dacmid ,otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC);

	otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyError = abs(otp_param.pdaf_param.pdafverify.pdafResult.SobelDAC - 
																	(dacmid - otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyDAC));
	if(otp_param.pdaf_param.pdafverify.pdafResult.PDAFVerifyError >
	   otp_param.pdaf_param.pdafverify.PDToleranceError)
	{
		uts.log.Error(_T("DCC verify error !"));
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto end;
	}

	// ============================================================
	// MTK Linearity Test
	// ============================================================
	int LinearGridX,LinearGridY;
	pdaf_get_LinearitySize(DCCTable,LinearGridX,LinearGridY);

	double *LinearTable = new double[LinearGridX*LinearGridX];
	pdaf_get_Linearity(DCCTable,LinearTable);

	otp_param.pdaf_param.pdafverify.pdafResult.PDAFMTKLinearity = (LinearTable[ 2*LinearGridY+ 2 ] +
		                                                           LinearTable[ 2*LinearGridY+ 3 ] + 
																   LinearTable[ 3*LinearGridY+ 2 ] + 
																   LinearTable[ 3*LinearGridY+ 3 ] )/4.0 ;
	RELEASE_ARRAY(LinearTable);

	// ============================================================
	// Save Bin
	// ============================================================
	{
		CFile fp2;
		CString strDirPath;
		CString strFilePath;

		SYSTEMTIME st;
		GetLocalTime(&st);

		strDirPath.Format(_T("%s\\Data-%s-%s-%s-%04d%02d%02d_DumpBin\\"),
			uts.info.strResultPath,
			uts.info.strProjectName,
			uts.info.strLineName,
			uts.info.strStationName,
			st.wYear,
			st.wMonth, 
			st.wDay);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		strFilePath.Format(_T("%s\\%s.bin"),
			strDirPath,
			uts.info.strSN);

		fp2.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		fp2.Write(DCCTable, otp_dcc_len);
		fp2.Close();
	}
	//-------------------------------------------------------------
end:
	RELEASE_ARRAY(SPCTable);
	RELEASE_ARRAY(DCCTable);
	RELEASE_ARRAY(RAW10Image);

	return SET_ERROR(ret);
}