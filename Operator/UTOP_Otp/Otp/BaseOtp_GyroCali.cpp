#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "EEAInterface.h"
#include <direct.h>

#pragma comment(lib, "UTTL_EEAIF.lib")

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::Image;

bool BaseOtp::VibratorControl(char *CommmandData)
{
	bool res = false;
	char ReceiveData[256];
	USES_CONVERSION;

	if(uts.COM_RS232.GetComOpened() == TRUE)
	{
		uts.log.Debug(_T("Send : %s"),A2T(CommmandData));
		uts.COM_RS232.WriteSyncPort(CommmandData, strlen(CommmandData));

		memset(ReceiveData,0,sizeof(BYTE)*256);
		uts.COM_RS232.ReadSyncPort(ReceiveData, 256);

		uts.log.Debug(_T("Read: %s"),A2T(ReceiveData));

		/*if(ReceiveData[0] == '\0') //if not receive sleep long
			Algorithm::Sleep(4000);
		else
		*/
			Sleep(4000);
	}
	return res;
}

bool BaseOtp::EEAVibratorControl(int HZ)
{
	USES_CONVERSION;

	int station = atoi(T2A(uts.info.strLineName));
	int command = HZ;

	HWND hwnd = NULL;
	hwnd = ::FindWindow(_T("WindowsForms10.Window.8.app.0.2bf8098_r17_ad1"),
		_T("Form1"));

	if(hwnd == NULL)
	{
		return false;
	}

	//Set Vibator
	::PostMessage(hwnd,WM_USER + 102,station,command);

	int ruunning = 1 ;
	//Wait Vibator response
	EEA_WaitStartTest((eStation)(station), 10000, &ruunning);


	return true;
}



