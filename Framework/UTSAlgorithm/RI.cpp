#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::Image;

namespace UTS
{
    namespace Algorithm
    {
        namespace RI
        {
            namespace RI_OpticalCenter
            {
                void RI_Y(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result)
                {
                    //------------------------------------------------------------------------------
                    // 初始化结果
                    for (int i = 0; i < Corner_SIZES; i++)
                    {
                        result.dRICorner[i] = 0.0;
                    }
                    result.dRI = 0.0;
                    result.dRIDelta = 0.0;
                    result.ptCenterXY.x = 0;
                    result.ptCenterXY.y = 0;

                    //?算RI
                    //	找出最亮區域 - 發現最亮區域使用Low pass Filter
                    double *_MeanBuffer = new double[nWidth * nHeight];
                    memset(_MeanBuffer, 0, sizeof(double) * nWidth * nHeight);

                    if ((nROISizeX % 2) == 0)
                    {
                        MeanFilter_CV(pYBuffer, nWidth, nHeight, nROISizeX + 1, nROISizeY + 1, _MeanBuffer);
                    }
                    else
                    {
                        MeanFilter_CV(pYBuffer, nWidth, nHeight, nROISizeX, nROISizeY, _MeanBuffer);
                    }

                    double CenterROIY = 0;
                    for (int y = 0; y < nHeight; y++)
                    {
                        for (int x = 0; x < nWidth; x++)
                        {					
                            if (CenterROIY < _MeanBuffer[y * nWidth + x])
                            {
                                CenterROIY = _MeanBuffer[y * nWidth + x];
                                result.ptCenterXY.x = x;
                                result.ptCenterXY.y = y;
                            }					
                        }
                    }

                    int _Pos_LU = ((nROISizeY / 2) * nWidth) + (nROISizeX / 2);
                    int _Pos_RU = (((nROISizeY / 2) + 1) * nWidth) - ((nROISizeX / 2) + 1);
                    int _Pos_LD = (nHeight - ((nROISizeY / 2) + 1)) * nWidth + (nROISizeX / 2);
                    int _Pos_RD = ((nHeight + 1) - ((nROISizeY / 2) + 1)) * nWidth - ((nROISizeX / 2) + 1);
                    result.dRICorner[Corner_LU] = _MeanBuffer[_Pos_LU] / CenterROIY;
                    result.dRICorner[Corner_RU] = _MeanBuffer[_Pos_RU] / CenterROIY;
                    result.dRICorner[Corner_LD] = _MeanBuffer[_Pos_LD] / CenterROIY;
                    result.dRICorner[Corner_RD] = _MeanBuffer[_Pos_RD] / CenterROIY;

                    double MaxCorner = 0;
                    double MinCorner = 65536;

                    for (int i = 0; i < Corner_SIZES; i++)
                    {
                        if (MaxCorner < result.dRICorner[i])
                        {
                            MaxCorner = result.dRICorner[i];
                        }
                        if (MinCorner > result.dRICorner[i])
                        {
                            MinCorner = result.dRICorner[i];
                        }
                    }
                    result.dRI = MinCorner;
                    result.dRIDelta = (MaxCorner - MinCorner);

                    if (result.dRI < 0)
                    {
                        result.dRI = 0;
                    }
                    if (result.dRIDelta < 0)
                    {
                        result.dRIDelta = 0;
                    }

                    RELEASE_ARRAY(_MeanBuffer);
                }

