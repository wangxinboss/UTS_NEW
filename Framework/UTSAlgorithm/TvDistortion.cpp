#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::RectangleChart;

namespace UTS
{
    namespace Algorithm
    {
        namespace TV_Distortion
        {
            namespace TV_Distortion_Rectangle
            {
                void TvDistortion(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const TVD_PARAM &param,
                    TVD_RESULT &result)
                {
                    //-------------------------------------------------------------------------
                    // Param assert
                    assert(nullptr != pBmpBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);

                    RECT_CHART_PARAM rectChartParam = {0};
                    rectChartParam.sizeROI.cx = param.sizeROI.cx;
                    rectChartParam.sizeROI.cy = param.sizeROI.cy;
                    RECT_CHART_RESULT rectChartResult = {0};
                    GetRectChartPoints(pBmpBuffer, nWidth, nHeight, rectChartParam, rectChartResult);

                    for (int i = 0; i < Corner_SIZES; i++)
                    {
                        memcpy(&result.roi.rcCorner[i], &rectChartResult.rcROI[i], sizeof(RECT));
                        memcpy(&result.roi.ptCorner[i], &rectChartResult.ptRectChartPoint[i], sizeof(POINT));
                    }
                    memcpy(&result.roi.rcTop, &rectChartResult.rcROI[4], sizeof(RECT));
                    memcpy(&result.roi.ptTop, &rectChartResult.ptRectChartPoint[4], sizeof(POINT));
                    memcpy(&result.roi.rcBottom, &rectChartResult.rcROI[5], sizeof(RECT));
                    memcpy(&result.roi.ptBottom, &rectChartResult.ptRectChartPoint[5], sizeof(POINT));

                    result.dALeft = Math::GetDistance2D(
                        result.roi.ptCorner[Corner_LU].x,
                        result.roi.ptCorner[Corner_LU].y,
                        result.roi.ptCorner[Corner_LD].x,
                        result.roi.ptCorner[Corner_LD].y);

                    result.dARight = Math::GetDistance2D(
                        result.roi.ptCorner[Corner_RU].x,
                        result.roi.ptCorner[Corner_RU].y,
                        result.roi.ptCorner[Corner_RD].x,
                        result.roi.ptCorner[Corner_RD].y);

                    result.dB = Math::GetDistance2D(
                        result.roi.ptTop.x,
                        result.roi.ptTop.y,
                        result.roi.ptBottom.x,
                        result.roi.ptBottom.y);

                    double dAvgA = (result.dALeft + result.dARight) / 2;
                    result.dTvDistortion = (dAvgA - result.dB) / (2 * result.dB) * 100.0;
                }
            }
        }
    }
}
