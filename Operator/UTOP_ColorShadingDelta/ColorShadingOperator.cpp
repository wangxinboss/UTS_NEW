#include "StdAfx.h"
#include "ColorShadingOperator.h"
#include "UTOP_ColorShadingDelta.h"

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dMeanA"),strValue,  _T("-10,-10,-10,-10,-10"), _T("MeanA(LU,RU,Center,LD,RD)"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dMeanA[0] = vecDoubleValue[0];
		m_param.dMeanA[1] = vecDoubleValue[1];
		m_param.dMeanA[2] = vecDoubleValue[2];
		m_param.dMeanA[3] = vecDoubleValue[3];
		m_param.dMeanA[4] = vecDoubleValue[4];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dMeanB"),strValue,  _T("-35,-35,-35,-35,-35"), _T("MeanB(LU,RU,Center,LD,RD)"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dMeanB[0] = vecDoubleValue[0];
		m_param.dMeanB[1] = vecDoubleValue[1];
		m_param.dMeanB[2] = vecDoubleValue[2];
		m_param.dMeanB[3] = vecDoubleValue[3];
		m_param.dMeanB[4] = vecDoubleValue[4];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDeltaSpec"), m_param.dDeltaSpec,1, _T("DeltaSpec"));


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
		if (m_dYvalue < m_param.dLTMinY || m_dYvalue > m_param.dLTMaxY)
		{
			*pnErrorCode = uts.errorcode.E_Linumance;
			goto end;
		}

		//------------------------------------------------------------------------------
		// 测试
		UTS::Algorithm::ColorShading_NOKIA::ColorShadingDelta(m_bufferObj.pBmpBuffer,
					m_bufferInfo.nWidth, m_bufferInfo.nHeight,
					m_param.nROIWidth,m_param.nROIHeight,
					m_param.dMeanA,m_param.dMeanB,
					m_result);
		//------------------------------------------------------------------------------
		// 判断规格
		if (m_result.dMaxDeltaC > m_param.dDeltaSpec )
		{
			*pnErrorCode = uts.errorcode.E_ColorShading;
		}
		else
		{
			*pnErrorCode = uts.errorcode.E_Pass;
		}

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
			        _T("Delta_Max,Delta_LU,Delta_RU,Delta_Center,Delta_LD,Delta_RD,")
					_T("L_LU,a_LU,b_LU,")
					_T("L_RU,a_RU,b_RU,")
					_T("L_Center,a_Center,b_Center,")
					_T("L_LD,a_LD,b_LD,")
					_T("L_RD,a_RD,b_RD,")
		            _T("Version,OP_SN\n");

		strData.Format(
		     _T("%s,%s,%.1f,%.1f,%s,")
			 _T("%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,")
			 _T("%.4f,%.4f,%.4f,")
			 _T("%.4f,%.4f,%.4f,")
			 _T("%.4f,%.4f,%.4f,")
			 _T("%.4f,%.4f,%.4f,")
			 _T("%.4f,%.4f,%.4f,")
		     _T("%s,%s\n")
		    , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
			, m_result.dMaxDeltaC,m_result.dDeltaC[0],m_result.dDeltaC[1],m_result.dDeltaC[2],m_result.dDeltaC[3],m_result.dDeltaC[4]
		    , m_result.lab[0].dL,m_result.lab[0].dA,m_result.lab[0].dB
			, m_result.lab[1].dL,m_result.lab[1].dA,m_result.lab[1].dB
			, m_result.lab[2].dL,m_result.lab[2].dA,m_result.lab[2].dB
			, m_result.lab[3].dL,m_result.lab[3].dA,m_result.lab[3].dB
			, m_result.lab[4].dL,m_result.lab[4].dA,m_result.lab[4].dB
			, strVersion, uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ColorShadingOperator);
	}
	//------------------------------------------------------------------------------
}