                /* 测试用，很慢，待优化
                double SumY(
                    unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    int nLTPtX,
                    int nLTPtY,
                    int nROISizeX,
                    int nROISizeY)
                {
                    POINT ptCurrent;
                    double dYSum = 0.0;
                    for (int ry = 0; ry < nROISizeY; ry++)
                    {
                        for (int rx = 0; rx < nROISizeX; rx++)
                        {
                            ptCurrent.x = nLTPtX + rx;
                            ptCurrent.y = nLTPtY + ry;
                            dYSum += pYBuffer[ptCurrent.y * nWidth + ptCurrent.x];
                        }
                    }
                    return dYSum;
                }

                void RI_Y(
                    unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result)
                {
                    //------------------------------------------------------------------------------
                    // 初始化结果
                    for (int i = 0; i < Corner_SIZES; i++)
                    {
                        result.dRICorner[i] = 0.0;
                    }
                    result.dRI = 0.0;
                    result.dRIDelta = 0.0;
                    result.ptCenterXY.x = 0;
                    result.ptCenterXY.y = 0;

                    // 找出最亮区域， 算出亮度和值
                    double dMaxYSum = 0.0;
                    POINT ptMaxYLT; // 最亮区域的左上角点
                    for (int y = 0; y < nHeight - nROISizeY + 1; y++)
                    {
                        for (int x = 0; x < nWidth - nROISizeX + 1; x++)
                        {
                            double dYSum = SumY(pYBuffer, nWidth, nHeight, x, y, nROISizeX, nROISizeY);
                            if (dYSum > dMaxYSum)
                            {
                                dMaxYSum = dYSum;
                                ptMaxYLT.x = x;
                                ptMaxYLT.y = y;
                            }			
                        }
                    }
                    // 防止除零错误
                    if (dMaxYSum < FLOAT_ZERO_THRESHOLD)
                    {
                        dMaxYSum = FLOAT_ZERO_THRESHOLD;
                    }
                    // 算出四个角的亮度和值
                    double dCornerYSum[Corner_SIZES] = {0};
                    dCornerYSum[Corner_LU] = SumY(pYBuffer, nWidth, nHeight, 0, 0, nROISizeX, nROISizeY);
                    dCornerYSum[Corner_RU] = SumY(pYBuffer, nWidth, nHeight, (nWidth - nROISizeX), 0, nROISizeX, nROISizeY);
                    dCornerYSum[Corner_LD] = SumY(pYBuffer, nWidth, nHeight, 0, (nHeight - nROISizeY), nROISizeX, nROISizeY);
                    dCornerYSum[Corner_RD] = SumY(pYBuffer, nWidth, nHeight, (nWidth - nROISizeX), (nHeight - nROISizeY), nROISizeX, nROISizeY);

                    // 设置结果
                    result.ptCenterXY.x = ptMaxYLT.x + nROISizeX / 2;
                    result.ptCenterXY.y = ptMaxYLT.y + nROISizeY / 2;
                    for (int i = 0; i < Corner_SIZES; i++)
                    {
                        result.dRICorner[i] = dCornerYSum[i] / dMaxYSum;
                    }

                    double MaxCorner = 0;
                    double MinCorner = INT_MAX;

                    for (int i = 0; i < Corner_SIZES; i++)
                    {
                        if (MaxCorner < result.dRICorner[i])
                        {
                            MaxCorner = result.dRICorner[i];
                        }
                        if (MinCorner > result.dRICorner[i])
                        {
                            MinCorner = result.dRICorner[i];
                        }
                    }
                    result.dRI = MinCorner;
                    result.dRIDelta = (MaxCorner - MinCorner);
                    if (result.dRI < 0)
                    {
                        result.dRI = 0;
                    }
                    if (result.dRIDelta < 0)
                    {
                        result.dRIDelta = 0;
                    }
                }
                */

                void RI_Raw(
                    unsigned char *pRawBuffer,
                    int nWidth,
                    int nHeight,
                    int nBayerType,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result)
                {
                    unsigned char *pBmpBuffer = new unsigned char[nWidth * nHeight * 3];
                    memset(pBmpBuffer, 0, nWidth * nHeight * 3);
                    CImageProc::GetInstance().RawToBmp(nBayerType, pRawBuffer, pBmpBuffer, nWidth, nHeight);
                    RI_RGB(pBmpBuffer, nWidth, nHeight, nROISizeX, nROISizeY, result);
                    RELEASE_ARRAY(pBmpBuffer);
                }

                void RI_RGB(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result)
                {
                    //-------------------------------------------------------------------------
                    // RGB buffer => Y buffer
                    unsigned char *pYBuffer = new unsigned char[nWidth * nHeight];
                    memset(pYBuffer, 0, nWidth * nHeight);
                    CImageProc::GetInstance().Cal_RGBtoYBuffer(pBmpBuffer, nWidth, nHeight, pYBuffer);

                    //-------------------------------------------------------------------------
                    // Call Y buffer RI Algorithm
                    RI_Y(pYBuffer, nWidth, nHeight, nROISizeX, nROISizeY, result);
                    RELEASE_ARRAY(pYBuffer);

                    //-------------------------------------------------------------------------
                    // 将结果上下翻转
                    SWAP(double, result.dRICorner[Corner_LU], result.dRICorner[Corner_LD]);
                    SWAP(double, result.dRICorner[Corner_RU], result.dRICorner[Corner_RD]);
                    result.ptCenterXY.y = nHeight - result.ptCenterXY.y;
                }
            }

