#include "StdAfx.h"
#include "ResolutionOperator.h"
#include "UTOP_Resolution.h"
#include "VCM.h"
#include "Global_Memory_SFC.h"
#include "EEProm.h"
#include "otpdb.h"


#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "CommonFunc.lib")
#pragma comment(lib, "UTS_EEProm.lib")

using namespace UTS;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace DDM;

namespace UTS
{
    ResolutionOperator::ResolutionOperator(void)
    {
        OPERATOR_INIT;
    }

    ResolutionOperator::~ResolutionOperator(void)
    {
    }

    BOOL ResolutionOperator::OnReadSpec()
    {
        CString strKey, strComment, strValue;
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        int nValue = 0;

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nFixRoiEn"), m_param.nFixRoiEn, 0, _T("1: Enable / 0: Disable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nManualThroughFocusEn"), m_param.nManualThroughFocusEn, 0, _T("1: Enable / 0: Disable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nManualThroughFocusFrameCount"), m_param.nManualThroughFocusFrameCount, 0, _T("frame count per sec."));
        //------------------------------------------------------------------------------
        // crosshair
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("crosshair.sizeDisFromCT.cx"), nValue, 200, _T("Crosshair ROI horizontal distance from Center."));
        m_param.crosshair.sizeDisFromCT.cx = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("crosshair.sizeDisFromCT.cy"), nValue, 200, _T("Crosshair ROI vertical distance from Center."));
        m_param.crosshair.sizeDisFromCT.cy = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("crosshair.nROISize"), m_param.crosshair.nROISize, 200, _T("Crosshair ROI size."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("crosshair.dField"), m_param.crosshair.dField, 0.6, _T("Field of crosshair."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("crosshair.dCodeVariation"), m_param.crosshair.dCodeVariation, 0.0, _T("CrossHair Code Variation."));
        //------------------------------------------------------------------------------
        // white patch
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("whitePatch.sizeDisFromCT.cx"), nValue, 100, _T("WhitePatch ROI horizontal distance from Center."));
        m_param.whitePatch.sizeDisFromCT.cx = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("whitePatch.sizeDisFromCT.cy"), nValue, 0, _T("WhitePatch ROI vertical distance from Center."));
        m_param.whitePatch.sizeDisFromCT.cy = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("whitePatch.nROISize"), m_param.whitePatch.nROISize, 100, _T("WhitePatch ROI size."));
        //------------------------------------------------------------------------------
        // weight
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("weight.nHiEnable"), m_param.weight.nHiEnable, 1, _T("1: Enable / 0: Disable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("weight.dHiSpec"), m_param.weight.dHiSpec, 0.0, _T("Spec of weight value."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("weight.dHiDiffSpec"), m_param.weight.dHiDiffSpec, 100.0, _T("Spec of weight diff value."));
        //------------------------------------------------------------------------------
        // divide spec
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("divideSpec.nEnable"), m_param.divideSpec.nEnable, 0, _T("1: Enable / 0: Disable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("divideSpec.nFieldIndex"), m_param.divideSpec.nFieldIndex, 0, _T("Divide spec Field index. Base from 0, if -1, use weight value."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("divideSpec.nSpecA"), m_param.divideSpec.nSpecA, 70, _T("Spec of class A"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("divideSpec.nSpecB"), m_param.divideSpec.nSpecB, 50, _T("Spec of class B"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("divideSpec.nSpecC"), m_param.divideSpec.nSpecC, 30, _T("Spec of class C"));
        //------------------------------------------------------------------------------
        // sfr
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nSfrType"), m_param.sfr.nSfrType, 0, _T("0:Real, 1:AllAverage, 2: MidAverage"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nSfrStackCount"), m_param.sfr.nSfrStackCount, 1, _T("Stack count."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.sizeSFR_ROI.cx"), nValue, 100, _T("Horizontal size."));
        m_param.sfr.sizeSFR_ROI.cx = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.sizeSFR_ROI.cy"), nValue, 100, _T("Vertical size."));
        m_param.sfr.sizeSFR_ROI.cy = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.dFrqNum[SFR_Frq_Hi]"), m_param.sfr.dFrqNum[SFR_Frq_Hi], 0.125, _T("High frq number."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.dFrqNum[SFR_Frq_Lo]"), m_param.sfr.dFrqNum[SFR_Frq_Lo], 0.25, _T("Low frq number. if -1, do not show."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.dSFRScore"), m_param.sfr.dSFRScore, -1.0, _T("SFR score. if -1, Not use."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nSfrRoiCount"), m_param.sfr.nSfrRoiCount, 13, _T("Total Sfr roi count."));

        double arrDefaultROIField[SFR_PLUS_MAX_ROI] = {0.0, 0.4, 0.4, 0.4, 0.4, 0.6, 0.6, 0.6, 0.6, 0.8, 0.8, 0.8, 0.8};
        double arrDefaultROIAngle[SFR_PLUS_MAX_ROI] = {0.0, 36.870, 143.130, 216.870, 323.130, 36.870, 143.130, 216.870, 323.130, 36.870, 143.130, 216.870, 323.130};
        CString arrDefaultROIName[SFR_PLUS_MAX_ROI] = {_T("CT"), _T("RU"), _T("LU"), _T("LD"), _T("RD"), _T("RU"), _T("LU"), _T("LD"), _T("RD"), _T("RU"), _T("LU"), _T("LD"), _T("RD")};
        m_param.sfr.vecSfrRoi.clear();
        for (int i = 0; i < m_param.sfr.nSfrRoiCount; i++)
        {
            SINGLE_SFR_ROI_INFO sgROI = {0};
            sgROI.nIndex = i;

            strKey.Format(_T("sfr.vecSfrRoi[%03d].dROIField"), i);
            strComment.Format(_T("Field of sfr roi %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgROI.dROIField, arrDefaultROIField[i], strComment);

            strKey.Format(_T("sfr.vecSfrRoi[%03d].dROIAngle"), i);
            strComment.Format(_T("Angle of sfr roi %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgROI.dROIAngle, arrDefaultROIAngle[i], strComment);

            //CString strDefaultRoiName;
            //strDefaultRoiName.Format(_T("ROI%d"), i);
            strKey.Format(_T("sfr.vecSfrRoi[%03d].strROIName"), i);
            strComment.Format(_T("Name of sfr roi %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgROI.strROIName, arrDefaultROIName[i]/*strDefaultRoiName*/, strComment);

            m_param.sfr.vecSfrRoi.push_back(sgROI);
        }

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nFieldCount"), m_param.sfr.nFieldCount, 4, _T("Field count."));
        m_param.sfr.vecField.clear();
        for (int i = 0; i < m_param.sfr.nFieldCount; i++)
        {
            SINGLE_FIELD_INFO sgField = {0};
            //sgField.nIndex = i;

            //strKey.Format(_T("sfr.vecField[%d].nEnable"), i);
            //strComment.Format(_T("1: Enable / 0: Disable."));
            //uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgField.nEnable, 1, strComment);

            strKey.Format(_T("sfr.vecField[%d].dHiSpec"), i);
            strComment.Format(_T("Hi frq spec of field %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgField.dHiSpec, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].dHiDeltaSpec"), i);
            strComment.Format(_T("Hi frq delta spec of field %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgField.dHiDeltaSpec, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].dWeight"), i);
            strComment.Format(_T("Sfr weight of field %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgField.dWeight, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].dDeltaWeight"), i);
            strComment.Format(_T("Sfr Delta weight of field %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgField.dDeltaWeight, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].SfrIndexList"), i);
            strComment.Format(_T("Sfr roi list of field %d. For example: 0,1,2,3"), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, strValue, EMPTY_STR, strComment);
            SplitInt(strValue, sgField.vecSfrIndex);
            
            m_param.sfr.vecField.push_back(sgField);
        }
        //------------------------------------------------------------------------------
        // vcm
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_ThroughDriveEn"), m_param.vcm.nVCM_ThroughDriveEn, 0, _T("1: Enable / 0: Disable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_ThroughROIIndex"), m_param.vcm.nVCM_ThroughROIIndex, 0, _T("Through focus ROI index."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_StepTime"), m_param.vcm.nVCM_StepTime, 350, _T("Step time."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_FinalDelayTime"), m_param.vcm.nVCM_FinalDelayTime, 1500, _T("Final delay time."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_GetDelayTime"), m_param.vcm.nVCM_GetDelayTime, 1200, _T("Get image delay time."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_DACMin"), m_param.vcm.nVCM_DACMin, 0, _T("Min DAC."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_DACMax"), m_param.vcm.nVCM_DACMax, 400, _T("Max DAC."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_DACStep"), m_param.vcm.nVCM_DACStep, 4, _T("1 step DAC number."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_ReverseThroughEn"), m_param.vcm.nVCM_ReverseThroughEn, 0, _T("Reverse through focus in detail range."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_MeasureCurrentEn"), m_param.vcm.nVCM_MeasureCurrentEn, 0, _T("1: Enable / 0: Disable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_CurrentPIN"), m_param.vcm.nVCM_CurrentPIN, 3, _T("0:PIN1, 1:PIN2, 2:PIN3, 3:PIN4, 4:PIN35, 5:PIN36"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_NotMovingThreshold"), m_param.vcm.nVCM_NotMovingThreshold, 20, _T("VCM not moving threshold."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_PreThroughEn"), m_param.vcm.nVCM_PreThroughEn, 0, _T("1: Enable / 0: Disable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_PreThroughDivideCount"), m_param.vcm.nVCM_PreThroughDivideCount, 1, _T("Pre through focus divide count."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_PreThroughStepTime"), m_param.vcm.nVCM_PreThroughStepTime, 0, _T("Prefocus Step time."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.dVCM_PreThroughDownPersent"), m_param.vcm.dVCM_PreThroughDownPersent, 0.40, _T("Turning Point Down Persent."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.dVCM_PreThroughPeakMin"), m_param.vcm.dVCM_PreThroughPeakMin, 20.0, _T("The min value of peak."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nGlitch_DetectEn"), m_param.vcm.nGlitch_DetectEn, 0, _T("1: Enable / 0: Disable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nGlitch_SmoothEn"), m_param.vcm.nGlitch_SmoothEn, 0, _T("1: Enable / 0: Disable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nGlitch_Threshold"), m_param.vcm.nGlitch_Threshold, 20, _T("Glitch threshold."));
        
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

		//DB
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nAF_Commit_Type"), m_param.nAF_Commit_Type, 0, _T("0: Disable 1:Inf 2:MUP"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nStartCurrent_En"), m_param.nStartCurrent_En, 0, _T("0: Disable 1:Enable"));

        return TRUE;
    }
	
	void ResolutionOperator::OnPreviewStart()
	{
		if (0 == _tcscmp(uts.info.strProjectName, _T("CA188")))
		{
			DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, 0);
		}else if ((0 == _tcscmp(uts.info.strProjectName, _T("CA210")))|| 
			      (0 == _tcscmp(uts.info.strProjectName, _T("CA211"))))
		{
			unsigned char tmp[6];
			uint16_t HallData[4];
			USES_CONVERSION;

			UTS::EEPROMDriver *eeprom;
			eeprom = GetEEPromDriverInstance(m_pDevice, 5, 0xA0);
			eeprom->Read(0x0018, tmp, sizeof(tmp));

			HallData[0] = get_le_val(tmp + 0, 1);
			HallData[1] = get_le_val(tmp + 1, 1);
			HallData[2] = get_be_val(tmp + 2, 2);
			HallData[3] = get_be_val(tmp + 4, 2);

			DLLSetVCM_Initial(m_pDevice, uts.info.nVCMType,HallData);
		}else
			DLLSetVCM_Initial(m_pDevice,uts.info.nVCMType,NULL);


	}


    BOOL ResolutionOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        //------------------------------------------------------------------------------
        // 单帧数据初始化结果
        m_result.bIsCenterYExp = FALSE;
        m_result.dCenterYavg = 0.0;
        //m_result.dYvalue = 0.0;(会拉低帧率2fps左右，故而屏蔽)
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
 //          m_SfrQueue.AddSfrResult(m_result.stSfrResult);
 //         m_SfrQueue.GetSfrResult(m_result.stSfrResult);
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
        // 计算画面平均亮度(会拉低帧率2fps左右，故而屏蔽)
        //UTS::Algorithm::CalYavg(pBmpBuffer, nWidth, nHeight, m_result.dYvalue);

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
            textRel.dWidthPersent = 0.2;
            textRel.dHeightPersent = 0.05 * i;
            strSfrDelta.Format(_T("SFRFiled%d_HiDelta: %.1f"),
                i, m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Hi]);
            textRel.strText = strSfrDelta;
            uiMark.vecUITextRel.push_back(textRel);

//             textRel.dWidthPersent = 0.6;
//             textRel.dHeightPersent = 0.03 * i;
//             strSfrDelta.Format(_T("SFRFiled%d_LoDelta: %.1f"),
//                 i, m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Lo]);
//             textRel.strText = strSfrDelta;
//             uiMark.vecUITextRel.push_back(textRel);
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

        // 画面平均亮度(会拉低帧率2fps左右，故而屏蔽)
        //text.color = COLOR_YELLOW;
        //text.ptPos = CPoint(nWidth / 2, nHeight / 4);
        //text.strText.Format(_T("Yavg: %.1f"), m_result.dYvalue);
        //uiMark.vecUIText.push_back(text);

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

    BOOL ResolutionOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        pThis = this;
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);
        
        // 重新设定Sensor序列
        CString strRegName = m_strOperatorName;
        if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
            strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
        {
            uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
            *pnErrorCode = uts.errorcode.E_Fail;
            goto end;
        }

        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
            // AF时寻焦
            if (m_param.vcm.nVCM_ThroughDriveEn != 0)
            {
                if (!DoThroughFocus(pbIsRunning))
                {
				//	*pnErrorCode = uts.errorcode.E_ERR_ThroughFocus;
                //    goto end;
                }

                // VCM不动判定
                uts.log.Debug(_T("SFRMax[%.1f] SFRMin[%.1f] NotMoveThreashold[%d]"),
                    m_result.dThroughHiSFRMax,
                    m_result.dThroughHiSFRMin,
                    m_param.vcm.nVCM_NotMovingThreshold);
                if (m_result.dThroughHiSFRMax - m_result.dThroughHiSFRMin <= m_param.vcm.nVCM_NotMovingThreshold)
                {
                    m_result.bIsVCMMove = FALSE;
                    *pnErrorCode = uts.errorcode.E_VCM;
                    goto end;
                }
                // Glitch判定
                if (m_param.vcm.nGlitch_DetectEn != 0)
                {
                    double dGlitch = 0.0;
                    for (int nCurrentDac = m_param.vcm.nVCM_DACMin;
                        nCurrentDac <= m_param.vcm.nVCM_DACMax;
                        nCurrentDac += m_param.vcm.nVCM_DACStep)
                    {
                        if (nCurrentDac > m_param.vcm.nVCM_DACMin)
                        {
                            if (m_param.vcm.nGlitch_SmoothEn != 0)
                            {
                                dGlitch = abs(m_result.dCTSFRHiSmoothValue[nCurrentDac] - m_result.dCTSFRHiSmoothValue[nCurrentDac - m_param.vcm.nVCM_DACStep]);
                            }
                            else
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

                    uts.log.Debug(_T("MaxGlitch[%.1f] GlitchThreashold[%d]"),
                        m_result.dMaxGlitch,
                        m_param.vcm.nGlitch_Threshold);
                    if (m_result.dMaxGlitch >= m_param.vcm.nGlitch_Threshold)
                    {
                        m_result.bHaveGlitch = TRUE;
                        *pnErrorCode = uts.errorcode.E_Glicth;
                        goto end;
                    }
                }
            }
            DoImageSFRTest(pnErrorCode);
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }
end:
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass
            || *pnErrorCode == uts.errorcode.E_PassA
            || *pnErrorCode == uts.errorcode.E_PassB
            || *pnErrorCode == uts.errorcode.E_PassC);

		if(m_bResult)
		{ 
			CommitDAC(m_result.nThroughPeakDac);
			if(m_param.nStartCurrent_En == 1) Update_start_current();
		}

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

    void ResolutionOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
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

    void ResolutionOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void ResolutionOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);
        CString strVcmMoveResult = ((m_param.vcm.nVCM_ThroughDriveEn != 0) ? (m_result.bIsVCMMove ? PASS_STR : FAIL_STR) : _T("NO JUDGE"));
        CString strGlitchResult = ((m_param.vcm.nVCM_ThroughDriveEn != 0 && m_param.vcm.nGlitch_DetectEn != 0) ? (m_result.bHaveGlitch ? FAIL_STR : PASS_STR) : _T("NO JUDGE"));

        //-------------------------------------------------------------------------
        // Head
        strHeader = _T("Time,SN,TestTime(ms),WhitePatchY,Result,");
        strData.Format(_T("%s,%s,%.1f,%.1f,%s,"),
            lpTime,
            uts.info.strSN,
            m_TimeCounter.GetPassTime(),
            //m_result.dYvalue,(会拉低帧率2fps左右，故而屏蔽)
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

		if(m_param.nStartCurrent_En == 1) 
		{
			strHeader.AppendFormat(_T("StartCurrent,"));
			
			strData.AppendFormat(_T("%d,"), m_result.nStart_current);
		}

        //-------------------------------------------------------------------------
        // Tail
        strHeader.Append(_T("ThroughPeak_DAC,ThroughPeak_SFR,VCM_Move,Glitch,Glitch_Loc,Glitch_Result,Version,OP_SN\n"));
        strData.AppendFormat(_T("%d,%.1f,%s,%s,%d,%.1f,%s,%s\n"),
            m_result.nThroughPeakDac,
            m_result.dThroughHiSFRMax,
            strVcmMoveResult, 
            strGlitchResult,
            m_result.nGlitchDac,
            m_result.dMaxGlitch,
            strVersion,
            uts.info.strUserId);
    }

    void ResolutionOperator::SaveThroughFocus(LPCTSTR lpOperatorName)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        CString strTime;
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d:%03d"),
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        int _count = (m_param.vcm.nVCM_DACMax - m_param.vcm.nVCM_DACMin) / m_param.vcm.nVCM_DACStep + 1;

        //-------------------------------------------------------------------------
        // 数据准备
        TestItemResult tir = {0};
        tir.sItemName = lpOperatorName;
        // 文件头
        tir.sHeader = _T("Time,SN,TestTime,DAC_Start,DAC_End,DAC_Div,Loc");
        for (int i = 0; i < _count; i++)
        {
            tir.sHeader.AppendFormat(_T(",DAC[%03d]"),
                m_param.vcm.nVCM_DACMin + i * m_param.vcm.nVCM_DACStep);
        }
        tir.sHeader.Append(_T("\n"));
        // 文件内容
        tir.sData.Format(_T("%s,%s,%.1lf,%d,%d,%d")
            , strTime
            , uts.info.strSN
            , m_ThroughFocusTime
            , m_param.vcm.nVCM_DACMin
            , m_param.vcm.nVCM_DACMax
            , m_param.vcm.nVCM_DACStep
            );
        BOOL bFirstLine = TRUE;
        if (m_param.vcm.nVCM_MeasureCurrentEn != 0)
        {
            tir.sData.Append(_T(",Current"));
            for (int i = 0; i < _count; i++)
            {
                tir.sData.AppendFormat(_T(",%.01lf"), 
                    m_result.dThroughVCMCurrent[m_param.vcm.nVCM_DACMin + i * m_param.vcm.nVCM_DACStep]);
            }
            tir.sData.Append(_T("\n"));
            bFirstLine = FALSE;
        }
        if (m_param.weight.nHiEnable != 0)
        {
            if (bFirstLine)
            {
                tir.sData.Append(_T(",Weight"));
            }
            else
            {
                tir.sData.Append(_T(",,,,,,Weight"));
            }
            for (int i = 0; i < _count; i++)
            {
                tir.sData.AppendFormat(_T(",%.01lf"), 
                    m_result.dWeightThroughValue[m_param.vcm.nVCM_DACMin + i * m_param.vcm.nVCM_DACStep]);
            }
            tir.sData.Append(_T("\n"));
            bFirstLine = FALSE;
        }
        for (int j = 0; j < m_param.sfr.nSfrRoiCount; j++)
        {
            if (0 == j)
            {
                if (bFirstLine)
                {
                    tir.sData.AppendFormat(_T(",%s"), m_param.sfr.vecSfrRoi[j].strROIName);
                }
                else
                {
                    tir.sData.AppendFormat(_T(",,,,,,%s"), m_param.sfr.vecSfrRoi[j].strROIName);
                }
            }
            else
            {
                tir.sData.AppendFormat(_T(",,,,,,%s"), m_param.sfr.vecSfrRoi[j].strROIName);
            }
            for (int i = 0; i < _count; i++)
            {
                tir.sData.AppendFormat(_T(",%.01lf"),
                    m_result.dSFRHiThroughValue[j][m_param.vcm.nVCM_DACMin + i * m_param.vcm.nVCM_DACStep]);
            }
            tir.sData.Append(_T("\n"));
        }

        if (uts.info.nShopFlowEn != 0)
        {
            GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
            gmsfc->vecTestItemResult.push_back(tir);
        }

        //-------------------------------------------------------------------------
        // 输出Data file
        CString strFilePath = GetResultCSVFilename(lpOperatorName, st);

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

        //------------------------------------------------------------------------------
        // 没有文件头时，或者开启预调焦时，输出文件头
        if (fileLength < 3 || (m_param.vcm.nVCM_ThroughDriveEn != 0 && m_param.vcm.nVCM_PreThroughEn != 0))
        {
            _ftprintf_s(fp, tir.sHeader);
        }

        //------------------------------------------------------------------------------
        // 输出文件内容
        _ftprintf_s(fp, tir.sData);
        fclose(fp);
    }

    BOOL ResolutionOperator::DoImageSFRTest(int *pnErrorCode)
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
        uts.log.Debug(_T(">>> Last SFR = %.1f <<<"), GetCurrentValue());

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

    double ResolutionOperator::GetCurrentValue()
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

    double ResolutionOperator::GetDacValue(int nDac)
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

    void ResolutionOperator::SetVcmMove(int nVCM_CurrentDAC)
    {
        DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, nVCM_CurrentDAC);
        uts.log.Debug(_T("Move2Dac[%d]"), nVCM_CurrentDAC);
    }

    BOOL ResolutionOperator::DoStepSfr(
        __in ThroughFocus_Std::eThroughfocusPhase ePhase,
        __in int nCurrentDac,
        __out double &dKeySfr)
    {
        UI_MARK uiMark;
        // 抓图
        if (!m_pDevice->Recapture(m_bufferObj, 1, 1))
        {
            uts.log.Error(_T("Recapture error. "));
            return FALSE;
        }
        OnPreview(
            m_bufferObj.pBmpBuffer,
            m_bufferInfo.nWidth,
            m_bufferInfo.nHeight,
            uiMark);
        // 画图
        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
        dKeySfr = GetCurrentValue();
      
		uts.log.Debug(_T("DAC[%d] SFR[%.1f]"), nCurrentDac, dKeySfr);

        if (m_param.weight.nHiEnable != 0)
        {
            m_result.dWeightThroughValue[nCurrentDac] = m_result.dWeightSfr[SFR_Frq_Hi];
        }
        if (dKeySfr > m_result.dThroughHiSFRMax)
        {
            m_result.dThroughHiSFRMax = dKeySfr;

			//if (ePhase == ThroughFocus_Std::eThroughfocusPhase::TFP_DETAIL_FOCUS)
			{
				m_result.nThroughPeakDac = nCurrentDac;
			}
        }
        if (dKeySfr < m_result.dThroughHiSFRMin)
        {
            m_result.dThroughHiSFRMin = dKeySfr;
        }

        if (ePhase == ThroughFocus_Std::eThroughfocusPhase::TFP_DETAIL_FOCUS)
        {
			// 记录ThroughFocus过程数据
			for (int i = 0; i < m_param.sfr.nSfrRoiCount; i++)
			{
				m_result.dSFRHiThroughValue[i][nCurrentDac] = m_result.stSfrResult.dSFR[i][SFR_Frq_Hi];
			}

            // 记录电流数据
            if (m_param.vcm.nVCM_MeasureCurrentEn != 0)
            {
                CStringA strHeader, strKey;
                int nArrDynamicCurrentOffset[6] = {0};
                for (int i = 0; i < ARRAYSIZE(nArrDynamicCurrentOffset); i++)
                {
                    strKey.Format("%d_DynamicCurrentOffset[%d]", uts.info.nDeviceIndex, i);
                    nArrDynamicCurrentOffset[i] = DOUBLE2INT(uts.info.mapShared[strKey.GetBuffer()]);
                }

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
                memcpy(arrBuffer + sizeof(int), &nArrDynamicCurrentOffset[nPinIndex], sizeof(int));
                if (!m_pDevice->ReadValue(
                    eDeviceReadValueType::DRVT_CURRENT_DYNAMIC_MEASURE,
                    arrBuffer,
                    sizeof(arrBuffer)))
                {
                    uts.log.Error(_T("Device ReadValue DRVT_CURRENT_DYNAMIC_MEASURE Error. Pin = %d"), nPinIndex);
                    return FALSE;
                }
                memcpy(&dVCMCurrent, arrBuffer, sizeof(double));
                m_result.dThroughVCMCurrent[nCurrentDac] = dVCMCurrent;
            }

            // 记录Glitch数据
            if (m_param.vcm.nGlitch_DetectEn != 0)
            {
                if (m_param.vcm.nGlitch_SmoothEn != 0)
                {
                    if (m_param.vcm.nVCM_ReverseThroughEn == 0)
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
                    }
                    else
                    {
                        if (nCurrentDac == m_param.vcm.nVCM_DACMax)
                        {
                            m_result.dCTSFRHiSmoothValue[nCurrentDac] = GetDacValue(nCurrentDac);
                        }
                        else if (nCurrentDac == m_param.vcm.nVCM_DACMax - m_param.vcm.nVCM_DACStep)
                        {
                            m_result.dCTSFRHiSmoothValue[nCurrentDac] = (GetDacValue(nCurrentDac + m_param.vcm.nVCM_DACStep) * 2.0 + GetDacValue(nCurrentDac)) / 3.0;
                        }
                        else
                        {
                            m_result.dCTSFRHiSmoothValue[nCurrentDac] = (GetDacValue(nCurrentDac + m_param.vcm.nVCM_DACStep * 2) + GetDacValue(nCurrentDac + m_param.vcm.nVCM_DACStep) + GetDacValue(nCurrentDac)) / 3.0;
                        }
                    }
                }
            }
        }

        return TRUE;
    }

    BOOL ResolutionOperator::DoThroughFocus(BOOL *pbIsRunning)
    {
        //-------------------------------------------------------------------------
        // 初始化结果
        m_result.nThroughPeakDac = m_param.vcm.nVCM_DACMin;
        m_result.dThroughHiSFRMax = 0.0;
        m_result.dThroughHiSFRMin = 100.0;
        m_result.nGlitchDac = 0;
        m_result.dMaxGlitch = 0.0;
        m_result.bHaveGlitch = FALSE;
        m_result.bIsVCMMove = TRUE;
        memset(&m_result.dThroughVCMCurrent, 0, sizeof(m_result.dThroughVCMCurrent));
        memset(&m_result.dSFRHiThroughValue, 0, sizeof(m_result.dSFRHiThroughValue));
        memset(&m_result.dCTSFRHiSmoothValue, 0, sizeof(m_result.dCTSFRHiSmoothValue));
        memset(&m_result.dWeightThroughValue, 0, sizeof(m_result.dWeightThroughValue));
        
        ThroughFocus_Std::THROUGHFOCUS_PARAM stTFparam = {0};
        stTFparam.dPreFocusDownRate = m_param.vcm.dVCM_PreThroughDownPersent;
        stTFparam.dPreFocusPeakMin = m_param.vcm.dVCM_PreThroughPeakMin;
        stTFparam.lpfnDoStepSfr = CallbackDoStepSfr;
        stTFparam.lpfnLogOut = CallbackLogOut;
        stTFparam.lpfnSetVcmMove = CallbackSetVcmMove;
        stTFparam.nDetailReverseThroughEn = m_param.vcm.nVCM_ReverseThroughEn;
        stTFparam.nDetailStepDac = m_param.vcm.nVCM_DACStep;
        stTFparam.nDetailStepTime = m_param.vcm.nVCM_StepTime;
        stTFparam.nMaxDac = m_param.vcm.nVCM_DACMax;
        stTFparam.nMinDac = m_param.vcm.nVCM_DACMin;
        stTFparam.nPreFocusEn = m_param.vcm.nVCM_PreThroughEn;
        stTFparam.nPreFocusStepCount = m_param.vcm.nVCM_PreThroughDivideCount;
        stTFparam.nPreFocusStepTime = m_param.vcm.nVCM_PreThroughStepTime;
        stTFparam.nSuperStepTime = m_param.vcm.nVCM_FinalDelayTime;

        m_bEnableStack = FALSE;    // 为了在跑ThroughFocus时，不叠数值
        m_ThroughFocusTime = 0.0;
        m_ThroughFocusTimeCount.SetStartTime();
        int nRet = ThroughFocus_Std::DoThroughFocus(pbIsRunning, stTFparam);
        m_ThroughFocusTime = m_ThroughFocusTimeCount.GetPassTime();
        m_bEnableStack = TRUE;

        if (nRet != ThroughFocus_Std::TFE_OK)
        {
            return FALSE;
        }
        m_param.vcm.nVCM_DACMax = stTFparam.nMaxDac;
        m_param.vcm.nVCM_DACMin = stTFparam.nMinDac;
        return TRUE;
    }

    //------------------------------------------------------------------------------
    ResolutionOperator *pThis = nullptr;
    void CallbackLogOut(__in LPCTSTR lpLogText)
    {
        uts.log.Debug(lpLogText);
    }

    void CallbackSetVcmMove(__in int nCurrentDac)
    {
        if (nullptr != pThis)
        {
            pThis->SetVcmMove(nCurrentDac);
        }
    }

    BOOL CallbackDoStepSfr(
        __in ThroughFocus_Std::eThroughfocusPhase ePhase,
        __in int nCurrentDac,
        __out double &dKeySfr) 
    {
        BOOL bRet = FALSE;
        if (nullptr != pThis)
        {
            bRet = pThis->DoStepSfr(ePhase, nCurrentDac, dKeySfr);
        }
        return bRet;
    }

	int ResolutionOperator::Update_start_current(void)
	{
		int ret = 0;
		USES_CONVERSION;

		m_result.nStart_current = m_param.vcm.nVCM_DACMin -
			(m_param.vcm.nVCM_DACMax - m_param.vcm.nVCM_DACMin) / m_param.vcm.nVCM_PreThroughDivideCount;

		if (m_result.nStart_current < m_param.vcm.nVCM_DACMin) m_result.nStart_current = m_param.vcm.nVCM_DACMin;

		unsigned char AFDAC[2] = {0};
		put_le_val( m_result.nStart_current, AFDAC, sizeof(AFDAC));

		uts.log.Debug(_T("Update Macro DAC to DataBase... Byte[0] = %02x, Byte[1] = %02x"),  AFDAC[0], AFDAC[1]);

		ret = uts.otpdb->UpdateOtpByType(uts.otpdb->GetModuleID(T2A(uts.info.strSN)), OTPDB_OTPTYPE_AFSTART, (char *)AFDAC, sizeof(AFDAC));
		if (ret < 0)
		{
			uts.log.Error(_T("update af start current data error!!!"));
		}


		return 0;
	}


	BOOL ResolutionOperator::CommitDAC(int _DAC)
	{
		uts.log.Debug(_T("UPdate init DAC DB"));
		USES_CONVERSION;
		int DBtype;

		//0: Disable 1:Inf 2:MUP
		if (uts.info.nOtpDBType != -1)
		{
			int ret = 0;
			uts.otpdb->get_instance();

			switch (m_param.nAF_Commit_Type)
			{
			case 1:
				DBtype = OTPDB_OTPTYPE_AFINF;
				break;
			case 2:
				DBtype = OTPDB_OTPTYPE_AFMUP;
				break;
			default:
				return TRUE;
			}

			unsigned char AFDAC[2] = {0};
			put_le_val( _DAC, AFDAC, sizeof(AFDAC));
	
			uts.log.Debug(_T("Update Macro DAC to DataBase... Byte[0] = %02x, Byte[1] = %02x"),  AFDAC[0], AFDAC[1]);

			ret = uts.otpdb->UpdateOtpByType(uts.otpdb->GetModuleID(T2A(uts.info.strSN)), DBtype, (char *)AFDAC, sizeof(AFDAC));
			if (ret < 0)
			{
				uts.log.Error(_T("update af %d peak  data error!!!"),DBtype);
			}
		}


		return TRUE;
	}



    BaseOperator* GetOperator(void)
    {
        return (new ResolutionOperator);
    }
    //------------------------------------------------------------------------------
}
