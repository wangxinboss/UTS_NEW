#include "StdAfx.h"
#include "Algorithm.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace RectangleChart
        {
            void GetRectChartPoints(
                const unsigned char *pBmpBuffer,
                int nWidth,
                int nHeight,
                const RECT_CHART_PARAM &param,
                RECT_CHART_RESULT &result)
            {
                // Corner_LU
                result.rcROI[Corner_LU].left = 0;
                result.rcROI[Corner_LU].right = param.sizeROI.cx;
                result.rcROI[Corner_LU].top = 0;
                result.rcROI[Corner_LU].bottom = param.sizeROI.cy;

                // Corner_LD
                result.rcROI[Corner_LD].left = 0;
                result.rcROI[Corner_LD].right = param.sizeROI.cx;
                result.rcROI[Corner_LD].top = nHeight - param.sizeROI.cy;
                result.rcROI[Corner_LD].bottom = nHeight;

                // Corner_RU
                result.rcROI[Corner_RU].left = nWidth - param.sizeROI.cx;
                result.rcROI[Corner_RU].right = nWidth;
                result.rcROI[Corner_RU].top = 0;
                result.rcROI[Corner_RU].bottom = param.sizeROI.cy;

                // Corner_RD
                result.rcROI[Corner_RD].left = nWidth - param.sizeROI.cx;
                result.rcROI[Corner_RD].right = nWidth;
                result.rcROI[Corner_RD].top = nHeight - param.sizeROI.cy;
                result.rcROI[Corner_RD].bottom = nHeight;

                // Top
                result.rcROI[4].left = (nWidth - param.sizeROI.cx) / 2;
                result.rcROI[4].right = result.rcROI[4].left + param.sizeROI.cx;
                result.rcROI[4].top = 0;
                result.rcROI[4].bottom = param.sizeROI.cy;

                // Bottom
                result.rcROI[5].left = (nWidth - param.sizeROI.cx) / 2;
                result.rcROI[5].right = result.rcROI[5].left + param.sizeROI.cx;
                result.rcROI[5].top = nHeight - param.sizeROI.cy;
                result.rcROI[5].bottom = nHeight;

                for (int i = 0; i < Corner_SIZES + 2; i++)
                {
                    CrossHair::CCrossHair crossHair;
                    float crossvalue;
                    POINT patternPos = crossHair.Cross_LinePosition(
                        pBmpBuffer,
                        nWidth,
                        nHeight,
                        result.rcROI[i].left,
                        result.rcROI[i].top,
                        result.rcROI[i].right - result.rcROI[i].left,
                        result.rcROI[i].bottom - result.rcROI[i].top,
                        &crossvalue);  
                    if (patternPos.x == -1 || patternPos.y == -1)
                    {
                        result.ptRectChartPoint[i].x = (result.rcROI[i].right + result.rcROI[i].left) / 2;
                        result.ptRectChartPoint[i].y = (result.rcROI[i].bottom + result.rcROI[i].top) / 2;
                    }
                    else
                    {
                        result.ptRectChartPoint[i].x = patternPos.x;
                        result.ptRectChartPoint[i].y = patternPos.y;
                    }
                }
            }
        }
    }
}
