#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_MTF_Ref.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef enum MTF_REF
{
	Center = 0,
	LeftTop,
	LeftBottom,
	RightTop,
	RightBottom,
	MTF_COUNT,
} ;


namespace UTS
{
	CString strROIName[MTF_COUNT] = {
		_T("CT"),
		_T("LeftTop"),
		_T("LeftBottom"),
		_T("RightTop"),
		_T("RightBottom"),
	};
	CString strDefaultValue[MTF_COUNT] = {
		_T("295,215,64,64"),
		_T("39,23,64,64"),
		_T("39,407,64,64"),
		_T("551,23,64,64"),
		_T("551,407,64,64"),
	};

	void ShiftMTFROI(MTF::MTF_Nintendo::MTF_PARAM &param,unsigned char *pYBuffer,int nWidth,int nHeight,int MaskROI,int MaskGap)
	{
		double dAvgY,dMaxY,dMinY;
		POINT NewLTPoint;

		for (int i = 0; i < param.nMtfROICount; i++)
		{
			// get ROI white & black
			GetBlockAvgY(pYBuffer, nWidth, nHeight, param.arrMtfROIPos[i], dAvgY);

			if(pYBuffer[param.arrMtfROIPos[i].top* nWidth + param.arrMtfROIPos[i].left ] > dAvgY) //White
			{
				dMaxY = dAvgY;
				NewLTPoint.x = param.arrMtfROIPos[i].left;
				NewLTPoint.y = param.arrMtfROIPos[i].top;

				for (int y = (param.arrMtfROIPos[i].top - MaskROI); y <= (param.arrMtfROIPos[i].top + MaskROI); y++)
				{
					for (int x = (param.arrMtfROIPos[i].left - MaskROI); x <= (param.arrMtfROIPos[i].left + MaskROI); x++)
					{
						if((pYBuffer[y * nWidth + x] - MaskGap) > dMaxY) 
						{
							dMaxY = pYBuffer[y * nWidth + x];
							NewLTPoint.x = x;
							NewLTPoint.y = y;

						}
					}
				}
			}else
			{
				dMinY = dAvgY;
				NewLTPoint.x = param.arrMtfROIPos[i].left;
				NewLTPoint.y = param.arrMtfROIPos[i].top;

				for (int y = (param.arrMtfROIPos[i].top - MaskROI); y <= (param.arrMtfROIPos[i].top + MaskROI); y++)
				{
					for (int x = (param.arrMtfROIPos[i].left - MaskROI); x <= (param.arrMtfROIPos[i].left + MaskROI); x++)
					{
						if((pYBuffer[y * nWidth + x] + MaskGap) < dMinY) 
						{
							dMinY = pYBuffer[y * nWidth + x];
							NewLTPoint.x = x;
							NewLTPoint.y = y;

						}
					}
				}
			}

			//Shift New MTF ROI
			int nBlockWidth  = param.arrMtfROIPos[i].right - param.arrMtfROIPos[i].left;
			int nBlockHeight = param.arrMtfROIPos[i].bottom - param.arrMtfROIPos[i].top;

			param.arrMtfROIPos[i].left = NewLTPoint.x;
			param.arrMtfROIPos[i].right = param.arrMtfROIPos[i].left + nBlockWidth;
			param.arrMtfROIPos[i].top = NewLTPoint.y;
			param.arrMtfROIPos[i].bottom = param.arrMtfROIPos[i].top + nBlockHeight;

		}


	}

	double GetBalanceValue(double dRes1, double dRes2)
	{
		
		return (abs(dRes1 - dRes2));
	}

	MTFOperator::MTFOperator(void)
	{
		OPERATOR_INIT;
	}

	MTFOperator::~MTFOperator(void)
	{
	}

