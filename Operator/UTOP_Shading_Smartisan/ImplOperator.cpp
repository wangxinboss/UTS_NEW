#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_Shading_Smartisan.h"
//#include "Algorithm.h"

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::FixPatternNoise::FixPatternNoise_WD;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
	ImplOperator::ImplOperator(void)
	{
		OPERATOR_INIT;
	}

	ImplOperator::~ImplOperator(void)
	{
	}

	BOOL ImplOperator::OnReadSpec()
	{
		CString strSection = OSUtil::GetFileName(m_strModuleFile);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
		
		//CString strValue;
		//vector<double> vecDoubleValue;
		//uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShadingSpec"),strValue,  _T("0.65,1.03"), _T("worst corner Shading Spec(cornerAvgY / centerAvgY), >= Spec will PASS"));
		//SplitDouble(strValue, vecDoubleValue);
		//m_param.dShadingSpec.min = vecDoubleValue[0];
		//m_param.dShadingSpec.max = vecDoubleValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShadingDeltaSpec"), m_param.dShadingDeltaSpec, 0.1, _T("Shading delta Spec, <= Spec will PASS"));
		
		
		//uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShadingGoldenDeltaSpec"), m_param.dShadingDeltaGoldenSpec, 0.1, _T("Shading Golden delta Spec, <= Spec will PASS"));
		//uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShadingGolden"), m_param.dShadingGolden, 0.1, _T("Shading Golden "));


		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);

		//------------------------------------------------------------------------------
		// 初始化结果
		WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

		*pnErrorCode = uts.errorcode.E_Pass;
		m_dYvalue = 0.0;
		memset(&m_result,0,sizeof(Shading_RESULT));
		//------------------------------------------------------------------------------

		//Save img
		CString strDirPath;
		CString strImg;


		int nOutMode = 0;

		if (!m_pDevice->ReadValue(
			eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
			&nOutMode, sizeof(nOutMode)))
		{
			uts.log.Error(_T("GetSensorOutMode error."));
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}

		// 抓图
		if (m_param.nReCapture != 0)
		{
			// 重新设定Sensor序列
			CString strRegName = m_strOperatorName;
			if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_WRITE_SENSOR_REGISTER,
				strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
			{
				uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
				*pnErrorCode = uts.errorcode.E_Fail;
				goto end;
			}

			// 抓图
			if (!m_pDevice->Recapture(
				m_bufferObj,
				uts.info.nLTDD_DummyFrame,
				uts.info.nLTDD_AvgFrame))
			{
				uts.log.Error(_T("Recapture error. "));
				*pnErrorCode = uts.errorcode.E_NoImage;
				goto end;
			}
		}
		else
		{
			// 使用上次的抓图
			m_pDevice->GetCapturedBuffer(m_bufferObj);
		}

		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		//------------------------------------------------------------------------------
		// 判断画面平均亮度
		UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
		if (m_dYvalue < m_param.dLTMinY || m_dYvalue > m_param.dLTMaxY)
		{
			uts.log.Error(_T("Linumance = %.2f"), m_dYvalue);
			*pnErrorCode = uts.errorcode.E_Linumance;
			goto end;
		}

		//check FPN
		FPNInfo m_FPNInfo;
		FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
		if (m_FPNInfo.m_dRowDifMax > 3.5  || m_FPNInfo.m_dColDifMax > 3.5)
		{
			uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,3.5);
			*pnErrorCode = uts.errorcode.E_FixPatternNoise;;
			goto end;
		}
		uts.log.Info(_T("check FPN : m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,3.5);


		CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
		//	memcpy(RAW10Image,m_bufferObj.pFrameBuffer,m_bufferInfo.nWidth* m_bufferInfo.nHeight*2);
		uts.log.Info(_T("get raw10 pass"));
		strDirPath.Format(_T("%s\\Img_for_Smartisan\\"),
			uts.info.strResultPath);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);

		strImg.Format(_T("%s\\Img_for_Smartisan\\%s_Shading"),uts.info.strResultPath,uts.info.strSensorId);

		uts.log.Info(_T("start save raw10"));
		if (!uts.imgFile.SaveRawFile(strImg,(BYTE*)RAW10Image,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
		{
			uts.log.Error(_T("Shading: failed to save raw file!"));
			*pnErrorCode = uts.errorcode.E_ColorShading;
			goto end;
		}
		uts.log.Info(_T("save raw10 pass"));
		//------------------------------------------------------------------------------
		// 测试
		if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
		{
			CImageProc::GetInstance().Cal_RGBtoYBuffer(m_bufferObj.pBmpBuffer, 
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight, 
				m_bufferObj.pYBuffer);

			/*Shading_Y_Smartisan(
				m_bufferObj.pYBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_result);*/
			Shading_Y(
				RAW10Image,
				nOutMode,
				64.0,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_result);
		}
		else
		{
			uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
			*pnErrorCode = uts.errorcode.E_NoImage;
			goto end;
		}

		//------------------------------------------------------------------------------
		// 判断规格
		double dShadingDelta;
		//double dShading_golden_Delta;
		double dShading_Coner_Max;
		double dShading_Coner_Min;
		double dShading_Coner[4];

		dShading_Coner[0] = m_result.dShadingCorner[0];
		dShading_Coner[1] = m_result.dShadingCorner[4];
		dShading_Coner[2] = m_result.dShadingCorner[20];
		dShading_Coner[3] = m_result.dShadingCorner[24];

		dShading_Coner_Max = dShading_Coner[0];
		dShading_Coner_Min = dShading_Coner[0];

		for(int i = 1; i< 4;i++){
			if(dShading_Coner_Max < dShading_Coner[i])
				dShading_Coner_Max = dShading_Coner[i];

			if(dShading_Coner_Min > dShading_Coner[i])
				dShading_Coner_Min = dShading_Coner[i];
		}

		dShadingDelta = dShading_Coner_Max - dShading_Coner_Min;

		//dShading_golden_Delta = ((dShading_Coner_Max-m_param.dShadingGolden) > (m_param.dShadingGolden-dShading_Coner_Min) ? (dShading_Coner_Max-m_param.dShadingGolden) : (m_param.dShadingGolden-dShading_Coner_Min));
		
		//uts.log.Info(_T("dShadingGolden = %.3f"), m_param.dShadingGolden);
		uts.log.Info(_T("dShading_Coner_Max = %.3f"), dShading_Coner_Max);
		uts.log.Info(_T("dShading_Coner_Min = %.3f"), dShading_Coner_Min);
		uts.log.Info(_T("dShadingDelta = %.3f"), dShadingDelta);
		uts.log.Info(_T("dShadingDeltaSpec = %.3f"), m_param.dShadingDeltaSpec);
		//uts.log.Info(_T("dShading_golden_Delta = %.3f"), dShading_golden_Delta);
		//uts.log.Info(_T("dShadingDeltaGoldenSpec = %.3f"), m_param.dShadingDeltaGoldenSpec);
		

		if ( dShadingDelta > m_param.dShadingDeltaSpec)
		{
			*pnErrorCode = uts.errorcode.E_RI;
			uts.log.Error(_T("Shading out of spec"));
			goto end;
		}


		//检查与golden模组的差异
		/*
		Num	Y_block_1	Y_block_5	Y_block_21	Y_block_25
		231	0.39356708	0.374920168	0.365317732	0.362884583
		LOW	0.34356708	0.324920168	0.315317732	0.312884583
		HIG	0.44356708	0.424920168	0.415317732	0.412884583
		*/

		double value = 0;
		double spec_low = 0;
		double spec_hig = 0;
		int nConner = 0;

		nConner = 0;
		value = dShading_Coner[nConner];
		spec_low = 0.34356708;
		spec_hig = 0.44356708;
		if(IS_IN_RANGE(value,spec_low,spec_hig))
		{
			uts.log.Debug(_T("check  conner %d value is in spec value:%f spec %f~%f "),nConner,value,spec_low,spec_hig);
		}
		else
		{
			uts.log.Error(_T("check  conner %d value is out spec value:%f spec %f~%f "),nConner,value,spec_low,spec_hig);
			*pnErrorCode = uts.errorcode.E_RI;
			goto end;
		}

		nConner = 1;
		value = dShading_Coner[nConner];
		spec_low = 0.324920168;
		spec_hig = 0.424920168;
		if(IS_IN_RANGE(value,spec_low,spec_hig))
		{
			uts.log.Debug(_T("check  conner %d value is in spec value:%f spec %f~%f "),nConner,value,spec_low,spec_hig);
		}
		else
		{
			uts.log.Error(_T("check  conner %d value is out spec value:%f spec %f~%f "),nConner,value,spec_low,spec_hig);
			*pnErrorCode = uts.errorcode.E_RI;
			goto end;
		}


		nConner = 2;
		value = dShading_Coner[nConner];
		spec_low = 0.315317732;
		spec_hig = 0.415317732;
		if(IS_IN_RANGE(value,spec_low,spec_hig))
		{
			uts.log.Debug(_T("check  conner %d value is in spec value:%f spec %f~%f "),nConner,value,spec_low,spec_hig);
		}
		else
		{
			uts.log.Error(_T("check  conner %d value is out spec value:%f spec %f~%f "),nConner,value,spec_low,spec_hig);
			*pnErrorCode = uts.errorcode.E_RI;
			goto end;
		}


		nConner = 3;
		value = dShading_Coner[nConner];
		spec_low = 0.312884583;
		spec_hig = 0.412884583;
		if(IS_IN_RANGE(value,spec_low,spec_hig))
		{
			uts.log.Debug(_T("check  conner %d value is in spec value:%f spec %f~%f "),nConner,value,spec_low,spec_hig);
		}
		else
		{
			uts.log.Error(_T("check  conner %d value is out spec value:%f spec %f~%f "),nConner,value,spec_low,spec_hig);
			*pnErrorCode = uts.errorcode.E_RI;
			goto end;
		}


end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		//------------------------------------------------------------------------------
		// 保存图片文件
		RELEASE_ARRAY(RAW10Image);
		if (m_param.nReCapture != 0)
		{
			SaveImage();
		}

		//------------------------------------------------------------------------------
		// 保存数据文件
		SaveData();

		return m_bResult;
	}

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
		vecReturnValue.push_back(uts.errorcode.E_Linumance);
		vecReturnValue.push_back(uts.errorcode.E_RI);
	}

	void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		vecRegister.push_back(m_strOperatorName);
	}

	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		strHeader = _T("Time,SN,TestTime(ms),Y_Avg,Shading_Result,")
			_T("Shading_0,Shading_1,Shading_2,Shading_3,Shading_4,Shading_5,Shading_6,Shading_7,Shading_8,Shading_9,Shading_10,Shading_11,Shading_12,Shading_13,Shading_14,Shading_15,Shading_16,Shading_17,Shading_18,Shading_19,Shading_20,Shading_21,Shading_22,Shading_23,Shading_24,Shading_Delta,")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%.1f,%s,")
			_T("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,")
			_T("%.3f,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
			, m_result.dShadingCorner[0],m_result.dShadingCorner[1],m_result.dShadingCorner[2],m_result.dShadingCorner[3],m_result.dShadingCorner[4]
			, m_result.dShadingCorner[5],m_result.dShadingCorner[6],m_result.dShadingCorner[7],m_result.dShadingCorner[8],m_result.dShadingCorner[9]
			, m_result.dShadingCorner[10],m_result.dShadingCorner[11],m_result.dShadingCorner[12],m_result.dShadingCorner[13],m_result.dShadingCorner[14]
			, m_result.dShadingCorner[15],m_result.dShadingCorner[16],m_result.dShadingCorner[17],m_result.dShadingCorner[18],m_result.dShadingCorner[19]
			, m_result.dShadingCorner[20],m_result.dShadingCorner[21],m_result.dShadingCorner[22],m_result.dShadingCorner[23],m_result.dShadingCorner[24]
			, m_result.dShadingDelta
			, m_result.dWorstRatio
			, strVersion, uts.info.strUserId);
	}
	void ImplOperator::Shading_Y(WORD* pRaw10,int rawBayerType,double blackLvl,int nWidth,int nHeight,Shading_RESULT &result)
	{
		for (int i = 0; i < 25; i++)
		{
			result.dShadingCorner[i] = 0.0;
		}

		result.dShadingDelta = 0.0;

		//?算
		RECT rcROI[25];

		SIZE sizeROI;

		sizeROI.cx = nWidth/5;
		sizeROI.cy = nHeight/5;

		// roi
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				rcROI[i*5+j].left   = j * sizeROI.cx;
				rcROI[i*5+j].right  = j * sizeROI.cx + sizeROI.cx - 1;
				rcROI[i*5+j].top    = i * sizeROI.cy;
				rcROI[i*5+j].bottom = i * sizeROI.cy + sizeROI.cy - 1;
			}
		}

		double MaxCorner = 0;
		double MinCorner = 65536;

		GetROIAvgYY(pRaw10, rawBayerType, blackLvl, nWidth, nHeight, rcROI[12], result.dAvgY[12]); //flip bmp 
		for (int i = 0; i < 25; i++)
		{
			GetROIAvgYY(pRaw10, rawBayerType, blackLvl, nWidth, nHeight, rcROI[i], result.dAvgY[i]); //flip bmp 

			if(result.dAvgY[12] != 0.0)
				result.dShadingCorner[i] = result.dAvgY[i]/result.dAvgY[12]; 
			else
				result.dShadingCorner[i] = 0.0;

			if (MaxCorner < result.dShadingCorner[i])
			{
				MaxCorner = result.dShadingCorner[i];
			}
			if (MinCorner > result.dShadingCorner[i])
			{
				MinCorner = result.dShadingCorner[i];
			}
		}
		result.dShadingDelta = (MaxCorner - MinCorner);
		//result.dWorstRatio = MinCorner;

		// 将结果上下翻转
		SWAP(double, result.dShadingCorner[0], result.dShadingCorner[20]);
		SWAP(double, result.dShadingCorner[1], result.dShadingCorner[21]);
		SWAP(double, result.dShadingCorner[2], result.dShadingCorner[22]);
		SWAP(double, result.dShadingCorner[3], result.dShadingCorner[23]);
		SWAP(double, result.dShadingCorner[4], result.dShadingCorner[24]);
		SWAP(double, result.dShadingCorner[5], result.dShadingCorner[15]);
		SWAP(double, result.dShadingCorner[6], result.dShadingCorner[16]);
		SWAP(double, result.dShadingCorner[7], result.dShadingCorner[17]);
		SWAP(double, result.dShadingCorner[8], result.dShadingCorner[18]);
		SWAP(double, result.dShadingCorner[9], result.dShadingCorner[19]);

	}
	void ImplOperator::GetROIAvgYY(WORD* pRaw10,int rawBayerType,double blackLvl,int nWidth,int nHeight,const RECT& rect,__out double &dYAvg)
	{
		double avgRGrGbB[4] ={0.0};

		int roiWidth,roiHeight;

		roiWidth = (int)(rect.right - rect.left + 1);
		roiHeight = (int)(rect.bottom - rect.top + 1);

		WORD* roiBuf = new WORD[roiHeight * roiWidth];

		for (int y=0;y<roiHeight;y++)
		{
			for (int x=0;x<roiWidth;x++)
			{
				int X = x + rect.left;
				int Y = y + rect.top;
				roiBuf[y*roiWidth+x] = pRaw10[Y*nWidth+X];
			}
		}
		//-------------------------------------------------
		/*
		2. Get average R, G, B value in the ROI window.
		R_Avg, B_Avg, G_Avg
		*/		
		double ChannelA = 0;
		double ChannelB = 0;
		double ChannelC = 0;
		double ChannelD = 0;				

		int Count = 0;
		for (int y = 0; y< roiHeight; y+=2)
		{
			for (int x = 0; x< roiWidth; x+=2)
			{
				ChannelA += double(roiBuf[(y*roiWidth + x)]);
				Count++;
			}
		}
		ChannelA /= double(Count);

		Count = 0;
		for (int y = 0; y< roiHeight; y+=2)
		{
			for (int x = 1; x< roiWidth; x+=2)
			{
				ChannelB += double(roiBuf[(y*roiWidth+x)]);
				Count++;
			}
		}
		ChannelB /= double(Count);

		Count = 0;
		for (int y = 1; y< roiHeight; y+=2)
		{
			for (int x = 0; x< roiWidth; x+=2)
			{
				ChannelC += double(roiBuf[(y*roiWidth+x)]);
				Count++;
			}
		}
		ChannelC /= double(Count);

		Count = 0;
		for (int y = 1; y< roiHeight; y+=2)
		{
			for (int x = 1; x< roiWidth; x+=2)
			{
				ChannelD += double(roiBuf[(y*roiWidth+x)]);
				Count++;
			}
		}
		ChannelD /= double(Count);

		double RGGBValue[4] = {0.0};
		//RGGBValue[0]:R, RGGBValue[1]:Gr, RGGBValue[2]:Gb, RGGBValue[3]:B
		switch (rawBayerType)
		{
		case 1://B Gb Gr R
			RGGBValue[0] = ChannelD;
			RGGBValue[1] = ChannelC;
			RGGBValue[2] = ChannelB;
			RGGBValue[3] = ChannelA;
			break;

		case 2://R Gr Gb B
			RGGBValue[0] = ChannelA;
			RGGBValue[1] = ChannelB;
			RGGBValue[2] = ChannelC;
			RGGBValue[3] = ChannelD;
			break;

		case 3://Gb B R Gr
			RGGBValue[0] = ChannelC;
			RGGBValue[1] = ChannelD;
			RGGBValue[2] = ChannelA;
			RGGBValue[3] = ChannelB;
			break;

		case 4://Gr R B Gb
			RGGBValue[0] = ChannelB;
			RGGBValue[1] = ChannelA;
			RGGBValue[2] = ChannelD;
			RGGBValue[3] = ChannelC;
			break;
		}

		avgRGrGbB[0] = RGGBValue[0]-blackLvl;
		avgRGrGbB[1] = RGGBValue[1]-blackLvl;
		avgRGrGbB[2] = RGGBValue[2]-blackLvl;
		avgRGrGbB[3] = RGGBValue[3]-blackLvl;

		double R = (avgRGrGbB[0] > 1023.0 ? 1023.0 : avgRGrGbB[0]);
		double Gr = (avgRGrGbB[1] > 1023.0 ? 1023.0 : avgRGrGbB[1]);
		double Gb = (avgRGrGbB[2] > 1023.0 ? 1023.0 : avgRGrGbB[2]);
		double B = (avgRGrGbB[3] > 1023.0 ? 1023.0 : avgRGrGbB[3]);
		double G=(Gr+Gb)/2;
		dYAvg=0.299*R+0.587*G+0.114*B;
	}
	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
