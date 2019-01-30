#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_SaveImage.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::Image;

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

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSaveimageMode"), m_param.nSaveImageMode, 1, _T("0: Null 1: BMP 2:RAW8 3:RAW10 4:BMP_RAW8"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));

		CString strValue;
		vector<int> vecValue;
		vector<double> vecDoubleValue;

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("WhitePatchROI"), strValue,  _T("640,480,200,200"), _T("ROI(Left,Top,width,Height)"));
		SplitInt(strValue, vecValue);
		m_param.WhitePatchROI.left = vecValue[0];
		m_param.WhitePatchROI.top = vecValue[1];
		m_param.WhitePatchROI.right = vecValue[0] + vecValue[2];
		m_param.WhitePatchROI.bottom = vecValue[1] + vecValue[3];

		return TRUE;
	}

	BOOL ImplOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		//------------------------------------------------------------------------------
		// 判断画面平均亮度
		int nROISizeX,nROISizeY;
		nROISizeX = m_param.WhitePatchROI.right - m_param.WhitePatchROI.left;
		nROISizeY = m_param.WhitePatchROI.bottom - m_param.WhitePatchROI.top;

		unsigned char *pROIDataWhitePatch = new BYTE[nROISizeX * nROISizeY * 3];
		memset(pROIDataWhitePatch, 0, nROISizeX * nROISizeY * 3);
		CImageProc::GetInstance().GetBMPBlockBuffer(
			pBmpBuffer,
			pROIDataWhitePatch,
			nWidth,
			nHeight,
			m_param.WhitePatchROI.left,
			m_param.WhitePatchROI.top,
			nROISizeX,
			nROISizeY);

		UTS::Algorithm::CalYavg(pROIDataWhitePatch, nROISizeX, nROISizeY, m_result.dCenterYavg);
		
		RELEASE_ARRAY(pROIDataWhitePatch);

		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_TEXT text;
	
		rc.color = COLOR_BLUE;

		if (m_result.dCenterYavg < m_param.dLTMinY || 
			m_result.dCenterYavg > m_param.dLTMaxY)
		{
			rc.color = COLOR_RED;
		}

		rc.rcPos = m_param.WhitePatchROI;
		uiMark.vecUiRect.push_back(rc);
		text.color = rc.color;
		text.ptPos = CPoint(rc.rcPos.left, rc.rcPos.bottom);
		text.strText.Format(_T("%.1f"), m_result.dCenterYavg);
		uiMark.vecUIText.push_back(text);

		//------------------------------------------------------------------------------

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
		memset(&m_result,0,sizeof(OPERATOR_RESULT));
		//------------------------------------------------------------------------------
		// 抓图
		CString index;
		CString strDirPath;
		CString strFilePath;
		SYSTEMTIME st;

		// 抓图
		if (!m_pDevice->Recapture(m_bufferObj))
		{
			uts.log.Error(_T("Recapture error. "));
			*pnErrorCode = uts.errorcode.E_NoImage;
			return m_bResult;
		}

		//------------------------------------------------------------------------------
		// 判断画面平均亮度
		int nROISizeX,nROISizeY;
		nROISizeX = m_param.WhitePatchROI.right - m_param.WhitePatchROI.left;
		nROISizeY = m_param.WhitePatchROI.bottom - m_param.WhitePatchROI.top;

		unsigned char *pROIDataWhitePatch = new BYTE[nROISizeX * nROISizeY * 3];
		memset(pROIDataWhitePatch, 0, nROISizeX * nROISizeY * 3);
		CImageProc::GetInstance().GetBMPBlockBuffer(
			m_bufferObj.pBmpBuffer,
			pROIDataWhitePatch,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight,
			m_param.WhitePatchROI.left,
			m_param.WhitePatchROI.top,
			nROISizeX,
			nROISizeY);

		Algorithm::CalYavgExp(
			pROIDataWhitePatch,
			nROISizeX,
			nROISizeY,
			m_result.dCenterYavg);

		RELEASE_ARRAY(pROIDataWhitePatch);

		if (m_result.dCenterYavg < m_param.dLTMinY || 
			m_result.dCenterYavg > m_param.dLTMaxY)
		{
			*pnErrorCode = uts.errorcode.E_Linumance;
			goto end;
		}

		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// 使用上次的抓图
		m_pDevice->GetCapturedBuffer(m_bufferObj);
		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		BYTE *RAW10Image_HL = new BYTE[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
	
		//-------------------------------------------------------------------------
		USES_CONVERSION;
		//Save Dump FW 

		GetLocalTime(&st);

		strDirPath.Format(_T("%s\\XCal_%04d%02d%02d\\"),
			uts.info.strResultPath,
			st.wYear,
			st.wMonth, 
			st.wDay);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);

		strDirPath.Format(_T("%s\\XCal_%04d%02d%02d\\%s"),
			uts.info.strResultPath,
			st.wYear,
			st.wMonth, 
			st.wDay,
			uts.info.strSN);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		
		
		index = (uts.info.nDeviceIndex == 0 ? "L" : "R");
		
		strFilePath.Format(_T("%s\\%s"),strDirPath,index);

		//////////////////////////////////////////////////////////////////////////
		
		switch (m_param.nSaveImageMode)
		{
		case Image_TYPE_BMP:
			if (!uts.imgFile.SaveBmpFile(strFilePath,m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
			{
				*pnErrorCode = uts.errorcode.E_Fail;
			}
			break;
		case Image_TYPE_RAW8:
			if (!uts.imgFile.SaveRawFile(strFilePath,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
			{
				*pnErrorCode = uts.errorcode.E_Fail;
			}
			break;
		case Image_TYPE_RAW10:
			CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_HL,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
			if (!uts.imgFile.SaveRawFile(strFilePath,RAW10Image_HL,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
			{
				*pnErrorCode = uts.errorcode.E_Fail;
			}
			break;
		case Image_TYPE_RAW8BMP:
			strFilePath.Append(_T(".bmp"));
			if (!uts.imgFile.WriteBMPImage(strFilePath,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,1))
			{
				*pnErrorCode = uts.errorcode.E_Fail;
			}


			break;
		}
		
		RELEASE_ARRAY(RAW10Image_HL);
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
		vecReturnValue.push_back(uts.errorcode.E_Linumance);
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

		strHeader = _T("Time,SN,TestTime(ms),Y_Avg,Result,")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%.1f,%s,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(),m_result.dCenterYavg, strResult
			, strVersion, uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
