#include "StdAfx.h"
#include "ResolutionOperator.h"
#include "UTOP_Resolution_Mirage.h"
//#include "Global_Memory_SFC.h"
#include "Algorithm.h"
#include "otpdb.h"

#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "CommonFunc.lib")

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
       //------------------------------------------------------------------------------
       // white patch
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("whitePatch.sizeDisFromCT.cx"), nValue, 100, _T("WhitePatch ROI horizontal distance from Center."));
        m_param.whitePatch.sizeDisFromCT.cx = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("whitePatch.sizeDisFromCT.cy"), nValue, 0, _T("WhitePatch ROI vertical distance from Center."));
        m_param.whitePatch.sizeDisFromCT.cy = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("whitePatch.nROISize"), m_param.whitePatch.nROISize, 100, _T("WhitePatch ROI size."));
        //------------------------------------------------------------------------------
        // sfr
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("DetectSFRROI.cx"), nValue, 500, _T("SFR ROI size.x"));
		m_param.DetectSFRROI.cx = nValue;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("DetectSFRROI.cy"), nValue, 500, _T("SFR ROI size.y"));
		m_param.DetectSFRROI.cy = nValue;

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nSfrType"), m_param.sfr.nSfrType, 0, _T("0:Real, 1:AllAverage, 2: MidAverage"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nSfrStackCount"), m_param.sfr.nSfrStackCount, 1, _T("Stack count."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.sizeSFR_ROI.cx"), nValue, 100, _T("Horizontal size."));
        m_param.sfr.sizeSFR_ROI.cx = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.sizeSFR_ROI.cy"), nValue, 100, _T("Vertical size."));
        m_param.sfr.sizeSFR_ROI.cy = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.dFrqNum[SFR_Frq_Hi]"), m_param.sfr.dFrqNum[SFR_Frq_Hi], 0.125, _T("High frq number."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.dFrqNum[SFR_Frq_Lo]"), m_param.sfr.dFrqNum[SFR_Frq_Lo], 0.25, _T("Low frq number. if -1, do not show."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.dSFRScore"), m_param.sfr.dSFRScore, -1.0, _T("SFR score. if -1, Not use."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nSfrRoiCount"), m_param.sfr.nSfrRoiCount, 9, _T("Total Sfr roi count."));

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
            strKey.Format(_T("sfr.vecSfrRoi[%03d].strROIName"), i);
            strComment.Format(_T("Name of sfr roi %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgROI.strROIName, arrDefaultROIName[i]/*strDefaultRoiName*/, strComment);

            m_param.sfr.vecSfrRoi.push_back(sgROI);
        }

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nFieldCount"), m_param.sfr.nFieldCount, 3, _T("Field count."));
        m_param.sfr.vecField.clear();
        for (int i = 0; i < m_param.sfr.nFieldCount; i++)
        {
            SINGLE_FIELD_INFO sgField = {0};
            strKey.Format(_T("sfr.vecField[%d].dHiSpec"), i);
            strComment.Format(_T("Hi frq spec of field %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgField.dHiSpec, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].dHiDeltaSpec"), i);
            strComment.Format(_T("Hi frq delta spec of field %d."), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, sgField.dHiDeltaSpec, 0.0, strComment);

            strKey.Format(_T("sfr.vecField[%d].SfrIndexList"), i);
            strComment.Format(_T("Sfr roi list of field %d. For example: 0,1,2,3"), i);
            uts.dbCof.GetOperatorSingleSpec(strSection, strKey, strValue, EMPTY_STR, strComment);
            SplitInt(strValue, sgField.vecSfrIndex);
            
            m_param.sfr.vecField.push_back(sgField);
        }
        //------------------------------------------------------------------------------        
        SFRPLUS_PARAM_STATIC sfrParamStatic = {0};
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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nSfrType"), m_param.sfr.nSfrType, 0, _T("0:Real, 1:AllAverage, 2: MidAverage"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("sfr.nSfrStackCount"), m_param.sfr.nSfrStackCount, 1, _T("Stack count."));

		m_SfrQueue.Initialize(m_param.sfr.nSfrRoiCount, m_param.sfr.nSfrStackCount, m_param.sfr.nSfrType);
		m_bEnableStack = TRUE;

        return TRUE;
    }
	
	void ResolutionOperator::OnPreviewStart()
	{

	}


    BOOL ResolutionOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        //------------------------------------------------------------------------------
        // 单帧数据初始化结果
        m_result.bIsCenterYExp = FALSE;
        m_result.dCenterYavg = 0.0;
        memset(&m_result.rcWhitePatch, 0, sizeof(RECT));
        memset(&m_result.stSfrResult, 0, sizeof(SFRPLUS_RESULT));
        m_result.vecFieldResult.clear();
        //------------------------------------------------------------------------------
        // 计算SFR数值
        SFRPLUS_PARAM_DYNAMIC sfrParamDynamic = {0};
        //------------------------------------------------------------------------------
        // 计算十字位置
