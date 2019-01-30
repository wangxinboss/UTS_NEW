#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::Math;
using namespace UTS::Algorithm::CrossHair;
using namespace UTS::Algorithm::SFR::SfrCalc;

namespace UTS
{
    namespace Algorithm
    {
        namespace SFR
        {
            namespace SfrPlus
            {
                /*
                Set param for calc SFR
                */
                void CSfrPlus::SetParam(const SFRPLUS_PARAM_STATIC &staticParam)
                {
                    memcpy(&m_staticParam, &staticParam, sizeof(SFRPLUS_PARAM_STATIC));
                    for (int i = 0; i < staticParam.nSfrCount; i++)
                    {
                        m_dSinValue[i] = sin(Angle2Radian(staticParam.dROIAngle[i]));
                        m_dCosValue[i] = cos(Angle2Radian(staticParam.dROIAngle[i]));
                        if (0 <= staticParam.dROIAngle[i] && staticParam.dROIAngle[i] < 90)
                        {
                            // Block中心在1象限
                            m_nType[i] = 1;
                        }
                        else if (90 <= staticParam.dROIAngle[i] && staticParam.dROIAngle[i] < 180)
                        {
                            // Block中心在2象限
                            m_nType[i] = 2;
                        }
                        else if (180 <= staticParam.dROIAngle[i] && staticParam.dROIAngle[i] < 270)
                        {
                            // Block中心在3象限
                            m_nType[i] = 3;
                        }
                        else
                        {
                            // Block中心在4象限
                            m_nType[i] = 4;
                        }
                    }
                }

