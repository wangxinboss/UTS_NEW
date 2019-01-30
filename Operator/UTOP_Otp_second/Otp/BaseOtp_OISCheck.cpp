#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "Global_Memory_SFC.h"
#include <direct.h>


using namespace UTS::Algorithm;
using namespace UTS::Algorithm::Image;

void  BaseOtp::SaveImage(LPCTSTR lpName)
{
	CString strName;
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	//-------------------------------------------------------------------------
	// Êä³öImage file
	CString strFilePath;
	if (uts.info.nShopFlowEn != 0)
	{
		GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
		OSUtil::CreateMultipleDirectory(uts.info.strShopFlowFilePath);
		strFilePath.Format(_T("%s\\%s-%s-%s"),
			OSUtil::GetNoBSPath(uts.info.strShopFlowFilePath),
			uts.info.strSN,
			gmsfc->sSFC_ServerTime,
			strName);
	}
	else
	{
		CString strDirPath;
		strDirPath.Format(_T("%s\\Image-%s-%s-%s-%04d%02d%02d\\"),
			uts.info.strResultPath,
			uts.info.strProjectName,
			uts.info.strLineName,
			uts.info.strStationName,
			st.wYear,
			st.wMonth, 
			st.wDay);
		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		strFilePath.Format(_T("%s\\%s_%s_%02d%02d%02d"),
			strDirPath,
			m_szSN,
			lpName,
			st.wHour,
			st.wMinute, 
			st.wSecond);
	}

	if (uts.info.nSaveBmpFile != 0)
	{
		if (!uts.imgFile.SaveBmpFile(
			strFilePath,
			m_bufferObj.pBmpBuffer,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight))
		{
			CString strMsg;
			strMsg.Format(_T("SaveBmpFile Fail. Path = %s"), strFilePath);
			uts.log.Error(strMsg);
			AfxMessageBox(strMsg);
			return;
		}
		if (uts.info.nZipFile != 0)
		{
			OSUtil::ZipFile(strFilePath + ".bmp.rar", strFilePath + ".bmp");
		}
	}
}

void  BaseOtp::SaveRAWImage(BYTE *RAWBuffer,int width,int height,LPCTSTR lpName)
{
	CString strName;

	SYSTEMTIME st;
	GetLocalTime(&st);
	//-------------------------------------------------------------------------
	// Êä³öImage file
	CString strFilePath;
	if (uts.info.nShopFlowEn != 0)
	{
		GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
		OSUtil::CreateMultipleDirectory(uts.info.strShopFlowFilePath);
		strFilePath.Format(_T("%s\\%s-%s-%s"),
			OSUtil::GetNoBSPath(uts.info.strShopFlowFilePath),
			uts.info.strSN,
			gmsfc->sSFC_ServerTime,
			strName);
	}
	else
	{
		CString strDirPath;
		strDirPath.Format(_T("%s\\Image-%s-%s-%s-%04d%02d%02d\\"),
			uts.info.strResultPath,
			uts.info.strProjectName,
			uts.info.strLineName,
			uts.info.strStationName,
			st.wYear,
			st.wMonth, 
			st.wDay);
		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		strFilePath.Format(_T("%s\\%s_%s_%02d%02d%02d"),
			strDirPath,
			m_szSN,
			lpName,
			st.wHour,
			st.wMinute, 
			st.wSecond);
	}

	if (uts.info.nSaveRawFile != 0)
	{
		if (!uts.imgFile.SaveRawFile(
			strFilePath,
			RAWBuffer,
			width,
			height))
		{
			CString strMsg;
			strMsg.Format(_T("SaveBmpFile Fail. Path = %s"), strFilePath);
			uts.log.Error(strMsg);
			AfxMessageBox(strMsg);
			return;
		}
		if (uts.info.nZipFile != 0)
		{
			OSUtil::ZipFile(strFilePath + ".bmp.rar", strFilePath + ".bmp");
		}
	}
}


