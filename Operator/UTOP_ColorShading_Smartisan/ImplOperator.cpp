#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_ColorShading_Smartisan.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::FixPatternNoise::FixPatternNoise_WD;

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

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dRG_Delta_spec"), m_param.dRG_Delta_Spec, 0.07, _T("dRG_Delta_spec"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dBG_Delta_spec"), m_param.dBG_Delta_Spec, 0.07, _T("dBG_Delta_spec"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dRG_Golden_Delta_spec"), m_param.dRG_Golden_Delta_Spec, 0.07, _T("dRG_Golden_Delta_spec"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dBG_Golden_Delta_spec"), m_param.dBG_Golden_Delta_Spec, 0.07, _T("dBG_Golden_Delta_spec"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dRG_Golden"), m_param.dRG_Golden, 0.07, _T("dRG_Golden"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dBG_Golden"), m_param.dBG_Golden, 0.07, _T("dBG_Golden"));
		//CString strValue;
		//vector<double> vecDoubleValue;
		//uts.dbCof.GetOperatorSingleSpec(strSection, _T("dColorShadingSpec"),strValue,  _T("0.70,1.25"), _T("worst corner ColorShading Spec[(cornerAvgR/cornerAvgG )/ (centerAvgR/centerAvgG)], >= Spec will PASS"));
		//SplitDouble(strValue, vecDoubleValue);
		//m_param.dColorShadingSpec.min = vecDoubleValue[0];
		//m_param.dColorShadingSpec.max = vecDoubleValue[1];
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
		memset(&m_result,0,sizeof(ColorShading_RESULT));
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
			Algorithm_Smartisan::ColorShading_Smartisan::ColorShading_Y_Smartisan(
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

		uts.log.Info(_T("start judge spec"));
		//------------------------------------------------------------------------------
		// 判断规格
		double RatioBG_Coner[4],RatioRG_Coner[4];
		double RatioBG_Max,RatioBG_Min;
		double RatioRG_Max,RatioRG_Min;
		double RatioRG_Delta,RatioBG_Delta;
		double RatioRG_Golden_Delta,RatioBG_Golden_Delta;

		RatioRG_Coner[0] = m_result.dRatioRG[0];
		RatioRG_Coner[1] = m_result.dRatioRG[4];
		RatioRG_Coner[2] = m_result.dRatioRG[20];
		RatioRG_Coner[3] = m_result.dRatioRG[24];

		RatioBG_Coner[0] = m_result.dRatioBG[0];
		RatioBG_Coner[1] = m_result.dRatioBG[4];
		RatioBG_Coner[2] = m_result.dRatioBG[20];
		RatioBG_Coner[3] = m_result.dRatioBG[24];

		RatioRG_Max = RatioRG_Coner[0];
		RatioRG_Min = RatioRG_Coner[0];

		RatioBG_Max = RatioBG_Coner[0];
		RatioBG_Min = RatioBG_Coner[0];

		for(int i = 1; i <4;i++){
			if(RatioRG_Max < RatioRG_Coner[i])
				RatioRG_Max = RatioRG_Coner[i];
			if(RatioBG_Max < RatioBG_Coner[i])
				RatioBG_Max = RatioBG_Coner[i];

			if(RatioRG_Min > RatioRG_Coner[i])
				RatioRG_Min = RatioRG_Coner[i];
			if(RatioBG_Min > RatioBG_Coner[i])
				RatioBG_Min = RatioBG_Coner[i];
		}

		RatioRG_Delta = RatioRG_Max - RatioRG_Min;
		RatioBG_Delta = RatioBG_Max - RatioBG_Min;

		
		RatioRG_Golden_Delta = ((RatioRG_Max-m_param.dRG_Golden) > (m_param.dRG_Golden-RatioRG_Min) ? (RatioRG_Max-m_param.dRG_Golden) : (m_param.dRG_Golden-RatioRG_Min));
		RatioBG_Golden_Delta = ((RatioBG_Max-m_param.dBG_Golden) > (m_param.dBG_Golden-RatioBG_Min) ? (RatioBG_Max-m_param.dBG_Golden) : (m_param.dBG_Golden-RatioBG_Min));

		uts.log.Info(_T("dRG_Golden = %.3f"), m_param.dRG_Golden);
		uts.log.Info(_T("dBG_Golden = %.3f"), m_param.dBG_Golden);
		uts.log.Info(_T("RatioRG_Max = %.3f"), RatioRG_Max);
		uts.log.Info(_T("RatioRG_Min = %.3f"), RatioRG_Min);
		uts.log.Info(_T("RatioBG_Max = %.3f"), RatioBG_Max);
		uts.log.Info(_T("RatioBG_Min = %.3f"), RatioBG_Min);
		uts.log.Info(_T("RatioRG_Delta = %.3f"), RatioRG_Delta);
		uts.log.Info(_T("RatioBG_Delta = %.3f"), RatioBG_Delta);
		uts.log.Info(_T("dRG_Delta_Spec = %.3f"), m_param.dRG_Delta_Spec);
		uts.log.Info(_T("dBG_Delta_Spec = %.3f"), m_param.dBG_Delta_Spec);
		uts.log.Info(_T("RatioRG_Golden_Delta = %.3f"), RatioRG_Golden_Delta);
		uts.log.Info(_T("RatioBG_Golden_Delta = %.3f"), RatioBG_Golden_Delta);
		uts.log.Info(_T("dRG_Golden_Delta_Spec = %.3f"), m_param.dRG_Golden_Delta_Spec);
		uts.log.Info(_T("dBG_Golden_Delta_Spec = %.3f"), m_param.dBG_Golden_Delta_Spec);
		if ( RatioRG_Delta > m_param.dRG_Delta_Spec|| 
			 RatioBG_Delta > m_param.dBG_Delta_Spec||	
			 RatioRG_Golden_Delta > m_param.dRG_Golden_Delta_Spec||  
			 RatioBG_Golden_Delta > m_param.dBG_Golden_Delta_Spec)
		{
			uts.log.Error(_T("Colorshading out of spec"));
			*pnErrorCode = uts.errorcode.E_ColorShading;
		}

end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		RELEASE_ARRAY(RAW10Image);
		//------------------------------------------------------------------------------
		// 保存图片文件
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

		strHeader = _T("Time,SN,TestTime(ms),Y_Avg,ColorShading_Result,")
			_T("R_0,R_1,R_2,R_3,R_4,R_5,R_6,R_7,R_8,R_9,R_10,R_11,R_12,R_13,R_14,R_15,R_16,R_17,R_18,R_19,R_20,R_21,R_22,R_23,R_24,")
			_T("Gr_0,Gr_1,Gr_2,Gr_3,Gr_4,Gr_5,Gr_6,Gr_7,Gr_8,Gr_9,Gr_10,Gr_11,Gr_12,Gr_13,Gr_14,Gr_15,Gr_16,Gr_17,Gr_18,Gr_19,Gr_20,Gr_21,Gr_22,Gr_23,Gr_24,")
			_T("Gb_0,Gb_1,Gb_2,Gb_3,Gb_4,Gb_5,Gb_6,Gb_7,Gb_8,Gb_9,Gb_10,Gb_11,Gb_12,Gb_13,Gb_14,Gb_15,Gb_16,Gb_17,Gb_18,Gb_19,Gb_20,Gb_21,Gb_22,Gb_23,Gb_24,")
			_T("B_0,B_1,B_2,B_3,B_4,B_5,B_6,B_7,B_8,B_9,B_10,B_11,B_12,B_13,B_14,B_15,B_16,B_17,B_18,B_19,B_20,B_21,B_22,B_23,B_24,")
			_T("dRatioRG_0,dRatioRG_1,dRatioRG_2,dRatioRG_3,dRatioRG_4,dRatioRG_5,dRatioRG_6,dRatioRG_7,dRatioRG_8,dRatioRG_9,dRatioRG_10,dRatioRG_11,dRatioRG_12,dRatioRG_13,dRatioRG_14,dRatioRG_15,dRatioRG_16,dRatioRG_17,dRatioRG_18,dRatioRG_19,dRatioRG_20,dRatioRG_21,dRatioRG_22,dRatioRG_23,dRatioRG_24,")
			_T("dRatioBG_0,dRatioBG_1,dRatioBG_2,dRatioBG_3,dRatioBG_4,dRatioBG_5,dRatioBG_6,dRatioBG_7,dRatioBG_8,dRatioBG_9,dRatioBG_10,dRatioBG_11,dRatioBG_12,dRatioBG_13,dRatioBG_14,dRatioBG_15,dRatioBG_16,dRatioBG_17,dRatioBG_18,dRatioBG_19,dRatioBG_20,dRatioBG_21,dRatioBG_22,dRatioBG_23,dRatioBG_24,")
			_T("dWorstRatioBG,dWorstRatioBG,")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%.1f,%s,")
			_T("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,")
			_T("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,")
			_T("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,")
			_T("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,")
			_T("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,")
			_T("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,")
			_T("%.3f,%.3f,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
			, m_result.dAvg4chanel[0].R,m_result.dAvg4chanel[1].R,m_result.dAvg4chanel[2].R,m_result.dAvg4chanel[3].R,m_result.dAvg4chanel[4].R
			, m_result.dAvg4chanel[5].R,m_result.dAvg4chanel[6].R,m_result.dAvg4chanel[7].R,m_result.dAvg4chanel[8].R,m_result.dAvg4chanel[9].R
			, m_result.dAvg4chanel[10].R,m_result.dAvg4chanel[11].R,m_result.dAvg4chanel[12].R,m_result.dAvg4chanel[13].R,m_result.dAvg4chanel[14].R
			, m_result.dAvg4chanel[15].R,m_result.dAvg4chanel[16].R,m_result.dAvg4chanel[17].R,m_result.dAvg4chanel[18].R,m_result.dAvg4chanel[19].R
			, m_result.dAvg4chanel[20].R,m_result.dAvg4chanel[21].R,m_result.dAvg4chanel[22].R,m_result.dAvg4chanel[23].R,m_result.dAvg4chanel[24].R
			, m_result.dAvg4chanel[0].Gr,m_result.dAvg4chanel[1].Gr,m_result.dAvg4chanel[2].Gr,m_result.dAvg4chanel[3].Gr,m_result.dAvg4chanel[4].Gr
			, m_result.dAvg4chanel[5].Gr,m_result.dAvg4chanel[6].Gr,m_result.dAvg4chanel[7].Gr,m_result.dAvg4chanel[8].Gr,m_result.dAvg4chanel[9].Gr
			, m_result.dAvg4chanel[10].Gr,m_result.dAvg4chanel[11].Gr,m_result.dAvg4chanel[12].Gr,m_result.dAvg4chanel[13].Gr,m_result.dAvg4chanel[14].Gr
			, m_result.dAvg4chanel[15].Gr,m_result.dAvg4chanel[16].Gr,m_result.dAvg4chanel[17].Gr,m_result.dAvg4chanel[18].Gr,m_result.dAvg4chanel[19].Gr
			, m_result.dAvg4chanel[20].Gr,m_result.dAvg4chanel[21].Gr,m_result.dAvg4chanel[22].Gr,m_result.dAvg4chanel[23].Gr,m_result.dAvg4chanel[24].Gr
			, m_result.dAvg4chanel[0].Gb,m_result.dAvg4chanel[1].Gb,m_result.dAvg4chanel[2].Gb,m_result.dAvg4chanel[3].Gb,m_result.dAvg4chanel[4].Gb
			, m_result.dAvg4chanel[5].Gb,m_result.dAvg4chanel[6].Gb,m_result.dAvg4chanel[7].Gb,m_result.dAvg4chanel[8].Gb,m_result.dAvg4chanel[9].Gb
			, m_result.dAvg4chanel[10].Gb,m_result.dAvg4chanel[11].Gb,m_result.dAvg4chanel[12].Gb,m_result.dAvg4chanel[13].Gb,m_result.dAvg4chanel[14].Gb
			, m_result.dAvg4chanel[15].Gb,m_result.dAvg4chanel[16].Gb,m_result.dAvg4chanel[17].Gb,m_result.dAvg4chanel[18].Gb,m_result.dAvg4chanel[19].Gb
			, m_result.dAvg4chanel[20].Gb,m_result.dAvg4chanel[21].Gb,m_result.dAvg4chanel[22].Gb,m_result.dAvg4chanel[23].Gb,m_result.dAvg4chanel[24].Gb
			, m_result.dAvg4chanel[0].B,m_result.dAvg4chanel[1].B,m_result.dAvg4chanel[2].B,m_result.dAvg4chanel[3].B,m_result.dAvg4chanel[4].B
			, m_result.dAvg4chanel[5].B,m_result.dAvg4chanel[6].B,m_result.dAvg4chanel[7].B,m_result.dAvg4chanel[8].B,m_result.dAvg4chanel[9].B
			, m_result.dAvg4chanel[10].B,m_result.dAvg4chanel[11].B,m_result.dAvg4chanel[12].B,m_result.dAvg4chanel[13].B,m_result.dAvg4chanel[14].B
			, m_result.dAvg4chanel[15].B,m_result.dAvg4chanel[16].B,m_result.dAvg4chanel[17].B,m_result.dAvg4chanel[18].B,m_result.dAvg4chanel[19].B
			, m_result.dAvg4chanel[20].B,m_result.dAvg4chanel[21].B,m_result.dAvg4chanel[22].B,m_result.dAvg4chanel[23].B,m_result.dAvg4chanel[24].B
			, m_result.dRatioRG[0],m_result.dRatioRG[1],m_result.dRatioRG[2],m_result.dRatioRG[3],m_result.dRatioRG[4]
		    , m_result.dRatioRG[5],m_result.dRatioRG[6],m_result.dRatioRG[7],m_result.dRatioRG[8],m_result.dRatioRG[9]
			, m_result.dRatioRG[10],m_result.dRatioRG[11],m_result.dRatioRG[12],m_result.dRatioRG[13],m_result.dRatioRG[14]
			, m_result.dRatioRG[15],m_result.dRatioRG[16],m_result.dRatioRG[17],m_result.dRatioRG[18],m_result.dRatioRG[19]
			, m_result.dRatioRG[20],m_result.dRatioRG[21],m_result.dRatioRG[22],m_result.dRatioRG[23],m_result.dRatioRG[24]
			, m_result.dRatioBG[0],m_result.dRatioBG[1],m_result.dRatioBG[2],m_result.dRatioBG[3],m_result.dRatioBG[4]
			, m_result.dRatioBG[5],m_result.dRatioBG[6],m_result.dRatioBG[7],m_result.dRatioBG[8],m_result.dRatioBG[9]
			, m_result.dRatioBG[10],m_result.dRatioBG[11],m_result.dRatioBG[12],m_result.dRatioBG[13],m_result.dRatioBG[14]
			, m_result.dRatioBG[15],m_result.dRatioBG[16],m_result.dRatioBG[17],m_result.dRatioBG[18],m_result.dRatioBG[19]
			, m_result.dRatioBG[20],m_result.dRatioBG[21],m_result.dRatioBG[22],m_result.dRatioBG[23],m_result.dRatioBG[24]
		, m_result.dWorstRatioBG , m_result.dWorstRatioRG
			, strVersion, uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
