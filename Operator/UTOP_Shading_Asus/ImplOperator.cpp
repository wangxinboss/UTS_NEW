#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_Shading_Asus.h"

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
		
		CString strValue;
		vector<double> vecDoubleValue;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShadingSpec"),strValue,  _T("0.65,1.03"), _T("worst corner Shading Spec(cornerAvgY / centerAvgY), >= Spec will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dShadingSpec.min = vecDoubleValue[0];
		m_param.dShadingSpec.max = vecDoubleValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShadingDeltaSpec"), m_param.dShadingDeltaSpec, 0.1, _T("Shading delta Spec, <= Spec will PASS"));
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
		*pnErrorCode = uts.errorcode.E_Pass;
		m_dYvalue = 0.0;
		memset(&m_result,0,sizeof(Shading_RESULT));
		//------------------------------------------------------------------------------
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

		//------------------------------------------------------------------------------
		// 测试
		if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
		{
			CImageProc::GetInstance().Cal_RGBtoYBuffer(m_bufferObj.pBmpBuffer, 
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight, 
				m_bufferObj.pYBuffer);

			Shading_Y(
				m_bufferObj.pYBuffer,
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
		
		
		if ( m_result.dWorstRatio < m_param.dShadingSpec.min
		||   m_result.dWorstRatio> m_param.dShadingSpec.max)
		{
			*pnErrorCode = uts.errorcode.E_RI;
		}
		

		if ( m_result.dShadingDelta > m_param.dShadingDeltaSpec)
		{
			*pnErrorCode = uts.errorcode.E_RI;
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
			_T("Shading_LU,Shading_RU,Shading_LD,Shading_RD,Shading_Delta,")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%.1f,%s,")
			_T("%.3f,%.3f,%.3f,%.3f,%.3f,")
			_T("%.3f,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
			, m_result.dShadingCorner[Corner_LU], m_result.dShadingCorner[Corner_RU]
			, m_result.dShadingCorner[Corner_LD], m_result.dShadingCorner[Corner_RD], m_result.dShadingDelta
			, m_result.dWorstRatio
			, strVersion, uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
