#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_ColorAccuracy_Altek.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef enum ColorBlock_REF
{
	Blue = 0,
	Green,
	Red,
	Yellow,
	Magenta,
	Cyan,
	Color_COUNT
} ;


namespace UTS
{
	CString strROIName[Color_COUNT] = {
		_T("Blue"),
		_T("Green"),
		_T("Red"),
		_T("Yellow"),
		_T("Magenta"),
		_T("Cyan"),
	};
	CString strDefaultValue[Color_COUNT] = {
		_T("120,900,32,32, 15.24,  51.22, -69.76"),
		_T("240,900,32,32, 62.61, -54.87,  55.7"),
		_T("360,900,32,32, 42.3 ,  67.46,  56.61"),
		_T("480,900,32,32, 87.43,  -2.93,  87.49"),
		_T("600,900,32,32, 45.34,  73.76,  -8.63"),
		_T("720,900,32,32, 56.93, -18.09, -30.48"),
	};

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));

		CString strValue;
		vector<int> vecValue;
		vector<double> vecDoubleValue;

		for (int i = 0; i < Color_COUNT ;i++)
		{
			CString strKeyName;
			strKeyName.Format(_T("ColorBlock[%s]"), strROIName[i]);
			CString strComment;
			strComment.Format(_T("ROI[%s](左,上,宽,高,R_Value,G_Value,B_Value)"), strROIName[i]);
			uts.dbCof.GetOperatorSingleSpec(strSection, strKeyName, strValue, strDefaultValue[i], strComment);
			SplitInt(strValue, vecValue);
			m_param.ColorBlock[i].ColorROIPos.left = vecValue[0];
			m_param.ColorBlock[i].ColorROIPos.top = vecValue[1];
			m_param.ColorBlock[i].ColorROIPos.right = vecValue[0] + vecValue[2];
			m_param.ColorBlock[i].ColorROIPos.bottom = vecValue[1] + vecValue[3];
			m_param.ColorBlock[i].SpecLab.dL = vecValue[4];
			m_param.ColorBlock[i].SpecLab.dA = vecValue[5];
			m_param.ColorBlock[i].SpecLab.dB = vecValue[6];
		}


		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Spec.dDelta"), strValue, _T("0,10"), _T("DeltaSpec(Min,Max)"));
		SplitInt(strValue, vecValue);
		m_param.dDeltaSpec.min = vecValue[0];
		m_param.dDeltaSpec.max = vecValue[1];

		return TRUE;
	}

	BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_POINT pt;
		UI_TEXT text;
		UI_TEXT_REL textRel;
		UI_LINE line;

		// ref black

		for (int i = 0; i < Color_COUNT ;i++)
		{
			rc.color = COLOR_WHITE ;
			memcpy(&rc.rcPos, &m_param.ColorBlock[i].ColorROIPos, sizeof(RECT));
			uiMark.vecUiRect.push_back(rc);
		}


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
		memset(m_CAResult,0,sizeof(m_CAResult));

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


		// 测试
		if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
		{
			for (int i = 0; i < Color_COUNT ;i++)
			{
				RGBTRIPLE rgb;
					 
				UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer,
					m_bufferInfo.nWidth, 
					m_bufferInfo.nHeight, 
					0,
					m_param.ColorBlock[i].ColorROIPos, 
					rgb);

				ColorSpace::RGBDouble2Lab(
					rgb.rgbtRed,
					rgb.rgbtGreen,
					rgb.rgbtBlue,
					m_CAResult[i].ColorLab.dL,
					m_CAResult[i].ColorLab.dA,
					m_CAResult[i].ColorLab.dB);

				m_CAResult[i].Delta = Math::GetDistance2D(m_param.ColorBlock[i].SpecLab.dA,m_param.ColorBlock[i].SpecLab.dB,
					                                      m_CAResult[i].ColorLab.dA,m_CAResult[i].ColorLab.dB);

			}
		}
		else
		{
			uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
			*pnErrorCode = uts.errorcode.E_NoImage;
			goto end;
		}

		//------------------------------------------------------------------------------
		// 判断规格
		*pnErrorCode = uts.errorcode.E_Pass;
		for (int i = 0; i < Color_COUNT ;i++)
		{
			if (m_CAResult[i].Delta < m_param.dDeltaSpec.min || m_CAResult[i].Delta > m_param.dDeltaSpec.max)
			{
				*pnErrorCode = uts.errorcode.E_ColorLevel;
			}
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
		vecReturnValue.push_back(uts.errorcode.E_ColorLevel); //tofix
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

		//-------------------------------------------------------------------------
		// Head
		strHeader = _T("Time,SN,TestTime(ms),Result,");
		strData.Format(_T("%s,%s,%.1f,%s,"),
			lpTime,
			uts.info.strSN,
			m_TimeCounter.GetPassTime(),
			strResult);

		// ColorBlock
		for (int i = 0; i < Color_COUNT; i++)
		{
			strHeader.AppendFormat(_T("Color[%s]_L,Color[%s]_A,Color[%s]_B,Color[%s]_Delta,"), strROIName[i], strROIName[i], strROIName[i], strROIName[i]);
			strData.AppendFormat(_T("%.2f,%.2f,%.2f,%.2f,"), m_CAResult[i].ColorLab.dL
													       , m_CAResult[i].ColorLab.dA
													       , m_CAResult[i].ColorLab.dB
													       , m_CAResult[i].Delta);
		}


		//-------------------------------------------------------------------------
		// Tail
		strHeader.Append(_T("Version,OP_SN\n"));
		strData.AppendFormat(_T("%s,%s\n"),
			strVersion,
			uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
