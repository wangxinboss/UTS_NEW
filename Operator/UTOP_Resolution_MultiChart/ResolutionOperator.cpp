#include "StdAfx.h"
#include "ResolutionOperator.h"
#include "UTOP_Resolution_MultiChart.h"
#include "VCM.h"
#include "Global_Memory_SFC.h"

#include "otpdb.h"


#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "CommonFunc.lib")

#include "../../3rdparty/MultiSFR/algorithm_geometry_multisfr.h"
#pragma comment(lib, "algorithm.lib")
//#pragma comment(lib, "algorithm_geometry_multisfr.lib")

using namespace UTS;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace DDM;

#define OFFSETX 100
#define OFFSETY 100



namespace UTS
{
	void  ResolutionOperator::SaveImage2(
		unsigned char *ImageBuffer,
		int Width,
		int Height,
		LPCTSTR lpName)
	{
		CString strName;

		SYSTEMTIME st;
		GetLocalTime(&st);
		//-------------------------------------------------------------------------
		// 输出Image file
		CString strFilePath;

		CString strDirPath;
		strDirPath.Format(_T("%s\\Image-%s-%s-%s-%04d%02d%02d\\"),
			uts.info.strResultPath,
			uts.info.strProjectName,
			uts.info.strLineName,
			uts.info.strStationName,
			st.wYear,
			st.wMonth, 
			st.wDay);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		strFilePath.Format(_T("%s\\%s"),
			strDirPath,
			lpName);

		if (!uts.imgFile.SaveBmpFile(
			strFilePath,
			ImageBuffer,
			Width,
			Height))
		{
			CString strMsg;
			strMsg.Format(_T("SaveBmpFile Fail. Path = %s"), strFilePath);
			uts.log.Error(strMsg);
			AfxMessageBox(strMsg);
			return;
		}
	}

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

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.dFrqNum[SFR_Frq_Hi]"), m_param.sfr.dFrqNum[SFR_Frq_Hi], 0.125, _T("High frq number."));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.dFrqNum[SFR_Frq_Lo]"), m_param.sfr.dFrqNum[SFR_Frq_Lo], 0.25, _T("Low frq number. if -1, do not show."));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.dROIField"), m_param.dROIField, 0.7, _T("Field of ROI"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_StepTime"), m_param.vcm.nVCM_StepTime, 350, _T("Step time."));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_FinalDelayTime"), m_param.vcm.nVCM_FinalDelayTime, 1500, _T("Final delay time."));
		
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_DACMin"), m_param.vcm.nVCM_DACMin, 0, _T("Min DAC."));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_DACMax"), m_param.vcm.nVCM_DACMax, 400, _T("Max DAC."));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcm.nVCM_DACStep"), m_param.vcm.nVCM_DACStep, 4, _T("1 step DAC number."));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("CutImageWidth"), m_param.CutImageWidth, 550, _T("CutImageWidth"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("CutImageHeight"), m_param.CutImageHeight, 550, _T("CutImageHeight"));


		/*
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
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgROI.strROIName, arrDefaultROIName[i], strComment);

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

		//DB
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nAF_Commit_Type"), m_param.nAF_Commit_Type, 0, _T("0: Disable 1:Inf 2:MUP"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nStartCurrent_En"), m_param.nStartCurrent_En, 0, _T("0: Disable 1:Enable"));
		*/
		m_param.szPtCount.cx = 3;
		m_param.szPtCount.cy = 3;

	//	m_param.szPtCount.cx = 31;
	//	m_param.szPtCount.cy = 19;

		m_param.sfr.nSfrRoiCount = 5;

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

			int ret = uts.otpdb->GetOtpByType(uts.otpdb->GetModuleID(T2A(uts.info.strSN)),11 , 
											  (char *)tmp, sizeof(tmp));
			if (ret < 0)
			{
				uts.log.Error(_T("get Hall data error!!!"));
				return;
			}

			HallData[0] = get_le_val(tmp + 0, 1);
			HallData[1] = get_le_val(tmp + 1, 1);
			HallData[2] = get_le_val(tmp + 2, 2);
			HallData[3] = get_le_val(tmp + 4, 2);

			DLLSetVCM_Initial(m_pDevice, uts.info.nVCMType,HallData);
		}
		
	}


    BOOL ResolutionOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
    {
		

        return TRUE;
    }

    BOOL ResolutionOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
		//------------------------------------------------------------------------------
		// 初始化
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);

		//------------------------------------------------------------------------------
		// 初始化结果
		*pnErrorCode = uts.errorcode.E_Pass;
		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_POINT pt;
		UI_TEXT text;
		UI_TEXT_REL textRel;
		UI_LINE line;
		UI_MARK uiMark;

		foundation::dCoordinate *MultiSFRIntersectionCoordinate = new foundation::dCoordinate[m_param.szPtCount.cx*m_param.szPtCount.cy];
		memset(MultiSFRIntersectionCoordinate,0,sizeof(foundation::dCoordinate)*m_param.szPtCount.cx*m_param.szPtCount.cy);
		foundation::dCoordinate *MultiSFRROICenterCoordinate = new foundation::dCoordinate[(2*m_param.szPtCount.cx*m_param.szPtCount.cy-m_param.szPtCount.cx-m_param.szPtCount.cy)];
		unsigned char *CutBMPBuffer = new unsigned char[m_param.CutImageWidth * m_param.CutImageHeight*3];								 
		memset(CutBMPBuffer,0,sizeof(unsigned char)*(m_param.CutImageWidth * m_param.CutImageHeight)*3);

		m_param.nManualThroughFocusFrameCount = 0;
		m_result.dThroughHiSFRMax = 0;
		m_result.nThroughPeakDac = 0;
		memset(m_result.dSFRHiThroughValue,0,sizeof(m_result.dSFRHiThroughValue));
		
		POINT TestROI[5];
		TestROI[0].x = int(m_bufferInfo.nWidth/2);
		TestROI[0].y = int(m_bufferInfo.nHeight/2);
		TestROI[1].x = int(m_bufferInfo.nWidth* 0.5 - m_bufferInfo.nWidth* 0.5 *m_param.dROIField) ;
		TestROI[1].y = int(m_bufferInfo.nHeight* 0.5 - m_bufferInfo.nHeight* 0.5 *m_param.dROIField);	 
		TestROI[2].x = int(m_bufferInfo.nWidth* 0.5 + m_bufferInfo.nWidth* 0.5 *m_param.dROIField );
		TestROI[2].y = int(m_bufferInfo.nHeight* 0.5 - m_bufferInfo.nHeight* 0.5 *m_param.dROIField);	 
		TestROI[3].x = int(m_bufferInfo.nWidth* 0.5 - m_bufferInfo.nWidth* 0.5 *m_param.dROIField );
		TestROI[3].y = int(m_bufferInfo.nHeight* 0.5 + m_bufferInfo.nHeight* 0.5 *m_param.dROIField);
		TestROI[4].x = int(m_bufferInfo.nWidth* 0.5 + m_bufferInfo.nWidth* 0.5 *m_param.dROIField );
		TestROI[4].y = int(m_bufferInfo.nHeight* 0.5 + m_bufferInfo.nHeight* 0.5 *m_param.dROIField);	 

		double CurrentXY[2];

		//Start->End
		int _DAC = m_param.vcm.nVCM_DACMin;
		int X = -1 ;
		int Y = -1 ;
		bool bFind = false;

		while ((_DAC <= m_param.vcm.nVCM_DACMax) )
		{				
			DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, _DAC);
			Algorithm::Sleep(m_param.vcm.nVCM_StepTime);

			uiMark.clear();

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

			uts.log.Debug(L"DAC = 0x%x ThroughSFR = %.2f",_DAC,m_result.dThroughHiSFRMax);
			_DAC += m_param.vcm.nVCM_DACStep;
			memset(m_result.dSfr,0,sizeof(double)*5);

			
			for (int i = 0 ; i < 5; i++)
			{
				for (X = -1 ; X < 2 ;X++)
				{
					for (Y = -1 ; Y < 2 ;Y++)
					{
						UTS::Algorithm::Image::CImageProc::GetInstance().GetBMPBlockBuffer(
							m_bufferObj.pBmpBuffer,
							CutBMPBuffer,
							m_bufferInfo.nWidth ,
							m_bufferInfo.nHeight,
							TestROI[i].x - m_param.CutImageWidth/2 +OFFSETX*X,
							TestROI[i].y - m_param.CutImageHeight/2+OFFSETY*Y,
							m_param.CutImageWidth,
							m_param.CutImageHeight);

						SaveImage2(CutBMPBuffer,m_param.CutImageWidth,m_param.CutImageHeight,L"test");
						SaveImage2(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,L"Full");

						uts.log.Debug(_T("Start Test.."));

						//SFR start 
						if (!algorithm::geometry::multisfr::GetMultiSFRIntersectionCoordinate(CutBMPBuffer,
							m_param.CutImageWidth,
							m_param.CutImageHeight,
							m_param.szPtCount.cx,
							m_param.szPtCount.cy,
							MultiSFRIntersectionCoordinate))
						{
							uts.log.Error(_T("GetChessboardPos error. "));
							// 画图
							//m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);
							//continue;
							bFind = false;
						}
						else
						{
							uts.log.Debug(_T("GetChessboardPos !"));
							bFind = true;
							break;
						}
					}

					if(bFind)
					{
						break;
					}
				}

				

				memset(MultiSFRROICenterCoordinate,0,sizeof(foundation::dCoordinate)*(2*m_param.szPtCount.cx*m_param.szPtCount.cy-m_param.szPtCount.cx-m_param.szPtCount.cy));

				algorithm::geometry::multisfr::GetMultiSFRROICoordinate(MultiSFRIntersectionCoordinate,
																		m_param.szPtCount.cx,
																		m_param.szPtCount.cy,
																		MultiSFRROICenterCoordinate);
						
				CurrentXY[0] = m_param.CutImageWidth/2;
				CurrentXY[1] = m_param.CutImageHeight/2;

				int iROICount = algorithm::geometry::multisfr::GetSFRROICoordinate(MultiSFRROICenterCoordinate,(2*m_param.szPtCount.cx*m_param.szPtCount.cy-m_param.szPtCount.cx-m_param.szPtCount.cy),CurrentXY);
				if(iROICount == 0) continue;
				uts.log.Debug(_T("SFRCount:%d "),iROICount);

				foundation::iROI* ROIInfo = new foundation::iROI[iROICount];
				memset(ROIInfo,0,sizeof(foundation::iROI)*iROICount);

				algorithm::geometry::multisfr::GetSFRROICoordinate(MultiSFRROICenterCoordinate,(2*m_param.szPtCount.cx*m_param.szPtCount.cy-m_param.szPtCount.cx-m_param.szPtCount.cy),CurrentXY,iROICount,ROIInfo);

				RECT FocusROI;
				for (int j = 0 ; j< iROICount ; j++)
				{
					FocusROI.left = ROIInfo[j].x + (TestROI[i].x - (m_param.CutImageWidth/2)) + OFFSETX*X;
					FocusROI.right = FocusROI.left + ROIInfo[j].width ;
					FocusROI.top = ROIInfo[j].y  + (TestROI[i].y - (m_param.CutImageHeight/2))+ OFFSETY*Y;
					FocusROI.bottom = FocusROI.top + ROIInfo[j].height;

					double dLoSFR ;
					double dhiSFR ;
					Algorithm::SFR::SfrCalc::GetBlockSFR(
						m_bufferObj.pBmpBuffer,
						m_bufferInfo.nWidth ,
						m_bufferInfo.nHeight,
						FocusROI,
						m_param.sfr.dFrqNum[SFR_Frq_Hi],
						m_param.sfr.dFrqNum[SFR_Frq_Lo],
						dhiSFR,
						dLoSFR);

					m_result.dSfr[i] += dhiSFR;

					rc.color = COLOR_BLUE;
					memcpy(&rc.rcPos, &FocusROI, sizeof(RECT));
					uiMark.vecUiRect.push_back(rc);
				}

				m_result.dSfr[i]/= iROICount; 
				m_result.dSFRHiThroughValue[i][m_param.nManualThroughFocusFrameCount] = m_result.dSfr[i];

				uts.log.Debug(_T("SFR[%d]:%.2f "),i,m_result.dSfr[i]);

				text.color = COLOR_BLUE;
				text.ptPos = CPoint(rc.rcPos.left, rc.rcPos.bottom);
				text.strText.Format(_T("%.1f"), m_result.dSfr[i]);
				uiMark.vecUIText.push_back(text);
				RELEASE_ARRAY(ROIInfo);	
			}
		
			m_param.nManualThroughFocusFrameCount ++ ;
			//SFR end

			// 画图
			m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
			if(m_result.dThroughHiSFRMax <  m_result.dSfr[0]) 
			{
				m_result.dThroughHiSFRMax = m_result.dSfr[0];
				m_result.nThroughPeakDac = _DAC;
			}
		}

		uiMark.clear();

		DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, m_result.nThroughPeakDac);
		Algorithm::Sleep(m_param.vcm.nVCM_FinalDelayTime);
		memset(m_result.dSfr,0,sizeof(double)*5);

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

		for (int i = 0 ; i < 5; i++)
		{
			for (X = -1 ; X < 2 ;X++)
			{
				for (Y = -1 ; Y < 2 ;Y++)
				{
					UTS::Algorithm::Image::CImageProc::GetInstance().GetBMPBlockBuffer(
						m_bufferObj.pBmpBuffer,
						CutBMPBuffer,
						m_bufferInfo.nWidth ,
						m_bufferInfo.nHeight,
						TestROI[i].x - m_param.CutImageWidth/2 +OFFSETX*X,
						TestROI[i].y - m_param.CutImageHeight/2+OFFSETY*Y,
						m_param.CutImageWidth,
						m_param.CutImageHeight);

					//SFR start 
					if (!algorithm::geometry::multisfr::GetMultiSFRIntersectionCoordinate(CutBMPBuffer,
						m_param.CutImageWidth,
						m_param.CutImageHeight,
						m_param.szPtCount.cx,
						m_param.szPtCount.cy,
						MultiSFRIntersectionCoordinate))
					{
						uts.log.Error(_T("GetChessboardPos error. "));
						// 画图
						//m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);
						//continue;
						bFind = false;
					}
					else
					{
						bFind = true;
						break;
					}
				}

				if(bFind)
				{
					break;
				}
			}

			algorithm::geometry::multisfr::GetMultiSFRROICoordinate(MultiSFRIntersectionCoordinate,
				m_param.szPtCount.cx,
				m_param.szPtCount.cy,
				MultiSFRROICenterCoordinate);

			CurrentXY[0] = m_param.CutImageWidth/2;
			CurrentXY[1] = m_param.CutImageHeight/2;

			int iROICount = algorithm::geometry::multisfr::GetSFRROICoordinate(MultiSFRROICenterCoordinate,(2*m_param.szPtCount.cx*m_param.szPtCount.cy-m_param.szPtCount.cx-m_param.szPtCount.cy),CurrentXY);
			foundation::iROI* ROIInfo = new foundation::iROI[iROICount];
			memset(ROIInfo,0,sizeof(foundation::iROI)*iROICount);
			algorithm::geometry::multisfr::GetSFRROICoordinate(MultiSFRROICenterCoordinate,(2*m_param.szPtCount.cx*m_param.szPtCount.cy-m_param.szPtCount.cx-m_param.szPtCount.cy),CurrentXY,iROICount,ROIInfo);

			RECT FocusROI;

			for (int j = 0 ; j< iROICount ; j++)
			{
				FocusROI.left = ROIInfo[j].x + (TestROI[i].x - (m_param.CutImageWidth/2)) + OFFSETX*X;
				FocusROI.right = FocusROI.left + ROIInfo[j].width ;
				FocusROI.top = ROIInfo[j].y  + (TestROI[i].y - (m_param.CutImageHeight/2))+ OFFSETY*Y;
				FocusROI.bottom = FocusROI.top + ROIInfo[j].height;

				double dLoSFR ;
				double dhiSFR ;
				Algorithm::SFR::SfrCalc::GetBlockSFR(
					m_bufferObj.pBmpBuffer,
					m_bufferInfo.nWidth,
					m_bufferInfo.nHeight,
					FocusROI,
					m_param.sfr.dFrqNum[SFR_Frq_Hi],
					m_param.sfr.dFrqNum[SFR_Frq_Lo],
					dhiSFR,
					dLoSFR);

				m_result.dSfr[i] += dhiSFR;
				rc.color = COLOR_BLUE;
				memcpy(&rc.rcPos, &FocusROI, sizeof(RECT));
				uiMark.vecUiRect.push_back(rc);
			}
			m_result.dSfr[i]/= iROICount; 
			
			uts.log.Debug(_T("SFR[%d]:%.2f "),i,m_result.dSfr[i]);

			text.color = COLOR_BLUE;
			text.ptPos = CPoint(rc.rcPos.left, rc.rcPos.bottom);
			text.strText.Format(_T("%.1f"), m_result.dSfr[i]);
			uiMark.vecUIText.push_back(text);
			RELEASE_ARRAY(ROIInfo);	
		}

		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);


