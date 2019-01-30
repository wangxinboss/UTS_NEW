#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_CheckWhiteChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace UTS
{
	ImplOperator::ImplOperator(void)
	{
		OPERATOR_INIT;
		memset(&m_result,0,sizeof(OPERATOR_RESULT));
	}

	ImplOperator::~ImplOperator(void)
	{
	}

	BOOL ImplOperator::OnReadSpec()
	{
		CString strValue;
		vector<int> vecValue;
		vector<double> vecDoubleValue;

		CString strSection = OSUtil::GetFileName(m_strModuleFile);

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iTryCount"), m_param.iTryCount, 20, _T("TryCount"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iCheckDelta"), m_param.iCheckDelta, 10, _T("CheckDelta"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("stAEInfo"), strValue, _T("288,224,64,32"), _T("AE_ROI(左,上,宽,高)"));
		SplitInt(strValue, vecValue);
		m_param.stAEParam.left = vecValue[0];
		m_param.stAEParam.top = vecValue[1];
		m_param.stAEParam.right = vecValue[0] + vecValue[2];
		m_param.stAEParam.bottom = vecValue[1] + vecValue[3];

		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		UI_MARK uiMark;
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);
		*pnErrorCode = uts.errorcode.E_Pass;
		//------------------------------------------------------------------------------
		int _count = 0;
		RGBTRIPLE rgb;
	
		while (_count < m_param.iTryCount)
		{
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

			UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer, 
				m_bufferInfo.nWidth, 
				m_bufferInfo.nHeight, 
				0, m_param.stAEParam, rgb);

			m_result.m_nowdvalue = YVALUE(rgb.rgbtBlue, rgb.rgbtGreen, rgb.rgbtRed);

			uts.log.Debug(_T("Light: %.2f Count :%d"),m_result.m_nowdvalue,_count);

			if( abs(m_result.m_nowdvalue - m_result.m_predvalue) > m_param.iCheckDelta )
			{
				_count = 0;
			}else
				_count ++;

			m_result.m_predvalue = m_result.m_nowdvalue;

			// 画图
			m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
		}

		//------------------------------------------------------------------------------
		// 判断规格
	
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
	}

	void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		
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

		strHeader.Append(_T("Value,"));
		strData.AppendFormat(_T("%.1f,"), 
			m_result.m_nowdvalue);
			
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
