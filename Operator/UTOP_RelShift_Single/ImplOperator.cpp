#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_RelShift_Single.h"
#include "algorithm_geometry_multisfr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "algorithm.lib")



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
		vector<int> vecValue2;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dRotateAngleSpec[Min,Max]"), strValue, _T("-0.25,0.25"), _T("回转规格(顺时针旋转为正)"));
		SplitDouble(strValue, vecValue);
		m_param.dRotateAngleSpec.min = vecValue[0];
		m_param.dRotateAngleSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dTiltXAngleSpec[Min,Max]"), strValue, _T("-0.08,0.08"), _T("回转规格(顺时针旋转为正)"));
		SplitDouble(strValue, vecValue);
		m_param.dTiltXAngleSpec.min = vecValue[0];
		m_param.dTiltXAngleSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dTiltYAngleSpec[Min,Max]"), strValue, _T("-0.08,0.08"), _T("回转规格(顺时针旋转为正)"));
		SplitDouble(strValue, vecValue);
		m_param.dTiltYAngleSpec.min = vecValue[0];
		m_param.dTiltYAngleSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dPixelSize"), m_param.dPixelSize, 1.0, _T("Pixel Size"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLenEFL"), m_param.dLensEFL, 4.04, _T("Lens EFL"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLensDFOV"), m_param.dLensDFOV, 77.41, _T("Lens DFOV"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("szPtCount[X,Y]"), strValue, _T("21,15"), _T("X,Y"));
		SplitInt(strValue, vecValue2);
		m_param.szPtCount.cx = vecValue2[0];
		m_param.szPtCount.cy = vecValue2[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("ClipImage[top,left,bottom,right]"), strValue, _T("0,0,960,1280"), _T("[top,left,bottom,right]"));
		SplitInt(strValue, vecValue2);
		m_param.ClipImage.top = vecValue2[0];
		m_param.ClipImage.left = vecValue2[1];
		m_param.ClipImage.bottom = vecValue2[2];
		m_param.ClipImage.right = vecValue2[3];

		vecValue.clear();

		return TRUE;
	}

	BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		UI_LINE line;

		// Big crosshair
		line.color = COLOR_RED;
		line.ptBegin = CPoint(nWidth / 2, 0);
		line.ptEnd = CPoint(nWidth / 2, nHeight);
		uiMark.vecUiLine.push_back(line);
		line.ptBegin = CPoint(0, nHeight / 2);
		line.ptEnd = CPoint(nWidth, nHeight / 2);
		uiMark.vecUiLine.push_back(line);

		UI_RECT rc;
		rc.color = COLOR_BLUE;
		rc.rcPos.bottom = m_param.ClipImage.bottom;
		rc.rcPos.right = m_param.ClipImage.right;
		rc.rcPos.top = m_param.ClipImage.top;
		rc.rcPos.left = m_param.ClipImage.left;
		uiMark.vecUiRect.push_back(rc);

		/*
		int roiw = m_param.ClipImage.right - m_param.ClipImage.left; 
		int roiH = m_param.ClipImage.bottom - m_param.ClipImage.top; 
		unsigned char *pROIBuffer = new unsigned char[roiw*roiH*3];

		// 测试
		algorithm::geometry::multisfr::ChessboardCaptureInfo MyChessboardCaptureInfo;

		MyChessboardCaptureInfo.PixelSize = m_param.dPixelSize;		//(um)
		MyChessboardCaptureInfo.EFL = m_param.dLensEFL;				//(mm)

		//Chart size ???
		MyChessboardCaptureInfo.ChartWidth = 444;		//(mm)
		MyChessboardCaptureInfo.ChartHeight = 320;		//(mm)
		MyChessboardCaptureInfo.CaptureDistance = 400.0;	//(mm)

		//由FOV算真?狀況
		MyChessboardCaptureInfo.DFOV = m_param.dLensDFOV;				//(°)

		MyChessboardCaptureInfo.ImageWidth = roiw;
		MyChessboardCaptureInfo.ImageHeight = roiH;

		MyChessboardCaptureInfo.IntersectionXCount = m_param.szPtCount.cx;
		MyChessboardCaptureInfo.IntersectionYCount = m_param.szPtCount.cy;

		CImageProc::GetInstance().GetBMPBlockBuffer(
			bufferObj.pBmpBuffer,
			pROIBuffer,
			nWidth,
			nHeight,
			m_param.ClipImage.left,
			m_param.ClipImage.top,
			roiw,
			roiH);

		CImageProc::GetInstance().GetFlipedBmpBGRtoRGBBuffer(
			pROIBuffer,
			roiw,
			roiH,
			bufferObj.pFrameBuffer);

		RELEASE_ARRAY(pROIBuffer);

		//?算棋盤Chart的點位置
		algorithm::geometry::multisfr::SixAxisInfo MySixAxisInfo;
		bool res = CameraGeometyRelationship(bufferObj.pFrameBuffer,
			roiw,
			roiH,
			MyChessboardCaptureInfo,
			MySixAxisInfo);

		m_result.dTiltX = MySixAxisInfo.Tilt_X ;
		m_result.dTiltY = MySixAxisInfo.Tilt_Y;
		m_result.dRotateAngle = MySixAxisInfo.RotationAngle;
		m_result.dShiftX = MySixAxisInfo.Center_X  + m_param.ClipImage.left - nWidth/2;
		m_result.dShiftY = MySixAxisInfo.Center_Y  + m_param.ClipImage.top- nHeight/2;

		uts.log.Debug(_T("TiltX : %.2f TiltY : %.2f Rotate:%.2f"),m_result.dTiltX,m_result.dTiltY,m_result.dRotateAngle);
		uts.log.Debug(_T("Center(x,y) = (%.1f,%.1f)"),m_result.dShiftX,m_result.dShiftY);

		CStringA strHeader, strKey;

		// 保存结果到共享(R)
		strHeader = (uts.info.nDeviceIndex == 0 ? "right" : "left");
		strKey.Format("%s_dTiltX", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dTiltX;
		strKey.Format("%s_dTiltY", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dTiltY;
		strKey.Format("%s_dRotateAngle", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dRotateAngle;
		strKey.Format("%s_Center_X", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dShiftX ;
		strKey.Format("%s_Center_Y", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dShiftY;
		strKey.Format("%s_FOV_Z", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = MySixAxisInfo.FOV_Z;
		strKey.Format("%s_PixelSize", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_param.dPixelSize;
		*/
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

		int roiw = m_param.ClipImage.right - m_param.ClipImage.left; 
		int roiH = m_param.ClipImage.bottom - m_param.ClipImage.top; 
		unsigned char *pROIBuffer = new unsigned char[roiw*roiH*3];
		unsigned char *pROIBuffer2 = new unsigned char[roiw*roiH*3];

		memset(pROIBuffer,0,sizeof(unsigned char)*roiw*roiH*3);
		memset(pROIBuffer2,0,sizeof(unsigned char)*roiw*roiH*3);

		//------------------------------------------------------------------------------
		// 测试
		algorithm::geometry::multisfr::ChessboardCaptureInfo MyChessboardCaptureInfo;

		MyChessboardCaptureInfo.PixelSize = m_param.dPixelSize;		//(um)
		MyChessboardCaptureInfo.EFL = m_param.dLensEFL;				//(mm)

		//Chart size ???
		MyChessboardCaptureInfo.ChartWidth = 887.95;		//(mm)
		MyChessboardCaptureInfo.ChartHeight = 727.53;		//(mm)
		MyChessboardCaptureInfo.CaptureDistance = 600.0;	//(mm)

		//由FOV算真?狀況
		MyChessboardCaptureInfo.DFOV = m_param.dLensDFOV;				//(°)

		MyChessboardCaptureInfo.ImageWidth = roiw;
		MyChessboardCaptureInfo.ImageHeight = roiH;

		MyChessboardCaptureInfo.IntersectionXCount = m_param.szPtCount.cx;
		MyChessboardCaptureInfo.IntersectionYCount = m_param.szPtCount.cy;

		CImageProc::GetInstance().GetBMPBlockBuffer(
			m_bufferObj.pBmpBuffer,
			pROIBuffer,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight,
			m_param.ClipImage.left,
			m_param.ClipImage.top,
			roiw,
			roiH);

		CImageProc::GetInstance().GetFlipedBmpBGRtoRGBBuffer(
			pROIBuffer,
			roiw,
			roiH,
			pROIBuffer2);

		RELEASE_ARRAY(pROIBuffer);

		//?算棋盤Chart的點位置
		algorithm::geometry::multisfr::SixAxisInfo MySixAxisInfo;
		bool res = CameraGeometyRelationship(pROIBuffer2,
								  roiw,
								  roiH,
								  MyChessboardCaptureInfo,
								  MySixAxisInfo);

		RELEASE_ARRAY(pROIBuffer2);

		m_result.dTiltX = MySixAxisInfo.Tilt_X ;
		m_result.dTiltY = MySixAxisInfo.Tilt_Y;
		m_result.dRotateAngle = MySixAxisInfo.RotationAngle;
		m_result.dShiftX = MySixAxisInfo.Center_X  + m_param.ClipImage.left - m_bufferInfo.nWidth/2;
		m_result.dShiftY = MySixAxisInfo.Center_Y  + m_param.ClipImage.top- m_bufferInfo.nHeight/2;

		uts.log.Debug(_T("TiltX : %.2f TiltY : %.2f Rotate:%.2f"),m_result.dTiltX,m_result.dTiltY,m_result.dRotateAngle);
		uts.log.Debug(_T("Center(x,y) = (%.1f,%.1f)"),m_result.dShiftX,m_result.dShiftY);
		// 保存结果到共享(R)
		strHeader = (uts.info.nDeviceIndex == 0 ? "right" : "left");
		strKey.Format("%s_dTiltX", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dTiltX;
		strKey.Format("%s_dTiltY", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dTiltY;
		strKey.Format("%s_dRotateAngle", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dRotateAngle;
		strKey.Format("%s_Center_X", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dShiftX ;
		strKey.Format("%s_Center_Y", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dShiftY;
		strKey.Format("%s_FOV_Z", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = MySixAxisInfo.FOV_Z;
		strKey.Format("%s_PixelSize", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_param.dPixelSize;

		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		//-------------------------------------------------------------------------


		//------------------------------------------------------------------------------
		// 判断规格(回转)
		*pnErrorCode = uts.errorcode.E_Pass;

		if ( res == false
			||   m_result.dTiltX < m_param.dTiltXAngleSpec.min
			||   m_result.dTiltX > m_param.dTiltXAngleSpec.max
			||	 m_result.dTiltY < m_param.dTiltYAngleSpec.min
			||   m_result.dTiltY > m_param.dTiltYAngleSpec.max
			||	 m_result.dRotateAngle < m_param.dRotateAngleSpec.min
			||   m_result.dRotateAngle > m_param.dRotateAngleSpec.max)
		{
			*pnErrorCode = uts.errorcode.E_Rotate;
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

		strHeader.Append(_T("RotateAngle,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dRotateAngle);
		strHeader.Append(_T("TiltX,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dTiltX);
		strHeader.Append(_T("TiltY,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dTiltY);
		strHeader.Append(_T("ShiftX,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dShiftX);
		strHeader.Append(_T("ShiftY,"));
		strData.AppendFormat(_T("%.4f,"), m_result.dShiftY);
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
