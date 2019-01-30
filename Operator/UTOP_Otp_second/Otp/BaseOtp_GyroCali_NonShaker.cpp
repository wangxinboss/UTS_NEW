#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"

#include <direct.h>
//Ryan@20160506

#include "MeasurementLibrary.h"
#pragma comment(lib,"MeasurementLibrary.lib")

#define AlphaYaw 0.000711452
#define AlphaPitch 0.000718207
#define PixelsSize 1.0

float From2sComplementToFloat(UINT32 u32HEX)
{
	double fResult = 0.0;
	if((u32HEX >= 0x00000000) && (u32HEX <0x80000000))
		fResult = ((double)u32HEX / (double)0x7FFFFFFF);
	else if((u32HEX >= 0x80000000) && (u32HEX <=0xFFFFFFFF))
		fResult = 0 - ((double) (u32HEX ^ 0xFFFFFFFF) / (double)0x7FFFFFFF);

	return (float)fResult;
}


UINT32 FromFloatTo2sComplement2(float a_fData)
{
	UINT64 u64Result = 0x7FFFFFFF;
	double dTemp =0; 

	if(a_fData >= 0)
		dTemp = 0x7FFFFFFF * a_fData;
	else
		dTemp = ((DWORD)(0x7FFFFFFF * (-a_fData)) ^ 0xFFFFFFFF) +1;

	u64Result = (UINT32) dTemp & 0xFFFFFFFF;

	return (UINT32)u64Result;
}

void  BaseOtp::SaveImage2(LPCTSTR lpName)
{
	CString strName;

	SYSTEMTIME st;
	GetLocalTime(&st);
	//-------------------------------------------------------------------------
	// 输出Image file
	CString strFilePath;
	
	CString strDirPath;
	strDirPath.Format(_T("%s\\CA188_TestImage\\"),
		uts.info.strResultPath);

	UTS::OSUtil::CreateMultipleDirectory(strDirPath);
	strFilePath.Format(_T("%s\\%s"),
		strDirPath,
		lpName);
	
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
}


