#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_RelShift_SFR_Dual.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef struct MODULEINFO
{
	double Center_X;		// Pixel or um
	double Center_Y;		// Pixel or um
	double dEFL;
	double dPixelsize;
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

// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dScaleSpec[Min,Max]"), strValue, _T("0.9704,1.0305"), _T("缩放规格"));
// 		SplitDouble(strValue, vecValue);
// 		m_param.dScaleSpec.min = vecValue[0];
// 		m_param.dScaleSpec.max = vecValue[1];

// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dRotateAngle[Min,Max]"), strValue, _T("-0.5,0.5"), _T("角度规格"));
// 		SplitDouble(strValue, vecValue);
// 		m_param.dRotateAngleSpec.min = vecValue[0];
// 		m_param.dRotateAngleSpec.max = vecValue[1];
// 
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dTiltXAngleSpec[Min,Max]"), strValue, _T("-0.5,0.5"), _T("回转规格(顺时针旋转为正)"));
		SplitDouble(strValue, vecValue);
		m_param.dTiltXAngleSpec.min = vecValue[0];
		m_param.dTiltXAngleSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dTiltYAngleSpec[Min,Max]"), strValue, _T("-0.5,0.5"), _T("回转规格(顺时针旋转为正)"));
		SplitDouble(strValue, vecValue);
		m_param.dTiltYAngleSpec.min = vecValue[0];
		m_param.dTiltYAngleSpec.max = vecValue[1];
		vecValue.clear();

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("CaptureDistance"), m_param.CaptureDistance, 600.0, _T("Distance between module and chart"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("ModuleBaseLine"), m_param.ModuleBaseLine, 161.5, _T("Distance between two module"));
		
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("ChartGridSize"), m_param.ChartGridSize, 90, _T("Chart Grid Size"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("ImageGridSize"), m_param.ImageGridSize, 90, _T("Image Grid Size"));


		return TRUE;
	}

	BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		


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

			strKey.Format("%s_Center_X", strHeader);
			inf[i].Center_X = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_Center_Y", strHeader);
			inf[i].Center_Y = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_EFL", strHeader);
			inf[i].dEFL = (double)(uts.info.mapShared[strKey.GetBuffer()]);
			strKey.Format("%s_Pixelsize", strHeader);
			inf[i].dPixelsize = (double)(uts.info.mapShared[strKey.GetBuffer()]);
		}

		double StandardShiftX = 0,ObjectDistance = 0,EnlargeRatio = 0,SensorShift ;

		ObjectDistance = 1/((1/inf[0].dEFL) - (1/m_param.CaptureDistance));
		EnlargeRatio = m_param.CaptureDistance / ObjectDistance;
		SensorShift = (m_param.ModuleBaseLine / EnlargeRatio)*1000;
		StandardShiftX = SensorShift/inf[0].dPixelsize;

		m_result.dShiftY = inf[1].Center_Y - inf[0].Center_Y;
		m_result.dShiftX = inf[1].Center_X - inf[0].Center_X - StandardShiftX;

		m_result.dTiltX = Radian2Angle(atan((m_result.dShiftX * (m_param.ModuleBaseLine/StandardShiftX)) / m_param.CaptureDistance));
		m_result.dTiltY = Radian2Angle(atan(m_result.dShiftY * (m_param.ModuleBaseLine/StandardShiftX)/ m_param.CaptureDistance));

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
			|| m_result.dShiftX < m_param.dShiftXSpec.min
			|| m_result.dShiftX > m_param.dShiftXSpec.max
			|| m_result.dShiftY < m_param.dShiftYSpec.min
			|| m_result.dShiftY > m_param.dShiftYSpec.max)
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
		strHeader.Append(_T("TiltX,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dTiltX);
		strHeader.Append(_T("TiltY,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dTiltY);

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
