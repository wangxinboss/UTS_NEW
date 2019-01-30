#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_RelShift_Diff_MultiChart.h"

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

		CString strValue;
		vector<double> vecValue;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShiftXSpec[Min,Max]"), strValue, _T("-118.0,-55.0"), _T("X位移规格"));
		SplitDouble(strValue, vecValue);
		m_param.dShiftXSpec[Min] = vecValue[0];
		m_param.dShiftXSpec[Max] = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dShiftYSpec[Min,Max]"), strValue, _T("-27.0,27.0"), _T("Y位移规格"));
		SplitDouble(strValue, vecValue);
		m_param.dShiftYSpec[Min] = vecValue[0];
		m_param.dShiftYSpec[Max] = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dScaleSpec[Min,Max]"), strValue, _T("0.9704,1.0305"), _T("缩放规格"));
		SplitDouble(strValue, vecValue);
		m_param.dScaleSpec[Min] = vecValue[0];
		m_param.dScaleSpec[Max] = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dAngleSpec[Min,Max]"), strValue, _T("-0.5,0.5"), _T("角度规格"));
		SplitDouble(strValue, vecValue);
		m_param.dAngleSpec[Min] = vecValue[0];
		m_param.dAngleSpec[Max] = vecValue[1];

		//-------------------------------------------------------------------------

		return TRUE;
	}

	BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		vector<POINTFLOAT> vecPt;

		// 初始化结果
		m_result.dShiftX = m_result.dShiftY = m_result.dAngle = m_result.dScale = 0.0;

		// 读取共享数据(R)
		CStringA strHeader, strKey;

		for (int j = 0; j < 3; j++)
		{
			strHeader = "right";
			strKey.Format("%s_P%d_x", strHeader, j);
			m_result.ptPR[j].x = (float)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_P%d_y", strHeader, j);
			m_result.ptPR[j].y = (float)(uts.info.mapShared[strKey.GetBuffer()]);

			strKey.Format("%s_S%d_x", strHeader, j);
			m_result.ptSR[j].x = (float)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_S%d_y", strHeader, j);
			m_result.ptSR[j].y = (float)(uts.info.mapShared[strKey.GetBuffer()]);

			strKey.Format("%s_Q%d_x", strHeader, j);
			m_result.ptQR[j].x = (float)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_Q%d_y", strHeader, j);
			m_result.ptQR[j].y = (float)(uts.info.mapShared[strKey.GetBuffer()]);

			strHeader = "left";
			strKey.Format("%s_P%d_x", strHeader, j);
			m_result.ptPL[j].x = (float)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_P%d_y", strHeader, j);
			m_result.ptPL[j].y = (float)(uts.info.mapShared[strKey.GetBuffer()]);

			strKey.Format("%s_S%d_x", strHeader, j);
			m_result.ptSL[j].x = (float)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_S%d_y", strHeader, j);
			m_result.ptSL[j].y = (float)(uts.info.mapShared[strKey.GetBuffer()]);

			strKey.Format("%s_Q%d_x", strHeader, j);
			m_result.ptQL[j].x = (float)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_Q%d_y", strHeader, j);
			m_result.ptQL[j].y = (float)(uts.info.mapShared[strKey.GetBuffer()]);
		}

		//-------------------------------------------------------------------------
		// 计算结果
		for (int j = 0; j < 3; j++)
		{
			double x1 = m_result.ptPL[j].x - m_result.ptQL[j].x;
			double x2 = m_result.ptPR[j].x - m_result.ptQR[j].x;
			double y1 = m_result.ptPL[j].y - m_result.ptQL[j].y;
			double y2 = m_result.ptPR[j].y - m_result.ptQR[j].y;

			double sx1 = m_result.ptSL[j].x - nWidth/2;
			double sy1 = nHeight/2 - m_result.ptSL[j].y;
			double sx2 = m_result.ptSR[j].x - nWidth/2;
			double sy2 = nHeight/2 - m_result.ptSR[j].y;

			m_result.dShiftX += (sx2 - ((sx1 * (x1 * x2 + y1 * y2) - sy1 * (y2 * x1 - x2 * y1)) / (x1 * x1 + y1 * y1)));
			m_result.dShiftY += (sy2 - ((sx1 * (y2 * x1 - x2 * y1) + sy1 * (x1 * x2 - y1 * y2)) / (x1 * x1 + y1 * y1)));
			m_result.dScale += (sqrt((double)(x2 * x2 + y2 * y2) / (double)(x1 * x1 + y1 * y1)));
			m_result.dAngle += (Radian2Angle(atan((double)(x1 * y2 - x2 * y1) / (double)(x1 * x2 + y2 * y1))));
		}
		m_result.dShiftX /= 3.0;
		m_result.dShiftY /= 3.0;
		m_result.dScale /= 3.0;
		m_result.dAngle /= 3.0;

		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_POINT pt;
		UI_TEXT text;
		UI_TEXT_REL textRel;
		UI_LINE line;


		//------------------------------------------------------------------------------
		// 判断规格
		int text_Top = 60 , text_GapY = 100;

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("ShiftX: %.2f"), m_result.dShiftX);
		if (!IS_IN_RANGE(m_result.dShiftX, m_param.dShiftXSpec[Min], m_param.dShiftXSpec[Max]))
		{
			text.color = COLOR_RED;
		}
		uiMark.vecUIText.push_back(text);

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top + text_GapY;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("ShiftY: %.2f"), m_result.dShiftY);
		if (!IS_IN_RANGE(m_result.dShiftY, m_param.dShiftYSpec[Min], m_param.dShiftYSpec[Max]))
		{
			text.color = COLOR_RED;
		}
		uiMark.vecUIText.push_back(text);

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top + 2*text_GapY;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("Scale: %.2f"), m_result.dScale);
		if (!IS_IN_RANGE(m_result.dScale, m_param.dScaleSpec[Min], m_param.dScaleSpec[Max]))
		{
			text.color = COLOR_RED;
		}
		uiMark.vecUIText.push_back(text);

		text.ptPos.x = nWidth/2;
		text.ptPos.y = text_Top + 3*text_GapY;
		text.color = COLOR_BLUE;
		text.strText.Format(_T("Angle: %.2f"), m_result.dAngle);
		if (!IS_IN_RANGE(m_result.dAngle, m_param.dAngleSpec[Min], m_param.dAngleSpec[Max]))
		{
			text.color = COLOR_RED;
		}
		uiMark.vecUIText.push_back(text);


		//Draw Right ROI
		rc.rcPos.left = DOUBLE2LONG(m_result.ptPR[0].x) + DOUBLE2LONG((m_param.dShiftXSpec[Min] + m_param.dShiftXSpec[Max])/2);
		rc.rcPos.top =  DOUBLE2LONG(m_result.ptPR[0].y);
		rc.rcPos.right  = DOUBLE2LONG(m_result.ptQR[2].x) + DOUBLE2LONG((m_param.dShiftXSpec[Min] + m_param.dShiftXSpec[Max])/2);
		rc.rcPos.bottom = DOUBLE2LONG(m_result.ptQR[2].y);

		rc.color = COLOR_RED;
		uiMark.vecUiRect.push_back(rc);

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
		m_result.dShiftX = m_result.dShiftY = m_result.dAngle = m_result.dScale = 0.0;
		//-------------------------------------------------------------------------
		// 读取共享数据(R)
		CStringA strHeader, strKey;
		for (int j = 0; j < 3; j++)
		{
			strHeader = "right";
			strKey.Format("%s_P%d_x", strHeader, j);
			m_result.ptPR[j].x = (float)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_P%d_y", strHeader, j);
			m_result.ptPR[j].y = (float)(uts.info.mapShared[strKey.GetBuffer()]);

			strKey.Format("%s_S%d_x", strHeader, j);
			m_result.ptSR[j].x = (float)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_S%d_y", strHeader, j);
			m_result.ptSR[j].y = (float)(uts.info.mapShared[strKey.GetBuffer()]);

			strKey.Format("%s_Q%d_x", strHeader, j);
			m_result.ptQR[j].x = (float)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_Q%d_y", strHeader, j);
			m_result.ptQR[j].y = (float)(uts.info.mapShared[strKey.GetBuffer()]);
		}
		// 计算结果
		for (int j = 0; j < 3; j++)
		{
			double x1 = m_result.ptPL[j].x - m_result.ptQL[j].x;
			double x2 = m_result.ptPR[j].x - m_result.ptQR[j].x;
			double y1 = m_result.ptPL[j].y - m_result.ptQL[j].y;
			double y2 = m_result.ptPR[j].y - m_result.ptQR[j].y;
			double sx1 = m_result.ptSL[j].x - m_bufferInfo.nWidth/2;
			double sy1 = m_bufferInfo.nHeight/2 - m_result.ptSL[j].y;
			double sx2 = m_result.ptSR[j].x - m_bufferInfo.nWidth/2;
			double sy2 = m_bufferInfo.nHeight/2 - m_result.ptSR[j].y;
			m_result.dShiftX += (sx2 - ((sx1 * (x1 * x2 + y1 * y2) - sy1 * (y2 * x1 - x2 * y1)) / (x1 * x1 + y1 * y1)));
			m_result.dShiftY += (sy2 - ((sx1 * (y2 * x1 - x2 * y1) + sy1 * (x1 * x2 - y1 * y2)) / (x1 * x1 + y1 * y1)));
			m_result.dScale += (sqrt((double)(x2 * x2 + y2 * y2) / (double)(x1 * x1 + y1 * y1)));
			m_result.dAngle += (Radian2Angle(atan((double)(x1 * y2 - x2 * y1) / (double)(x1 * x2 + y2 * y1))));
		}
		m_result.dShiftX /= 3.0;
		m_result.dShiftY /= 3.0;
		m_result.dScale /= 3.0;
		m_result.dAngle /= 3.0;

		//------------------------------------------------------------------------------
		// 判断规格
		if (!IS_IN_RANGE(m_result.dShiftX, m_param.dShiftXSpec[Min], m_param.dShiftXSpec[Max]))
		{
			*pnErrorCode = uts.errorcode.E_RelativeShift;
			goto end;
		}
		if (!IS_IN_RANGE(m_result.dShiftY, m_param.dShiftYSpec[Min], m_param.dShiftYSpec[Max]))
		{
			*pnErrorCode = uts.errorcode.E_RelativeShift;
			goto end;
		}
		if (!IS_IN_RANGE(m_result.dScale, m_param.dScaleSpec[Min], m_param.dScaleSpec[Max]))
		{
			*pnErrorCode = uts.errorcode.E_RelativeShift;
			goto end;
		}
		if (!IS_IN_RANGE(m_result.dAngle, m_param.dAngleSpec[Min], m_param.dAngleSpec[Max]))
		{
			*pnErrorCode = uts.errorcode.E_RelativeShift;
			goto end;
		}

		*pnErrorCode = uts.errorcode.E_Pass;

end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

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
		vecReturnValue.push_back(uts.errorcode.E_RelativeShift);
	}

	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		//-------------------------------------------------------------------------
		// Head
		strHeader = _T("Time,SN,SensorID,DeviceIndex,TestTime(ms),Result,");
		strData.Format(_T("%s,%s,%s,%d,%.1f,%s,"),
			lpTime,
			uts.info.strSN,
			uts.info.strSensorId,
			uts.info.nDeviceIndex,
			m_TimeCounter.GetPassTime(),
			strResult);

		//-------------------------------------------------------------------------
		// Body
		strHeader.Append(_T("Scale,angle,ShiftX,ShiftY,"));
		strData.AppendFormat(_T("%.4f,%.2f,%.1f,%.1f,"),
			m_result.dScale,
			m_result.dAngle,
			m_result.dShiftX,
			m_result.dShiftY);

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
