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
            namespace SfrStd
            {
                /*
                function: Get the whole bmp image SFRs
                */
                void GetAllSFR(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const SFR_PARAM &param,
                    SFR_RESULT &result)
                {
                    //-------------------------------------------------------------------------
                    // SFR
                    //-------------------------------------------------------------------------
                    // 获取SFR抓取点的坐标
                    //-------------------------------------------------------------------------
                    // 中心视场ROI
                    memcpy(&result.roi.ptCenter, &param.ptChartCenter, sizeof(POINT));
                    GetRectByCenter(result.roi.ptCenter, param.SFR_ROI, result.roi.rcCenter);
                    // 计算中心视场的SFR值
                    GetBlockSFR(
                        pBmpBuffer,
                        nWidth,
                        nHeight,
                        result.roi.rcCenter,
                        param.dFrqNum[SFR_Frq_Hi],
                        param.dFrqNum[SFR_Frq_Lo],
                        result.dCenterSFR[SFR_Frq_Hi],
                        result.dCenterSFR[SFR_Frq_Lo]);

                    // 虚拟出每个周边视场的十字的中心坐标
                    POINT ptCrossHairFx[FIELD_SIZES][Cross_Hair_Num] = {0};
                    for (int i = 0; i < Cross_Hair_Num; i++)
                    {
                        long distanceX = result.roi.ptCenter.x - param.ptCrossHairCenter[i].x;
                        long distanceY = result.roi.ptCenter.y - param.ptCrossHairCenter[i].y;
                        for (int j = 0; j < FIELD_SIZES; j++)
                        {
                            double dDistanceOffsetX = distanceX * param.dFieldOffset[j] / param.dCrossHairOffset;
                            double dDistanceOffsetY = distanceY * param.dFieldOffset[j] / param.dCrossHairOffset;
                            ptCrossHairFx[j][i].x = DOUBLE2LONG(result.roi.ptCenter.x - dDistanceOffsetX);
                            ptCrossHairFx[j][i].y = DOUBLE2LONG(result.roi.ptCenter.y - dDistanceOffsetY);
                        }
                    }
                    // 得到每个视场每个ROI和SFR值
                    for (int i = 0; i < FIELD_SIZES; i++)
                    {
                        if ((param.FieldTestEnable[i] & MaskL) != 0)  // Left
                        {
                            memcpy(&result.roi.ptEdge[i][SfrFieldLoc_L], &ptCrossHairFx[i][Cross_Hair_Left], sizeof(POINT));
                            GetRectByCenter(result.roi.ptEdge[i][SfrFieldLoc_L], param.SFR_ROI, result.roi.rcEdge[i][SfrFieldLoc_L]);
                            GetBlockSFR(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcEdge[i][SfrFieldLoc_L],
                                param.dFrqNum[SFR_Frq_Hi],
                                param.dFrqNum[SFR_Frq_Lo],
                                result.dEdgeSFR[i][SfrFieldLoc_L][SFR_Frq_Hi],
                                result.dEdgeSFR[i][SfrFieldLoc_L][SFR_Frq_Lo]);
                        }
                        if ((param.FieldTestEnable[i] & MaskR) != 0)  // Right
                        {
                            memcpy(&result.roi.ptEdge[i][SfrFieldLoc_R], &ptCrossHairFx[i][Cross_Hair_Right], sizeof(POINT));
                            GetRectByCenter(result.roi.ptEdge[i][SfrFieldLoc_R], param.SFR_ROI, result.roi.rcEdge[i][SfrFieldLoc_R]);
                            GetBlockSFR(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcEdge[i][SfrFieldLoc_R],
                                param.dFrqNum[SFR_Frq_Hi],
                                param.dFrqNum[SFR_Frq_Lo],
                                result.dEdgeSFR[i][SfrFieldLoc_R][SFR_Frq_Hi],
                                result.dEdgeSFR[i][SfrFieldLoc_R][SFR_Frq_Lo]);
                        }
                        if ((param.FieldTestEnable[i] & MaskU) != 0)  // UP
                        {
                            memcpy(&result.roi.ptEdge[i][SfrFieldLoc_U], &ptCrossHairFx[i][Cross_Hair_Top], sizeof(POINT));
                            GetRectByCenter(result.roi.ptEdge[i][SfrFieldLoc_U], param.SFR_ROI, result.roi.rcEdge[i][SfrFieldLoc_U]);
                            GetBlockSFR(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcEdge[i][SfrFieldLoc_U],
                                param.dFrqNum[SFR_Frq_Hi],
                                param.dFrqNum[SFR_Frq_Lo],
                                result.dEdgeSFR[i][SfrFieldLoc_U][SFR_Frq_Hi],
                                result.dEdgeSFR[i][SfrFieldLoc_U][SFR_Frq_Lo]);
                        }
                        if ((param.FieldTestEnable[i] & MaskD) != 0)  // Down
                        {
                            memcpy(&result.roi.ptEdge[i][SfrFieldLoc_D], &ptCrossHairFx[i][Cross_Hair_Bottom], sizeof(POINT));
                            GetRectByCenter(result.roi.ptEdge[i][SfrFieldLoc_D], param.SFR_ROI, result.roi.rcEdge[i][SfrFieldLoc_D]);
                            GetBlockSFR(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcEdge[i][SfrFieldLoc_D],
                                param.dFrqNum[SFR_Frq_Hi],
                                param.dFrqNum[SFR_Frq_Lo],
                                result.dEdgeSFR[i][SfrFieldLoc_D][SFR_Frq_Hi],
                                result.dEdgeSFR[i][SfrFieldLoc_D][SFR_Frq_Lo]);
                        }
                        if ((param.FieldTestEnable[i] & MaskLU) != 0)  // LeftUp
                        {
                            Algorithm::Math::GetLastPointOfRectangle(
                                ptCrossHairFx[i][Cross_Hair_Left], result.roi.ptCenter,
                                ptCrossHairFx[i][Cross_Hair_Top], result.roi.ptEdge[i][SfrFieldLoc_LU]);
                            GetRectByCenter(result.roi.ptEdge[i][SfrFieldLoc_LU], param.SFR_ROI, result.roi.rcEdge[i][SfrFieldLoc_LU]);
                            GetBlockSFR(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcEdge[i][SfrFieldLoc_LU],
                                param.dFrqNum[SFR_Frq_Hi],
                                param.dFrqNum[SFR_Frq_Lo],
                                result.dEdgeSFR[i][SfrFieldLoc_LU][SFR_Frq_Hi],
                                result.dEdgeSFR[i][SfrFieldLoc_LU][SFR_Frq_Lo]);
                        }
                        if ((param.FieldTestEnable[i] & MaskRU) != 0)  // RightUp
                        {
                            Algorithm::Math::GetLastPointOfRectangle(
                                ptCrossHairFx[i][Cross_Hair_Top], result.roi.ptCenter,
                                ptCrossHairFx[i][Cross_Hair_Right], result.roi.ptEdge[i][SfrFieldLoc_RU]);
                            GetRectByCenter(result.roi.ptEdge[i][SfrFieldLoc_RU], param.SFR_ROI, result.roi.rcEdge[i][SfrFieldLoc_RU]);
                            GetBlockSFR(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcEdge[i][SfrFieldLoc_RU],
                                param.dFrqNum[SFR_Frq_Hi],
                                param.dFrqNum[SFR_Frq_Lo],
                                result.dEdgeSFR[i][SfrFieldLoc_RU][SFR_Frq_Hi],
                                result.dEdgeSFR[i][SfrFieldLoc_RU][SFR_Frq_Lo]);
                        }
                        if ((param.FieldTestEnable[i] & MaskLD) != 0)  // LeftDown
                        {
                            Algorithm::Math::GetLastPointOfRectangle(
                                ptCrossHairFx[i][Cross_Hair_Bottom], result.roi.ptCenter,
                                ptCrossHairFx[i][Cross_Hair_Left], result.roi.ptEdge[i][SfrFieldLoc_LD]);
                            GetRectByCenter(result.roi.ptEdge[i][SfrFieldLoc_LD], param.SFR_ROI, result.roi.rcEdge[i][SfrFieldLoc_LD]);
                            GetBlockSFR(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcEdge[i][SfrFieldLoc_LD],
                                param.dFrqNum[SFR_Frq_Hi],
                                param.dFrqNum[SFR_Frq_Lo],
                                result.dEdgeSFR[i][SfrFieldLoc_LD][SFR_Frq_Hi],
                                result.dEdgeSFR[i][SfrFieldLoc_LD][SFR_Frq_Lo]);
                        }
                        if ((param.FieldTestEnable[i] & MaskRD) != 0)  // RightDown
                        {
                            Algorithm::Math::GetLastPointOfRectangle(
                                ptCrossHairFx[i][Cross_Hair_Right], result.roi.ptCenter,
                                ptCrossHairFx[i][Cross_Hair_Bottom], result.roi.ptEdge[i][SfrFieldLoc_RD]);
                            GetRectByCenter(result.roi.ptEdge[i][SfrFieldLoc_RD], param.SFR_ROI, result.roi.rcEdge[i][SfrFieldLoc_RD]);
                            GetBlockSFR(
                                pBmpBuffer,
                                nWidth,
                                nHeight,
                                result.roi.rcEdge[i][SfrFieldLoc_RD],
                                param.dFrqNum[SFR_Frq_Hi],
                                param.dFrqNum[SFR_Frq_Lo],
                                result.dEdgeSFR[i][SfrFieldLoc_RD][SFR_Frq_Hi],
                                result.dEdgeSFR[i][SfrFieldLoc_RD][SFR_Frq_Lo]);
                        }
                    }   // for FIELD_SIZES
                }   // GetAllSFR
            }   // namespace ChartSFR
        }   //namespace ChartAnalyse
    }   // namespace Algorithm
}   // namespace UTS

