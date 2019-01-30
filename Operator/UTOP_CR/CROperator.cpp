#include "StdAfx.h"
#include "CROperator.h"
#include "UTOP_CR.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    CrOperator::CrOperator(void)
    {
        OPERATOR_INIT;
    }

    CrOperator::~CrOperator(void)
    {
    }

    BOOL CrOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));

        m_param.stCrParam.stColorParam.nColNum = 6;
        m_param.stCrParam.stColorParam.nRowNum = 1;
        m_param.stCrParam.nColorCount = 6/*MAX_COLOR_NUM*/;
        for (int i = 0; i < m_param.stCrParam.nColorCount; i++)
        {
            m_param.dSpecDeltaE[i] = 10.0;
            m_param.stCrParam.stArrColorInfo[i].nColorIndex = i;
        }

        m_param.stCrParam.stArrColorInfo[0].stColorLab.dL = 28.778;
        m_param.stCrParam.stArrColorInfo[0].stColorLab.dA = 14.179;
        m_param.stCrParam.stArrColorInfo[0].stColorLab.dB = -50.297;

        m_param.stCrParam.stArrColorInfo[1].stColorLab.dL = 55.261;
        m_param.stCrParam.stArrColorInfo[1].stColorLab.dA = -38.342;
        m_param.stCrParam.stArrColorInfo[1].stColorLab.dB = 31.37;

        m_param.stCrParam.stArrColorInfo[2].stColorLab.dL = 42.101;
        m_param.stCrParam.stArrColorInfo[2].stColorLab.dA = 53.378;
        m_param.stCrParam.stArrColorInfo[2].stColorLab.dB = 28.19;

        m_param.stCrParam.stArrColorInfo[3].stColorLab.dL = 81.733;
        m_param.stCrParam.stArrColorInfo[3].stColorLab.dA = 4.039;
        m_param.stCrParam.stArrColorInfo[3].stColorLab.dB = 79.819;

        m_param.stCrParam.stArrColorInfo[4].stColorLab.dL = 51.935;
        m_param.stCrParam.stArrColorInfo[4].stColorLab.dA = 49.986;
        m_param.stCrParam.stArrColorInfo[4].stColorLab.dB = -14.574;

        m_param.stCrParam.stArrColorInfo[5].stColorLab.dL = 51.038;
        m_param.stCrParam.stArrColorInfo[5].stColorLab.dA = -28.631;
        m_param.stCrParam.stArrColorInfo[5].stColorLab.dB = -28.638;

        m_param.stCrParam.sizeColorROI.cx = 25;
        m_param.stCrParam.sizeColorROI.cy = 25;

        m_param.stCrParam.stColorParam.nChartMargin = 1;
        m_param.stCrParam.stColorParam.nYThreshold = 60;
        m_param.stCrParam.stColorParam.rcChartROI.left = 150;
        m_param.stCrParam.stColorParam.rcChartROI.right = 500;
        m_param.stCrParam.stColorParam.rcChartROI.top = 120;
        m_param.stCrParam.stColorParam.rcChartROI.bottom = 190;

        return TRUE;
    }

    BOOL CrOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        ColorReproducibility(pBmpBuffer, nWidth, nHeight, m_param.stCrParam, m_result);

        //------------------------------------------------------------------------------
        // OSD绘制
        UI_RECT rc;
        UI_POINT pt;
        UI_TEXT text;
        // Chart ROI
        rc.color = COLOR_LIME;
        rc.rcPos = m_param.stCrParam.stColorParam.rcChartROI;
        uiMark.vecUiRect.push_back(rc);

        // Chart边框
        rc.color = COLOR_BLUE;
        rc.rcPos = m_result.roi.rcChartRect;
        uiMark.vecUiRect.push_back(rc);

        // 每个颜色的ROI
        pt.color = COLOR_RED;
        for (int i = 0; i < m_param.stCrParam.nColorCount; i++)
        {
            pt.ptPos = m_result.roi.ptColorCenter[i];
            uiMark.vecUiPoint.push_back(pt);
            rc.rcPos = m_result.roi.rcColorRect[i];
            uiMark.vecUiRect.push_back(rc);
        }

        return TRUE;
    }

    BOOL CrOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);
        
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

        //------------------------------------------------------------------------------
        // 测试
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
            UI_MARK uiMark;
            OnPreview(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, uiMark);
            for (int i = 0; i < m_param.stCrParam.nColorCount; i++)
            {
                uts.log.Debug(_T("ColorIndex = %d, AvgR = %f, AvgG = %f, AvgB = %f, L = %f, a = %f, b = %f"),
                    m_param.stCrParam.stArrColorInfo[i].nColorIndex,
                    m_result.rgbAvg[i].dR, m_result.rgbAvg[i].dG, m_result.rgbAvg[i].dB,
                    m_result.labVal[i].dL, m_result.labVal[i].dA, m_result.labVal[i].dB);
            }
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
        for (int i = 0; i < m_param.stCrParam.nColorCount; i++)
        {
            if (m_result.dDeltaE[i] >= m_param.dSpecDeltaE[i])
            {
                *pnErrorCode = uts.errorcode.E_ColorCheck;
                break;
            }
        }

end:
        BOOL bRet = TRUE;
        if (*pnErrorCode != uts.errorcode.E_Pass)
        {
            bRet = FALSE;
        }
        return bRet;
    }

    void CrOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new CrOperator);
    }
    //------------------------------------------------------------------------------
}