//Ryan@20160506
int BaseOtp::GyroCali_NonShaker(void *args)
{
	int ret = OTPCALI_ERROR_NO;

	CString fname_Slt, fname_Vrt, fname_Hrz;
	UINT	m_ImageBias, m_ImageBias2,m_Offset;
	DOUBLE	m_Distance, m_dblSpace;
	float	m_fltCodeMin, m_fltCodeMax;
	RECT	m_clipping;

	mlMeasurementData *slt = NULL;
	mlMeasurementData *hrz = NULL;
	mlMeasurementData *vrt = NULL;

	mlErrorCode m_mlError = ML_OK;

	dev->GetBufferInfo(m_bufferInfo);

	m_Offset = 1536;
	m_Distance = 20/10.0;
	m_dblSpace = 6/10.0;
	m_ImageBias = 0;
	m_ImageBias2 = 0;
	m_fltCodeMin = (float)otp_param.ois_param.dfltCodeMin;
	m_fltCodeMax = (float)otp_param.ois_param.dfltCodeMax;
	m_clipping.left = 0;
	m_clipping.top  = 0;
	m_clipping.bottom = m_bufferInfo.nHeight;
	m_clipping.right = m_bufferInfo.nWidth;

	//Just for Shaker Calibration Mapping 
	float fix = 0.0;
	UINT32 Gain = 0;
	do_GetGyroGainX(&Gain);
	fix = From2sComplementToFloat(Gain);
	otp_param.ois_param.oiscal_NonShakerResult.RealGainX = (double)fix;

	Gain = FromFloatTo2sComplement2(fix);
	do_GetGyroGainY(&Gain);
	fix = From2sComplementToFloat(Gain);
	otp_param.ois_param.oiscal_NonShakerResult.RealGainY = (double)fix;
	
	uts.log.Debug(_T("GainX = %.3f GainY = %.3f"),otp_param.ois_param.oiscal_NonShakerResult.RealGainX
											     ,otp_param.ois_param.oiscal_NonShakerResult.RealGainY);

	//Capture images start =============================================================================
	//TneSltPos(n), ╃弊, 1~7	
	USES_CONVERSION;
	uts.log.Debug(_T("Capture Slt images.."));

	for (int i = 1; i < 8; i++)
	{
		do_SetSltPos(i);

		Algorithm::Sleep(200);

		if(!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error !!"));
			//goto end;
		}
		dev->DisplayImage(m_bufferObj.pBmpBuffer);

		fname_Slt.Format(_T("slt_%02d"),i);
		SaveImage2(fname_Slt);
	}

	//TneVrtPos(n), ╃, 1~7
	uts.log.Debug(_T("Capture Vrt images.."));
	for (int i = 1; i < 8; i++)
	{
		do_SetVrtPos(i);

		Algorithm::Sleep(200);

		if(!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error !!"));
			//goto end;
		}
		dev->DisplayImage(m_bufferObj.pBmpBuffer);

		fname_Vrt.Format(_T("vrt_%02d"),i);
		SaveImage2(fname_Vrt);
	}

	//TneHrzPos(n), ╃钧坪, 1~7
	uts.log.Debug(_T("Capture Hrz images.."));
	for (int i = 1; i < 8; i++)
	{
		do_SetHrzPos(i);

		Algorithm::Sleep(200);

		if(!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error !!"));
			//goto end;
		}
		dev->DisplayImage(m_bufferObj.pBmpBuffer);

		fname_Hrz.Format(_T("hrz_%02d"),i);
		SaveImage2(fname_Hrz);
	}
	
	//Capture images end ===============================================================================
	uts.log.Debug(_T("Check SharpMark.."));

	try
	{
		//Check ShapeMark images
		if( !PathFileExists(_T("..\\Result\\CA188_ShapeMark\\Mark_TOP.bmp")))
		{
			uts.log.Error(_T("ShapeMark_Top not found"));
			throw( ML_ERROR );
		}
		if( !PathFileExists(_T("..\\Result\\CA188_ShapeMark\\Mark_LEFT.bmp")))
		{
			uts.log.Error(_T("ShapeMark_Left not found"));
			throw( ML_ERROR );
		}
		if( !PathFileExists(_T("..\\Result\\CA188_ShapeMark\\Mark_RIGHT.bmp")))
		{
			uts.log.Error(_T("ShapeMark_Right not found"));
			throw( ML_ERROR );
		}
		if( !PathFileExists(_T("..\\Result\\CA188_ShapeMark\\Mark_BOTTOM.bmp")))
		{
			uts.log.Error(_T("ShapeMark_Bottom not found"));
			throw( ML_ERROR );
		}
		if( !PathFileExists(_T("..\\Result\\CA188_ShapeMark\\Mark_CENTER.bmp")) )
		{
			uts.log.Error(_T("ShapeMark_Center not found"));
			throw( ML_ERROR );
		}

		if( !PathFileExists(_T("..\\Result\\CA188_TestImage\\slt_01.bmp")))
		{
			uts.log.Error(_T("ShapeMark_Top not found"));
			throw( ML_ERROR );
		}

		//---------------------------------
		m_mlError = mlCreateMeasurementData( 7, "..\\Result\\CA188_TestImage\\", "slt_%02d.bmp", &slt);

		if( m_mlError != ML_OK )
		{
			uts.log.Error(_T("mlCreateMeasurementData(slt)"));
			throw( m_mlError );
		}
	
		m_mlError = mlCreateMeasurementData( 7, "..\\Result\\CA188_TestImage\\", "hrz_%02d.bmp", &hrz);

		if( m_mlError != ML_OK )
		{
			uts.log.Error(_T("mlCreateMeasurementData(hrz)\n"));
			throw( m_mlError );
		}

		//PITCH, Vrt, Y
		m_mlError = mlCreateMeasurementData( 7, "..\\Result\\CA188_TestImage\\", "vrt_%02d.bmp", &vrt);

		if( m_mlError != ML_OK )
		{
			uts.log.Error(_T("mlCreateMeasurementData(vrt)\n"));
			throw( m_mlError );
		}
		
		//------------------------------------------------
		mlImageAnalysisConf conf;
		mlGetImageAnalysisConf(slt, &conf);
		conf.analysisMethod = ML_ANALYSIS_SHAPE_MATCHING;
		conf.imageBias = m_ImageBias;			// Ver 1.0.1.0
		conf.smPatternSizeMax = m_fltCodeMax;
		conf.smPatternSizeMin =m_fltCodeMin;
		conf.smClippingMargin = 0;
		conf.pxSensorSize = PixelsSize;
		conf.qrFinderPatternSizeMin = 30;
		conf.qrFinderPatternSizeMax = 60;

		sprintf(conf.bottomPatternFile, "..\\Result\\CA188_ShapeMark\\Mark_BOTTOM.bmp");
		sprintf(conf.centerPatternFile, "..\\Result\\CA188_ShapeMark\\Mark_CENTER.bmp");
		sprintf(conf.leftPatternFile, "..\\Result\\CA188_ShapeMark\\Mark_LEFT.bmp");
		sprintf(conf.rightPatternFile, "..\\Result\\CA188_ShapeMark\\Mark_RIGHT.bmp");
		sprintf(conf.topPatternFile, "..\\Result\\CA188_ShapeMark\\Mark_TOP.bmp");

		mlSetImageAnalysisConf(slt, &conf);
		
		//CrossTalk  
		conf.imageBias = m_ImageBias2;
		mlSetImageAnalysisConf(hrz, &conf);
		mlSetImageAnalysisConf(vrt, &conf);
		//---------------------------------
		for (int i = 0; i < 7; i++)
		{
			// imageNoは、１から始まる
			m_mlError = mlAnalysisSelectImage(slt, i + 1, &m_clipping);
			if( m_mlError != ML_OK )
			{
				uts.log.Error(_T("mlAnalysisSelectImage(slt_%02d)"), i + 1);
				throw( m_mlError );
			}
		}

		// 各種?算
		//---------------------------------
		mlComputationValue ret;
		mlComputationValue correlation;
		mlComputationValue moveAve;
		mlFinalCalValue fval;

		// STEP1： 縦・横の平均?離を?める
		m_mlError = mlCalDistancebAverage( slt, &(fval.distanceb) );
		if( m_mlError != ML_OK )
		{
			uts.log.Error(_T("mlCalDistancebAverage(slt)\n"));
			throw( m_mlError );
		}

		otp_param.ois_param.oiscal_NonShakerResult.Yaw_Dist = fval.distanceb.lateral;
		otp_param.ois_param.oiscal_NonShakerResult.Pitch_Dist = fval.distanceb.lengthwise;

		// STEP1： 画角を?める
		double degree = mlCalAngleOfView( (double)m_Distance, (double)m_dblSpace );
	
		// STEP1： ピクセ?/degree を?める
		m_mlError = mlPixelDividedDegree(slt, degree, &(fval.pixelDividedDegree) );
		if( m_mlError != ML_OK )
		{
			uts.log.Error(_T("mlPixelDividedDegree(slt)"));
			throw( m_mlError );
		}

		// STEP2～3：両端画?の移動差分Pixel?を?める
		m_mlError = mlCalMoveDifference(slt, &(fval.moveDifference), &correlation, &moveAve);
		if( m_mlError != ML_OK )
		{
			uts.log.Error(_T("mlCalMoveDifference(slt)"));
			throw( m_mlError );
		}
		otp_param.ois_param.oiscal_NonShakerResult.Yaw_MoveDiff = fval.moveDifference.lateral;
		otp_param.ois_param.oiscal_NonShakerResult.Pitch_MoveDiff = fval.moveDifference.lengthwise;

		// ?ーザー設定値を登録
		fval.offset.lengthwise = fval.offset.lateral = m_Offset;
		fval.alpha.lengthwise = AlphaPitch;
		fval.alpha.lateral = AlphaYaw;

		// STEP4：最終?算?果を?める
		m_mlError = mlCalFinal( &fval, &ret );
		if( m_mlError != ML_OK )
		{
			uts.log.Error(_T("mlCalFinal(slt)"));
			throw( m_mlError );
		}

		otp_param.ois_param.oiscal_NonShakerResult.Yaw_Gain = ret.lateral;
		otp_param.ois_param.oiscal_NonShakerResult.Pitch_Gain = ret.lengthwise;

		// ク?ストーク
		// Calculation mixing coefficient
		
		mlMixingValue	mixVal;

		m_mlError = mlCalMixCoeff( hrz, vrt, &m_clipping, &mixVal );
		if( m_mlError != ML_OK )
		{
			uts.log.Error(_T("mlCalMixCoeff"));
			throw( m_mlError );
		}
		
		// ?ニア?ティ補正
		// Calculation linearity correction coefficient
		mlLinearityValue	linVal;
		unsigned long	dacX[7] = { 0x36000000, 0x24000000, 0x12000000, 0x00000000, 0xEE000000, 0xDC000000, 0xCA000000 };
		unsigned long	dacY[7] = { 0x36000000, 0x24000000, 0x12000000, 0x00000000, 0xEE000000, 0xDC000000, 0xCA000000 };

		double	positionX[7];
		double	positionY[7];
		unsigned short thresholdX[7];
		unsigned short thresholdY[7];
		unsigned long	coefAXL[7];		//! for Fixed point
		unsigned long	coefBXL[7];		//! for Fixed point
		unsigned long	coefAYL[7];		//! for Fixed point
		unsigned long	coefBYL[7];		//! for Fixed point

		// setup parameters
		linVal.measurecount = 7;

		linVal.dacX = (unsigned long *)&dacX;
		linVal.dacY = (unsigned long *)&dacY;
		linVal.positionX = (double *)&positionX;
		linVal.positionY = (double *)&positionY;
		linVal.thresholdX = (unsigned short *)&thresholdX;
		linVal.thresholdY = (unsigned short *)&thresholdY;
		linVal.coefAXL = (unsigned long *)&coefAXL;
		linVal.coefBXL = (unsigned long *)&coefBXL;
		linVal.coefAYL = (unsigned long *)&coefAYL;
		linVal.coefBYL = (unsigned long *)&coefBYL;

		m_mlError = mlCalLinearCorr(slt, &m_clipping, &linVal);
		if( m_mlError != ML_OK )
		{
			uts.log.Error(_T("mlCalLinearCorr\n"));
			throw( m_mlError );
		}
		
		// マーク位置表示
		//---------------------------------
		mlPoint pt;
		mlSize sz;

		uts.log.Info(_T("************** Center mark position ***************"));
		for( int i=1; i <= 7; i++ )
		{
			if( mlGetPositionCoordinates( slt, i, ML_MARK_INDEX_CENTER, &pt, &sz ) == ML_OK )
			{
				uts.log.Debug(_T("MARK[%d] : X=%6.1f, Y=%6.1f, W=%6.1f, H=%6.1f"), i, pt.x, pt.y, sz.width, sz.height);
			}
			else
			{
				uts.log.Error(_T("MARKnot detected"));
				throw( m_mlError );
			}
		}
	}

	catch ( mlErrorCode e )
	{
		CString str;
		switch( e )
		{
		case ML_ARGUMENT_ERROR:
			uts.log.Error(_T("ML_ARGUMENT_ERROR(%d)"),e);
			break;
		case ML_ARGUMENT_NULL_ERROR:
			uts.log.Error(_T("ML_ARGUMENT_NULL_ERROR(%d)"),e);
			break;
		case ML_DIRECTORY_NOT_EXIST_ERROR:
			uts.log.Error(_T("ML_DIRECTORY_NOT_EXIST_ERROR(%d)"),e);
			break;
		case ML_ERROR:
			uts.log.Error(_T("ML_ERROR(%d)"),e);
			break;
		case ML_FILE_IO_ERROR:
			uts.log.Error(_T("ML_FILE_IO_ERROR(%d)"),e);
			break;
		case ML_FILE_NOT_EXIST_ERROR:
			uts.log.Error(_T("ML_FILE_NOT_EXIST_ERROR(%d)"),e);
			break;
		case ML_MEMORY_ERROR:
			uts.log.Error(_T("ML_MEMORY_ERROR(%d)"),e);
			break;
		case ML_MULTIPLEX_DETECTION_MARK_ERROR:
			uts.log.Error(_T("ML_MULTIPLEX_DETECTION_MARK_ERROR(%d)"),e);
			break;
		case ML_NOT_EXECUTABLE:
			uts.log.Error(_T("ML_NOT_EXECUTABLE(%d)\n"),e);
			break;
		case ML_THERE_UNANALYZED_IMAGE_ERROR:
			uts.log.Error(_T("ML_THERE_UNANALYZED_IMAGE_ERROR(%d)"),e);
			break;
		case ML_UNDETECTED_MARK_ERROR:
			uts.log.Error(_T("ML_UNDETECTED_MARK_ERROR(%d)"),e);
			break;
		default:
			uts.log.Error(_T("Library error(%d)"),e);
			break;
		}

		goto end;
	}
	catch (...)
	{
		uts.log.Error(_T("unknownError : Somthing error occurred"));
	}

	uts.log.Debug(_T("Yaw_Dist  = %.3f"),otp_param.ois_param.oiscal_NonShakerResult.Yaw_Dist);
	uts.log.Debug(_T("Pitch_Dist  = %.3f"),otp_param.ois_param.oiscal_NonShakerResult.Pitch_Dist);
	uts.log.Debug(_T("Yaw_MoveDiff  = %.3f"),otp_param.ois_param.oiscal_NonShakerResult.Yaw_MoveDiff);
	uts.log.Debug(_T("Pitch_MoveDiff  = %.3f"),otp_param.ois_param.oiscal_NonShakerResult.Pitch_MoveDiff);
	uts.log.Debug(_T("Yaw_Gain CalX = %.3f"),otp_param.ois_param.oiscal_NonShakerResult.Yaw_Gain);
	uts.log.Debug(_T("Pitch_Gain CalY  = %.3f"),otp_param.ois_param.oiscal_NonShakerResult.Pitch_Gain);

	
	//write the final result via I2C
	do_SetgyroX(otp_param.ois_param.oiscal_NonShakerResult.Yaw_Gain);
	do_SetgyroY(otp_param.ois_param.oiscal_NonShakerResult.Pitch_Gain);
	
	Algorithm::Sleep(1000);

	//Save Gyro Gain
	do_SaveGyroGainData(otp_param.ois_param.oiscal_NonShakerResult.Yaw_Gain,otp_param.ois_param.oiscal_NonShakerResult.Pitch_Gain);
	
end:
	//---------------------------------
	if (slt)
		mlDeleteMeasurementData(slt);
	if (hrz)
		mlDeleteMeasurementData(hrz);
	if (vrt)
		mlDeleteMeasurementData(vrt);

	if( m_mlError != ML_OK )
	{
		ret = OTPCALI_ERROR_GYROCALI;
	}

	return SET_ERROR(ret);
}

