#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_RelShift_MultiChart.h"

#include "../../3rdparty/MultiSFR/algorithm_geometry_multisfr.h"
#pragma comment(lib, "algorithm.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
	double ImplOperator::CalculateTiltX(OPERATOR_RESULT result)
	{
		double dTilt = 0.0;

		double dis1 =  Math::GetDistance2D(DOUBLE2LONG(result.ptP[0].x),DOUBLE2LONG(result.ptP[0].y),
			                               DOUBLE2LONG(result.ptQ[0].x),DOUBLE2LONG(result.ptQ[0].y));

		double dis2 =  Math::GetDistance2D(DOUBLE2LONG(result.ptP[2].x),DOUBLE2LONG(result.ptP[2].y),
										   DOUBLE2LONG(result.ptQ[2].x),DOUBLE2LONG(result.ptQ[2].y));

		dTilt = dis1/dis2;

		return dTilt;
	}

	double ImplOperator::CalculateTiltY(OPERATOR_RESULT result)
	{
		double dTilt = 0.0;

		double dis1 =  Math::GetDistance2D(DOUBLE2LONG(result.ptP[0].x),DOUBLE2LONG(result.ptP[0].y),
										   DOUBLE2LONG(result.ptP[2].x),DOUBLE2LONG(result.ptP[2].y));

		double dis2 =  Math::GetDistance2D(DOUBLE2LONG(result.ptQ[0].x),DOUBLE2LONG(result.ptQ[0].y),
										   DOUBLE2LONG(result.ptQ[2].x),DOUBLE2LONG(result.ptQ[2].y));

		dTilt = dis1/dis2;

		return dTilt;
	}



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
  
        m_param.szPtCount.cx = 31;
        m_param.szPtCount.cy = 19;

		for (int j = 0; j < 3; j++)
		{
			m_param.nPIndex[j] = (j*8 + 1) * m_param.szPtCount.cx + 1;
			m_param.nSIndex[j] = m_param.nPIndex[j] + 14;
			m_param.nQIndex[j] = m_param.nSIndex[j] + 14;
		}

        CString strValue;
        vector<double> vecValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dRotateAngleSpec[Min,Max]"), strValue, _T("-1.5,1.5"), _T("回转规格(顺时针旋转为正)"));
        SplitDouble(strValue, vecValue);
        m_param.dRotateAngleSpec[Min] = vecValue[0];
        m_param.dRotateAngleSpec[Max] = vecValue[1];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dTiltXAngleSpec"), strValue, _T("-0.95,1.05"), _T("TiltX"));
		SplitDouble(strValue, vecValue);
		m_param.dTiltXAngleSpec[Min] = vecValue[0];
		m_param.dTiltXAngleSpec[Max] = vecValue[1];


		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dTiltYAngleSpec"), strValue, _T("-0.95,1.05"), _T("TiltY"));
		SplitDouble(strValue, vecValue);
		m_param.dTiltYAngleSpec[Min] = vecValue[0];
		m_param.dTiltYAngleSpec[Max] = vecValue[1];

		//vecValue.clear();

        return TRUE;
    }

    BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        m_pDevice->GetBufferInfo(m_bufferInfo);

		//foundation::dCoordinate MultiSFRIntersectionCoordinate[600];
		foundation::dCoordinate *MultiSFRIntersectionCoordinate = new foundation::dCoordinate[m_param.szPtCount.cx*m_param.szPtCount.cy];
		memset(MultiSFRIntersectionCoordinate,0,sizeof(foundation::dCoordinate)*m_param.szPtCount.cx*m_param.szPtCount.cy);
                     
		if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
		{
			if (!algorithm::geometry::multisfr::GetMultiSFRIntersectionCoordinate(bufferObj.pBmpBuffer,
				nWidth,
				nHeight,
				m_param.szPtCount.cx,
				m_param.szPtCount.cy,
				MultiSFRIntersectionCoordinate))
			{
				uts.log.Error(_T("GetChessboardPos error. "));
				RELEASE_ARRAY(MultiSFRIntersectionCoordinate);
				return TRUE;
			}
			
            //------------------------------------------------------------------------------
            // OSD绘制
            UI_RECT rc;
            UI_POINT pt;
            UI_TEXT text;
            UI_TEXT_REL textRel;
            UI_LINE line;

			//
			for (int y = 0 ; y < m_param.szPtCount.cy ; y++ )
			{
				for (int x = 0; x < m_param.szPtCount.cx ; x++ )
				{
					MultiSFRIntersectionCoordinate[ y * m_param.szPtCount.cx+x ].y = nHeight - MultiSFRIntersectionCoordinate[ y * m_param.szPtCount.cx+x ].y;
					
					pt.color = COLOR_LIME;
					pt.ptPos.x = DOUBLE2LONG(MultiSFRIntersectionCoordinate[ y * m_param.szPtCount.cx+x ].x );
					pt.ptPos.y = DOUBLE2LONG(MultiSFRIntersectionCoordinate[ y * m_param.szPtCount.cx+x ].y );

					uiMark.vecUiPoint.push_back(pt);
				}
			}

			for (int j = 0; j < 3; j++)
			{
				m_result.ptP[j].x = float(MultiSFRIntersectionCoordinate[m_param.nPIndex[j]].x);
				m_result.ptP[j].y = float(MultiSFRIntersectionCoordinate[m_param.nPIndex[j]].y);
				text.ptPos.x = DOUBLE2LONG(m_result.ptP[j].x);
				text.ptPos.y = DOUBLE2LONG(m_result.ptP[j].y + 2);
				text.color = pt.color = COLOR_RED;
				text.strText.Format(_T("P%d"), j);
				uiMark.vecUIText.push_back(text);

				pt.color = COLOR_RED;
				pt.ptPos.x = DOUBLE2LONG(m_result.ptP[j].x );
				pt.ptPos.y = DOUBLE2LONG(m_result.ptP[j].y );
				uiMark.vecUiPoint.push_back(pt);

				m_result.ptS[j].x = float(MultiSFRIntersectionCoordinate[m_param.nSIndex[j]].x);
				m_result.ptS[j].y = float(MultiSFRIntersectionCoordinate[m_param.nSIndex[j]].y);
				text.ptPos.x = DOUBLE2LONG(m_result.ptS[j].x);
				text.ptPos.y = DOUBLE2LONG(m_result.ptS[j].y + 2);
				text.color = pt.color = COLOR_BLUE;
				text.strText.Format(_T("S%d"), j);
				uiMark.vecUIText.push_back(text);
				pt.color = COLOR_BLUE;
				pt.ptPos.x = DOUBLE2LONG(m_result.ptS[j].x );
				pt.ptPos.y = DOUBLE2LONG(m_result.ptS[j].y );

				uiMark.vecUiPoint.push_back(pt);

				m_result.ptQ[j].x = float(MultiSFRIntersectionCoordinate[m_param.nQIndex[j]].x);
				m_result.ptQ[j].y = float(MultiSFRIntersectionCoordinate[m_param.nQIndex[j]].y);
				text.ptPos.x = DOUBLE2LONG(m_result.ptQ[j].x);
				text.ptPos.y = DOUBLE2LONG(m_result.ptQ[j].y + 2);
				text.color = pt.color = COLOR_RED;
				text.strText.Format(_T("Q%d"), j);
				uiMark.vecUIText.push_back(text);
				pt.color = COLOR_RED;
				pt.ptPos.x = DOUBLE2LONG(m_result.ptQ[j].x );
				pt.ptPos.y = DOUBLE2LONG(m_result.ptQ[j].y );

				uiMark.vecUiPoint.push_back(pt);
			}


			m_result.dRotateAngle = 0.0;
			double dRotate = 0.0;

			for (int j = 0; j < 3; j++)
			{
				double dx = m_result.ptQ[j].x - m_result.ptP[j].x;
				double dy = m_result.ptQ[j].y - m_result.ptP[j].y;
				dRotate = Radian2Angle(atan(dy / dx));
				m_result.dRotateAngle += dRotate;
			}
			int text_Top = 60 , text_GapY = 100;

			m_result.dRotateAngle /= 3.0;

			text.ptPos.x = nWidth/5;
			text.ptPos.y = text_Top;
			text.color = COLOR_BLACK;
			text.strText.Format(_T("Rotate: %.2f"), m_result.dRotateAngle);
			uts.log.Debug(_T("Rotate: %.2f"), m_result.dRotateAngle);
			if (!IS_IN_RANGE(m_result.dRotateAngle, m_param.dRotateAngleSpec[Min], m_param.dRotateAngleSpec[Max]))
			{
				text.color = COLOR_RED;
			}
			uiMark.vecUIText.push_back(text);

			//Tilt 
			m_result.dTiltX = m_result.dTiltY = 0.0;

			m_result.dTiltX = CalculateTiltX(m_result);
			m_result.dTiltY = CalculateTiltY(m_result);

			text.ptPos.x = nWidth/5;
			text.ptPos.y = text_Top + text_GapY;
			text.color = COLOR_BLACK;
			text.strText.Format(_T("TiltX: %.2f"), m_result.dTiltX);
			if (!IS_IN_RANGE(m_result.dTiltX, m_param.dTiltXAngleSpec[Min], m_param.dTiltXAngleSpec[Max]))
			{
				text.color = COLOR_RED;
			}
			uiMark.vecUIText.push_back(text);

			text.ptPos.x = nWidth/5;
			text.ptPos.y = text_Top + 2*text_GapY;
			text.color = COLOR_BLACK;
			text.strText.Format(_T("TiltY: %.2f"), m_result.dTiltY);
			if (!IS_IN_RANGE(m_result.dTiltY, m_param.dTiltYAngleSpec[Min], m_param.dTiltYAngleSpec[Max]))
			{
				text.color = COLOR_RED;
			}
			uiMark.vecUIText.push_back(text);
			//-------------------------------------------------------------------------
			// 保存结果到共享(R)
			CStringA strHeader, strKey;

			for (int j = 0; j < 3; j++)
			{
				strHeader = (uts.info.nDeviceIndex == 0 ? "left" : "right");
				strKey.Format("%s_P%d_x", strHeader, j);
				uts.info.mapShared[strKey.GetBuffer()] = m_result.ptP[j].x;
				strKey.Format("%s_P%d_y", strHeader, j);
				uts.info.mapShared[strKey.GetBuffer()] = m_result.ptP[j].y;

				strKey.Format("%s_S%d_x", strHeader, j);
				uts.info.mapShared[strKey.GetBuffer()] = m_result.ptS[j].x;
				strKey.Format("%s_S%d_y", strHeader, j);
				uts.info.mapShared[strKey.GetBuffer()] = m_result.ptS[j].y;

				strKey.Format("%s_Q%d_x", strHeader, j);
				uts.info.mapShared[strKey.GetBuffer()] = m_result.ptQ[j].x;
				strKey.Format("%s_Q%d_y", strHeader, j);
				uts.info.mapShared[strKey.GetBuffer()] = m_result.ptQ[j].y;
			}
        }
        
		RELEASE_ARRAY(MultiSFRIntersectionCoordinate);

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
		foundation::dCoordinate *MultiSFRIntersectionCoordinate = new foundation::dCoordinate[m_param.szPtCount.cx*m_param.szPtCount.cy];
		memset(MultiSFRIntersectionCoordinate,0,sizeof(foundation::dCoordinate)*m_param.szPtCount.cx*m_param.szPtCount.cy);
        //------------------------------------------------------------------------------
        // 初始化结果
        m_result.dRotateAngle = 0.0;

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
        // 测试
		
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
			if (!algorithm::geometry::multisfr::GetMultiSFRIntersectionCoordinate(m_bufferObj.pBmpBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_param.szPtCount.cx,
				m_param.szPtCount.cy,
				MultiSFRIntersectionCoordinate))
            {
                uts.log.Error(_T("GetChessboardPos error. "));
                *pnErrorCode = uts.errorcode.E_NoImage;
                goto end;
            }
        }
        else
        {
            uts.log.Error(_T("BufferType error. "));
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        // 画图
        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

		for (int j = 0; j < 3; j++)
		{
			m_result.ptP[j].x = float(MultiSFRIntersectionCoordinate[m_param.nPIndex[j]].x);
			m_result.ptP[j].y = float(MultiSFRIntersectionCoordinate[m_param.nPIndex[j]].y);

			m_result.ptS[j].x = float(MultiSFRIntersectionCoordinate[m_param.nSIndex[j]].x);
			m_result.ptS[j].y = float(MultiSFRIntersectionCoordinate[m_param.nSIndex[j]].y);
		
			m_result.ptQ[j].x = float(MultiSFRIntersectionCoordinate[m_param.nQIndex[j]].x);
			m_result.ptQ[j].y = float(MultiSFRIntersectionCoordinate[m_param.nQIndex[j]].y);	
		}

        //------------------------------------------------------------------------------
        // 判断规格(回转)
		m_result.dRotateAngle = 0.0;

		for (int j = 0; j < 3; j++)
		{
			double dx = m_result.ptQ[j].x - m_result.ptP[j].x;
			double dy = m_result.ptQ[j].y - m_result.ptP[j].y;
			m_result.dRotateAngle += Radian2Angle(atan(dy / dx));
		}

		m_result.dRotateAngle /= 3.0;

		m_result.dTiltX = m_result.dTiltY = 0.0;

		m_result.dTiltX = CalculateTiltX(m_result);
		m_result.dTiltY = CalculateTiltY(m_result);

		if (!IS_IN_RANGE(m_result.dRotateAngle, m_param.dRotateAngleSpec[Min], m_param.dRotateAngleSpec[Max]))
		{
			*pnErrorCode = uts.errorcode.E_Rotate;
			goto end;
		}

		if (!IS_IN_RANGE(m_result.dTiltX, m_param.dTiltXAngleSpec[Min], m_param.dTiltXAngleSpec[Max]))
		{
			*pnErrorCode = uts.errorcode.E_Tilt;
			goto end;
		}
		if (!IS_IN_RANGE(m_result.dTiltY, m_param.dTiltYAngleSpec[Min], m_param.dTiltYAngleSpec[Max]))
		{
			*pnErrorCode = uts.errorcode.E_Tilt;
			goto end;
		}
        
        *pnErrorCode = uts.errorcode.E_Pass;

end:
		RELEASE_ARRAY(MultiSFRIntersectionCoordinate);
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

        
        for (int j = 0; j < 3; j++)
        {
            strHeader.AppendFormat(_T("P%dx,P%dy,S%dx,S%dy,Q%dx,Q%dy,"),
                j, j, j, j, j, j);
            strData.AppendFormat(_T("%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,"),
                m_result.ptP[j].x, m_result.ptP[j].y,
                m_result.ptS[j].x, m_result.ptS[j].y,
                m_result.ptQ[j].x, m_result.ptQ[j].y);
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
