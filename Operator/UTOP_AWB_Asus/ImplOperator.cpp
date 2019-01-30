#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_AWB_Asus.h"

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dAWBSpec_R"),strValue,  _T("0.93,1.07"), _T("AWB_R Spec [SpecMin <= (AvgR/AvgG ) <= SpecMax] will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dAWBSpec_R.min = vecDoubleValue[0];
		m_param.dAWBSpec_R.max = vecDoubleValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dAWBSpec_B"),strValue,  _T("0.93,1.07"), _T("AWB_B Spec [SpecMin <= (AvgB/AvgG ) <= SpecMax] will PASS"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dAWBSpec_B.min = vecDoubleValue[0];
		m_param.dAWBSpec_B.max = vecDoubleValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("RECT_AWB"),strValue,  _T("100,100,50,50"), _T("x,y,width,height"));
		uts.log.Debug(_T("section:%s ROI:%s"),strSection,strValue);
		SplitDouble(strValue, vecDoubleValue);
		m_param.Rect_AWB.left = (int)vecDoubleValue[0];
		m_param.Rect_AWB.top  = (int)vecDoubleValue[1];
		m_param.Rect_AWB.right = (int)vecDoubleValue[2] + m_param.Rect_AWB.left;
		m_param.Rect_AWB.bottom = (int)vecDoubleValue[3] + m_param.Rect_AWB.top;

		uts.log.Debug(_T("ROI:%d %d %d %d"),m_param.Rect_AWB.left,m_param.Rect_AWB.top,m_param.Rect_AWB.right,m_param.Rect_AWB.bottom);
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
		memset(&m_result,0,sizeof(AWB_RESULT));
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
			RGBTRIPLE rgb;

			UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer, 
				m_bufferInfo.nWidth, 
				m_bufferInfo.nHeight, 
				0, m_param.Rect_AWB, rgb);

			m_result.dAWB_R = double(rgb.rgbtRed) / double(rgb.rgbtGreen);
			m_result.dAWB_G = double(rgb.rgbtGreen) / double(rgb.rgbtGreen);
			m_result.dAWB_B = double(rgb.rgbtBlue) / double(rgb.rgbtGreen);


		}
		else
		{
			uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
			*pnErrorCode = uts.errorcode.E_NoImage;
			goto end;
		}

		//------------------------------------------------------------------------------
		// 判断规格
		if ( m_result.dAWB_R < m_param.dAWBSpec_R.min
			||   m_result.dAWB_R > m_param.dAWBSpec_R.max
			||	 m_result.dAWB_B < m_param.dAWBSpec_B.min
			||   m_result.dAWB_B > m_param.dAWBSpec_B.max)
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

	BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		// OSD绘制
		UI_RECT rc;
		UI_POINT pt;
		UI_TEXT text;
		UI_TEXT_REL textRel;
		UI_LINE line;

		memcpy(&rc.rcPos, &m_param.Rect_AWB, sizeof(RECT));
		uiMark.vecUiRect.push_back(rc);

		return TRUE;
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

		strHeader = _T("Time,SN,TestTime(ms),Y_Avg,AWB_Result,")
			_T("dRatioR,dRatioB,dRatioB,")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%.1f,%s,")
			_T("%.3f,%.3f,%.3f,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
			, m_result.dAWB_R, m_result.dAWB_G,m_result.dAWB_B
			, strVersion, uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