int BaseOtp::GyroCali(void *args)
{
	int ret = OTPCALI_ERROR_NO;
	UI_RECT rc;
	UI_MARK uiMark;
	Algorithm::OISAnalysis m_OISAnalysis;

	uts.board.ShowMsg(_T("OIS Cal..."));    

	struct OIS_PAPRAM *oiscal = &otp_param.ois_param;

	memset(&oiscal->oiscalResult,0,sizeof(oiscal->oiscalResult));
	dev->GetBufferInfo(m_bufferInfo);
	
	rc.color = COLOR_BLUE;
	memcpy(&rc.rcPos, &oiscal->OISROI, sizeof(RECT));
	uiMark.vecUiRect.push_back(rc);

	//
	//Sleep(2000);

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		if(!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error again !!"));
			goto end;
		}
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

	int ROIWidth = oiscal->OISROI.right - oiscal->OISROI.left;
	int ROIHeight = oiscal->OISROI.bottom - oiscal->OISROI.top;

	unsigned char *_CenterBMPBuffer = new unsigned char[ROIWidth * ROIHeight*3];								 
	memset(_CenterBMPBuffer,0,sizeof(unsigned char)*(ROIWidth * ROIHeight)*3);
	unsigned char *_CenterGBuffer = new unsigned char[ROIWidth * ROIHeight];								 
	memset(_CenterGBuffer,0,sizeof(unsigned char)*(ROIWidth * ROIHeight));

	UTS::Algorithm::Image::CImageProc::GetInstance().GetBMPBlockBuffer(
		m_bufferObj.pBmpBuffer,
		_CenterBMPBuffer,
		m_bufferInfo.nWidth ,
		m_bufferInfo.nHeight,
		oiscal->OISROI.left,
		oiscal->OISROI.top,
		ROIWidth,
		ROIHeight);

	
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

	oiscal->nThresholdA = int (threshold - 20);
	oiscal->nThresholdB = int (threshold);
	
	uts.log.Debug(_T("thresholdA : %d thresholdB : %d"),oiscal->nThresholdA,oiscal->nThresholdB);

	m_OISAnalysis.SetStaticImage(
		_CenterBMPBuffer,
		ROIWidth,
		ROIHeight,
		oiscal->nROISize,
		oiscal->nThresholdA,
		oiscal->nThresholdB);

	SaveImage(_T("StaticImage"));

	if(oiscal->nAutoControlShaker == 1) //Control Vibrator
	{
		//VibratorControl("1D06HZ");//1.0 degree 6HZ
		VibratorControl("1D04HZ");//1.0 degree 4HZ
	} else if(oiscal->nAutoControlShaker == 0) //Show Message need User Control
	{
		::MessageBox(NULL,_T("Please Turn on the Vibrator"),_T("Note"),MB_OK);
	}
	else if(oiscal->nAutoControlShaker == 2) //EEA
	{
		EEAVibratorControl(4);
	}

	//Static OFF
	//Get OIS OFF image
	//Set Width/Height/ROISize/Threshold
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
		oiscal->OISROI.left,
		oiscal->OISROI.top,
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
		oiscal->nROISize);


	double posX = 0 , posY = 0 , 
		x = oiscal->dGyroTurnStartX,
		y = oiscal->dGyroTurnStartY;

	double minX = ROIWidth;
	double minY = ROIHeight;

	//OIS Control:OIS ON1
	do_OISON();

	//Gyro gain initial value
	do_SetgyroX(oiscal->dGyroTurnStartX);
	do_SetgyroY(oiscal->dGyroTurnStartY);

	Algorithm::Sleep(1000);

	while(x <=  oiscal->dGyroTurnEndX  ||
		  y <=  oiscal->dGyroTurnEndY)
	{
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

		UTS::Algorithm::Image::CImageProc::GetInstance().GetBMPBlockBuffer(
			m_bufferObj.pBmpBuffer,
			_CenterBMPBuffer,
			m_bufferInfo.nWidth ,
			m_bufferInfo.nHeight,
			oiscal->OISROI.left,
			oiscal->OISROI.top,
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
		m_OISAnalysis.Get_OIS_Status(_CenterBMPBuffer,	 
			oiscal->oiscalResult.StaticCircleWidth,oiscal->oiscalResult.StaticCircleHeight,
			oiscal->oiscalResult.OISOFFCircleWidth,oiscal->oiscalResult.OISOFFCircleHeight,
			oiscal->oiscalResult.DynamicCircleWidth,oiscal->oiscalResult.DynamicCircleHeight);

		if( x < oiscal->dGyroTurnEndX )
		{
			x += oiscal->dGyroTurnGapX;
			oiscal->oiscalResult.PointCountX[0][oiscal->oiscalResult.countX] = x;
			oiscal->oiscalResult.PointCountX[1][oiscal->oiscalResult.countX] = abs(oiscal->oiscalResult.DynamicCircleWidth - 
																				   oiscal->oiscalResult.StaticCircleWidth);
			uts.log.Debug(_T("x= %.3f, SizeX[%d] : %.2f"),x,oiscal->oiscalResult.countX,
															oiscal->oiscalResult.PointCountX[1][oiscal->oiscalResult.countX]);

			//Find min gyro gain X
			if (oiscal->oiscalResult.PointCountX[1][oiscal->oiscalResult.countX] < minX)
			{
				minX = oiscal->oiscalResult.PointCountX[1][oiscal->oiscalResult.countX];
				posX = x;
			}

			oiscal->oiscalResult.countX++;
		}

		do_SetgyroX(x);

		if( y < oiscal->dGyroTurnEndY)
		{
			y += oiscal->dGyroTurnGapY;
			oiscal->oiscalResult.PointCountY[0][oiscal->oiscalResult.countY] = y;
			oiscal->oiscalResult.PointCountY[1][oiscal->oiscalResult.countY] = (float) abs(oiscal->oiscalResult.DynamicCircleHeight - 
																						   oiscal->oiscalResult.StaticCircleHeight); 

			uts.log.Debug(_T("y= %.3f, SizeY[%d] : %.2f"),y,oiscal->oiscalResult.countY,
															oiscal->oiscalResult.PointCountY[1][oiscal->oiscalResult.countY]);

			//Find min gyro gain Y
			if (oiscal->oiscalResult.PointCountY[1][oiscal->oiscalResult.countY] < minY)
			{
				minY = oiscal->oiscalResult.PointCountY[1][oiscal->oiscalResult.countY];
				posY = y;
			}

			oiscal->oiscalResult.countY++;
		}

		do_SetgyroY(y);

		Algorithm::Sleep(500);
	}

	//write the final result via I2C
	do_SetgyroX(posX);
	uts.log.Debug(_T("CalX ================================= = %.3f"),posX);
	oiscal->oiscalResult.uCalX = posX;

	//write the final result via I2C	
	do_SetgyroY(posY);
	uts.log.Debug(_T("CalY ================================= = %.3f"),posY);
	oiscal->oiscalResult.uCalY = posY;

	Algorithm::Sleep(1000);
	//////////////////////////////////////////////////////////////////////////
	//Get Calibration image 
	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		if(!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));
			ret = OTPCALI_ERROR_SENSOR;
			goto end;
		}
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

	UTS::Algorithm::Image::CImageProc::GetInstance().GetBMPBlockBuffer(
		m_bufferObj.pBmpBuffer,
		_CenterBMPBuffer,
		m_bufferInfo.nWidth ,
		m_bufferInfo.nHeight,
		oiscal->OISROI.left,
		oiscal->OISROI.top,
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
	//-------------------------------------
	m_OISAnalysis.Get_OIS_Status(_CenterBMPBuffer,	 
		oiscal->oiscalResult.StaticCircleWidth,oiscal->oiscalResult.StaticCircleHeight,
		oiscal->oiscalResult.OISOFFCircleWidth,oiscal->oiscalResult.OISOFFCircleHeight,
		oiscal->oiscalResult.DynamicCircleWidth,oiscal->oiscalResult.DynamicCircleHeight);

	if((oiscal->oiscalResult.StaticCircleWidth - oiscal->oiscalResult.DynamicCircleWidth == 1) ||
	   (oiscal->oiscalResult.StaticCircleWidth - oiscal->oiscalResult.DynamicCircleWidth == 2))
	{
		oiscal->oiscalResult.DynamicCircleWidth = oiscal->oiscalResult.StaticCircleWidth ; 
	}

	if((oiscal->oiscalResult.StaticCircleHeight - oiscal->oiscalResult.DynamicCircleHeight == 1) ||
	   (oiscal->oiscalResult.StaticCircleHeight - oiscal->oiscalResult.DynamicCircleHeight == 2))
	{
		oiscal->oiscalResult.DynamicCircleHeight = oiscal->oiscalResult.StaticCircleHeight ; 
	}

	oiscal->oiscalResult.ratio_x =  100.0- (double(oiscal->oiscalResult.DynamicCircleWidth - oiscal->oiscalResult.StaticCircleWidth +0.1 )/
		double(oiscal->oiscalResult.OISOFFCircleWidth  - oiscal->oiscalResult.StaticCircleWidth + 0.1))*100.0;

	oiscal->oiscalResult.ratio_y =  100.0- (double(oiscal->oiscalResult.DynamicCircleHeight - oiscal->oiscalResult.StaticCircleHeight +0.1)/
		double(oiscal->oiscalResult.OISOFFCircleHeight -  oiscal->oiscalResult.StaticCircleHeight + 0.1))*100.0;

	oiscal->oiscalResult.DB_x =  -1 * 20.0 * log10((abs(double(oiscal->oiscalResult.DynamicCircleWidth - oiscal->oiscalResult.StaticCircleWidth))+0.1)/
		(abs(double(oiscal->oiscalResult.OISOFFCircleWidth  - oiscal->oiscalResult.StaticCircleWidth))+0.1));
	oiscal->oiscalResult.DB_y =  -1 * 20.0 * log10((abs(double(oiscal->oiscalResult.DynamicCircleHeight - oiscal->oiscalResult.StaticCircleHeight))+0.1)/
		(abs(double(oiscal->oiscalResult.OISOFFCircleHeight -  oiscal->oiscalResult.StaticCircleHeight))+0.1));

	uts.log.Debug(_T("DB_x = %.3f"),oiscal->oiscalResult.DB_x);
	uts.log.Debug(_T("DB_y = %.3f"),oiscal->oiscalResult.DB_y);

	if(oiscal->oiscalResult.DB_x < oiscal->dOIS_MinX_db ||
	   oiscal->oiscalResult.DB_y < oiscal->dOIS_MinY_db)
	{
		ret = OTPCALI_ERROR_GYROCALI;
	}

	SaveImage(_T("FinalCalImage"));

	//////////////////////////////////////////////////////////////////////////
	//Save Gyro Gain
	do_SaveGyroGainData(posX,posY);


end:
	if(oiscal->nAutoControlShaker == 1)
	VibratorControl("STOP");//STOP
	else if(oiscal->nAutoControlShaker == 0)
	::MessageBox(NULL,_T("Please Turn OFF the Vibrator"),_T("Note"),MB_OK);
	else if(oiscal->nAutoControlShaker == 2) //EEA
	{
		EEAVibratorControl(4);
	}


	uts.board.ShowMsg(_T(" "));              // 取消提示

//	RELEASE_ARRAY(_CenterGBuffer);
	RELEASE_ARRAY(_CenterBMPBuffer);


	return SET_ERROR(ret);
}