            namespace RI_LogicalCenter
            {
                void RI_Y(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result)
                {
                    //------------------------------------------------------------------------------
                    // 初始化结果
                    for (int i = 0; i < Corner_SIZES; i++)
                    {
                        result.dRICorner[i] = 0.0;
                    }
                    result.dRI = 0.0;
                    result.dRIDelta = 0.0;
                    result.ptCenterXY.x = nWidth / 2;
                    result.ptCenterXY.y = nHeight / 2;

                    //?算RI
                    //	找出最亮區域 - 發現最亮區域使用Low pass Filter
                    double *_MeanBuffer = new double[nWidth * nHeight];
                    memset(_MeanBuffer, 0, sizeof(double) * nWidth * nHeight);

                    if ((nROISizeX % 2) == 0)
                    {
                        MeanFilter_CV(pYBuffer, nWidth, nHeight, nROISizeX + 1, nROISizeY + 1, _MeanBuffer);
                    }
                    else
                    {
                        MeanFilter_CV(pYBuffer, nWidth, nHeight, nROISizeX, nROISizeY, _MeanBuffer);
                    }

                    int _Pos_CT = ((nHeight / 2) * nWidth) + (nWidth / 2); //Aiber V1.T.23
                    int _Pos_LU = ((nROISizeY / 2) * nWidth) + (nROISizeX / 2);
                    int _Pos_RU = (((nROISizeY / 2) + 1) * nWidth) - ((nROISizeX / 2) + 1);
                    int _Pos_LD = (nHeight - ((nROISizeY / 2) + 1)) * nWidth + (nROISizeX / 2);
                    int _Pos_RD = ((nHeight + 1) - ((nROISizeY / 2) + 1)) * nWidth - ((nROISizeX / 2) + 1);

                    double CenterROIY = _MeanBuffer[_Pos_CT];   //Aiber V1.T.23
                    result.dRICorner[Corner_LU] = _MeanBuffer[_Pos_LU] / CenterROIY;
                    result.dRICorner[Corner_RU] = _MeanBuffer[_Pos_RU] / CenterROIY;
                    result.dRICorner[Corner_LD] = _MeanBuffer[_Pos_LD] / CenterROIY;
                    result.dRICorner[Corner_RD] = _MeanBuffer[_Pos_RD] / CenterROIY;

                    double MaxCorner = 0;
                    double MinCorner = 65536;

                    for (int i = 0; i < Corner_SIZES; i++)
                    {
                        if (MaxCorner < result.dRICorner[i])
                        {
                            MaxCorner = result.dRICorner[i];
                        }
                        if (MinCorner > result.dRICorner[i])
                        {
                            MinCorner = result.dRICorner[i];
                        }
                    }
                    result.dRI = MinCorner;
                    result.dRIDelta = (MaxCorner - MinCorner);

                    if (result.dRI < 0)
                    {
                        result.dRI = 0;
                    }
                    if (result.dRIDelta < 0)
                    {
                        result.dRIDelta = 0;
                    }

                    RELEASE_ARRAY(_MeanBuffer);
                }

                void RI_Raw(
                    unsigned char *pRawBuffer,
                    int nWidth,
                    int nHeight,
                    int nBayerType,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result)
                {
                    unsigned char *pBmpBuffer = new unsigned char[nWidth * nHeight * 3];
                    memset(pBmpBuffer, 0, nWidth * nHeight * 3);
                    CImageProc::GetInstance().RawToBmp(nBayerType, pRawBuffer, pBmpBuffer, nWidth, nHeight);
                    RI_RGB(pBmpBuffer, nWidth, nHeight, nROISizeX, nROISizeY, result);
                    RELEASE_ARRAY(pBmpBuffer);
                }

                void RI_RGB(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result)
                {
                    //-------------------------------------------------------------------------
                    // RGB buffer => Y buffer
                    unsigned char *pYBuffer = new unsigned char[nWidth * nHeight];
                    memset(pYBuffer, 0, nWidth * nHeight);
                    CImageProc::GetInstance().Cal_RGBtoYBuffer(pBmpBuffer, nWidth, nHeight, pYBuffer);

                    //-------------------------------------------------------------------------
                    // Call Y buffer RI Algorithm
                    RI_Y(pYBuffer, nWidth, nHeight, nROISizeX, nROISizeY, result);
                    RELEASE_ARRAY(pYBuffer);

                    //-------------------------------------------------------------------------
                    // 将结果上下翻转
                    SWAP(double, result.dRICorner[Corner_LU], result.dRICorner[Corner_LD]);
                    SWAP(double, result.dRICorner[Corner_RU], result.dRICorner[Corner_RD]);
                    result.ptCenterXY.y = nHeight - result.ptCenterXY.y;
                }
            }


        }
    }
}
