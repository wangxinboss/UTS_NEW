#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "VCM.h"
#include <direct.h>

#include "Otp/Ref/MTK_PDAF/PDAF_MTK.h"

#pragma comment(lib, "UTSAlgorithm.lib")

using namespace UTS::Algorithm::Image;

int BaseOtp::mtk_dcc_cali(char *filename)
{
	int ret = OTPCALI_ERROR_NO;
	uint8_t *DCCTable;

	int Lens_pos[10] = {0};
	unsigned short* RAW10Image[10] ;

	AF_INT af;
	GetAFData(&af);
	if (af.inf < 0 || af.mup < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
	}

	//VCM Move INF 
	DLLSetVCM_Move(dev, uts.info.nVCMType, af.inf);
	Sleep(300);
	
	dev->GetBufferInfo(m_bufferInfo);

	for (int i = 0; i < 10 ; i++)
	{
		Lens_pos[i] = af.inf + int(((af.mup - af.inf))/9.0 * (i));
		uts.log.Debug(_T("Lens : %d "),Lens_pos[i]);

		RAW10Image[i] = new unsigned short[m_bufferInfo.nWidth *m_bufferInfo.nHeight* 2];
		memset(RAW10Image[i],0,sizeof(unsigned short) * m_bufferInfo.nWidth *this-> m_bufferInfo.nHeight * 2);
		
		DLLSetVCM_Move(dev, uts.info.nVCMType, Lens_pos[i]);
		Sleep(300);
		dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame);
		CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image[i],m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	}

	// ============================================================
	// pdaf_init - set the ini file and get capture num, max output size
	// ============================================================
	int Res = 0;
	int cap_num = -1;
	int max_size = -1;
	Res |= pdaf_init(filename, cap_num, max_size);
	uts.log.Debug(_T("Pdaf_init : %d ,Cap_num : %d ,Max_Size : %d"),Res,cap_num,max_size);
	DCCTable = new unsigned char[max_size];
	
	// ============================================================
	// pdaf_dcc
	// ============================================================
	int DCC_Size;
	// pdaf_dcc15cm
	//Res |=  pdaf_dcc((char**)RAW10Image, Lens_pos, DCC_Size, DCCTable);


	//
	uint8_t *SPCTable = new uint8_t[otp_spc_len];

	if (otpDB->GetOtpByType(mid, 12, (char *)SPCTable,
		otp_spc_len) < 0)
	{
		ret = OTPCALI_ERROR_PDAF_DCC;
		goto end;
	}

	Res |=  pdaf_dcc((char**)RAW10Image, Lens_pos,(char*)SPCTable, DCC_Size, DCCTable);
	uts.log.Debug(_T("Pdaf_spc : %d OutSize : %d"),Res,DCC_Size);

	// ============================================================
	// DCC_verify
	// ============================================================
	Res |= pdaf_verify_dcc(DCCTable);
	uts.log.Debug(_T("Verify pdaf_dcc : %d"),Res) ;

	if(Res != 0)
	{
		uts.log.Error(_T("SONY SPC cali error!"));
		ret = OTPCALI_ERROR_PDAF_SPC;
		goto end; 
	}else
	{
		if (otpDB->UpdateOtpByType(mid, 8, (char*)DCCTable, DCC_Size) < 0) 
		{
			uts.log.Error(_T("Failed to update DCC data to DB!"));
			goto end; 
		}
	}

end:
	for (int i = 0; i < 10 ; i++)
	{
		RELEASE_ARRAY(RAW10Image[i]);
	}

	RELEASE_ARRAY(DCCTable);
	RELEASE_ARRAY(SPCTable);

	return SET_ERROR(ret);
}