end:
		RELEASE_ARRAY(MultiSFRROICenterCoordinate);
		RELEASE_ARRAY(MultiSFRIntersectionCoordinate);
		RELEASE_ARRAY(CutBMPBuffer);

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
     //   CString strVcmMoveResult = ((m_param.vcm.nVCM_ThroughDriveEn != 0) ? (m_result.bIsVCMMove ? PASS_STR : FAIL_STR) : _T("NO JUDGE"));
      //  CString strGlitchResult = ((m_param.vcm.nVCM_ThroughDriveEn != 0 && m_param.vcm.nGlitch_DetectEn != 0) ? (m_result.bHaveGlitch ? FAIL_STR : PASS_STR) : _T("NO JUDGE"));

        //-------------------------------------------------------------------------
        // Head
        strHeader = _T("Time,SN,TestTime(ms),Result,");
        strData.Format(_T("%s,%s,%.1f,%s,"),
            lpTime,
            uts.info.strSN,
            m_TimeCounter.GetPassTime(),
            //m_result.dYvalue,(会拉低帧率2fps左右，故而屏蔽)
            //m_result.dCenterYavg,
            strResult);

        //-------------------------------------------------------------------------
		 for (int i = 0; i < 5; i++)
		 {
			 strHeader.AppendFormat(_T("Filed%d_Hi,"), i);
			 strData.AppendFormat(_T("%.1f,"), m_result.dSfr[i]);
		 }
		 /*
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
			*/
		 // Tail
		 strHeader.Append(_T("Version,OP_SN\n"));
		 strData.AppendFormat(_T("%s,%s\n"),
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
       // int _count = (m_param.vcm.nVCM_DACMax - m_param.vcm.nVCM_DACMin) / m_param.vcm.nVCM_DACStep + 1;
		int _count = m_param.nManualThroughFocusFrameCount;
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
      
        for (int j = 0; j < m_param.sfr.nSfrRoiCount; j++)
        {
			if(j!=0)
			{
				 tir.sData.Append(_T(",,,,,,"));
			}else
			{
				tir.sData.Append(_T(","));
			}

            for (int i = 0; i < _count; i++)
            {
                tir.sData.AppendFormat(_T(",%.01lf"),
                    m_result.dSFRHiThroughValue[j][i]);
            }
            tir.sData.Append(_T("\n"));
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