//         if (m_param.nFixRoiEn != 0)
//         {
// 
//         }
//         else
//         {
// 
//         }
		int roiw = m_param.DetectSFRROI.cx; 
		int roiH = m_param.DetectSFRROI.cy; 
		unsigned char *pROIBuffer = new unsigned char[roiw*roiH*3];

		double dFullField = Algorithm::Math::GetDistance2D(
							0,
							0,
							nWidth/2,
							nHeight/2);

		for (int i = 0; i < m_param.sfr.nSfrRoiCount/2 ; i ++)
		{
			//if(i !=7 ) continue;

			UTS::Algorithm::SFRBlock::SFRROI sfrroi;

			int ROIX = int (nWidth/2 + m_param.sfr.vecSfrRoi[2*i].dROIField * dFullField * cos(Angle2Radian(m_param.sfr.vecSfrRoi[2*i].dROIAngle)));
			int ROIY = int (nHeight/2+ m_param.sfr.vecSfrRoi[2*i].dROIField * dFullField * sin(Angle2Radian(m_param.sfr.vecSfrRoi[2*i].dROIAngle)));

			CImageProc::GetInstance().GetBMPBlockBuffer(
				pBmpBuffer,
				pROIBuffer,
				nWidth,
				nHeight,
				ROIX - roiw/2,
				ROIY - roiH/2,
				roiw,
				roiH);

			if(ROI_SFR(pROIBuffer,roiw,roiH,&sfrroi) == false)
			{
				sfrroi.x = roiw/2;
				sfrroi.y = roiH/2;
				sfrroi.width = roiw/2;
				sfrroi.height = roiH/2;
			}

			//V
			if(i == 0 || i== 1 || i== 2 || i == 5 || i== 8)
			{
				m_result.stSfrResult.roi.rcRect[2*i].left = sfrroi.x + (ROIX - roiw/2) + (sfrroi.width/2) - (m_param.sfr.sizeSFR_ROI.cx/2);
				m_result.stSfrResult.roi.rcRect[2*i].top =  sfrroi.y + (ROIY - roiH/2) - (m_param.sfr.sizeSFR_ROI.cy/2);
				m_result.stSfrResult.roi.rcRect[2*i].right = m_result.stSfrResult.roi.rcRect[2*i].left + m_param.sfr.sizeSFR_ROI.cx;
				m_result.stSfrResult.roi.rcRect[2*i].bottom = m_result.stSfrResult.roi.rcRect[2*i].top + m_param.sfr.sizeSFR_ROI.cy;
 			}
			else
			{
				m_result.stSfrResult.roi.rcRect[2*i].left = sfrroi.x + (ROIX - roiw/2) - (sfrroi.width/2) - (m_param.sfr.sizeSFR_ROI.cx/2);
				m_result.stSfrResult.roi.rcRect[2*i].top =  sfrroi.y + (ROIY - roiH/2) - (m_param.sfr.sizeSFR_ROI.cy/2);;
				m_result.stSfrResult.roi.rcRect[2*i].right = m_result.stSfrResult.roi.rcRect[2*i].left + m_param.sfr.sizeSFR_ROI.cx;
				m_result.stSfrResult.roi.rcRect[2*i].bottom = m_result.stSfrResult.roi.rcRect[2*i].top + m_param.sfr.sizeSFR_ROI.cy;
			}

			GetBlockSFR(
				pBmpBuffer,
				nWidth,
				nHeight,
				m_result.stSfrResult.roi.rcRect[2*i],
				m_param.sfr.dFrqNum[SFR_Frq_Hi],
				m_param.sfr.dFrqNum[SFR_Frq_Lo],
				m_result.stSfrResult.dSFR[2*i][SFR_Frq_Hi],
				m_result.stSfrResult.dSFR[2*i][SFR_Frq_Lo]);

			//////////////////////////////////////////////////////////////////////////
			//H
			if(i == 0 || i== 3 || i== 4 || i == 7 || i== 8)
			{
				m_result.stSfrResult.roi.rcRect[2*i+1].left = sfrroi.x + (ROIX - roiw/2) - (m_param.sfr.sizeSFR_ROI.cx/2);
				m_result.stSfrResult.roi.rcRect[2*i+1].top =  sfrroi.y + (ROIY - roiH/2) - (sfrroi.height/2) - (m_param.sfr.sizeSFR_ROI.cy/2);
				m_result.stSfrResult.roi.rcRect[2*i+1].right = m_result.stSfrResult.roi.rcRect[2*i+1].left + m_param.sfr.sizeSFR_ROI.cx;
				m_result.stSfrResult.roi.rcRect[2*i+1].bottom = m_result.stSfrResult.roi.rcRect[2*i+1].top + m_param.sfr.sizeSFR_ROI.cy;
			}
			else
			{
				m_result.stSfrResult.roi.rcRect[2*i+1].left = sfrroi.x + (ROIX - roiw/2) - (m_param.sfr.sizeSFR_ROI.cx/2);
				m_result.stSfrResult.roi.rcRect[2*i+1].top =  sfrroi.y + (ROIY - roiH/2) + (sfrroi.height/2) - (m_param.sfr.sizeSFR_ROI.cy/2);
				m_result.stSfrResult.roi.rcRect[2*i+1].right = m_result.stSfrResult.roi.rcRect[2*i+1].left + m_param.sfr.sizeSFR_ROI.cx;
				m_result.stSfrResult.roi.rcRect[2*i+1].bottom = m_result.stSfrResult.roi.rcRect[2*i+1].top + m_param.sfr.sizeSFR_ROI.cy;
			}

			GetBlockSFR(
				pBmpBuffer,
				nWidth,
				nHeight,
				m_result.stSfrResult.roi.rcRect[2*i+1],
				m_param.sfr.dFrqNum[SFR_Frq_Hi],
				m_param.sfr.dFrqNum[SFR_Frq_Lo],
				m_result.stSfrResult.dSFR[2*i+1][SFR_Frq_Hi],
				m_result.stSfrResult.dSFR[2*i+1][SFR_Frq_Lo]);
		}

		if (m_bEnableStack)
		{
			m_SfrQueue.AddSfrResult(m_result.stSfrResult);
			m_SfrQueue.GetSfrResult(m_result.stSfrResult);
		}

		RELEASE_ARRAY(pROIBuffer);
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
        // 中心亮度
        unsigned char *pROIDataWhitePatch = new BYTE[m_param.whitePatch.nROISize * m_param.whitePatch.nROISize * 3];
        memset(pROIDataWhitePatch, 0, m_param.whitePatch.nROISize * m_param.whitePatch.nROISize * 3);
        m_result.rcWhitePatch.left = (nWidth/2) + m_param.whitePatch.sizeDisFromCT.cx - m_param.whitePatch.nROISize / 2;
        m_result.rcWhitePatch.top = (nHeight/2) + m_param.whitePatch.sizeDisFromCT.cy - m_param.whitePatch.nROISize / 2;
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
//         m_result.bIsCenterYExp = Algorithm::CalYavgExp(
//             pROIDataWhitePatch,
//             m_param.whitePatch.nROISize,
//             m_param.whitePatch.nROISize,
//             m_result.dCenterYavg);
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

		line.ptBegin = CPoint(0, 0);
		line.ptEnd = CPoint(nWidth, nHeight );
		uiMark.vecUiLine.push_back(line);

		line.ptBegin = CPoint(0, nHeight );
		line.ptEnd = CPoint(nWidth, 0);
		uiMark.vecUiLine.push_back(line);
     
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

			if(m_param.sfr.vecField[i].vecSfrIndex.size() > 1)
			{
				textRel.dWidthPersent = 0.2;
				textRel.dHeightPersent = 0.05 * i;
				strSfrDelta.Format(_T("SFRFiled%d_HiDelta: %.1f"),
					i, m_result.vecFieldResult[i].dSfrDelta[SFR_Frq_Hi]);
				textRel.strText = strSfrDelta;
				uiMark.vecUITextRel.push_back(textRel);
			}
        }   // for i

        // white patch
