#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::ColorChart;

namespace UTS
{
    namespace Algorithm
    {
        namespace CR
        {
            namespace CR_Std
            {
                void ColorReproducibility(
                    unsigned char* pBmpBuffer,
                    int width,
                    int height,
                    const CR_PARAM &param,
                    CR_RESULT &result)
                {
                    //------------------------------------------------------------------------------
                    // get position of color chart
                    GetChartRect(pBmpBuffer, width, height, param.stColorParam, result.roi.rcChartRect);

                    for (int i = 0; i < param.nColorCount; i++)
                    {
                        //------------------------------------------------------------------------------
                        // get position of one color
                        int nRow = param.stArrColorInfo[i].nColorIndex / param.stColorParam.nColNum;
                        int nCol = param.stArrColorInfo[i].nColorIndex % param.stColorParam.nColNum;
                        GetColorCenterPoint(&result.roi.rcChartRect, nRow, nCol, &result.roi.ptColorCenter[i]);
                        Math::GetRectByCenter(result.roi.ptColorCenter[i], param.sizeColorROI, result.roi.rcColorRect[i]);

                        //------------------------------------------------------------------------------
                        // calculate deltaC & deltaE
                        GetROIAvgRGB(pBmpBuffer, width, height, result.roi.rcColorRect[i], result.rgbAvg[i]);
                        ColorSpace::RGBDouble2Lab(
                            result.rgbAvg[i].dR,
                            result.rgbAvg[i].dG,
                            result.rgbAvg[i].dB,
                            result.labVal[i].dL,
                            result.labVal[i].dA,
                            result.labVal[i].dB);
                        result.dDeltaE[i] = Math::GetDistance3D(
                            param.stArrColorInfo[i].stColorLab.dL,
                            param.stArrColorInfo[i].stColorLab.dA,
                            param.stArrColorInfo[i].stColorLab.dB,
                            result.labVal[i].dL,
                            result.labVal[i].dA,
                            result.labVal[i].dB);
                        result.dDeltaC[i] = Math::GetDistance2D(
                            param.stArrColorInfo[i].stColorLab.dA,
                            param.stArrColorInfo[i].stColorLab.dB,
                            result.labVal[i].dA,
                            result.labVal[i].dB);
                    }
                }
            }
        }
    }
}

