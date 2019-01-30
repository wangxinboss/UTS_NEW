#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::Image;

namespace UTS
{
    namespace Algorithm
    {
        namespace ColorChart
        {
            int ROW_NUM = 0;
            int COL_NUM = 0;

            void HorizontalHistogram(
                const unsigned char* pYbuffer,
                int nWidth,
                int nHeight,
                int *pSumHorizonHistogram,
                double *pAvgHorizonHistogram)
            {
                for (int i = 0; i < nWidth; i++)
                {
                    pSumHorizonHistogram[i] = 0;
                    for (int j = 0; j < nHeight; j++)
                    {
                        pSumHorizonHistogram[i] += pYbuffer[j * nWidth + i];
                    }
                    pAvgHorizonHistogram[i] = pSumHorizonHistogram[i] / (double)nHeight;
                }
            }

            void VerticalHistogram(
                const unsigned char* pYbuffer,
                int nWidth,
                int nHeight,
                int *pSumVerticalHistogram,
                double *pAvgVerticalHistogram)
            {
                for (int i = 0; i < nHeight; i++)
                {
                    pSumVerticalHistogram[i] = 0;
                    for (int j = 0; j < nWidth; j++)
                    {
                        pSumVerticalHistogram[i] += pYbuffer[i * nWidth + j];
                    }
                    pAvgVerticalHistogram[i] = pSumVerticalHistogram[i] / (double)nWidth;
                }
            }

            void GetSeriesSide(
                const double *pSeries,
                int nCount,
                double dThreshold,
                long &lLeftSide,
                long &lRightSide)
            {
                BOOL bFindLeft = FALSE;
                BOOL bFindRight = FALSE;
                for (int i = 0; i < nCount / 2; i++)
                {
                    if (!bFindLeft)
                    {
                        if (pSeries[i] < dThreshold)
                        {
                            bFindLeft = TRUE;
                        }
                        lLeftSide = i;
                    }
                    if (!bFindRight)
                    {
                        if (pSeries[nCount - i - 1] < dThreshold)
                        {
                            bFindRight = TRUE;
                        }
                        lRightSide = nCount - i - 1;
                    }
                }
            }

            void GetChartRect(
                const unsigned char *pBmpBuffer,
                int nWidth,
                int nHeight,
                const COLOR_CHART_PARAM& param,
                RECT &rcChart)
            {
                ROW_NUM = param.nRowNum;
                COL_NUM = param.nColNum;

                //------------------------------------------------------------------------------
                // 从整个Bmp中，取出关心的部分
                int nROIWidth = param.rcChartROI.Width();
                int nROIHeight = param.rcChartROI.Height();
                int nChartROIBufferSize = nROIWidth * nROIHeight;
                unsigned char *pChartROIBmpBuffer = new unsigned char[nChartROIBufferSize * 3];
                CImageProc::GetInstance().GetBMPBlockBuffer(
                    pBmpBuffer, pChartROIBmpBuffer, nWidth, nHeight,
                    param.rcChartROI.left, param.rcChartROI.top, nROIWidth, nROIHeight);

                //------------------------------------------------------------------------------
                // 将关心部分，转成Y值
                unsigned char *pChartROIYBuffer = new unsigned char[nChartROIBufferSize];
                CImageProc::GetInstance().Cal_RGBtoYBuffer(pChartROIBmpBuffer, nROIWidth, nROIHeight, pChartROIYBuffer);
                CImageProc::GetInstance().ThreasholdYBuffer(pChartROIYBuffer, nROIWidth, nROIHeight, param.nYThreshold);
                RELEASE_ARRAY(pChartROIBmpBuffer);

                //------------------------------------------------------------------------------
                // 左右边界
                long lLeft = 0;
                long lRight = 0;
                int *pSumHori = new int[nROIWidth];
                double *pAvgHori = new double[nROIWidth];
                HorizontalHistogram(pChartROIYBuffer, nROIWidth, nROIHeight, pSumHori, pAvgHori);
                GetSeriesSide(pAvgHori, nROIWidth, 180, lLeft, lRight);
                RELEASE_ARRAY(pAvgHori);
                RELEASE_ARRAY(pSumHori);

                //------------------------------------------------------------------------------
                // 上下边界
                long lTop = 0;
                long lBottom = 0;
                int *pSumVertial = new int[nROIHeight];
                double *pAvgVertial = new double[nROIHeight];
                VerticalHistogram(pChartROIYBuffer, nROIWidth, nROIHeight, pSumVertial, pAvgVertial);
                GetSeriesSide(pAvgVertial, nROIHeight, 180, lTop, lBottom);
                RELEASE_ARRAY(pAvgVertial);
                RELEASE_ARRAY(pSumVertial);
                RELEASE_ARRAY(pChartROIYBuffer);

                //------------------------------------------------------------------------------
                // 取得Chart的位置
                rcChart.left = param.rcChartROI.left + lLeft + param.nChartMargin;
                rcChart.right = param.rcChartROI.left + lRight - param.nChartMargin;
                rcChart.top = param.rcChartROI.top + lTop + param.nChartMargin;
                rcChart.bottom = param.rcChartROI.top + lBottom - param.nChartMargin;
            }

            void GetColorCenterPoint(
                const RECT *prcChart,
                int nRow,
                int nCol,
                POINT *pColorPoint)
            {
                double dHalfBlockX = abs(prcChart->right - prcChart->left) / (COL_NUM * 2.0);
                double dHalfBlockY = abs(prcChart->bottom - prcChart->top) / (ROW_NUM * 2.0);

                pColorPoint->x = DOUBLE2LONG(prcChart->left + dHalfBlockX * (nCol * 2 + 1));
                pColorPoint->y = DOUBLE2LONG(prcChart->top + dHalfBlockY * (nRow * 2 + 1));
            }
        }
    }
}
