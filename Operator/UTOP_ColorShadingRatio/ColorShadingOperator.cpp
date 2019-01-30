#include "StdAfx.h"
#include "ColorShadingOperator.h"
#include "UTOP_ColorShadingRatio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
	ColorShadingOperator::ColorShadingOperator(void)
	{
		OPERATOR_INIT;
	}

	ColorShadingOperator::~ColorShadingOperator(void)
	{
	}

	BOOL ColorShadingOperator::OnReadSpec()
	{
		CString strSection = OSUtil::GetFileName(m_strModuleFile);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nROIWidth"), m_param.nROIWidth, 32, _T("ROI width(pixel count)"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nROIHeight"), m_param.nROIHeight, 32, _T("ROI height(pixel count)"));

		CString strValue;
		vector<double> vecDoubleValue;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dCenterRatioBGSpec"),strValue,  _T("0.8,1.2"), _T("Color Shading Spec(centerAvgB / centerAvgY),in Spec Range will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.stCenterRatioBGSpec.min = vecDoubleValue[0];
		m_param.stCenterRatioBGSpec.max = vecDoubleValue[1];
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dCenterRatioRGSpec"),strValue,  _T("0.8,1.2"), _T("Color Shading Spec(centerAvgR / centerAvgY),in Spec Range will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.stCenterRatioRGSpec.min = vecDoubleValue[0];
		m_param.stCenterRatioRGSpec.max = vecDoubleValue[1];
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dCenterRatioGrGbSpec"),strValue,  _T("0.9,1.1"), _T("Color Shading Spec(centerAvgR / centerAvgY),in Spec Range will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.stCenterRatioGrGbSpec.min = vecDoubleValue[0];
		m_param.stCenterRatioGrGbSpec.max = vecDoubleValue[1];
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dCornerRatioBGSpec"),strValue,  _T("0.7,1.3"), _T("Color Shading Spec(centerAvgB / centerAvgY),in Spec Range will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.stCornerRatioBGSpec.min = vecDoubleValue[0];
		m_param.stCornerRatioBGSpec.max = vecDoubleValue[1];
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dCornerRatioRGSpec"),strValue,  _T("0.7,1.3"), _T("Color Shading Spec(centerAvgR / centerAvgY),in Spec Range will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.stCornerRatioRGSpec.min = vecDoubleValue[0];
		m_param.stCornerRatioRGSpec.max = vecDoubleValue[1];

		return TRUE;
	}

	BOOL ColorShadingOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);

		//------------------------------------------------------------------------------
		// 初始化结果
		m_dYvalue = 0.0;
		memset(&m_result,0,sizeof(CS_DELTA_NOKIA_RESULT));
		//------------------------------------------------------------------------------
		// 抓图
		if (m_param.nReCapture != 0)
		{
			// 重新设定Sensor序列
			CString strRegName = m_strOperatorName;
			if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
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
		/*if (m_dYvalue < m_param.dLTMinY || m_dYvalue > m_param.dLTMaxY)
		{
			*pnErrorCode = uts.errorcode.E_Linumance;
			goto end;
		}
		*/
		uts.log.Debug(_T("1"));
		//------------------------------------------------------------------------------
		// 测试
		int nOutMode = 0;
		if (!GetSensorOutMode(nOutMode))
		{
			uts.log.Error(_T("GetSensorOutMode error."));
			*pnErrorCode = uts.errorcode.E_NoImage;
			goto end;
		}
		uts.log.Debug(_T("2"));
		UTS::Algorithm::ColorShading_NOKIA::ColorShadingRatio(m_bufferObj.pRaw8Buffer,
					m_bufferInfo.nWidth, m_bufferInfo.nHeight,nOutMode,0,
					m_param.nROIWidth,m_param.nROIHeight,m_result);
		//------------------------------------------------------------------------------
		uts.log.Debug(_T("3"));
		// 判断规格
		/*if (m_result.dCenterRatioBG < m_param.stCenterRatioBGSpec.min ||
			m_result.dCenterRatioBG > m_param.stCenterRatioBGSpec.max ||
			m_result.dCenterRatioRG < m_param.stCenterRatioRGSpec.min ||
			m_result.dCenterRatioRG > m_param.stCenterRatioRGSpec.max ||
			m_result.dCenterRatioGrGb < m_param.stCenterRatioGrGbSpec.min ||
			m_result.dCenterRatioGrGb > m_param.stCenterRatioGrGbSpec.max 
			)
		{
			*pnErrorCode = uts.errorcode.E_ColorShading;
		}
		else
		{
			*pnErrorCode = uts.errorcode.E_Pass;
		}
		uts.log.Debug(_T("4"));
		
		for (int i = 0 ;i < 4 ; i++ )
		{
			if(m_result.dCornerRatioBG[i] < m_param.stCornerRatioBGSpec.min ||
			   m_result.dCornerRatioBG[i] > m_param.stCenterRatioBGSpec.max ||
			   m_result.dCornerRatioRG[i] < m_param.stCornerRatioRGSpec.min ||
			   m_result.dCornerRatioRG[i] > m_param.stCenterRatioRGSpec.max )
			{
				*pnErrorCode = uts.errorcode.E_ColorShading;
			}
		}
		*/
		uts.log.Debug(_T("5"));
end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

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

	void ColorShadingOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
		vecReturnValue.push_back(uts.errorcode.E_Linumance);
		vecReturnValue.push_back(uts.errorcode.E_ColorShading);
	}

	void ColorShadingOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		vecRegister.push_back(m_strOperatorName);
	}

	void ColorShadingOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		strHeader = _T("Time,SN,TestTime(ms),Y_Avg,ColorShading_Result,")
			        _T("Center_B/G,Center_R/G,Center_Gr/Gb,LU_B/G,LU_R/G,RU_B/G,RU_R/G,LD_B/G,LD_R/G,RD_B/G,RD_R/G,")
		            _T("Version,OP_SN\n");

		strData.Format(
		    _T("%s,%s,%.1f,%.1f,%s,")
			 _T("%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,")
		    _T("%s,%s\n")
		    , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
			, m_result.dCenterRatioBG,m_result.dCenterRatioRG,m_result.dCenterRatioGrGb
			, m_result.dCornerRatioBG[0],m_result.dCornerRatioRG[0], m_result.dCornerRatioBG[1],m_result.dCornerRatioRG[1]
		    , m_result.dCornerRatioBG[2],m_result.dCornerRatioRG[2], m_result.dCornerRatioBG[3],m_result.dCornerRatioRG[3]
		    , strVersion, uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ColorShadingOperator);
	}
	//------------------------------------------------------------------------------
}
