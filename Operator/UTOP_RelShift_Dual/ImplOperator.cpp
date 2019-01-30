#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_RelShift_Dual.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment(lib, "algorithm.lib")

typedef struct MODULEINFO
{
	double Center_X;		// Pixel or um
	double Center_Y;		// Pixel or um
	double FOV_Z;			// °or um
	double RotationAngle;	// °
	double Tilt_X;			// °
	double Tilt_Y;			// °
	double PixelSize;
}
ModuleInfo;


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

		CString strValue;
		vector<double> vecValue;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShiftXSpec[Min,Max]"), strValue, _T("-118.0,-55.0"), _T("X位移规格"));
		SplitDouble(strValue, vecValue);
		m_param.dShiftXSpec.min = vecValue[0];
		m_param.dShiftXSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShiftYSpec[Min,Max]"), strValue, _T("-27.0,27.0"), _T("Y位移规格"));
		SplitDouble(strValue, vecValue);
		m_param.dShiftYSpec.min = vecValue[0];
		m_param.dShiftYSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dScaleSpec[Min,Max]"), strValue, _T("0.9704,1.0305"), _T("缩放规格"));
		SplitDouble(strValue, vecValue);
		m_param.dScaleSpec.min = vecValue[0];
		m_param.dScaleSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dRotateAngle[Min,Max]"), strValue, _T("-0.5,0.5"), _T("角度规格"));
		SplitDouble(strValue, vecValue);
		m_param.dRotateAngleSpec.min = vecValue[0];
		m_param.dRotateAngleSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dTiltXAngleSpec[Min,Max]"), strValue, _T("-0.5,0.5"), _T("回转规格(顺时针旋转为正)"));
		SplitDouble(strValue, vecValue);
		m_param.dTiltXAngleSpec.min = vecValue[0];
		m_param.dTiltXAngleSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dTiltYAngleSpec[Min,Max]"), strValue, _T("-0.5,0.5"), _T("回转规格(顺时针旋转为正)"));
		SplitDouble(strValue, vecValue);
		m_param.dTiltYAngleSpec.min = vecValue[0];
		m_param.dTiltYAngleSpec.max = vecValue[1];

		//


		vecValue.clear();

		return TRUE;
	}

	BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		ModuleInfo inf[2];

		// 初始化结果
		m_result.dShiftX = m_result.dShiftY = m_result.dRotateAngle = m_result.dScale = 0.0;

		// 读取共享数据(R)
		CStringA strHeader, strKey;

		for (int i = 0 ;i < 2 ;i++)
		{
			if( i == 0) strHeader = "right";
			else strHeader = "left";

			strKey.Format("%s_dTiltX", strHeader);
			inf[i].Tilt_X = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_dTiltY", strHeader);
			inf[i].Tilt_Y = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_dRotateAngle", strHeader);
			inf[i].RotationAngle = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_Center_X", strHeader);
			inf[i].Center_X = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_Center_Y", strHeader);
			inf[i].Center_Y = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_FOV_Z", strHeader);
			inf[i].FOV_Z = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_PixelSize", strHeader);
			inf[i].PixelSize = (double)(uts.info.mapShared[strKey.GetBuffer()]);
		}

		m_result.dTiltX = inf[0].Tilt_X - inf[1].Tilt_X;
		m_result.dTiltY = inf[0].Tilt_Y - inf[1].Tilt_Y;
		m_result.dRotateAngle = inf[0].RotationAngle - inf[1].RotationAngle;
		m_result.dShiftY = inf[0].Center_Y*inf[0].PixelSize - inf[1].Center_Y*inf[1].PixelSize;
		m_result.dShiftX = inf[0].Center_X*inf[0].PixelSize - inf[1].Center_X*inf[1].PixelSize;
		m_result.dScale = inf[0].FOV_Z/inf[1].FOV_Z;

		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_POINT pt;
		UI_TEXT text;
		UI_TEXT_REL textRel;
		UI_LINE line;

		//------------------------------------------------------------------------------
		// 判断规格
		int text_Top = 60 , text_GapY = 80;

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("ShiftX: %.2f"), m_result.dShiftX);
		if (!IS_IN_RANGE(m_result.dShiftX, m_param.dShiftXSpec.min, m_param.dShiftXSpec.max))
		{
			text.color = COLOR_RED;
		}
		uiMark.vecUIText.push_back(text);

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top + text_GapY;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("ShiftY: %.2f"), m_result.dShiftY);
		if (!IS_IN_RANGE(m_result.dShiftY, m_param.dShiftYSpec.min, m_param.dShiftYSpec.max))
		{
			text.color = COLOR_RED;
		}
		uiMark.vecUIText.push_back(text);

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top + 2*text_GapY;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("Scale: %.2f"), m_result.dScale);
		if (!IS_IN_RANGE(m_result.dScale, m_param.dScaleSpec.min, m_param.dScaleSpec.max))
		{
			text.color = COLOR_RED;
		}
		uiMark.vecUIText.push_back(text);

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top + 3*text_GapY;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("Angle: %.2f"), m_result.dRotateAngle);
		if (!IS_IN_RANGE(m_result.dRotateAngle, m_param.dRotateAngleSpec.min, m_param.dRotateAngleSpec.max))
		{
			text.color = COLOR_RED;
		}
		uiMark.vecUIText.push_back(text);

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top + 4*text_GapY;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("TiltX: %.2f"), m_result.dTiltX);
// 		if (!IS_IN_RANGE(m_result.dRotateAngle, m_param.dRotateAngleSpec.min, m_param.dRotateAngleSpec.max))
// 		{
// 			text.color = COLOR_RED;
// 		}
		uiMark.vecUIText.push_back(text);

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top + 5*text_GapY;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("TiltY: %.2f"), m_result.dTiltY);
		// 		if (!IS_IN_RANGE(m_result.dRotateAngle, m_param.dRotateAngleSpec.min, m_param.dRotateAngleSpec.max))
		// 		{
		// 			text.color = COLOR_RED;
		// 		}
		uiMark.vecUIText.push_back(text);

		//Draw Right ROI
		/*
		rc.rcPos.left = DOUBLE2LONG(m_result.ptPR[0].x) + DOUBLE2LONG((m_param.dShiftXSpec.min + m_param.dShiftXSpec.max)/2);
		rc.rcPos.top =  DOUBLE2LONG(m_result.ptPR[0].y);
		rc.rcPos.right  = DOUBLE2LONG(m_result.ptQR[3].x) + DOUBLE2LONG((m_param.dShiftXSpec.min + m_param.dShiftXSpec.max)/2);
		rc.rcPos.bottom = DOUBLE2LONG(m_result.ptQR[3].y);
		*/
		rc.color = COLOR_RED;
		uiMark.vecUiRect.push_back(rc);


		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		CStringA strHeader, strKey;
		UI_MARK uiMark;
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);
		vector<POINTFLOAT> vecPt;

		//------------------------------------------------------------------------------
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
		if (!m_pDevice->Recapture(m_bufferObj))
		{
			uts.log.Error(_T("Recapture error. "));
			*pnErrorCode = uts.errorcode.E_NoImage;
			goto end;
		}

		//------------------------------------------------------------------------------
		ModuleInfo inf[2];

		for (int i = 0 ;i < 2 ;i++)
		{
			if( i == 0) strHeader = "right";
			else strHeader = "left";

			strKey.Format("%s_dTiltX", strHeader);
			inf[i].Tilt_X = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_dTiltY", strHeader);
			inf[i].Tilt_Y = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_dRotateAngle", strHeader);
			inf[i].RotationAngle = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_Center_X", strHeader);
			inf[i].Center_X = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_Center_Y", strHeader);
			inf[i].Center_Y = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_FOV_Z", strHeader);
			inf[i].FOV_Z = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_PixelSize", strHeader);
			inf[i].PixelSize = (double)(uts.info.mapShared[strKey.GetBuffer()]);
		}

		m_result.dTiltX = inf[0].Tilt_X - inf[1].Tilt_X;
		m_result.dTiltY = inf[0].Tilt_Y - inf[1].Tilt_Y;
		m_result.dRotateAngle = inf[0].RotationAngle - inf[1].RotationAngle;
		m_result.dShiftY = inf[0].Center_Y*inf[0].PixelSize - inf[1].Center_Y*inf[1].PixelSize;
		m_result.dShiftX = inf[0].Center_X*inf[0].PixelSize - inf[1].Center_X*inf[1].PixelSize;
		m_result.dScale = inf[0].FOV_Z/inf[1].FOV_Z;

		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		//-------------------------------------------------------------------------


		//------------------------------------------------------------------------------
		// 判断规格(回转)
		*pnErrorCode = uts.errorcode.E_Pass;

		if ( m_result.dTiltX < m_param.dTiltXAngleSpec.min
			|| m_result.dTiltX > m_param.dTiltXAngleSpec.max
			|| m_result.dTiltY < m_param.dTiltYAngleSpec.min
			|| m_result.dTiltY > m_param.dTiltYAngleSpec.max
			|| m_result.dRotateAngle < m_param.dRotateAngleSpec.min
			|| m_result.dRotateAngle > m_param.dRotateAngleSpec.max
			|| m_result.dShiftX < m_param.dShiftXSpec.min
			|| m_result.dShiftX > m_param.dShiftXSpec.max
			|| m_result.dShiftY < m_param.dShiftYSpec.min
			|| m_result.dShiftY > m_param.dShiftYSpec.max
			|| m_result.dScale < m_param.dScaleSpec.min
			|| m_result.dScale > m_param.dScaleSpec.max)
		{
			*pnErrorCode = uts.errorcode.E_Fail;
		}

end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		//------------------------------------------------------------------------------
		// 保存图片文件
		SaveImage();

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
		vecReturnValue.push_back(uts.errorcode.E_Rotate);
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
		strHeader = _T("Time,SN,DeviceIndex,TestTime(ms),Result,");
		strData.Format(_T("%s,%s,%d,%.1f,%s,"),
			lpTime,
			uts.info.strSN,
			uts.info.nDeviceIndex,
			m_TimeCounter.GetPassTime(),
			strResult);

		//-------------------------------------------------------------------------
		// Body
		strHeader.Append(_T("ShiftX,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dShiftX);
		strHeader.Append(_T("ShiftY,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dShiftY);
		strHeader.Append(_T("Rotate,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dRotateAngle);
		strHeader.Append(_T("TiltX,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dTiltX);
		strHeader.Append(_T("TiltY,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dTiltY);
		strHeader.Append(_T("Scale,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dScale);

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