//         rc.color = (m_result.bIsCenterYExp ? COLOR_RED : COLOR_BLUE);
//         rc.rcPos = m_result.rcWhitePatch;
//         uiMark.vecUiRect.push_back(rc);
//         text.color = (m_result.bIsCenterYExp ? COLOR_RED : COLOR_BLUE);
//         text.ptPos = CPoint(rc.rcPos.left, rc.rcPos.top);
//         text.strText.Format(_T("%.1f"), m_result.dCenterYavg);
//         uiMark.vecUIText.push_back(text);

		//SFR ROI detect 
		for (int i = 0; i < m_param.sfr.nSfrRoiCount/2 ; i++)
		{
			rc.rcPos.left = int (nWidth/2 + m_param.sfr.vecSfrRoi[2*i].dROIField * dFullField * cos(Angle2Radian(m_param.sfr.vecSfrRoi[2*i].dROIAngle))) - roiw/2;
			rc.rcPos.top = int (nHeight/2+ m_param.sfr.vecSfrRoi[2*i].dROIField * dFullField * sin(Angle2Radian(m_param.sfr.vecSfrRoi[2*i].dROIAngle))) - roiH/2;
			rc.rcPos.right = rc.rcPos.left + roiw;
			rc.rcPos.bottom = rc.rcPos.top + roiH;

			rc.color =  COLOR_DARKGRAY;
			uiMark.vecUiRect.push_back(rc);
		}



        return TRUE;
    }

    BOOL ResolutionOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);
        
        // 重新设定Sensor序列
        CString strRegName = m_strOperatorName;
   
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
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
        //------------------------------------------------------------------------------
        // 保存图片文件
        SaveImage();
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
		vecReturnValue.push_back(uts.errorcode.E_NVMCheck);
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
        strHeader.Append(_T("Version,OP_SN\n"));
        strData.AppendFormat(_T("%s,%s\n"),
            strVersion,
            uts.info.strUserId);
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
      
        //-------------------------------------------------------------------------
        // 判断过曝
//         if (m_result.bIsCenterYExp)
//         {
//             *pnErrorCode = uts.errorcode.E_Linumance;
//             return FALSE;
//         }

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
      
        return TRUE;
    }

    BaseOperator* GetOperator(void)
    {
        return (new ResolutionOperator);
    }
    //------------------------------------------------------------------------------
}
