#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "VCM.h"
#include <direct.h>

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")

using namespace UTS::Algorithm::Image;

int BaseOtp::get_qulcomm_pd(uint8_t *spctable,int type, int inf, int mup, float per)
{
	dev->GetBufferInfo(m_bufferInfo);
	int framesize = m_bufferInfo.nWidth * m_bufferInfo.nHeight;

	int mid = (mup + inf) / 2;
	int dac80 = int((mid - inf) * per + 0.5);
	int negative_dac = mid - dac80;
	int positive_dac = mid + dac80;

	WORD * raw_negative,*raw_positive;
	raw_negative = (WORD *)calloc(framesize, sizeof(WORD));
	raw_positive = (WORD *)calloc(framesize, sizeof(WORD));

	DLLSetVCM_Move(dev, uts.info.nVCMType, negative_dac);
	Sleep(300);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		return -1;	
	}
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,raw_negative,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	DLLSetVCM_Move(dev, uts.info.nVCMType, positive_dac);
	Sleep(300);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		return -1;	
	}
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,raw_positive,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	int pd = do_qualcomm_DCCCali(spctable,raw_negative, raw_positive,
		m_bufferInfo.nWidth, m_bufferInfo.nHeight, 
		negative_dac, positive_dac);

	free(raw_negative);
	free(raw_positive);

	if (pd == -1)
	{
		uts.log.Error(_T("PD error !"));
	}

	uts.log.Debug(_T("Phase Difference Conversion Coefficient = %d"), pd);

	return pd;
}


int BaseOtp::do_qulcomm_dcc_cali(int dcctype)
{
	int ret = OTPCALI_ERROR_NO;
	PDAF_PAPRAM *pwb = &otp_param.pdaf_param;

	AF_INT af;
	GetAFData(&af);
	if (af.inf < 0 || af.mup < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
	}

	uint8_t SPCTable[0x1200];
	
	if (otpDB->GetOtpByType(mid, OTPDB_OTPTYPE_SPC, (char *)SPCTable,
		otp_spc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}
	
	pwb->pdafcalResult.PD1 = get_qulcomm_pd(SPCTable,dcctype, af.inf, af.mup, 0.8f);
	pwb->pdafcalResult.PD2 = get_qulcomm_pd(SPCTable,dcctype, af.inf, af.mup, 0.6f);

	if (pwb->pdafcalResult.PD1 <= 0 || pwb->pdafcalResult.PD2 <= 0)
	{
		uts.log.Error(_T("Failed to get_coef, coef0[%d], coef1[%d]!"), pwb->pdafcalResult.PD1, pwb->pdafcalResult.PD2);
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto out;
	}

	u8 tmp[4];
	put_le_val(pwb->pdafcalResult.PD1, tmp + 0, 2);
	put_le_val(pwb->pdafcalResult.PD2, tmp + 2, 2);

	int type = dcctype == 0 ? 8 : 9;
	if (otpDB->UpdateOtpByType(mid, type, (char *)tmp, sizeof(tmp)) < 0)
	{
		uts.log.Error(_T("Failed to update coef data to DB!"));
		ret = OTPCALI_ERROR_DB;
	}
	
out:
	return SET_ERROR(ret);
}

int BaseOtp::get_qulcomm_pd_REVL()
{
	int ret = OTPCALI_ERROR_NO;
	PDAF_PAPRAM *pwb = &otp_param.pdaf_param;

	AF_INT af;
	GetAFData(&af);

// 	af.inf = 327;
// 	af.mup = 548;

	if (af.inf < 0 || af.mup < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		uts.log.Error(_T("no AF data"));
	}

	uint8_t SPCTable[0x1200];
	uint8_t DCCTable[0x1200];
	int16_t Lens_pos[10] = {0};
	unsigned short* RAW10Image[10] ;

	if (otpDB->GetOtpByType(mid, OTPDB_OTPTYPE_SPC, (char *)SPCTable,
		otp_spc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		return SET_ERROR(ret);
		//goto out;
	}

	//VCM Move INF 
	DLLSetVCM_Initial(dev,uts.info.nVCMType, NULL);
	DLLSetVCM_Move(dev, uts.info.nVCMType, af.inf);
	Sleep(300);

	dev->GetBufferInfo(m_bufferInfo);

	CString filename;

	for (int i = 0; i < 10 ; i++)
	{
		Lens_pos[i] = af.inf + int(((af.mup - af.inf))/9.0 * (i));
		uts.log.Debug(_T("Lens : %d "),Lens_pos[i]);

		RAW10Image[i] = new unsigned short[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
		memset(RAW10Image[i],0,sizeof(unsigned short) * m_bufferInfo.nWidth* m_bufferInfo.nHeight);

		DLLSetVCM_Move(dev, uts.info.nVCMType, Lens_pos[i]);
		Sleep(300);
		dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame);
		CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image[i],m_bufferInfo.nWidth, m_bufferInfo.nHeight);
		dev->DisplayImage(m_bufferObj.pBmpBuffer);

// 		filename.Format(_T("%d"),i);
// 		if (!uts.imgFile.SaveRawFile(filename,(BYTE*)RAW10Image[i],m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
// 		{
// 			uts.log.Error(_T("TK LSC: failed to save raw file!"));
// 			return SET_ERROR(OTPCALI_ERROR_LSCCALI);
// 		}
	}

	otp_dcc_len = do_DCCCali(SPCTable,RAW10Image, (int16_t*)Lens_pos, DCCTable);

	if(otp_dcc_len != 0)
	{
		if (otpDB->UpdateOtpByType(mid, 8, (char *)DCCTable, otp_dcc_len) < 0)
		{
			uts.log.Error(_T("Failed to update coef data to DB!"));
			ret = OTPCALI_ERROR_DB;
		}
	}else
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
	}

	for (int i = 0; i < 10 ; i++)
	{
		RELEASE_ARRAY(RAW10Image[i]);
	}

	return SET_ERROR(ret);
}

