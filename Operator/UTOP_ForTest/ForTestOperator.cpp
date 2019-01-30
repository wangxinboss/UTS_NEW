#include "StdAfx.h"
#include "ForTestOperator.h"
#include "UTOP_ForTest.h"
//#include "AlgorithmSony.h"

//#pragma comment(lib, "UTSAlgorithm_Sony.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    ForTestOperator::ForTestOperator(void)
    {
        OPERATOR_INIT;
    }

    ForTestOperator::~ForTestOperator(void)
    {
    }

    BOOL ForTestOperator::OnReadSpec()
    {
        //CString strSection = OSUtil::GetFileName(m_strModuleFile);
        //uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));

        //m_param.stCrParam.stColorParam.nColNum = 6;
        //m_param.stCrParam.stColorParam.nRowNum = 1;
        //m_param.stCrParam.nColorCount = 6/*MAX_COLOR_NUM*/;
        //for (int i = 0; i < m_param.stCrParam.nColorCount; i++)
        //{
        //    m_param.dSpecDeltaE[i] = 10.0;
        //    m_param.stCrParam.stArrColorInfo[i].nColorIndex = i;
        //}

        //m_param.stCrParam.stArrColorInfo[0].stColorLab.dL = 28.778;
        //m_param.stCrParam.stArrColorInfo[0].stColorLab.dA = 14.179;
        //m_param.stCrParam.stArrColorInfo[0].stColorLab.dB = -50.297;

        //m_param.stCrParam.stArrColorInfo[1].stColorLab.dL = 55.261;
        //m_param.stCrParam.stArrColorInfo[1].stColorLab.dA = -38.342;
        //m_param.stCrParam.stArrColorInfo[1].stColorLab.dB = 31.37;

        //m_param.stCrParam.stArrColorInfo[2].stColorLab.dL = 42.101;
        //m_param.stCrParam.stArrColorInfo[2].stColorLab.dA = 53.378;
        //m_param.stCrParam.stArrColorInfo[2].stColorLab.dB = 28.19;

        //m_param.stCrParam.stArrColorInfo[3].stColorLab.dL = 81.733;
        //m_param.stCrParam.stArrColorInfo[3].stColorLab.dA = 4.039;
        //m_param.stCrParam.stArrColorInfo[3].stColorLab.dB = 79.819;

        //m_param.stCrParam.stArrColorInfo[4].stColorLab.dL = 51.935;
        //m_param.stCrParam.stArrColorInfo[4].stColorLab.dA = 49.986;
        //m_param.stCrParam.stArrColorInfo[4].stColorLab.dB = -14.574;

        //m_param.stCrParam.stArrColorInfo[5].stColorLab.dL = 51.038;
        //m_param.stCrParam.stArrColorInfo[5].stColorLab.dA = -28.631;
        //m_param.stCrParam.stArrColorInfo[5].stColorLab.dB = -28.638;

        //m_param.stCrParam.sizeColorROI.cx = 25;
        //m_param.stCrParam.sizeColorROI.cy = 25;

        //m_param.stCrParam.stColorParam.nChartMargin = 1;
        //m_param.stCrParam.stColorParam.nYThreshold = 60;
        //m_param.stCrParam.stColorParam.rcChartROI.left = 150;
        //m_param.stCrParam.stColorParam.rcChartROI.right = 500;
        //m_param.stCrParam.stColorParam.rcChartROI.top = 120;
        //m_param.stCrParam.stColorParam.rcChartROI.bottom = 190;

        return TRUE;
    }

    BOOL ForTestOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        return TRUE;
    }

    BOOL ForTestOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // ≥ı ºªØ
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);
        
        // ◊•Õº
        if (!m_pDevice->Recapture(
            m_bufferObj,
            0,
            1))
        {
            uts.log.Error(_T("Recapture error. "));
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        // ≤‚ ‘
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_RAW8) != 0)
        {
            int nOutMode = 0;
            if (!GetSensorOutMode(nOutMode))
            {
                uts.log.Error(_T("GetSensorOutMode error."));
                *pnErrorCode = uts.errorcode.E_NoImage;
                goto end;
            }

            using namespace UTS::Algorithm::DefectPixel::DefectPixel_Sony_For_SamsungTV;
            DEFECT_PIXEL_PARAM param;
            DEFECT_PIXEL_RESULT result;
            param.sizeNormalMean.cx = 31;
            param.sizeNormalMean.cy = 31;
            param.dThreshold = 0.3;

            DefectPixel(m_bufferObj.pRaw8Buffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight,
                nOutMode, param, result);


        }
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {

            //using namespace UTS::Algorithm::LSC::QualComm;
            //QUALCOMM_LSC_PARAM param;
            //param.nBayerMode = 1/*nOutMode*/;
            //param.nBlackLevel = 16;
            //param.nCenterBlockCountX = 15;
            //param.nCenterBlockCountY = 11;
            //param.nCenterBlockWidth = 200;
            //param.nCenterBlockHeight = 200;
            //QUALCOMM_LSC_RESULT result;

            UI_MARK uiMark;
            //UTS::Algorithm::LSC::QualComm::LSC_Cali(
            //    m_bufferObj.pRaw8Buffer,
            //    m_bufferInfo.nWidth,
            //    m_bufferInfo.nHeight,
            //    param,
            //    result);
            
            //OnPreview(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, uiMark);
            //for (int i = 0; i < m_param.stCrParam.nColorCount; i++)
            //{
            //    uts.log.Debug(_T("ColorIndex = %d, AvgR = %f, AvgG = %f, AvgB = %f, L = %f, a = %f, b = %f"),
            //        m_param.stCrParam.stArrColorInfo[i].nColorIndex,
            //        m_result.rgbAvg[i].dR, m_result.rgbAvg[i].dG, m_result.rgbAvg[i].dB,
            //        m_result.labVal[i].dL, m_result.labVal[i].dA, m_result.labVal[i].dB);
            //}
            // ª≠Õº
            m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

end:
        BOOL bRet = TRUE;
        if (*pnErrorCode != uts.errorcode.E_Pass)
        {
            bRet = FALSE;
        }
        return bRet;
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ForTestOperator);
    }
    //------------------------------------------------------------------------------
}
