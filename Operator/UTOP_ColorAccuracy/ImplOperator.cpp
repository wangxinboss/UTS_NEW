#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_ColorAccuracy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef enum ColorBlock_REF
{
	R = 0,
	G,
	B,
	Color_COUNT
} ;


namespace UTS
{
	CString strROIName[Color_COUNT] = {
		_T("R"),
		_T("G"),
		_T("B"),
	};
	CString strDefaultValue[Color_COUNT] = {
		_T("210,100,32,32,255,0,0"),
		_T("320,100,32,32,0,255,0"),
		_T("430,100,32,32,0,0,255"),
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
			m_param.ColorBlock[i].ColorValue.rgbtRed = vecValue[4];
			m_param.ColorBlock[i].ColorValue.rgbtGreen = vecValue[5];
			m_param.ColorBlock[i].ColorValue.rgbtBlue = vecValue[6];
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
			rc.color = COLOR_YELLOW ;
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
		memset(m_CUResult,0,sizeof(CU_Result));

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
				UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer,
					m_bufferInfo.nWidth, 
					m_bufferInfo.nHeight, 
					0,
					m_param.ColorBlock[i].ColorROIPos, 
					m_CUResult[i].rgb);

				m_CUResult[i].Delta = Math::GetDistance3D(m_param.ColorBlock[i].ColorValue.rgbtRed,m_param.ColorBlock[i].ColorValue.rgbtGreen,m_param.ColorBlock[i].ColorValue.rgbtBlue,
					                                      m_CUResult[i].rgb.rgbtRed,m_CUResult[i].rgb.rgbtGreen,m_CUResult[i].rgb.rgbtBlue);

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
			if (m_CUResult[i].Delta < m_param.dDeltaSpec.min || m_CUResult[i].Delta > m_param.dDeltaSpec.max)
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
			strHeader.AppendFormat(_T("Color[%s]_R,Color[%s]_G,Color[%s]_B,Color[%s]_Delta,"), strROIName[i], strROIName[i], strROIName[i], strROIName[i]);
			strData.AppendFormat(_T("%d,%d,%d,%.2f,"), m_CUResult[i].rgb.rgbtRed
													 , m_CUResult[i].rgb.rgbtGreen
													 , m_CUResult[i].rgb.rgbtBlue
													 , m_CUResult[i].Delta);
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