int BaseOtp::get_qulcomm_2pd_REVL()
{
	int ret = OTPCALI_ERROR_NO;
	PDAF_PAPRAM *pwb = &otp_param.pdaf_param;

	AF_INT af;
	GetAFData(&af);
	if (af.inf < 0 || af.mup < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		uts.log.Error(_T("no AF data"));
	}

	uint8_t SPCTable[0x1200];
	uint8_t DCCTable[0x1200];
	int16_t Lens_pos[10] = {0};
	unsigned short* RAW10Image[10] ;

	if (otpDB->GetOtpByType(mid, OTPDB_OTPTYPE_SPC, (char *)SPCTable,
		otp_spc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		return SET_ERROR(ret);
		//goto out;
	}

	//VCM Move INF 
	DLLSetVCM_Initial(dev,uts.info.nVCMType, NULL);
	DLLSetVCM_Move(dev, uts.info.nVCMType, af.inf);
	Sleep(300);

	dev->GetBufferInfo(m_bufferInfo);

	for (int i = 0; i < 10 ; i++)
	{
		Lens_pos[i] = af.inf + int(((af.mup - af.inf))/9.0 * (i));
		uts.log.Debug(_T("Lens : %d "),Lens_pos[i]);

		RAW10Image[i] = new unsigned short[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
		memset(RAW10Image[i],0,sizeof(unsigned short) * m_bufferInfo.nWidth* m_bufferInfo.nHeight * 2);

		DLLSetVCM_Move(dev, uts.info.nVCMType, Lens_pos[i]);
		Sleep(300);
		dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame);
		CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image[i],m_bufferInfo.nWidth*2, m_bufferInfo.nHeight);
		dev->DisplayImage(m_bufferObj.pBmpBuffer);

		// 		if (!uts.imgFile.SaveRawFile(_T("Capture_RAW10"),(BYTE*)RAW10Image[i],m_bufferInfo.nWidth*4,m_bufferInfo.nHeight))
		// 		{
		// 			uts.log.Error(_T("TK LSC: failed to save raw file!"));
		// 			return SET_ERROR(OTPCALI_ERROR_LSCCALI);
		// 		}
	}

	otp_dcc_len = do_DCCCali(SPCTable,RAW10Image, (int16_t*)Lens_pos, DCCTable);

	if(otp_dcc_len != 0)
	{
		if (otpDB->UpdateOtpByType(mid, 8, (char *)DCCTable, otp_dcc_len) < 0)
		{
			uts.log.Error(_T("Failed to update coef data to DB!"));
			ret = OTPCALI_ERROR_DB;
		}
	}else
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
	}

	for (int i = 0; i < 10 ; i++)
	{
		RELEASE_ARRAY(RAW10Image[i]);
	}

	return SET_ERROR(ret);
}