#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_RelShift_SFR_Single.h"
#include "Algorithm.h"

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
		vector<int> vecValue2;

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("ClipImage[top,left,bottom,right]"), strValue, _T("0,0,960,1280"), _T("[top,left,bottom,right]"));
		SplitInt(strValue, vecValue2);
		m_param.ClipImage.top = vecValue2[0];
		m_param.ClipImage.left = vecValue2[1];
		m_param.ClipImage.bottom = vecValue2[2];
		m_param.ClipImage.right = vecValue2[3];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("EFL"), m_param.dEFL, 2.296, _T("Module EFL."));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Pixelsize"), m_param.dPixelsize, 3.75, _T("Module dPixelsize."));

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
		memset(pROIBuffer,0,sizeof(unsigned char)*roiw*roiH*3);
	
		CImageProc::GetInstance().GetBMPBlockBuffer(
			m_bufferObj.pBmpBuffer,
			pROIBuffer,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight,
			m_param.ClipImage.left,
			m_param.ClipImage.top,
			roiw,
			roiH);

		UTS::Algorithm::SFRBlock::SFRROI sfrroi;
		if(ROI_SFR(pROIBuffer,roiw,roiH,&sfrroi) == false)
		{
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}
		
		m_result.dShiftX = sfrroi.x  + m_param.ClipImage.left ;
		m_result.dShiftY = sfrroi.y  + m_param.ClipImage.top;

		uts.log.Debug(_T("Center(x,y) = (%.1f,%.1f)"),m_result.dShiftX,m_result.dShiftY);
		
		// 保存结果到共享(R)
		strHeader = (uts.info.nDeviceIndex == 0 ? "right" : "left");
		strKey.Format("%s_Center_X", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dShiftX ;
		strKey.Format("%s_Center_Y", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_result.dShiftY;
		strKey.Format("%s_EFL", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_param.dEFL;
		strKey.Format("%s_Pixelsize", strHeader);
		uts.info.mapShared[strKey.GetBuffer()] = m_param.dPixelsize;

		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		//-------------------------------------------------------------------------


		//------------------------------------------------------------------------------
		// 判断规格(回转)

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
		strData.AppendFormat(_T("%.1f,"), m_result.dShiftX);
		strHeader.Append(_T("ShiftY,"));
		strData.AppendFormat(_T("%.1f,"), m_result.dShiftY);
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