	BOOL MTFOperator::OnReadSpec()
	{
		int nValue = 0;
		CString strValue;
		vector<int> vecValue;
		vector<double> vecDoubleValue;
		CString strSection = OSUtil::GetFileName(m_strModuleFile);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nMTFShiftMask"), m_param.nMTFShiftMask, 3, _T("MTF Shift Mask"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nMTFShiftMaskGap"), m_param.nMTFShiftMaskGap, 3, _T("MTF Shift Mask Gap"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Spec.dBalanceMTFSpec"), strValue, _T("0,10"), _T("CenterMTFSpec(Min,Max)"));
		SplitInt(strValue, vecValue);
		m_param.stBalanceSpec.min = vecValue[0];
		m_param.stBalanceSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Spec.dCenterSpec"), strValue, _T("60,100"), _T("CenterMTFSpec(Min,Max)"));
		SplitInt(strValue, vecValue);
		m_param.stMTFCenterSpec.min = vecValue[0];
		m_param.stMTFCenterSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Spec.dFieldSpec"), strValue, _T("60,100"), _T("FieldMTFSpec(Min,Max)"));
		SplitInt(strValue, vecValue);
		m_param.stMTFFieldSpec.min = vecValue[0];
		m_param.stMTFFieldSpec.max = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nMtfROICount"), m_param.stMtfParam.nMtfROICount, 5, _T("MTF ROI数量"));
		for (int i = 0; i < m_param.stMtfParam.nMtfROICount; i++)
		{
			CString strKeyName;
			strKeyName.Format(_T("stMtfROIInfo[%s]"), strROIName[i]);
			CString strComment;
			strComment.Format(_T("ROI[%s](左,上,宽,高)"), strROIName[i]);
			uts.dbCof.GetOperatorSingleSpec(strSection, strKeyName, strValue, strDefaultValue[i], strComment);
			SplitInt(strValue, vecValue);
			m_param.stMtfParam.arrMtfROIPos[i].left = vecValue[0];
			m_param.stMtfParam.arrMtfROIPos[i].top = vecValue[1];
			m_param.stMtfParam.arrMtfROIPos[i].right = vecValue[0] + vecValue[2];
			m_param.stMtfParam.arrMtfROIPos[i].bottom = vecValue[1] + vecValue[3];
		}

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("stRefWhiteInfo"), strValue, _T("180,225,48,48,40,180"), _T("参考白ROI(左,上,宽,高,最小亮度,最大亮度)"));
		SplitInt(strValue, vecValue);
		m_param.stMtfParam.rcRefWhite.left = vecValue[0];
		m_param.stMtfParam.rcRefWhite.top = vecValue[1];
		m_param.stMtfParam.rcRefWhite.right = vecValue[0] + vecValue[2];
		m_param.stMtfParam.rcRefWhite.bottom = vecValue[1] + vecValue[3];
		m_param.stRefWhiteSpec.min = vecValue[4];
		m_param.stRefWhiteSpec.max = vecValue[5];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("stRefBlackInfo"), strValue, _T("432,225,48,48,0,40"), _T("参考黑ROI(左,上,宽,高,最小亮度,最大亮度)"));
		SplitInt(strValue, vecValue);
		m_param.stMtfParam.rcRefBlack.left = vecValue[0];
		m_param.stMtfParam.rcRefBlack.top = vecValue[1];
		m_param.stMtfParam.rcRefBlack.right = vecValue[0] + vecValue[2];
		m_param.stMtfParam.rcRefBlack.bottom = vecValue[1] + vecValue[3];
		m_param.stRefBlackSpec.min = vecValue[4];
		m_param.stRefBlackSpec.max = vecValue[5];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("ptInfoPos"), strValue, _T("100,100"), _T("过程信息位置"));
		SplitInt(strValue, vecValue);
		m_param.ptInfoPos.x = vecValue[0];
		m_param.ptInfoPos.y = vecValue[1];


		return TRUE;
	}

	void MTFOperator::OnPreviewStart()
	{
		//-------------------------------------------------------------------------
		// 为调焦做准备
		m_result.dPeakMTF = 0.0;
		
	}


	BOOL MTFOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{		
		//-------------------------------------------------------------------------
		// 计算
		m_pDevice->GetBufferInfo(m_bufferInfo);
		if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_YUV24) != 0)
		{
			CImageProc::GetInstance().GetYUV24_YBuffer(
				bufferObj.pYuv24Buffer,
				nWidth,
				nHeight,
				bufferObj.pYBuffer);
		}
		else if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
		{
			CImageProc::GetInstance().Cal_RGBtoYBuffer(
				bufferObj.pBmpBuffer,
				nWidth,
				nHeight,
				bufferObj.pYBuffer);
			unsigned char *pBlockBuffer = new unsigned char[nWidth * nHeight];
			CImageProc::GetInstance().GetFlipedYBlockBuffer(
				bufferObj.pYBuffer,
				pBlockBuffer,
				nWidth, nHeight,
				0, 0,
				nWidth, nHeight);
			memcpy(bufferObj.pYBuffer, pBlockBuffer, nWidth * nHeight);
			RELEASE_ARRAY(pBlockBuffer);
		}

		//------------------------------------------------------------------------------
		BOOL bWhiteReady = FALSE;
		BOOL bBlackReady = FALSE;
		MTF::MTF_Nintendo::MTF_PARAM MTFParam;

		memcpy(&MTFParam,&m_param.stMtfParam,sizeof(MTF::MTF_Nintendo::MTF_PARAM));

		ShiftMTFROI(MTFParam,bufferObj.pYBuffer,nWidth,nHeight,m_param.nMTFShiftMask,m_param.nMTFShiftMaskGap);

		MTF::MTF_Nintendo::GetAllMTF_Y(
			bufferObj.pYBuffer,
			nWidth,
			nHeight,
			MTFParam,
			m_result.stMtfResult);

		//  [9/6/2015 Cindy]
		bWhiteReady = ((m_result.stMtfResult.dRefWhite_Min >= m_param.stRefWhiteSpec.min) &&
			(m_result.stMtfResult.dRefWhiteAvg  <= m_param.stRefWhiteSpec.max));

		bBlackReady = ((m_result.stMtfResult.dRefBlack_Max <= m_param.stRefBlackSpec.max) &&
			(m_result.stMtfResult.dRefBlackAvg  >= m_param.stRefBlackSpec.min));

		if (!bWhiteReady || !bBlackReady)
		{
			for (int i = 0; i < m_param.stMtfParam.nMtfROICount; i++)
			{
				m_result.stMtfResult.arrMTF[i] = 0.0;
			}
		}

		// MTF CT Peak
		if (m_result.stMtfResult.arrMTF[MTF_Center] > m_result.dPeakMTF)
		{
			m_result.dPeakMTF = m_result.stMtfResult.arrMTF[Center];
			
		}

		m_result.dBalanceLTRD = GetBalanceValue(m_result.stMtfResult.arrMTF[LeftTop], m_result.stMtfResult.arrMTF[RightBottom]);
		m_result.dBalanceRTLD = GetBalanceValue(m_result.stMtfResult.arrMTF[RightTop], m_result.stMtfResult.arrMTF[LeftBottom]);

		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_POINT pt;
		UI_TEXT text;
		UI_TEXT_REL textRel;
		UI_LINE line;

		// ref white
		rc.color = text.color = (bWhiteReady ? COLOR_BLUE : COLOR_RED);
		memcpy(&rc.rcPos, &m_param.stMtfParam.rcRefWhite, sizeof(RECT));
		uiMark.vecUiRect.push_back(rc);
		text.ptPos = CPoint(rc.rcPos.left - 130, rc.rcPos.bottom);
		text.strText.Format(_T("W_Y(%.1f %.1f)"),m_result.stMtfResult.dRefWhite_Min,m_result.stMtfResult.dRefWhiteAvg);

		uiMark.vecUIText.push_back(text);
		// ref black
		rc.color = text.color = (bBlackReady ? COLOR_BLUE : COLOR_RED);
		memcpy(&rc.rcPos, &m_param.stMtfParam.rcRefBlack, sizeof(RECT));
		uiMark.vecUiRect.push_back(rc);
		text.ptPos = CPoint(rc.rcPos.left - rc.rcPos.Width() / 4 - 30, rc.rcPos.bottom);
		text.strText.Format(_T("B_Y(%.1f %.1f)"),m_result.stMtfResult.dRefBlack_Max ,m_result.stMtfResult.dRefBlackAvg);
		uiMark.vecUIText.push_back(text);

		// mtf roi
		for (int i = 0; i < m_param.stMtfParam.nMtfROICount; i++)
		{
			if(i == Center)
			rc.color = text.color = ((m_result.stMtfResult.arrMTF[i] < m_param.stMTFCenterSpec.min || m_result.stMtfResult.arrMTF[i] > m_param.stMTFCenterSpec.max) ? COLOR_RED : COLOR_LIME);
			else
			rc.color = text.color = ((m_result.stMtfResult.arrMTF[i] < m_param.stMTFFieldSpec.min || m_result.stMtfResult.arrMTF[i] > m_param.stMTFFieldSpec.max) ? COLOR_RED : COLOR_LIME);
			
			memcpy(&rc.rcPos, &MTFParam.arrMtfROIPos[i], sizeof(RECT));
			uiMark.vecUiRect.push_back(rc);
			text.ptPos = CPoint(rc.rcPos.left, rc.rcPos.bottom);
			text.strText.Format(_T("%.1f"), m_result.stMtfResult.arrMTF[i]);
			uiMark.vecUIText.push_back(text);
		}

		// info
		textRel.color = ((m_result.stMtfResult.arrMTF[0] < m_param.stMTFCenterSpec.min || m_result.stMtfResult.arrMTF[0] > m_param.stMTFCenterSpec.max) ? COLOR_RED : COLOR_BLUE);
		textRel.dWidthPersent = (double)m_param.ptInfoPos.x / nWidth;
		textRel.dHeightPersent = (double)m_param.ptInfoPos.y / nHeight;
		textRel.strText.Format(_T("Peak: %.1f"), m_result.dPeakMTF);
		uiMark.vecUITextRel.push_back(textRel);

		textRel.color = ((m_result.dBalanceLTRD < m_param.stBalanceSpec.min || m_result.dBalanceLTRD > m_param.stBalanceSpec.max) ? COLOR_RED : COLOR_BLUE);
		textRel.dWidthPersent = (double)m_param.ptInfoPos.x / nWidth;
		textRel.dHeightPersent = (double)m_param.ptInfoPos.y / nHeight + 0.04;
		textRel.strText.Format(_T("Balance LT-RD: %.1f"), m_result.dBalanceLTRD);
		uiMark.vecUITextRel.push_back(textRel);

		textRel.color = ((m_result.dBalanceRTLD < m_param.stBalanceSpec.min || m_result.dBalanceRTLD > m_param.stBalanceSpec.max) ? COLOR_RED : COLOR_BLUE);
		textRel.dWidthPersent = (double)m_param.ptInfoPos.x / nWidth;
		textRel.dHeightPersent = (double)m_param.ptInfoPos.y / nHeight + 0.08;
		textRel.strText.Format(_T("Balance RT-LD: %.1f"), m_result.dBalanceRTLD);
		uiMark.vecUITextRel.push_back(textRel);

		return TRUE;
	}

	BOOL MTFOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		UI_MARK uiMark;
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);

		//------------------------------------------------------------------------------
		// 初始化结果
		*pnErrorCode = uts.errorcode.E_Pass;

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


		//------------------------------------------------------------------------------
		// 测试
		OnPreview(m_bufferObj, m_bufferInfo.nWidth, m_bufferInfo.nHeight, uiMark);
		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

		//------------------------------------------------------------------------------
		// 判断规格
		// ref Y
		if (m_result.stMtfResult.dRefWhiteAvg < m_param.stRefWhiteSpec.min
			|| m_result.stMtfResult.dRefWhiteAvg > m_param.stRefWhiteSpec.max
			|| m_result.stMtfResult.dRefBlackAvg < m_param.stRefBlackSpec.min 
			|| m_result.stMtfResult.dRefBlackAvg > m_param.stRefBlackSpec.max 
			)
		{
			*pnErrorCode = uts.errorcode.E_Linumance;
			goto end;
		}
		// mtf
		for (int i = 0; i < m_param.stMtfParam.nMtfROICount; i++)
		{
			if (i == Center)
			{
				if (m_result.stMtfResult.arrMTF[i] < m_param.stMTFCenterSpec.min || m_result.stMtfResult.arrMTF[i] > m_param.stMTFCenterSpec.max)
				{
					*pnErrorCode = uts.errorcode.E_FocusCheck;
					goto end;
				}

			}else if (m_result.stMtfResult.arrMTF[i] < m_param.stMTFFieldSpec.min || m_result.stMtfResult.arrMTF[i] > m_param.stMTFFieldSpec.max)
			{
				*pnErrorCode = uts.errorcode.E_FocusCheck;
				goto end;
			}
		}
		// balance
		if (m_result.dBalanceLTRD < m_param.stBalanceSpec.min
			|| m_result.dBalanceLTRD > m_param.stBalanceSpec.max
			|| m_result.dBalanceRTLD < m_param.stBalanceSpec.min
			|| m_result.dBalanceRTLD > m_param.stBalanceSpec.max)
		{
			*pnErrorCode = uts.errorcode.E_ERR_MTFDelta;
			goto end;
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

	void MTFOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
		vecReturnValue.push_back(uts.errorcode.E_FocusCheck);
		vecReturnValue.push_back(uts.errorcode.E_ERR_MTFDelta);
		
	}

	void MTFOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		vecRegister.push_back(m_strOperatorName);
	}

	void MTFOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
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

		// ref
		strHeader.AppendFormat(_T("RefWhite,RefBlack,"));
		strData.AppendFormat(_T("%.1f,%.1f,"),
			m_result.stMtfResult.dRefWhiteAvg,
			m_result.stMtfResult.dRefBlackAvg);

		// mtf
		for (int i = 0; i < m_param.stMtfParam.nMtfROICount; i++)
		{
			strHeader.AppendFormat(_T("MTF[%s],"), strROIName[i]);
			strData.AppendFormat(_T("%.1f,"), m_result.stMtfResult.arrMTF[i]);
		}

		// balance
		strHeader.Append(_T("BL_LT_RD,BL_RT_LD,"));
		strData.AppendFormat(_T("%.1f,%.1f,"), m_result.dBalanceLTRD, m_result.dBalanceRTLD);

		// peak
		strHeader.Append(_T("Peak,"));
		strData.AppendFormat(_T("%.1f,"), m_result.dPeakMTF);

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
		return (new MTFOperator);
	}
	//------------------------------------------------------------------------------
}
