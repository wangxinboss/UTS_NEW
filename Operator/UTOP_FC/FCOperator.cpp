#include "StdAfx.h"
#include "FCOperator.h"
#include "UTOP_FC.h"
#include "VCM.h"

#pragma comment(lib, "UTS_VCM.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    FCOperator::FCOperator(void)
    {
        m_strOperatorName = _T("FocusCheck");

        TCHAR szFileName[MAX_PATH] = {0};
        GetModuleFileName(theApp.m_hInstance, szFileName, MAX_PATH);
        m_strModuleFile = szFileName;
    }

    FCOperator::~FCOperator(void)
    {
    }

    BOOL FCOperator::OnReadSpec()
    {
        CString strKey, strComment, strValue;
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        int nValue = 0;

        uts.db.GetOperatorSingleSpec(strSection, _T("nFixRoiEn"), m_param.nFixRoiEn, 0, _T("1: Enable / 0: Disable"));
        uts.db.GetOperatorSingleSpec(strSection, _T("nManualThroughFocusEn"), m_param.nManualThroughFocusEn, 0, _T("1: Enable / 0: Disable"));
        uts.db.GetOperatorSingleSpec(strSection, _T("nManualThroughFocusFrameCount"), m_param.nManualThroughFocusFrameCount, 0, _T("frame count per sec."));
        //------------------------------------------------------------------------------
        // crosshair
        uts.db.GetOperatorSingleSpec(strSection, _T("crosshair.sizeDisFromCT.cx"), nValue, 200, _T("Crosshair ROI horizontal distance from Center."));
        m_param.crosshair.sizeDisFromCT.cx = nValue;
        uts.db.GetOperatorSingleSpec(strSection, _T("crosshair.sizeDisFromCT.cy"), nValue, 200, _T("Crosshair ROI vertical distance from Center."));
        m_param.crosshair.sizeDisFromCT.cy = nValue;
        uts.db.GetOperatorSingleSpec(strSection, _T("crosshair.nROISize"), m_param.crosshair.nROISize, 200, _T("Crosshair ROI size."));
        uts.db.GetOperatorSingleSpec(strSection, _T("crosshair.dField"), m_param.crosshair.dField, 0.6, _T("Field of crosshair."));
        uts.db.GetOperatorSingleSpec(strSection, _T("crosshair.dCodeVariation"), m_param.crosshair.dCodeVariation, 0.0, _T("CrossHair Code Variation."));
        //------------------------------------------------------------------------------
        // white patch
        uts.db.GetOperatorSingleSpec(strSection, _T("whitePatch.sizeDisFromCT.cx"), nValue, 100, _T("WhitePatch ROI horizontal distance from Center."));
        m_param.whitePatch.sizeDisFromCT.cx = nValue;
        uts.db.GetOperatorSingleSpec(strSection, _T("whitePatch.sizeDisFromCT.cy"), nValue, 0, _T("WhitePatch ROI vertical distance from Center."));
        m_param.whitePatch.sizeDisFromCT.cy = nValue;
        uts.db.GetOperatorSingleSpec(strSection, _T("whitePatch.nROISize"), m_param.whitePatch.nROISize, 100, _T("WhitePatch ROI size."));
        //------------------------------------------------------------------------------
        // weight
        uts.db.GetOperatorSingleSpec(strSection, _T("weight.nHiEnable"), m_param.weight.nHiEnable, 1, _T("1: Enable / 0: Disable"));
        uts.db.GetOperatorSingleSpec(strSection, _T("weight.dHiSpec"), m_param.weight.dHiSpec, 0.0, _T("Spec of weight value."));
        uts.db.GetOperatorSingleSpec(strSection, _T("weight.dHiDiffSpec"), m_param.weight.dHiDiffSpec, 100.0, _T("Spec of weight diff value."));
        //------------------------------------------------------------------------------
        // divide spec
        uts.db.GetOperatorSingleSpec(strSection, _T("divideSpec.nEnable"), m_param.divideSpec.nEnable, 0, _T("1: Enable / 0: Disable"));
        uts.db.GetOperatorSingleSpec(strSection, _T("divideSpec.nFieldIndex"), m_param.divideSpec.nFieldIndex, 0, _T("Divide spec Field index. Base from 0, if -1, use weight value."));
        uts.db.GetOperatorSingleSpec(strSection, _T("divideSpec.nSpecA"), m_param.divideSpec.nSpecA, 70, _T("Spec of class A"));
        uts.db.GetOperatorSingleSpec(strSection, _T("divideSpec.nSpecB"), m_param.divideSpec.nSpecB, 50, _T("Spec of class B"));
        uts.db.GetOperatorSingleSpec(strSection, _T("divideSpec.nSpecC"), m_param.divideSpec.nSpecC, 30, _T("Spec of class C"));
        //------------------------------------------------------------------------------
        // sfr
        uts.db.GetOperatorSingleSpec(strSection, _T("sfr.nSfrType"), m_param.sfr.nSfrType, 0, _T("0:Real, 1:AllAverage, 2: MidAverage"));
        uts.db.GetOperatorSingleSpec(strSection, _T("sfr.nSfrStackCount"), m_param.sfr.nSfrStackCount, 1, _T("Stack count."));
        uts.db.GetOperatorSingleSpec(strSection, _T("sfr.sizeSFR_ROI.cx"), nValue, 100, _T("Horizontal size."));
        m_param.sfr.sizeSFR_ROI.cx = nValue;
        uts.db.GetOperatorSingleSpec(strSection, _T("sfr.sizeSFR_ROI.cy"), nValue, 100, _T("Vertical size."));
        m_param.sfr.sizeSFR_ROI.cy = nValue;
        uts.db.GetOperatorSingleSpec(strSection, _T("sfr.dFrqNum[SFR_Frq_Hi]"), m_param.sfr.dFrqNum[SFR_Frq_Hi], 0.125, _T("High frq number."));
        uts.db.GetOperatorSingleSpec(strSection, _T("sfr.dFrqNum[SFR_Frq_Lo]"), m_param.sfr.dFrqNum[SFR_Frq_Lo], 0.25, _T("Low frq number. if -1, do not show."));
        uts.db.GetOperatorSingleSpec(strSection, _T("sfr.dSFRScore"), m_param.sfr.dSFRScore, -1.0, _T("SFR score. if -1, Not use."));
        uts.db.GetOperatorSingleSpec(strSection, _T("sfr.nSfrRoiCount"), m_param.sfr.nSfrRoiCount, 13, _T("Total Sfr roi count."));

        m_param.sfr.vecSfrRoi.clear();
        for (int i = 0; i < m_param.sfr.nSfrRoiCount; i++)
        {
            SINGLE_SFR_ROI_INFO sgROI = {0};
            sgROI.nIndex = i;

            strKey.Format(_T("sfr.vecSfrRoi[%03d].dROIField"), i);
            strComment.Format(_T("Field of sfr roi %d."), i);
            uts.db.GetOperatorSingleSpec(strSection, strKey, sgROI.dROIField, 0.6, strComment);

            strKey.Format(_T("sfr.vecSfrRoi[%03d].dROIAngle"), i);
            strComment.Format(_T("Angle of sfr roi %d."), i);
            uts.db.GetOperatorSingleSpec(strSection, strKey, sgROI.dROIAngle, 45.0, strComment);

            CString strDefaultRoiName;
            strDefaultRoiName.Format(_T("ROI%d"), i);
            strKey.Format(_T("sfr.vecSfrRoi[%03d].strROIName"), i);
            strComment.Format(_T("Name of sfr roi %d."), i);
            uts.db.GetOperatorSingleSpec(strSection, strKey, sgROI.strROIName, strDefaultRoiName, strComment);

            m_param.sfr.vecSfrRoi.push_back(sgROI);
        }

        uts.db.GetOperatorSingleSpec(strSection, _T("sfr.nFieldCount"), m_param.sfr.nFieldCount, 4, _T("Field count."));
        m_param.sfr.vecField.clear();
        for (int i = 0; i < m_param.sfr.nFieldCount; i++)
        {
            SINGLE_FIELD_INFO sgField = {0};
            sgField.nIndex = i;

            strKey.Format(_T("sfr.vecField[%d].nEnable"), i);
            strComment.Format(_T("1: Enable / 0: Disable."));
            uts.db.GetOperatorSingleSpec(strSection, strKey, sgField.nEnable, 1, strComment);

            strKey.Format(_T("sfr.vecField[%d].dHiSpec"), i);
            strComment.Format(_T("Hi frq spec of field %d."), i);
            uts.db.GetOperatorSingleSpec(strSection, strKey, sgField.dHiSpec, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].dHiDeltaSpec"), i);
            strComment.Format(_T("Hi frq delta spec of field %d."), i);
            uts.db.GetOperatorSingleSpec(strSection, strKey, sgField.dHiDeltaSpec, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].dWeight"), i);
            strComment.Format(_T("Sfr weight of field %d."), i);
            uts.db.GetOperatorSingleSpec(strSection, strKey, sgField.dWeight, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].dDeltaWeight"), i);
            strComment.Format(_T("Sfr Delta weight of field %d."), i);
            uts.db.GetOperatorSingleSpec(strSection, strKey, sgField.dDeltaWeight, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].SfrIndexList"), i);
            strComment.Format(_T("Sfr roi list of field %d. For example: 0,1,2,3"), i);
            uts.db.GetOperatorSingleSpec(strSection, strKey, strValue, EMPTY_STR, strComment);
            GetIntVectorByString(strValue, sgField.vecSfrIndex);
            
            m_param.sfr.vecField.push_back(sgField);
        }
        //------------------------------------------------------------------------------
        // vcm
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_ThroughDriveEn"), m_param.vcm.nVCM_ThroughDriveEn, 0, _T("1: Enable / 0: Disable"));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_ThroughROIIndex"), m_param.vcm.nVCM_ThroughROIIndex, 0, _T("Through focus ROI index."));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_TypeSelect"), m_param.vcm.nVCM_TypeSelect, 0, VCM_DISCRIPTION);
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_StepTime"), m_param.vcm.nVCM_StepTime, 350, _T("Step time."));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_FinalDelayTime"), m_param.vcm.nVCM_FinalDelayTime, 1500, _T("Final delay time."));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_GetDelayTime"), m_param.vcm.nVCM_GetDelayTime, 1200, _T("Get image delay time."));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_DACMin"), m_param.vcm.nVCM_DACMin, 0, _T("Min DAC."));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_DACMax"), m_param.vcm.nVCM_DACMax, 400, _T("Max DAC."));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_DACStep"), m_param.vcm.nVCM_DACStep, 4, _T("1 step DAC number."));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_MeasureCurrentEn"), m_param.vcm.nVCM_MeasureCurrentEn, 0, _T("1: Enable / 0: Disable"));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_CurrentPIN"), m_param.vcm.nVCM_CurrentPIN, 3, _T("0:PIN1, 1:PIN2, 2:PIN3, 3:PIN4, 4:PIN35, 5:PIN36"));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_NotMovingThreshold"), m_param.vcm.nVCM_NotMovingThreshold, 20, _T("VCM not moving threshold."));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nGlitch_DetectEn"), m_param.vcm.nGlitch_DetectEn, 0, _T("1: Enable / 0: Disable"));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nGlitch_SmoothEn"), m_param.vcm.nGlitch_SmoothEn, 0, _T("1: Enable / 0: Disable"));
        uts.db.GetOperatorSingleSpec(strSection, _T("vcm.nGlitch_Threshold"), m_param.vcm.nGlitch_Threshold, 20, _T("Glitch threshold."));
        
        SFRPLUS_PARAM_STATIC sfrParamStatic = {0};
        sfrParamStatic.dCrossHairField = m_param.crosshair.dField;
        sfrParamStatic.dFrqNum[SFR_Frq_Hi] = m_param.sfr.dFrqNum[SFR_Frq_Hi];
        sfrParamStatic.dFrqNum[SFR_Frq_Lo] = m_param.sfr.dFrqNum[SFR_Frq_Lo];
        sfrParamStatic.dSFRNum = m_param.sfr.dSFRScore;
        memcpy(&sfrParamStatic.SFR_ROI, &m_param.sfr.sizeSFR_ROI, sizeof(SIZE));
        sfrParamStatic.nSfrCount = m_param.sfr.nSfrRoiCount;
        for (int i = 0; i < m_param.sfr.nSfrRoiCount; i++)
        {
            sfrParamStatic.dROIField[i] = m_param.sfr.vecSfrRoi[i].dROIField;
            sfrParamStatic.dROIAngle[i] = m_param.sfr.vecSfrRoi[i].dROIAngle;
        }
        m_SfrPlus.SetParam(sfrParamStatic);

        // queue
        m_SfrQueue.Initialize(m_param.sfr.nSfrRoiCount, m_param.sfr.nSfrStackCount, m_param.sfr.nSfrType);
        m_bEnableStack = TRUE;

        return TRUE;
    }

    BOOL FCOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        //------------------------------------------------------------------------------
        // 单帧数据初始化结果
        m_result.bIsCenterYExp = FALSE;
        m_result.dCenterYavg = 0.0;
        m_result.dYvalue = 0.0;
        memset(&m_result.rcWhitePatch, 0, sizeof(RECT));
        memset(&m_result.stSfrResult, 0, sizeof(SFRPLUS_RESULT));
        m_result.vecFieldResult.clear();
        //------------------------------------------------------------------------------
        // 计算SFR数值
        SFRPLUS_PARAM_DYNAMIC sfrParamDynamic = {0};
        //------------------------------------------------------------------------------
        // 计算十字位置
        CCrossHair crossHair;
        if (m_param.nFixRoiEn != 0)
        {
            crossHair.ptChartCenter.x = nWidth / 2;
            crossHair.ptChartCenter.y = nHeight / 2;
            crossHair.ptCrossHairCenter[Cross_Hair_Left].x = DOUBLE2LONG((1.0 - m_param.crosshair.dField) * (nWidth / 2));
            crossHair.ptCrossHairCenter[Cross_Hair_Left].y = DOUBLE2LONG(crossHair.ptChartCenter.y);
            crossHair.ptCrossHairCenter[Cross_Hair_Right].x = DOUBLE2LONG((1.0 + m_param.crosshair.dField) * (nWidth / 2));
            crossHair.ptCrossHairCenter[Cross_Hair_Right].y = DOUBLE2LONG(crossHair.ptChartCenter.y);
            crossHair.ptCrossHairCenter[Cross_Hair_Top].x = DOUBLE2LONG(crossHair.ptChartCenter.x);
            crossHair.ptCrossHairCenter[Cross_Hair_Top].y = DOUBLE2LONG((1.0 - m_param.crosshair.dField) * (nHeight / 2));
            crossHair.ptCrossHairCenter[Cross_Hair_Bottom].x = DOUBLE2LONG(crossHair.ptChartCenter.x);
            crossHair.ptCrossHairCenter[Cross_Hair_Bottom].y = DOUBLE2LONG((1.0 + m_param.crosshair.dField) * (nHeight / 2));
        }
        else
        {
            crossHair.SetROI(
                m_param.crosshair.sizeDisFromCT.cx,
                m_param.crosshair.sizeDisFromCT.cy,
                m_param.crosshair.nROISize,
                m_param.crosshair.dCodeVariation);
            crossHair.Calculate(pBmpBuffer, nWidth, nHeight);
        }
        memcpy(&sfrParamDynamic.ptChartCenter, &crossHair.ptChartCenter, sizeof(POINT));
        memcpy(&sfrParamDynamic.ptCrossHairCenter, &crossHair.ptCrossHairCenter, Cross_Hair_Num * sizeof(POINT));
        m_SfrPlus.CalculateSFR(pBmpBuffer, nWidth, nHeight, sfrParamDynamic, m_result.stSfrResult);
        if (m_bEnableStack)
        {
            m_SfrQueue.AddSfrResult(m_result.stSfrResult);
            m_SfrQueue.GetSfrResult(m_result.stSfrResult);
        }
        //-------------------------------------------------------------------------
        // SFR Delta
        for (size_t i = 0; i < m_param.sfr.vecField.size(); i++)
        {
            SINGLE_FIELD_RESULT singleFieldResult = {0};
            double dFieldMinSfrHi = 100.0;
            double dFieldMinSfrLo = 100.0;
            double dFieldMaxSfrHi = 0.0;
            double dFieldMaxSfrLo = 0.0;
            double dFieldAvgSfrHi = 0.0;
            double dFieldAvgSfrLo = 0.0;
            double dFieldSfrDeltaHi = 0.0;
            double dFieldSfrDeltaLo = 0.0;
            if (m_param.sfr.vecField[i].vecSfrIndex.size() == 1)
            {
                dFieldMinSfrHi = dFieldMaxSfrHi = dFieldAvgSfrHi = m_result.stSfrResult.dSFR[m_param.sfr.vecField[i].vecSfrIndex[0]][SFR_Frq_Hi];
                dFieldMinSfrLo = dFieldMaxSfrLo = dFieldAvgSfrLo = m_result.stSfrResult.dSFR[m_param.sfr.vecField[i].vecSfrIndex[0]][SFR_Frq_Lo];
            }
            else
            {
                for (size_t j = 0; j < m_param.sfr.vecField[i].vecSfrIndex.size(); j++)
                {
                    dFieldMinSfrHi = min(m_result.stSfrResult.dSFR[m_param.sfr.vecField[i].vecSfrIndex[j]][SFR_Frq_Hi], dFieldMinSfrHi);
                    dFieldMinSfrLo = min(m_result.stSfrResult.dSFR[m_param.sfr.vecField[i].vecSfrIndex[j]][SFR_Frq_Lo], dFieldMinSfrLo);
                    dFieldMaxSfrHi = max(m_result.stSfrResult.dSFR[m_param.sfr.vecField[i].vecSfrIndex[j]][SFR_Frq_Hi], dFieldMaxSfrHi);
                    dFieldMaxSfrLo = max(m_result.stSfrResult.dSFR[m_param.sfr.vecField[i].vecSfrIndex[j]][SFR_Frq_Lo], dFieldMaxSfrLo);
                    dFieldAvgSfrHi += m_result.stSfrResult.dSFR[m_param.sfr.vecField[i].vecSfrIndex[j]][SFR_Frq_Hi];
                    dFieldAvgSfrLo += m_result.stSfrResult.dSFR[m_param.sfr.vecField[i].vecSfrIndex[j]][SFR_Frq_Lo];
                }
                dFieldSfrDeltaHi = fabs(dFieldMaxSfrHi - dFieldMinSfrHi);
                dFieldSfrDeltaLo = fabs(dFieldMaxSfrLo - dFieldMinSfrLo);
                dFieldAvgSfrHi /= m_param.sfr.vecField[i].vecSfrIndex.size();
                dFieldAvgSfrLo /= m_param.sfr.vecField[i].vecSfrIndex.size();
            }
            singleFieldResult.dMinSfr[SFR_Frq_Hi] = dFieldMinSfrHi;
            singleFieldResult.dMinSfr[SFR_Frq_Lo] = dFieldMinSfrLo;
            singleFieldResult.dSfrDelta[SFR_Frq_Hi] = dFieldSfrDeltaHi;
            singleFieldResult.dSfrDelta[SFR_Frq_Lo] = dFieldSfrDeltaLo;
            singleFieldResult.dAvgSfr[SFR_Frq_Hi] = dFieldAvgSfrHi;
            singleFieldResult.dAvgSfr[SFR_Frq_Lo] = dFieldAvgSfrLo;
            m_result.vecFieldResult.push_back(singleFieldResult);
        }

        //-------------------------------------------------------------------------
        // 计算Weight
        if (m_param.weight.nHiEnable != 0)
        {
            double dTotalWeight = 0.0;
            m_result.dWeightSfr[SFR_Frq_Hi] = 0.0;
            m_result.dWeightSfr[SFR_Frq_Lo] = 0.0;
            for (size_t i = 0; i < m_param.sfr.vecField.size(); i++)
            {
                m_result.dWeightSfr[SFR_Frq_Hi] += m_result.vecFieldResult[i].dAvgSfr[SFR_Frq_Hi] * m_param.sfr.vecField[i].dWeight;
                m_result.dWeightSfr[SFR_Frq_Lo] += m_result.vecFieldResult[i].dAvgSfr[SFR_Frq_Lo] * m_param.sfr.vecField[i].dWeight;
                m_result.dWeightSfr[SFR_Frq_Hi] += (100.0 - m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Hi]) * m_param.sfr.vecField[i].dDeltaWeight;
                m_result.dWeightSfr[SFR_Frq_Lo] += (100.0 - m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Lo]) * m_param.sfr.vecField[i].dDeltaWeight;
                dTotalWeight += (m_param.sfr.vecField[i].dWeight + m_param.sfr.vecField[i].dDeltaWeight);
            }
            m_result.dWeightSfr[SFR_Frq_Hi] /= dTotalWeight;
            m_result.dWeightSfr[SFR_Frq_Lo] /= dTotalWeight;
            m_result.dWeightMax[SFR_Frq_Hi] = max(m_result.dWeightMax[SFR_Frq_Hi], m_result.dWeightSfr[SFR_Frq_Hi]);
            m_result.dWeightMax[SFR_Frq_Lo] = max(m_result.dWeightMax[SFR_Frq_Lo], m_result.dWeightSfr[SFR_Frq_Lo]);
            m_result.dWeightDiff[SFR_Frq_Hi] = m_result.dWeightMax[SFR_Frq_Hi] - m_result.dWeightSfr[SFR_Frq_Hi];
            m_result.dWeightDiff[SFR_Frq_Lo] = m_result.dWeightMax[SFR_Frq_Lo] - m_result.dWeightSfr[SFR_Frq_Lo];
        }

        //-------------------------------------------------------------------------
        // 计算画面平均亮度
        UTS::Algorithm::CalYavg(pBmpBuffer, nWidth, nHeight, m_result.dYvalue);
        //-------------------------------------------------------------------------
        // 中心亮度
        unsigned char *pROIDataWhitePatch = new BYTE[m_param.whitePatch.nROISize * m_param.whitePatch.nROISize * 3];
        memset(pROIDataWhitePatch, 0, m_param.whitePatch.nROISize * m_param.whitePatch.nROISize * 3);
        m_result.rcWhitePatch.left = crossHair.ptChartCenter.x + m_param.whitePatch.sizeDisFromCT.cx - m_param.whitePatch.nROISize / 2;
        m_result.rcWhitePatch.top = crossHair.ptChartCenter.y + m_param.whitePatch.sizeDisFromCT.cy - m_param.whitePatch.nROISize / 2;
        m_result.rcWhitePatch.right = m_result.rcWhitePatch.left + m_param.whitePatch.nROISize;
        m_result.rcWhitePatch.bottom = m_result.rcWhitePatch.top + m_param.whitePatch.nROISize;
        CImageProc::GetInstance().GetBMPBlockBuffer(
            pBmpBuffer,
            pROIDataWhitePatch,
            nWidth,
            nHeight,
            m_result.rcWhitePatch.left,
            m_result.rcWhitePatch.top,
            m_param.whitePatch.nROISize,
            m_param.whitePatch.nROISize);
        m_result.bIsCenterYExp = Algorithm::CalYavgExp(
            pROIDataWhitePatch,
            m_param.whitePatch.nROISize,
            m_param.whitePatch.nROISize,
            m_result.dCenterYavg);
        RELEASE_ARRAY(pROIDataWhitePatch);

        //------------------------------------------------------------------------------
        // OSD绘制
        UI_RECT rc;
        UI_POINT pt;
        UI_TEXT text;
        UI_TEXT_REL textRel;
        UI_LINE line;
        // Big crosshair
        line.color = COLOR_LIGHTGREY;
        line.ptBegin = CPoint(nWidth / 2, 0);
        line.ptEnd = CPoint(nWidth / 2, nHeight);
        uiMark.vecUiLine.push_back(line);
        line.ptBegin = CPoint(0, nHeight / 2);
        line.ptEnd = CPoint(nWidth, nHeight / 2);
        uiMark.vecUiLine.push_back(line);
        // crosshair ROI
        pt.color = COLOR_RED;
        for (int i = 0; i < Cross_Hair_Num; i++)
        {
            pt.ptPos = crossHair.ptCrossHairCenter[i];
            uiMark.vecUiPoint.push_back(pt);
            rc.color = COLOR_BLUE;
            rc.rcPos = crossHair.rcCrossHairDetectROI[i];
            uiMark.vecUiRect.push_back(rc);
        }
        // sfr & Nq & SFR Delta
        CString strSfrDelta;
        for (size_t i = 0; i < m_param.sfr.vecField.size(); i++)
        {
            for (size_t j = 0; j < m_param.sfr.vecField[i].vecSfrIndex.size(); j++)
            {
                int nIndex = m_param.sfr.vecField[i].vecSfrIndex[j];
                if (m_result.stSfrResult.dSFR[nIndex][SFR_Frq_Hi]
                    >= m_param.sfr.vecField[i].dHiSpec)
                {
                    rc.color = COLOR_LIME;
                    text.color = COLOR_LIME;
                }
                else
                {
                    rc.color = COLOR_RED;
                    text.color = COLOR_RED;
                }
                pt.ptPos = m_result.stSfrResult.roi.ptCenter[nIndex];
                uiMark.vecUiPoint.push_back(pt);
                rc.rcPos = m_result.stSfrResult.roi.rcRect[nIndex];
                uiMark.vecUiRect.push_back(rc);

                text.strText.Format(_T("%.1f"), m_result.stSfrResult.dSFR[nIndex][SFR_Frq_Hi]);
                text.ptPos = CPoint(rc.rcPos.left, rc.rcPos.bottom);
                if (m_param.sfr.dFrqNum[SFR_Frq_Lo] >= 0.0)
                {
                    text.strText.AppendFormat(_T(" / %.1f"), m_result.stSfrResult.dSFR[nIndex][SFR_Frq_Lo]);
                    text.ptPos = CPoint(rc.rcPos.left - rc.rcPos.Width() / 4, rc.rcPos.bottom);
                }
                if (m_param.sfr.dSFRScore >= 0.0)
                {
                    text.strText.AppendFormat(_T(" / %.1f"), m_result.stSfrResult.dNq[nIndex] * 100);
                    text.ptPos = CPoint(rc.rcPos.left - rc.rcPos.Width() / 2, rc.rcPos.bottom);
                }
                uiMark.vecUIText.push_back(text);
            }   // for j
            if (m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Hi]
                <= m_param.sfr.vecField[i].dHiDeltaSpec)
            {
                textRel.color = COLOR_BLUE;
            }
            else
            {
                textRel.color = COLOR_RED;
            }
            textRel.dWidthPersent = 0.3;
            textRel.dHeightPersent = 0.03 * i;
            strSfrDelta.Format(_T("SFRFiled%d_HiDelta: %.1f"),
                i, m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Hi]);
            textRel.strText = strSfrDelta;
            uiMark.vecUITextRel.push_back(textRel);

            textRel.dWidthPersent = 0.6;
            textRel.dHeightPersent = 0.03 * i;
            strSfrDelta.Format(_T("SFRFiled%d_LoDelta: %.1f"),
                i, m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Lo]);
            textRel.strText = strSfrDelta;
            uiMark.vecUITextRel.push_back(textRel);
        }   // for i

        //-------------------------------------------------------------------------
        // Weight
        if (m_param.weight.nHiEnable != 0)
        {
            double dHeightPersent = 0.8;
            CString strText;
            // WeiDiff
            if (m_result.dWeightDiff[SFR_Frq_Hi] > m_param.weight.dHiDiffSpec)
            {
                textRel.color = COLOR_RED;
            }
            else
            {
                textRel.color = COLOR_BLUE;
            }
            textRel.dWidthPersent = 0.45;
            dHeightPersent += 0.03;
            textRel.dHeightPersent = dHeightPersent;
            textRel.strText.Format(_T("WeiDiff: %.1f"), m_result.dWeightDiff[SFR_Frq_Hi]);
            uiMark.vecUITextRel.push_back(textRel);

            // WeiMax
            if (m_result.dWeightMax[SFR_Frq_Hi] < m_param.weight.dHiSpec)
            {
                textRel.color = COLOR_RED;
            }
            else
            {
                textRel.color = COLOR_BLUE;
            }
            dHeightPersent += 0.03;
            textRel.dHeightPersent = dHeightPersent;
            textRel.strText.Format(_T("WeiMax:  %.1f"), m_result.dWeightMax[SFR_Frq_Hi]);
            uiMark.vecUITextRel.push_back(textRel);

            // WeiSFR
            if (m_result.dWeightSfr[SFR_Frq_Hi] < m_param.weight.dHiSpec)
            {
                textRel.color = COLOR_RED;
            }
            else
            {
                textRel.color = COLOR_BLUE;
            }
            dHeightPersent += 0.03;
            textRel.dHeightPersent = dHeightPersent;
            textRel.strText.Format(_T("WeiSFR:  %.1f"), m_result.dWeightSfr[SFR_Frq_Hi]);
            uiMark.vecUITextRel.push_back(textRel);
        }

        // 画面平均亮度
        text.color = COLOR_YELLOW;
        text.ptPos = CPoint(nWidth / 2, nHeight / 4);
        text.strText.Format(_T("Yavg: %.1f"), m_result.dYvalue);
        uiMark.vecUIText.push_back(text);
        // white patch
        rc.color = (m_result.bIsCenterYExp ? COLOR_RED : COLOR_BLUE);
        rc.rcPos = m_result.rcWhitePatch;
        uiMark.vecUiRect.push_back(rc);
        text.color = (m_result.bIsCenterYExp ? COLOR_RED : COLOR_BLUE);
        text.ptPos = CPoint(rc.rcPos.left, rc.rcPos.top);
        text.strText.Format(_T("%.1f"), m_result.dCenterYavg);
        uiMark.vecUIText.push_back(text);

        return TRUE;
    }

    BOOL FCOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);
        
        // 重新设定Sensor序列
        CString strRegName;
        strRegName.Format(_T("[%s]"), m_strOperatorName);
        if (!m_pDevice->WriteValue(
            eDeviceWriteValueType::DWVT_REGISTER_SET,
            strRegName.GetBuffer(),
            strRegName.GetLength() * sizeof(TCHAR)))    // fix bug #4, #5: 白板测试时后面几项的序列不起作用
        {
            uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET %s Error."), strRegName);
            *pnErrorCode = uts.errorcode.E_Fail;
            goto end;
        }

        if (m_bufferInfo.type == BUFFER_TYPE_BMP
            || m_bufferInfo.type == BUFFER_TYPE_RAW8)   // RAW8时也可以使用m_bufferObj.pBmpBuffer
        {
            // AF时寻焦
            BOOL bRet = TRUE;
            if (m_param.vcm.nVCM_ThroughDriveEn != 0)
            {
                //-------------------------------------------------------------------------
                // 初始化结果
                m_result.nThroughPeakDac = m_param.vcm.nVCM_DACMin;
                m_result.dThroughHiSFRMax = 0.0;
                m_result.dThroughHiSFRMin = 100.0;
                m_result.nGlitchDac = 0;
                m_result.dMaxGlitch = 0.0;
                m_result.bHaveGlitch = FALSE;
                memset(&m_result.dSFRHiThroughValue, 0, sizeof(double) * SFR_PLUS_MAX_ROI * Max_VCM_DAC);

                m_bEnableStack = FALSE;    // 为了在跑ThroughFocus时，不叠数值
                uts.log.Debug(_T("Through focus begin======"));
                bRet = ThroughFocus(pbIsRunning, pnErrorCode);
                uts.log.Debug(_T("Through focus end  ======"));
                if (!bRet)
                {
                    goto end;
                }
                // VCM不动判定
                if (m_result.dThroughHiSFRMax - m_result.dThroughHiSFRMin <= m_param.vcm.nVCM_NotMovingThreshold)
                {
                    m_result.bIsVCMMove = FALSE;
                    *pnErrorCode = uts.errorcode.E_VCM;
                    goto end;
                }
                // Glitch判定
                if (m_param.vcm.nGlitch_DetectEn != 0)
                {
                    if (m_result.dMaxGlitch >= m_param.vcm.nGlitch_Threshold)
                    {
                        m_result.bHaveGlitch = TRUE;
                        *pnErrorCode = uts.errorcode.E_Glicth;
                        goto end;
                    }
                }
                //------------------------------------------------------------------------------
                // 找到最清楚的点
                uts.log.Debug(_T("Try to move Best SFR Begin======"));
                uts.log.Debug(_T("ROI index = %d, Peak Dac = %d, Peak Value = %.1f"),
                    m_param.vcm.nVCM_ThroughROIIndex,
                    m_result.nThroughPeakDac,
                    GetDacValue(m_result.nThroughPeakDac));
                bRet = MoveToBestSFR(m_result.nThroughPeakDac, pnErrorCode);
                if (!bRet)
                {
                    goto end;
                }
                uts.log.Debug(_T("Try to move Best SFR End  ======"));
                m_bEnableStack = TRUE;
            }
            DoImageSFRTest(pnErrorCode);
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.type);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }
end:
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass
            || *pnErrorCode == uts.errorcode.E_PassA
            || *pnErrorCode == uts.errorcode.E_PassB
            || *pnErrorCode == uts.errorcode.E_PassC);

        //------------------------------------------------------------------------------
        // 保存图片文件
        SaveImage();

        //------------------------------------------------------------------------------
        // 保存数据文件
        CString strThroughFocusFilename;
        strThroughFocusFilename.Format(_T("%s-ThroughFocus"), m_strOperatorName);
        SaveThroughFocus(strThroughFocusFilename);
        SaveData();

        //-------------------------------------------------------------------------
        // 为下次调焦做准备
        m_result.dWeightMax[SFR_Frq_Hi] = 0.0;
        m_result.dWeightMax[SFR_Frq_Lo] = 0.0;

        return m_bResult;
    }

    void FCOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_FocusCheck);
        vecReturnValue.push_back(uts.errorcode.E_FocusDelta);
        vecReturnValue.push_back(uts.errorcode.E_FocusAdjustment);
        vecReturnValue.push_back(uts.errorcode.E_VCM);
        vecReturnValue.push_back(uts.errorcode.E_Glicth);
        vecReturnValue.push_back(uts.errorcode.E_Current);
    }

    void FCOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);
        CString strVcmMoveResult = ((m_param.vcm.nVCM_ThroughDriveEn != 0) ? (m_result.bIsVCMMove ? PASS_STR : FAIL_STR) : _T("NO JUDGE"));
        CString strGlitchResult = ((m_param.vcm.nVCM_ThroughDriveEn != 0 && m_param.vcm.nGlitch_DetectEn != 0) ? (m_result.bHaveGlitch ? FAIL_STR : PASS_STR) : _T("NO JUDGE"));

        //-------------------------------------------------------------------------
        // Head
        strHeader = _T("Time,SN,TestTime(ms),Y_Avg,WhitePatchY,Result,");
        strData.Format(_T("%s,%s,%.1f,%.1f,%.1f,%s,"),
            lpTime,
            uts.info.strSN,
            m_TimeCounter.GetPassTime(),
            m_result.dYvalue,
            m_result.dCenterYavg,
            strResult);

        //-------------------------------------------------------------------------
        // SFR
        for (int i = 0; i < m_param.sfr.nSfrRoiCount; i++)
        {
            strHeader.AppendFormat(_T("%s_Hi,"), m_param.sfr.vecSfrRoi[i].strROIName);
            if (m_param.sfr.dFrqNum[SFR_Frq_Lo] >= 0.0)
            {
                strHeader.AppendFormat(_T("%s_Lo,"), m_param.sfr.vecSfrRoi[i].strROIName);
            }
            if (m_param.sfr.dSFRScore >= 0.0)
            {
                strHeader.AppendFormat(_T("%s_Frq,"), m_param.sfr.vecSfrRoi[i].strROIName);
                strHeader.AppendFormat(_T("%s_Nq,"), m_param.sfr.vecSfrRoi[i].strROIName);
            }

            strData.AppendFormat(_T("%.1f,"), m_result.stSfrResult.dSFR[i][SFR_Frq_Hi]);
            if (m_param.sfr.dFrqNum[SFR_Frq_Lo] >= 0.0)
            {
                strData.AppendFormat(_T("%.1f,"), m_result.stSfrResult.dSFR[i][SFR_Frq_Lo]);
            }
            if (m_param.sfr.dSFRScore >= 0.0)
            {
                strData.AppendFormat(_T("%.3f,"), m_result.stSfrResult.dFrq[i]);
                strData.AppendFormat(_T("%.3f,"), m_result.stSfrResult.dNq[i]);
            }
        }

        //-------------------------------------------------------------------------
        // Weight
        if (m_param.weight.nHiEnable != 0)
        {
            strHeader.AppendFormat(_T("SFR_WeightHi,SFR_WeightDiffHi,"));
            strData.AppendFormat(_T("%.1f,%.1f,"), m_result.dWeightSfr[SFR_Frq_Hi], m_result.dWeightDiff[SFR_Frq_Hi]);
        }

        //-------------------------------------------------------------------------
        // Field
        for (int i = 0; i < m_param.sfr.nFieldCount; i++)
        {
            strHeader.AppendFormat(_T("Filed%d_HiDelta,"), i);
            if (m_param.sfr.dFrqNum[SFR_Frq_Lo] >= 0.0)
            {
                strHeader.AppendFormat(_T("Filed%d_LoDelta,"), i);
            }

            strData.AppendFormat(_T("%.1f,"), m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Hi]);
            if (m_param.sfr.dFrqNum[SFR_Frq_Lo] >= 0.0)
            {
                strData.AppendFormat(_T("%.1f,"), m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Lo]);
            }
        }

        //-------------------------------------------------------------------------
        // Tail
        strHeader.Append(_T("ThroughPeak_DAC,ThroughPeak_SFR,VCM_Move,Glitch,Glitch_Loc,Glitch_Result,Version,OP_SN\n"));
        strData.AppendFormat(_T("%d,%.1f,%s,%s,%d,%.1f,%s,%s\n"),
            m_result.nThroughPeakDac,
            GetDacValue(m_result.nThroughPeakDac),
            strVcmMoveResult, 
            strGlitchResult,
            m_result.nGlitchDac,
            m_result.dMaxGlitch,
            strVersion,
            uts.info.strUserId);
    }

    void FCOperator::SaveThroughFocus(LPCTSTR lpOperatorName)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        //-------------------------------------------------------------------------
        // 输出Data file
        CString strDirPath;
        strDirPath.Format(_T("%s\\Data-%s-%s-%s-%04d%02d%02d\\"),
            uts.info.strResultPath,
            uts.info.strProjectName,
            uts.info.strLineName,
            uts.info.strStationName,
            st.wYear,
            st.wMonth, 
            st.wDay);
        UTS::OSUtil::CreateMultipleDirectory(strDirPath);
        CString strFilePath;
        strFilePath.Format(_T("%s\\%s-%04d%02d%02d.csv"),
            strDirPath,
            lpOperatorName,
            st.wYear,
            st.wMonth, 
            st.wDay);

        FILE* fp = NULL;
        long fileLength ;
        errno_t etRet = _tfopen_s(&fp, strFilePath, _T("a+"));
        if (0 != etRet)
        {
            uts.log.Error(_T("Open file Fail. path = %s"), strFilePath);
            return;
        }
        fseek(fp, 0, SEEK_END);
        fileLength = ftell(fp);

        CString strTime;
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d:%03d"),
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        int _count = (m_param.vcm.nVCM_DACMax - m_param.vcm.nVCM_DACMin) / m_param.vcm.nVCM_DACStep + 1;
        //------------------------------------------------------------------------------
        // 输出文件头
        if (fileLength < 3)
        {	
            _ftprintf_s(fp, _T("Time,SN,TestTime,DAC_Start,DAC_End,DAC_Div,Loc"));
            for (int i = 0; i < _count; i++)
            {
                _ftprintf_s(fp, _T(",DAC[%03d]"), m_param.vcm.nVCM_DACMin + i * m_param.vcm.nVCM_DACStep);
            }
            _ftprintf_s(fp, _T("\n"));
        }

        //------------------------------------------------------------------------------
        // 输出文件内容
        _ftprintf_s(fp, _T("%s,%s,%.1lf,%d,%d,%d")
            , strTime
            , uts.info.strSN
            , m_ThroughFocusTimeCount.GetPassTime()
            , m_param.vcm.nVCM_DACMin
            , m_param.vcm.nVCM_DACMax
            , m_param.vcm.nVCM_DACStep
            );

        BOOL bFirstLine = TRUE;
        if (m_param.vcm.nVCM_MeasureCurrentEn != 0)
        {
            _ftprintf_s(fp, _T(",Current"));
            for (int i = 0; i < _count; i++)
            {
                _ftprintf_s(fp, _T(",%.01lf"), 
                    m_result.dThroughVCMCurrent[m_param.vcm.nVCM_DACMin + i * m_param.vcm.nVCM_DACStep]);
            }
            _ftprintf_s(fp, _T("\n"));
            bFirstLine = FALSE;
        }

        if (m_param.weight.nHiEnable != 0)
        {
            if (bFirstLine)
            {
                _ftprintf_s(fp, _T(",Weight"));
            }
            else
            {
                _ftprintf_s(fp, _T(",,,,,,Weight"));
            }
            for (int i = 0; i < _count; i++)
            {
                _ftprintf_s(fp, _T(",%.01lf"), 
                    m_result.dWeightThroughValue[m_param.vcm.nVCM_DACMin + i * m_param.vcm.nVCM_DACStep]);
            }
            _ftprintf_s(fp, _T("\n"));
            bFirstLine = FALSE;
        }

        for (int j = 0; j < m_param.sfr.nSfrRoiCount; j++)
        {
            if (0 == j)
            {
                if (bFirstLine)
                {
                    _ftprintf_s(fp, _T(",%s"), m_param.sfr.vecSfrRoi[j].strROIName);
                }
                else
                {
                    _ftprintf_s(fp, _T(",,,,,,%s"), m_param.sfr.vecSfrRoi[j].strROIName);
                }
            }
            else
            {
                _ftprintf_s(fp, _T(",,,,,,%s"), m_param.sfr.vecSfrRoi[j].strROIName);
            }
            for (int i = 0; i < _count; i++)
            {
                _ftprintf_s(fp, _T(",%.01lf"),
                    m_result.dSFRHiThroughValue[j][m_param.vcm.nVCM_DACMin + i * m_param.vcm.nVCM_DACStep]);
            }
            _ftprintf_s(fp, _T("\n"));
        }

        fclose(fp);
    }

    BOOL FCOperator::DoImageSFRTest(int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 测试
        UI_MARK uiMark;
        for (int i = 0; i < m_param.sfr.nSfrStackCount; i++)
        {
            // 抓图
            if (!m_pDevice->Recapture(m_bufferObj, 1, 1))
            {
                uts.log.Error(_T("Recapture error. "));
                *pnErrorCode = uts.errorcode.E_NoImage;
                return FALSE;
            }
            uiMark.clear();
            OnPreview(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, uiMark);
        }
        // 画图
        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

        //-------------------------------------------------------------------------
        // 判断过曝
        if (m_result.bIsCenterYExp)
        {
            *pnErrorCode = uts.errorcode.E_Linumance;
            return FALSE;
        }

        //-------------------------------------------------------------------------
        // 判断规格
        *pnErrorCode = uts.errorcode.E_Pass;
        for (size_t i = 0; i < m_param.sfr.vecField.size(); i++)
        {
            // sfr
            for (size_t j = 0; j < m_param.sfr.vecField[i].vecSfrIndex.size(); j++)
            {
                int nIndex = m_param.sfr.vecField[i].vecSfrIndex[j];
                if (m_result.stSfrResult.dSFR[nIndex][SFR_Frq_Hi] < m_param.sfr.vecField[i].dHiSpec)
                {
                    *pnErrorCode = uts.errorcode.E_FocusCheck;
                    return FALSE;
                }
            }
            // delta
            if (m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Hi] > m_param.sfr.vecField[i].dHiDeltaSpec)
            {
                *pnErrorCode = uts.errorcode.E_FocusDelta;
                return FALSE;
            }
        }
        // weight
        if (m_param.weight.nHiEnable != 0)
        {
            if (m_result.dWeightSfr[SFR_Frq_Hi] < m_param.weight.dHiSpec)
            {
                *pnErrorCode = uts.errorcode.E_FocusCheck;
                return FALSE;
            }
            if (m_result.dWeightDiff[SFR_Frq_Hi] > m_param.weight.dHiDiffSpec)
            {
                *pnErrorCode = uts.errorcode.E_FocusAdjustment;
                return FALSE;
            }
        }
        if (m_param.divideSpec.nEnable != 0)
        {
            double dSfr = 0.0;
            if (m_param.divideSpec.nFieldIndex < 0)
            {
                dSfr = m_result.dWeightSfr[SFR_Frq_Hi];
            }
            else
            {
                dSfr = m_result.vecFieldResult[m_param.divideSpec.nFieldIndex].dMinSfr[SFR_Frq_Hi];
            }
            if (dSfr >= m_param.divideSpec.nSpecA)
            {
                *pnErrorCode = uts.errorcode.E_PassA;
                return TRUE;
            }
            else if (dSfr >= m_param.divideSpec.nSpecB)
            {
                *pnErrorCode = uts.errorcode.E_PassB;
                return TRUE;
            }
            else if (dSfr >= m_param.divideSpec.nSpecC)
            {
                *pnErrorCode = uts.errorcode.E_PassC;
                return TRUE;
            }
            else
            {
                *pnErrorCode = uts.errorcode.E_FocusCheck;
                return TRUE;
            }
        }
        return TRUE;
    }

    BOOL FCOperator::ThroughFocus(BOOL *pbIsRunning, int *pnErrorCode)
    {
        m_ThroughFocusTimeCount.SetStartTime();
        //-------------------------------------------------------------------------
        // 跳到0
        SetVCM_Move(0);
        Sleep(m_param.vcm.nVCM_FinalDelayTime);

        UI_MARK uiMark;
        // 抓图
        if (!m_pDevice->Recapture(m_bufferObj, 1, 1))
        {
            uts.log.Error(_T("Recapture error. "));
            *pnErrorCode = uts.errorcode.E_NoImage;
            return FALSE;
        }
        OnPreview(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, uiMark);

        uts.log.Debug(_T("SFR = %.1f"), GetCurrentValue());

        //-------------------------------------------------------------------------
        // Through Focus Process
        for (int nCurrentDac = m_param.vcm.nVCM_DACMin;
            nCurrentDac <= m_param.vcm.nVCM_DACMax;
            nCurrentDac += m_param.vcm.nVCM_DACStep)
        {
            if (!(*pbIsRunning))
            {
                *pnErrorCode = uts.errorcode.E_Fail;
                return FALSE;
            }
            SetVCM_Move(nCurrentDac);
            Sleep(m_param.vcm.nVCM_StepTime);

            UI_MARK uiMark;
            // 抓图
            if (!m_pDevice->Recapture(m_bufferObj, 1, 1))
            {
                uts.log.Error(_T("Recapture error. "));
                *pnErrorCode = uts.errorcode.E_NoImage;
                return FALSE;
            }
            OnPreview(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, uiMark);

            // 画图
            m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

            for (int i = 0; i < m_param.sfr.nSfrRoiCount; i++)
            {
                m_result.dSFRHiThroughValue[i][nCurrentDac] = m_result.stSfrResult.dSFR[i][SFR_Frq_Hi];
            }
            if (m_param.weight.nHiEnable != 0)
            {
                m_result.dWeightThroughValue[nCurrentDac] = m_result.dWeightSfr[SFR_Frq_Hi];
            }

            double dCurrentDacValue = GetCurrentValue();
            uts.log.Debug(_T("SFR = %.1f"), dCurrentDacValue);

            if (dCurrentDacValue > m_result.dThroughHiSFRMax)
            {
                m_result.dThroughHiSFRMax = dCurrentDacValue;
                m_result.nThroughPeakDac = nCurrentDac;
            }
            if (dCurrentDacValue < m_result.dThroughHiSFRMin)
            {
                m_result.dThroughHiSFRMin = dCurrentDacValue;
            }

            if (m_param.vcm.nVCM_MeasureCurrentEn != 0)
            {
                double dVCMCurrent = 0.0;
                unsigned char arrBuffer[16] = {0};
                int nPinIndex = 0;
                //------------------------------------------------------------------------------
                // Measure Dynamic current
                // Input  Format: [int|PinIndexBaseFrom1][int|OffsetValue]
                // Output Format: [double|MeasuredCurrentValue]
                memset(arrBuffer, 0, sizeof(arrBuffer));
                nPinIndex = m_param.vcm.nVCM_CurrentPIN + 1;
                memcpy(arrBuffer, &nPinIndex, sizeof(int));
                memcpy(arrBuffer + sizeof(int), &uts.info.nArrDynamicCurrentOffset[nPinIndex], sizeof(int));
                if (!m_pDevice->ReadValue(
                    eDeviceReadValueType::DRVT_CURRENT_DYNAMIC_MEASURE,
                    arrBuffer,
                    sizeof(arrBuffer)))
                {
                    uts.log.Error(_T("Device ReadValue DRVT_CURRENT_DYNAMIC_MEASURE Error. Pin = %d"), nPinIndex);
                    *pnErrorCode = uts.errorcode.E_Current;
                    return FALSE;
                }
                memcpy(&dVCMCurrent, arrBuffer, sizeof(double));
                m_result.dThroughVCMCurrent[nCurrentDac] = dVCMCurrent;
            }

            if (m_param.vcm.nGlitch_DetectEn != 0)
            {
                double dGlitch = 0.0;
                if (m_param.vcm.nGlitch_SmoothEn != 0)
                {
                    if (nCurrentDac == m_param.vcm.nVCM_DACMin)
                    {
                        m_result.dCTSFRHiSmoothValue[nCurrentDac] = GetDacValue(nCurrentDac);
                    }
                    else if (nCurrentDac == m_param.vcm.nVCM_DACMin + m_param.vcm.nVCM_DACStep)
                    {
                        m_result.dCTSFRHiSmoothValue[nCurrentDac] = (GetDacValue(nCurrentDac - m_param.vcm.nVCM_DACStep) * 2.0 + GetDacValue(nCurrentDac)) / 3.0;
                    }
                    else
                    {
                        m_result.dCTSFRHiSmoothValue[nCurrentDac] = (GetDacValue(nCurrentDac - m_param.vcm.nVCM_DACStep * 2) + GetDacValue(nCurrentDac - m_param.vcm.nVCM_DACStep) + GetDacValue(nCurrentDac)) / 3.0;
                    }
                    
                    if (nCurrentDac > m_param.vcm.nVCM_DACMin)
                    {
                        dGlitch = abs(m_result.dCTSFRHiSmoothValue[nCurrentDac] - m_result.dCTSFRHiSmoothValue[nCurrentDac - m_param.vcm.nVCM_DACStep]);
                    }
                }
                else
                {
                    if (nCurrentDac > m_param.vcm.nVCM_DACMin)
                    {
                        dGlitch = abs(GetDacValue(nCurrentDac) - GetDacValue(nCurrentDac - m_param.vcm.nVCM_DACStep));
                    }
                }
                if (dGlitch > m_result.dMaxGlitch)
                {
                    m_result.dMaxGlitch = dGlitch;
                    m_result.nGlitchDac = nCurrentDac;
                }
            }
        }
        return TRUE;
    }

    BOOL FCOperator::MoveToBestSFR(int nMaxSFRDac, int *pnErrorCode)
    {
        SetVCM_Move(0);
        Sleep(m_param.vcm.nVCM_FinalDelayTime);

        int nBeforeDac = nMaxSFRDac - 5 * m_param.vcm.nVCM_DACStep;
        if (nBeforeDac < 0)
        {
            nBeforeDac = 0;
        }
        for (int i = 1; i <= 5; i++)
        {
            int nCurrentDac = nBeforeDac + i * m_param.vcm.nVCM_DACStep;
            SetVCM_Move(nCurrentDac);
            Sleep(m_param.vcm.nVCM_StepTime);

            UI_MARK uiMark;
            // 抓图
            if (!m_pDevice->Recapture(m_bufferObj, 1, 1))
            {
                uts.log.Error(_T("Recapture error. "));
                *pnErrorCode = uts.errorcode.E_NoImage;
                return FALSE;
            }
            OnPreview(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, uiMark);

            uts.log.Debug(_T("SFR = %.1f"), GetCurrentValue());

            // 画图
            m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
        }
        Sleep(m_param.vcm.nVCM_GetDelayTime);
        return TRUE;
    }

    void FCOperator::SetVCM_Move(int nVCM_CurrentDAC)
    {
        uts.log.Debug(_T("VCM move to DAC[%d]"), nVCM_CurrentDAC);
        DLLSetVCM_Move(m_pDevice, m_param.vcm.nVCM_TypeSelect, nVCM_CurrentDAC);
    }

    double FCOperator::GetDacValue(int nDac)
    {
        double dDacValue = 0.0;
        if (m_param.vcm.nVCM_ThroughROIIndex >= 0)
        {
            dDacValue = m_result.dSFRHiThroughValue[m_param.vcm.nVCM_ThroughROIIndex][nDac];
        }
        else
        {
            dDacValue = m_result.dWeightThroughValue[nDac];
        }
        return dDacValue;
    }

    double FCOperator::GetCurrentValue()
    {
        double dValue = 0.0;
        if (m_param.vcm.nVCM_ThroughROIIndex >= 0)
        {
            dValue = m_result.stSfrResult.dSFR[m_param.vcm.nVCM_ThroughROIIndex][SFR_Frq_Hi];
        }
        else
        {
            dValue = m_result.dWeightSfr[SFR_Frq_Hi];
        }
        return dValue;
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new FCOperator);
    }
    //------------------------------------------------------------------------------
}