                /*
                Get the whole bmp image SFRs
                */
                void CSfrPlus::CalculateSFR(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const SFRPLUS_PARAM_DYNAMIC &dynamicParam,
                    SFRPLUS_RESULT &result)
                {
                    //------------------------------------------------------------------------------
                    // 取得十字交点所在视场与画面中心的距离
                    double dRightCrossHairWidth = Algorithm::Math::GetDistance2D(
                        (double)(dynamicParam.ptChartCenter.x),
                        (double)(dynamicParam.ptChartCenter.y),
                        (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Right].x),
                        (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Right].y));
                    double dTopCrossHairWidth = Algorithm::Math::GetDistance2D(
                        (double)(dynamicParam.ptChartCenter.x),
                        (double)(dynamicParam.ptChartCenter.y),
                        (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Top].x),
                        (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Top].y));
                    double dLeftCrossHairWidth = Algorithm::Math::GetDistance2D(
                        (double)(dynamicParam.ptChartCenter.x),
                        (double)(dynamicParam.ptChartCenter.y),
                        (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Left].x),
                        (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Left].y));
                    double dBottomCrossHairWidth = Algorithm::Math::GetDistance2D(
                        (double)(dynamicParam.ptChartCenter.x),
                        (double)(dynamicParam.ptChartCenter.y),
                        (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Bottom].x),
                        (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Bottom].y));
                    double dRUDistanceCrossHair = sqrt(dRightCrossHairWidth * dRightCrossHairWidth + dTopCrossHairWidth * dTopCrossHairWidth);
                    double dLUDistanceCrossHair = sqrt(dLeftCrossHairWidth * dLeftCrossHairWidth + dTopCrossHairWidth * dTopCrossHairWidth);
                    double dLDDistanceCrossHair = sqrt(dLeftCrossHairWidth * dLeftCrossHairWidth + dBottomCrossHairWidth * dBottomCrossHairWidth);
                    double dRDDistanceCrossHair = sqrt(dRightCrossHairWidth * dRightCrossHairWidth + dBottomCrossHairWidth * dBottomCrossHairWidth);

                    //double dHalfCrossHairWidth = Algorithm::Math::GetDistance2D(
                    //    (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Left].x),
                    //    (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Left].y),
                    //    (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Right].x),
                    //    (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Right].y)) / 2.0;
                    //double dHalfCrossHairHeight = Algorithm::Math::GetDistance2D(
                    //    (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Top].x),
                    //    (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Top].y),
                    //    (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Bottom].x),
                    //    (double)(dynamicParam.ptCrossHairCenter[Cross_Hair_Bottom].y)) / 2.0;
                    //double dDistanceCrossHair = sqrt(dHalfCrossHairWidth * dHalfCrossHairWidth + dHalfCrossHairHeight * dHalfCrossHairHeight);

                    //------------------------------------------------------------------------------
                    // 计算每个Block的SFR
                    for (int i = 0; i < m_staticParam.nSfrCount; i++)
                    {
                        double dDistanceBlock = 0.0;
                        double dRatioX = 0.0;
                        double dRatioY = 0.0;
                        long distanceX = 0;
                        long distanceY = 0;
                        POINT ptFirst = {0};
                        POINT ptSecond = {0};
                        if (m_nType[i] == 1)
                        {
                            dDistanceBlock = dRUDistanceCrossHair * m_staticParam.dROIField[i] / m_staticParam.dCrossHairField;
                            dRatioX = +(dDistanceBlock * m_dCosValue[i] / dRightCrossHairWidth);
                            dRatioY = +(dDistanceBlock * m_dSinValue[i] / dTopCrossHairWidth);
                            distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Top].x);
                            distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Top].y);
                            ptFirst.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioY);
                            ptFirst.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioY);

                            distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Right].x);
                            distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Right].y);
                            ptSecond.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioX);
                            ptSecond.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioX);
                        }
                        else if (m_nType[i] == 2)
                        {
                            dDistanceBlock = dLUDistanceCrossHair * m_staticParam.dROIField[i] / m_staticParam.dCrossHairField;
                            dRatioX = -(dDistanceBlock * m_dCosValue[i] / dLeftCrossHairWidth);
                            dRatioY = +(dDistanceBlock * m_dSinValue[i] / dTopCrossHairWidth);
                            distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Left].x);
                            distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Left].y);
                            ptFirst.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioX);
                            ptFirst.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioX);

                            distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Top].x);
                            distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Top].y);
                            ptSecond.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioY);
                            ptSecond.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioY);
                        }
                        else if (m_nType[i] == 3)
                        {
                            dDistanceBlock = dLDDistanceCrossHair * m_staticParam.dROIField[i] / m_staticParam.dCrossHairField;
                            dRatioX = -(dDistanceBlock * m_dCosValue[i] / dLeftCrossHairWidth);
                            dRatioY = -(dDistanceBlock * m_dSinValue[i] / dBottomCrossHairWidth);
                            distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Bottom].x);
                            distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Bottom].y);
                            ptFirst.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioY);
                            ptFirst.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioY);

                            distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Left].x);
                            distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Left].y);
                            ptSecond.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioX);
                            ptSecond.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioX);
                        }
                        else
                        {
                            dDistanceBlock = dRDDistanceCrossHair * m_staticParam.dROIField[i] / m_staticParam.dCrossHairField;
                            dRatioX = +(dDistanceBlock * m_dCosValue[i] / dRightCrossHairWidth);
                            dRatioY = -(dDistanceBlock * m_dSinValue[i] / dBottomCrossHairWidth);
                            distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Right].x);
                            distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Right].y);
                            ptFirst.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioX);
                            ptFirst.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioX);

                            distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Bottom].x);
                            distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Bottom].y);
                            ptSecond.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioY);
                            ptSecond.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioY);
                        }

                        //// 计算Block中心距画面中心的距离
                        //double dDistanceBlock = dDistanceCrossHair * m_staticParam.dROIField[i] / m_staticParam.dCrossHairField;
                        //// 计算Block的横向缩放比率
                        //double dRatioX = -(dDistanceBlock * m_dCosValue[i] / dHalfCrossHairWidth);
                        //// 计算Block的纵向缩放比率
                        //double dRatioY = dDistanceBlock * m_dSinValue[i] / dHalfCrossHairHeight;

                        //if (m_nType[i] == 13)   // Block中心在1、3象限，顺序传入"纵"、"心"、"横"，求第四点
                        //{
                        //    distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Top].x);
                        //    distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Top].y);
                        //    ptFirst.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioY);
                        //    ptFirst.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioY);

                        //    distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Left].x);
                        //    distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Left].y);
                        //    ptSecond.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioX);
                        //    ptSecond.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioX);
                        //}
                        //else    // Block中心在2、4象限，顺序传入"横"、"心"、"纵"，求第四点
                        //{
                        //    distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Left].x);
                        //    distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Left].y);
                        //    ptFirst.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioX);
                        //    ptFirst.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioX);

                        //    distanceX = (dynamicParam.ptChartCenter.x - dynamicParam.ptCrossHairCenter[Cross_Hair_Top].x);
                        //    distanceY = (dynamicParam.ptChartCenter.y - dynamicParam.ptCrossHairCenter[Cross_Hair_Top].y);
                        //    ptSecond.x = DOUBLE2LONG(dynamicParam.ptChartCenter.x - distanceX * dRatioY);
                        //    ptSecond.y = DOUBLE2LONG(dynamicParam.ptChartCenter.y - distanceY * dRatioY);
                        //}
                        GetLastPointOfRectangle(ptFirst, dynamicParam.ptChartCenter, ptSecond, result.roi.ptCenter[i]);
                        GetRectByCenter(result.roi.ptCenter[i], m_staticParam.SFR_ROI, result.roi.rcRect[i]);
                        if (m_staticParam.dSFRNum >= 0)
                        {
                            GetBlockSFRWithFrq(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcRect[i],
                                m_staticParam.dFrqNum[SFR_Frq_Hi],
                                m_staticParam.dFrqNum[SFR_Frq_Lo],
                                m_staticParam.dSFRNum,
                                result.dSFR[i][SFR_Frq_Hi],
                                result.dSFR[i][SFR_Frq_Lo],
                                result.dFrq[i]);
                            result.dNq[i] = 2 * result.dFrq[i];
                        }
                        else
                        {
                            GetBlockSFR(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcRect[i],
                                m_staticParam.dFrqNum[SFR_Frq_Hi],
                                m_staticParam.dFrqNum[SFR_Frq_Lo],
                                result.dSFR[i][SFR_Frq_Hi],
                                result.dSFR[i][SFR_Frq_Lo]);
                        }
                    }   // for sfr count
                }   // CalculateSFR
            }   // namespace ChartSFR
        }   //namespace ChartAnalyse
    }   // namespace Algorithm
}   // namespace UTS

