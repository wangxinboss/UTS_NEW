#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "Otp/Ref/MTK_PDAF/PDAF_MTK.h"
#include "VCM.h"

#include <direct.h>

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::FixPatternNoise::FixPatternNoise_WD;

void FlipImage(unsigned short *IutputBuffer,unsigned short *OutputBuffer,int BufferLength)
{
	for (int i = 0 ;i< BufferLength;i++)
	{
		OutputBuffer[BufferLength - 1 - i] = IutputBuffer[i];
	}
}

int BaseOtp::SPCCali(void *args)
{
	int ret = OTPCALI_ERROR_NO;
	uint8_t output[0x1200];
	double m_dYvalue;
	FPNInfo m_FPNInfo;
	memset(&m_FPNInfo, 0, sizeof(FPNInfo));
	
	memset(output,0,sizeof(uint8_t)*1200);
	dev->GetBufferInfo(m_bufferInfo);

	DLLSetVCM_Initial(dev, uts.info.nVCMType, NULL);
	Algorithm::Sleep(500);

	AF_INT af;
	GetAFData(&af);

	uts.log.Info(_T("af.inf = %d ,af.mup = %d."),af.inf,af.mup);
	if (af.inf >= af.mup|| af.inf < 0|| af.mup <0 ){
		ret = OTPCALI_ERROR_PDAF_SPC;
		uts.log.Error(_T("af.inf >af.mup."));
		return SET_ERROR(ret);	
	}

	uts.log.Debug(_T("move lens to midcode"));
	DLLSetVCM_Move(dev, uts.info.nVCMType, (af.inf+af.mup)/2);
	Sleep(1000);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		return SET_ERROR(ret);	
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	//检查Y值是否在设定区间
	UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
	if (m_dYvalue < otp_param.fpn_param.dltMinY || m_dYvalue > otp_param.fpn_param.dltMaxY)
	{
		uts.log.Error(_T("Yavg OUT OF RANGE"));
		return SET_ERROR(OTPCALI_ERROR_PDAF_SPC);
	}

	//check FPN
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.fpn_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.fpn_param.dFPNSpec)
	{
		uts.log.Error(_T("check FPN spec NG   m_dRowDifMax= %f    m_dColDifMax= %f    dThreshold=%f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax, otp_param.fpn_param.dFPNSpec);
		return SET_ERROR(OTPCALI_ERROR_PDAF_SPC);
	}

	//////////////////////////////////////////////////////////////////////////
	//Save otp_lsc
	CString strDirPath;
	CString strFilePath;
	SYSTEMTIME st;
	GetLocalTime(&st);

	strDirPath.Format(_T("%sImage-%s-%s-%s-%04d%02d%02d_SPCOTPData"),
		uts.info.strResultPath,
		uts.info.strProjectName,
		uts.info.strLineName,
		uts.info.strStationName,
		st.wYear,
		st.wMonth, 
		st.wDay);

	UTS::OSUtil::CreateMultipleDirectory(strDirPath);
	////////////////////////////////////////////////////////////////////////////////
	if (otp_param.nReCapture == 1)
	{
		//AWB RAW file name
		strFilePath.Format(_T("%s\\%s_OV13855_BGGR_%dx%d_RAW8_%04d%02d%02d%02d%02d%02d_SPCCapture"),
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
			uts.log.Error(_T("OV SPC: failed to save raw file!"));
			return SET_ERROR(OTPCALI_ERROR_PDAF_SPC);
		}

		if (!uts.imgFile.SaveBmpFile(strFilePath,(BYTE*)m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
		{
			uts.log.Error(_T("OV SPC: failed to save bmp file!"));
			return SET_ERROR(OTPCALI_ERROR_PDAF_SPC);
		}
	}

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	if (!uts.imgFile.SaveRawFile(_T("Capture_RAW10"),(BYTE*)RAW10Image,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T(" OV SPC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_PDAF_SPC);
	}

	otp_spc_len = do_SPCCali(RAW10Image,m_bufferInfo.nWidth,m_bufferInfo.nHeight,output,sizeof(output));
	if(otp_spc_len != 0)
	{
		//do_save_buff("Qul_Gain_Map.txt",output,otp_spc_len);
		if (otpDB->UpdateOtpByType(mid, OTPDB_OTPTYPE_SPC, (char*)output, otp_spc_len) < 0) {
			uts.log.Error(_T("Failed to update SPC data to DB!"));
			ret  = OTPCALI_ERROR_DB;
		}
	}else
	{
		ret = OTPCALI_ERROR_PDAF_SPC;
	}

	RELEASE_ARRAY(RAW10Image);

	return SET_ERROR(ret);
}

int BaseOtp::SONYSPCCali()
{
	int ret = OTPCALI_ERROR_NO;

	dev->GetBufferInfo(m_bufferInfo);

	short *output      = new short[otp_spc_len];
	uint8_t *SPCTable  = new uint8_t[otp_spc_len];
	int *SPCJudgeTable = new int[otp_spc_len+2];
	WORD *RAW10Image   = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

	memset(output,0,otp_spc_len);
	memset(SPCTable,0,otp_spc_len);
	memset(SPCJudgeTable,0,otp_spc_len+2);

	DLLSetVCM_Initial(dev, uts.info.nVCMType, NULL);
	Algorithm::Sleep(500);

	AF_INT af;
	GetAFData(&af);

	uts.log.Info(_T("af.inf = %d ,af.mup = %d."),af.inf,af.mup);
	if (af.inf >= af.mup|| af.inf < 0|| af.mup <0 ){
		ret = OTPCALI_ERROR_PDAF_SPC;
		uts.log.Error(_T("af.inf >af.mup."));
		goto end;
	}

	uts.log.Debug(_T("move lens to midcode"));
	DLLSetVCM_Move(dev, uts.info.nVCMType, (af.inf+af.mup)/2);
	Sleep(1000);

	//1.Write Setting 1
	do_WriteSPCSetting1();
	Algorithm::Sleep(200);


	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto end; 
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	//check FPN
	FPNInfo m_FPNInfo;
	FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
	if (m_FPNInfo.m_dRowDifMax > otp_param.fpn_param.dFPNSpec || m_FPNInfo.m_dColDifMax > otp_param.fpn_param.dFPNSpec)
	{
		uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.fpn_param.dFPNSpec);
		return uts.errorcode.E_FixPatternNoise;;
	}
	uts.log.Info(_T("check FPN : m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,otp_param.fpn_param.dFPNSpec);

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	if (!uts.imgFile.SaveRawFile(_T("spc_brfore"),(BYTE*)RAW10Image,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight)){
		uts.log.Error(_T("failed to save spc_brfore file."));
		ret = OTPCALI_ERROR_PDAF_SPC;
		goto end;
	}

	ret = do_SONY_SPCCali(RAW10Image,output);
	if(ret != 0x01) {
		uts.log.Error(_T("SONY SPC cali error!"));
		ret = OTPCALI_ERROR_PDAF_SPC;
		goto end; 
	}

	uts.log.Info(_T("SPC cali done"));
	for (int i =0;i<otp_spc_len ;i++){
		//	SPCTable[i] = 0x00;
		SPCTable[i] = (uint8_t)(output[i]&0xff);
	}

	//3.SPC table write back	
	ret = do_SPC_writeback(SPCTable);
	if(ret < 0){
		uts.log.Error(_T("do_SPC_writeback error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto end;
	}
	do_WriteSPCSetting2();
	Algorithm::Sleep(1000);
	uts.log.Info(_T("SPC writeback done"));

	//4.spc verify
	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto end; 
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	if (!uts.imgFile.SaveRawFile(_T("spc_after"),(BYTE*)RAW10Image,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("failed to save spc_after file."));
		ret = OTPCALI_ERROR_PDAF_SPC;
		goto end;
	}

	ret = do_SONY_SPCVerify(RAW10Image,SPCJudgeTable);
	if(ret>=0 && SPCJudgeTable[0] && SPCJudgeTable[1])
	{
		if (otpDB->UpdateOtpByType(mid, OTPDB_OTPTYPE_SPC, (char *)SPCTable, otp_spc_len) < 0) 
		{
			uts.log.Error(_T("Failed to update SPC data to DB!"));
			ret = OTPCALI_ERROR_DB;
			goto end; 
		}
		uts.log.Info(_T("SPC Verify done"));
		ret = OTPCALI_ERROR_NO;
		do_save_buff("spc_map.txt",SPCTable,otp_spc_len);
	}else
	{
		uts.log.Error(_T("SONY SPC verify error!"));
		ret = OTPCALI_ERROR_PDAF_SPC;
		goto end; 
	}

end:
	RELEASE_ARRAY(output);
	RELEASE_ARRAY(SPCTable);
	RELEASE_ARRAY(SPCJudgeTable);
	RELEASE_ARRAY(RAW10Image);

	return SET_ERROR(ret);
}

int BaseOtp::mtk_spc_cali(uint16_t *pRaw10,char *filename, uint8_t out[])
{
	int ret = OTPCALI_ERROR_NO;
	uint8_t *SPCTable;

	// ============================================================
	// pdaf_init - set the ini file and get capture num, max output size
	// ============================================================
	int Res = 0;
	int cap_num = -1;
	int max_size = -1;
	Res |= pdaf_init(filename, cap_num, max_size);
	uts.log.Debug(_T("Pdaf_init : %d ,Cap_num : %d ,Max_Size : %d"),Res,cap_num,max_size);

	if(Res != 0)
	{
		ret = OTPCALI_ERROR_PDAF_SPC;
		goto end;
	}

	SPCTable = new unsigned char[max_size];
	// ============================================================
	// pdaf_spc
	// ============================================================
	int SPC_Size;
	Res |= pdaf_spc(pRaw10, SPC_Size, SPCTable);
	uts.log.Debug(_T("Pdaf_spc : %d OutSize : %d"),Res,SPC_Size);

	// ============================================================
	// spc_verify
	// ============================================================
	Res |= pdaf_verify_spc(SPCTable);
	uts.log.Debug(_T("pdaf_verify_spc : %d"),Res);

	if(Res != 0)
	{
		uts.log.Error(_T("SONY SPC cali error!"));
		ret = OTPCALI_ERROR_PDAF_SPC;
	}else
	{
		memcpy(out, SPCTable, SPC_Size);
	}


end:
	RELEASE_ARRAY(SPCTable);

	return SPC_Size;
}

int BaseOtp::SPCCali_2PD()
{
	int ret = OTPCALI_ERROR_NO;
	uint8_t output[0x1200];

	memset(output,0,sizeof(uint8_t)*1200);
	dev->GetBufferInfo(m_bufferInfo);

	AF_INT af;
	GetAFData(&af);
	if (af.inf < 0 || af.mup < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		uts.log.Error(_T("no AF data"));
	}

	DLLSetVCM_Initial(dev,uts.info.nVCMType,NULL);
	DLLSetVCM_Move(dev, uts.info.nVCMType, ((af.inf + af.mup)/2));
	Sleep(300);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		return SET_ERROR(ret);	
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*4];
	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth*2, m_bufferInfo.nHeight);

	if (!uts.imgFile.SaveRawFile(_T("Capture_RAW10"),(BYTE*)RAW10Image,m_bufferInfo.nWidth*4,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("TK LSC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	otp_spc_len = do_SPCCali(RAW10Image,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight,output,sizeof(output));
	if(otp_spc_len != 0)
	{
		if (otpDB->UpdateOtpByType(mid, OTPDB_OTPTYPE_SPC, (char*)output, otp_spc_len) < 0) {
			uts.log.Error(_T("Failed to update SPC data to DB!"));
			ret  = OTPCALI_ERROR_DB;
		}
	}else
	{
		ret = OTPCALI_ERROR_PDAF_SPC;
	}

	RELEASE_ARRAY(RAW10Image);

	return SET_ERROR(ret);
}
