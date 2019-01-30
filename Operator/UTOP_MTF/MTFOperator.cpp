#include "StdAfx.h"
#include "MTFOperator.h"
#include "UTOP_MTF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
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
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("mtfParam.sizeMtfROI.cx"), nValue, 32, _T("MTF ROI width."));
        m_param.mtfParam.sizeMtfROI.cx = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("mtfParam.sizeMtfROI.cy"), nValue, 32, _T("MTF ROI height."));
        m_param.mtfParam.sizeMtfROI.cy = nValue;
        CString strFieldList;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("mtfParam.strFieldList"), strFieldList, _T("0.4,0.6,0.8"), _T("Field list, split with comma."));
        SplitDouble(strFieldList, m_param.mtfParam.vecField);
        CString strFieldSpecList;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("Spec.strFieldSpecList"), strFieldSpecList, _T("60.0,40.0,30.0,20.0"), _T("Field spec list(CT,F1,F2,F3)"));
        SplitDouble(strFieldSpecList, m_param.vecFieldSpec);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Spec.strFieldDeltaSpecList"), strFieldSpecList, _T("10.0,20.0,30.0"), _T("Field Delta spec list(F1,F2,F3)"));
		SplitDouble(strFieldSpecList, m_param.vecFieldDeltaSpec);


        return TRUE;
    }

    BOOL MTFOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        //------------------------------------------------------------------------------
        // 初始化结果
        for (int i = 0; i < MtfStd::MtfFieldLoc_SIZES; i++)
        {
            m_result.dArrMTF[i] = 0.0;
        }

        //-------------------------------------------------------------------------
        // 计算
        MTF::MtfStd::GetAllMTF(pBmpBuffer, nWidth, nHeight, m_param.mtfParam, m_result);

        //------------------------------------------------------------------------------
        // OSD绘制
        UI_RECT rc;
        UI_POINT pt;
        UI_TEXT text;
        pt.color = COLOR_RED;
        for (size_t i = 0; i < MtfFieldLoc_SIZES; i++)
        {
            if (i > m_param.mtfParam.vecField.size() * 4)
            {
                break;
            }
			if (i == MtfFieldLoc_CT)
			{
				if (m_result.dArrMTF[i] >= m_param.vecFieldSpec[0])
				{
					rc.color = COLOR_LIME;
				}
				else
				{
					rc.color = COLOR_RED;
				}
			}
			else
			{
                int nFieldIndex = ((i - 1) / 4)+1;

                if (m_result.dArrMTF[i] >= m_param.vecFieldSpec[nFieldIndex])
                {
                    rc.color = COLOR_LIME;
                }
                else
                {
                    rc.color = COLOR_RED;
                }
            }

            rc.rcPos = m_result.rcArrROI[i];
            uiMark.vecUiRect.push_back(rc);
            pt.ptPos = m_result.ptArrROICenter[i];
            uiMark.vecUiPoint.push_back(pt);
            text.ptPos = CPoint(rc.rcPos.left, rc.rcPos.bottom);
            text.color = rc.color;
            text.strText.Format(_T("%.1f"), m_result.dArrMTF[i]);
            uiMark.vecUIText.push_back(text); 
        }

		int PosY = 50;
		for (size_t i = 0; i < m_param.mtfParam.vecField.size(); i++)
		{
			if(m_result.dArrMTFDelta[i]>m_param.vecFieldDeltaSpec[i])
			{
				text.color = COLOR_RED;
			}else
				text.color = COLOR_LIME;

				text.ptPos = CPoint(nWidth/2 - 50,PosY);
				text.strText.Format(_T("Filed%d_Delta: %.1f"), i+1,m_result.dArrMTFDelta[i]);
				uiMark.vecUIText.push_back(text); 
			PosY += 15;
		}


        return TRUE;
    }

    BOOL MTFOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        for (int i = 0; i < MtfStd::MtfFieldLoc_SIZES; i++)
        {
            m_result.dArrMTF[i] = 0.0;
        }

		for (int i = 0; i < MtfStd::MAX_EDGE_FIELD_COUNT; i++)
		{
			m_result.dArrMTFDelta[i] = 0.0;
		}
        
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
                0,
                1))
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
        
        //------------------------------------------------------------------------------
        // 测试
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
            UI_MARK uiMark;
            OnPreview(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, uiMark);
            // 画图
            m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
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
        for (size_t i = 0; i < MtfFieldLoc_SIZES; i++)
        {
            if (i > m_param.mtfParam.vecField.size() * 4)
            {
                break;
            }
            if (i == MtfFieldLoc_CT)
            {
                if (m_result.dArrMTF[i] < m_param.vecFieldSpec[0])
                {
                    *pnErrorCode = uts.errorcode.E_FocusCheck;
                    goto end;
                }
            }
            else
            {
                int nFieldIndex = ((i - 1) / 4)+1;
                if (m_result.dArrMTF[i] < m_param.vecFieldSpec[nFieldIndex])
                {
                    *pnErrorCode = uts.errorcode.E_FocusCheck;
                    goto end;
                }
            }
        }

		for (size_t i = 0; i < m_param.mtfParam.vecField.size(); i++)
		{
			if(m_result.dArrMTFDelta[i]>m_param.vecFieldDeltaSpec[i])
			{
				*pnErrorCode = uts.errorcode.E_FocusDelta;
				goto end;
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

    void MTFOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_FocusCheck);
		vecReturnValue.push_back(uts.errorcode.E_FocusDelta);
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

        strHeader = _T("Time,SN,TestTime(ms),Result,CT,")
            _T("F1_LU,F1_RU,F1_RD,F1_LD,")
            _T("F2_LU,F2_RU,F2_RD,F2_LD,")
            _T("F3_LU,F3_RU,F3_RD,F3_LD,")
			_T("F1_Delta,F2_Delta,F3_Delta,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%s,%.1f,")
            _T("%.1f,%.1f,%.1f,%.1f,")
            _T("%.1f,%.1f,%.1f,%.1f,")
            _T("%.1f,%.1f,%.1f,%.1f,")
			_T("%.1f,%.1f,%.1f,")
            _T("%s,%s\n"),
            lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult, m_result.dArrMTF[MtfFieldLoc_CT],
            m_result.dArrMTF[MtfFieldLoc1_LU], m_result.dArrMTF[MtfFieldLoc1_RU], m_result.dArrMTF[MtfFieldLoc1_RD], m_result.dArrMTF[MtfFieldLoc1_LD],
            m_result.dArrMTF[MtfFieldLoc2_LU], m_result.dArrMTF[MtfFieldLoc2_RU], m_result.dArrMTF[MtfFieldLoc2_RD], m_result.dArrMTF[MtfFieldLoc2_LD],
            m_result.dArrMTF[MtfFieldLoc3_LU], m_result.dArrMTF[MtfFieldLoc3_RU], m_result.dArrMTF[MtfFieldLoc3_RD], m_result.dArrMTF[MtfFieldLoc3_LD],
			m_result.dArrMTFDelta[0],m_result.dArrMTFDelta[1],m_result.dArrMTFDelta[2],
            strVersion, uts.info.strUserId);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new MTFOperator);
    }
    //------------------------------------------------------------------------------
}