int BaseOtp::OISCheck(void *args)
{
	int ret = OTPCALI_ERROR_NO;
	UI_RECT rc;
	UI_MARK uiMark;
	CString _messagebuf;

	USES_CONVERSION;

	struct OIS_PAPRAM *oischeck = &otp_param.ois_param;

	dev->GetBufferInfo(m_bufferInfo);

	rc.color = COLOR_BLUE;
	memcpy(&rc.rcPos, &oischeck->OISROI, sizeof(RECT));
	uiMark.vecUiRect.push_back(rc);
	
	memset(&oischeck->oischeckResult,0,sizeof(oischeck->oischeckResult));

	do_OISOFF();

	Sleep(2000);

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
	
	int ROIWidth = oischeck->OISROI.right - oischeck->OISROI.left;
	int ROIHeight = oischeck->OISROI.bottom - oischeck->OISROI.top;

	unsigned char *_CenterBMPBuffer = new unsigned char[ROIWidth * ROIHeight*3];								 
	memset(_CenterBMPBuffer,0,sizeof(unsigned char)*(ROIWidth * ROIHeight)*3);
	
	unsigned char *_CenterGBuffer = new unsigned char[ROIWidth * ROIHeight];								 
	memset(_CenterGBuffer,0,sizeof(unsigned char)*(ROIWidth * ROIHeight));


	UTS::Algorithm::Image::CImageProc::GetInstance().GetBMPBlockBuffer(
		m_bufferObj.pBmpBuffer,
		_CenterBMPBuffer,
		m_bufferInfo.nWidth ,
		m_bufferInfo.nHeight,
		oischeck->OISROI.left,
		oischeck->OISROI.top,
		ROIWidth,
		ROIHeight);

    Algorithm::OISAnalysis m_OISAnalysis;
	
	double threshold;

	//Get threshold
	CImageProc::GetInstance().Cal_RGBtoGBuffer(
		_CenterBMPBuffer,
		ROIWidth,
		ROIHeight,
		_CenterGBuffer);

	m_OISAnalysis.GetImageThreshold(_CenterGBuffer,
									1,
									ROIWidth,
									ROIHeight,
									NULL,
									threshold,
									90,
									7);

	oischeck->nThresholdA = int (threshold - 20);
	oischeck->nThresholdB = int (threshold);
	uts.log.Debug(_T("thresholdA : %d thresholdB : %d"),oischeck->nThresholdA,oischeck->nThresholdB);

	//Get threshold

	m_OISAnalysis.SetStaticImage(
		_CenterBMPBuffer,
		ROIWidth,
		ROIHeight,
		oischeck->nROISize,
		oischeck->nThresholdA,
		oischeck->nThresholdB);
	
	SaveImage(_T("StaticImage"));

	char CommandData[256];
	int TestHz = 0;
	vector<int> vecOISCheck;
	SplitInt(oischeck->OISCheckHz, vecOISCheck);


	for (size_t i = 0 ; i< vecOISCheck.size() ; i++)
	{
		TestHz = vecOISCheck[i];
		if(oischeck->nAutoControlShaker == 1)  //Control Vibrator
		{
			if(TestHz <= 6) //1degree2Hz/1degree2Hz
			sprintf_s(CommandData,256,"1D0%XHZ",TestHz);
			else
			sprintf_s(CommandData,256,"0D5%XHZ",TestHz);

			VibratorControl(CommandData);
		}
		else if(oischeck->nAutoControlShaker == 0) //Show Message need User Control
		{
			_messagebuf.Format(_T("Please Turn on the Vibrator_%d Hz"),TestHz);
			::MessageBox(NULL,_messagebuf,_T("Note"),MB_OK);
		}else if(oischeck->nAutoControlShaker == 2) //EEA
		{
			if(EEAVibratorControl(TestHz) == false)
			{
				uts.log.Error(_T("EEAVibratorControl error."));
				ret = OTPCALI_ERROR_DATACONSIST;
				goto end;
			}
		}
		
		//Get OIS OFF image
		//Set Width/Height/ROISize/Threshold
		if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));

			if(!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
			{
				uts.log.Error(_T("m_pDevice->Recapture error again !!"));
				continue;
			}
		}
		dev->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

		Algorithm::Sleep(2000);

		UTS::Algorithm::Image::CImageProc::GetInstance().GetBMPBlockBuffer(
			m_bufferObj.pBmpBuffer,
			_CenterBMPBuffer,
			m_bufferInfo.nWidth ,
			m_bufferInfo.nHeight,
			oischeck->OISROI.left,
			oischeck->OISROI.top,
			ROIWidth,
			ROIHeight);

		//Get threshold
		/*
		CImageProc::GetInstance().Cal_RGBtoGBuffer(
			_CenterBMPBuffer,
			ROIWidth,
			ROIHeight,
			_CenterGBuffer);

		m_OISAnalysis.GetImageThreshold(_CenterGBuffer,
			1,
			ROIWidth,
			ROIHeight,
			NULL,
			threshold,
			90,
			7);

		m_OISAnalysis.Level_A_Threshold  = int (threshold - 20);
		m_OISAnalysis.Level_B_Threshold  = int (threshold);
		*/
		//----

		m_OISAnalysis.SetOISOFFImage(_CenterBMPBuffer,
			ROIWidth,
			ROIHeight,
			oischeck->nROISize);

		_messagebuf.Format(_T("OISOFFImage_%d Hz"),TestHz);
		SaveImage(_messagebuf);//Save image

		m_OISAnalysis.GetCircleRECT(oischeck->oischeckResult.Vib[i].OISOFFCircleRECT);

		//OIS ON 
		//OIS Enable
		do_OISON();

		Algorithm::Sleep(2000);
	
		//::MessageBoxW(NULL,_messagebuf,L"wait",MB_OK);
				
		if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));

			if(!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
			{
				uts.log.Error(_T("m_pDevice->Recapture error again !!"));
				goto end;
			}
		}

		dev->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
		UTS::Algorithm::Image::CImageProc::GetInstance().GetBMPBlockBuffer(
			m_bufferObj.pBmpBuffer,
			_CenterBMPBuffer,
			m_bufferInfo.nWidth ,
			m_bufferInfo.nHeight,
			oischeck->OISROI.left,
			oischeck->OISROI.top,
			ROIWidth,
			ROIHeight);

		//Get threshold
		/*
		CImageProc::GetInstance().Cal_RGBtoGBuffer(
			_CenterBMPBuffer,
			ROIWidth,
			ROIHeight,
			_CenterGBuffer);

		m_OISAnalysis.GetImageThreshold(_CenterGBuffer,
			1,
			ROIWidth,
			ROIHeight,
			NULL,
			threshold,
			90,
			7);

		m_OISAnalysis.Level_A_Threshold  = int (threshold - 20);
		m_OISAnalysis.Level_B_Threshold  = int (threshold);
		*/
		//---------------------------------------------------------

		m_OISAnalysis.Get_OIS_Status(_CenterBMPBuffer,	 
			oischeck->oischeckResult.Vib[i].StaticCircleWidth,oischeck->oischeckResult.Vib[i].StaticCircleHeight,
			oischeck->oischeckResult.Vib[i].OISOFFCircleWidth,oischeck->oischeckResult.Vib[i].OISOFFCircleHeight,
			oischeck->oischeckResult.Vib[i].DynamicCircleWidth,oischeck->oischeckResult.Vib[i].DynamicCircleHeight);

		m_OISAnalysis.GetCircleRECT(oischeck->oischeckResult.Vib[i].DynamicCircleRECT);

		if((oischeck->oischeckResult.Vib[i].StaticCircleWidth - oischeck->oischeckResult.Vib[i].DynamicCircleWidth == 1) ||
		   (oischeck->oischeckResult.Vib[i].StaticCircleWidth - oischeck->oischeckResult.Vib[i].DynamicCircleWidth == 2))
		{
			oischeck->oischeckResult.Vib[i].DynamicCircleWidth = oischeck->oischeckResult.Vib[i].StaticCircleWidth ; 
		}

		if((oischeck->oischeckResult.Vib[i].StaticCircleHeight - oischeck->oischeckResult.Vib[i].DynamicCircleHeight == 1) ||
		   (oischeck->oischeckResult.Vib[i].StaticCircleHeight - oischeck->oischeckResult.Vib[i].DynamicCircleHeight == 2))
		{
			oischeck->oischeckResult.Vib[i].DynamicCircleHeight = oischeck->oischeckResult.Vib[i].StaticCircleHeight ; 
		}

		oischeck->oischeckResult.Vib[i].ratio_x =  100.0- (double(oischeck->oischeckResult.Vib[i].DynamicCircleWidth - oischeck->oischeckResult.Vib[i].StaticCircleWidth+0.1)/
													  double(oischeck->oischeckResult.Vib[i].OISOFFCircleWidth  - oischeck->oischeckResult.Vib[i].StaticCircleWidth+0.1))*100.0;

		oischeck->oischeckResult.Vib[i].ratio_y =  100.0- (double(oischeck->oischeckResult.Vib[i].DynamicCircleHeight - oischeck->oischeckResult.Vib[i].StaticCircleHeight)/
													  double(oischeck->oischeckResult.Vib[i].OISOFFCircleHeight -  oischeck->oischeckResult.Vib[i].StaticCircleHeight))*100.0;

		if(oischeck->oischeckResult.Vib[i].DynamicCircleWidth == oischeck->oischeckResult.Vib[i].StaticCircleWidth)
		{
			oischeck->oischeckResult.Vib[i].DB_x =  -1* 20.0 * log10((abs(double(oischeck->oischeckResult.Vib[i].DynamicCircleWidth - oischeck->oischeckResult.Vib[i].StaticCircleWidth))+1.0)/
																	 (abs(double(oischeck->oischeckResult.Vib[i].OISOFFCircleWidth  - oischeck->oischeckResult.Vib[i].StaticCircleWidth))+1.0));

		}else
			oischeck->oischeckResult.Vib[i].DB_x =  -1* 20.0 * log10((abs(double(oischeck->oischeckResult.Vib[i].DynamicCircleWidth - oischeck->oischeckResult.Vib[i].StaticCircleWidth)))/
																	 (abs(double(oischeck->oischeckResult.Vib[i].OISOFFCircleWidth  - oischeck->oischeckResult.Vib[i].StaticCircleWidth))));

		if(oischeck->oischeckResult.Vib[i].DynamicCircleHeight == oischeck->oischeckResult.Vib[i].StaticCircleHeight)
		{
			oischeck->oischeckResult.Vib[i].DB_y =  -1*20.0 * log10((abs(double(oischeck->oischeckResult.Vib[i].DynamicCircleHeight - oischeck->oischeckResult.Vib[i].StaticCircleHeight))+1.0)/
																	(abs(double(oischeck->oischeckResult.Vib[i].OISOFFCircleHeight -  oischeck->oischeckResult.Vib[i].StaticCircleHeight))+1.0));

		}else
		{
			oischeck->oischeckResult.Vib[i].DB_y =  -1*20.0 * log10((abs(double(oischeck->oischeckResult.Vib[i].DynamicCircleHeight - oischeck->oischeckResult.Vib[i].StaticCircleHeight)))/
																	(abs(double(oischeck->oischeckResult.Vib[i].OISOFFCircleHeight -  oischeck->oischeckResult.Vib[i].StaticCircleHeight))));

		}



		uts.log.Debug(_T("X :%.2f  Y :%.2f "),oischeck->oischeckResult.Vib[i].DB_x,oischeck->oischeckResult.Vib[i].DB_y);


		if(oischeck->oischeckResult.Vib[i].DB_x < oischeck->dOIS_MinX_db ||
		   oischeck->oischeckResult.Vib[i].DB_y < oischeck->dOIS_MinY_db)
		{
			ret = OTPCALI_ERROR_OISCHECK;
			uts.log.Error(_T("DB: %.2f %.2f Spec: %.2f %.2f"),
				oischeck->oischeckResult.Vib[i].DB_x,
				oischeck->oischeckResult.Vib[i].DB_y,
				oischeck->dOIS_MinX_db,
				oischeck->dOIS_MinY_db);
		}

		uts.log.Debug(_T("Static Size W- %d H- %d "),oischeck->oischeckResult.Vib[i].StaticCircleWidth,
												     oischeck->oischeckResult.Vib[i].StaticCircleHeight);

		uts.log.Debug(_T("OISOFF Size W- %d H- %d "),oischeck->oischeckResult.Vib[i].OISOFFCircleWidth,
												     oischeck->oischeckResult.Vib[i].OISOFFCircleHeight);

		uts.log.Debug(_T("Dynamic Size W- %d H- %d "),oischeck->oischeckResult.Vib[i].DynamicCircleWidth,
												      oischeck->oischeckResult.Vib[i].DynamicCircleHeight);

		_messagebuf.Format(_T("OISONImage_%d Hz"),TestHz);
		SaveImage(_messagebuf);//Save image

		//OIS OFF
		do_OISOFF();

		//stop
		if(oischeck->nAutoControlShaker == 1)
		VibratorControl("STOP");
		else if(oischeck->nAutoControlShaker == 2) //EEA
		{
			if(EEAVibratorControl(0) == false)
			{
				uts.log.Error(_T("EEAVibratorControl error."));
				ret = OTPCALI_ERROR_DATACONSIST;
				goto end;
			}
		}
	}

end:
	if(oischeck->nAutoControlShaker == 0)
	::MessageBox(NULL,_T("Please Turn OFF the Vibrator"),_T("Note"),MB_OK);
	

	RELEASE_ARRAY(_CenterBMPBuffer);
	RELEASE_ARRAY(_CenterGBuffer);

	return SET_ERROR(ret);
}
