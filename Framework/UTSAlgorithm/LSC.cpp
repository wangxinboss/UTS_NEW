#include "StdAfx.h"
#include "Algorithm.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace LSC
        {
            namespace QualComm
            {
                double GetOneChannelROIAvgY(
                    const unsigned char* pRawBuffer,
                    int nWidth,
                    int nHeight,
                    int nStartX,
                    int nStartY,
                    int nROIWidth,
                    int nROIHeight)
                {
                    double dAvg = 0.0;
                    for (int j = nStartY; j < nStartY + nROIHeight; j++)
                    {
                        for (int i = nStartX; i < nStartX + nROIWidth; i++)
                        {
                            dAvg += pRawBuffer[j * nWidth + i];
                        }
                    }
                    dAvg /= (nROIWidth * nROIHeight);
                    return dAvg;
                }

                BOOL LSC_Cali(
                    const unsigned char* pRawBuffer,
                    int nWidth,
                    int nHeight,
                    __in const QUALCOMM_LSC_PARAM &param,
                    __out QUALCOMM_LSC_RESULT &result)
                {
                    int nRet = TRUE;
                    // param check
                    if (nullptr == pRawBuffer)
                    {
                        return FALSE;
                    }
                    if (nWidth <= 0 || nHeight <= 0)
                    {
                        return FALSE;
                    }
                    if ((param.nCenterBlockWidth % 2 != 0)
                        || (param.nCenterBlockHeight % 2 != 0)
                        || (param.nCenterBlockCountX % 2 == 0)
                        || (param.nCenterBlockCountY % 2 == 0))
                    {
                        return FALSE;
                    }
                    if ((param.nCenterBlockCountX * param.nCenterBlockWidth >= nWidth)
                        || (param.nCenterBlockCountY * param.nCenterBlockHeight >= nHeight))
                    {
                        return FALSE;
                    }
                    if ((param.nCenterBlockCountX + 2) * (param.nCenterBlockCountY + 2) > MAX_BLOCK_COUNT)
                    {
                        return FALSE;
                    }
                    if (param.nBayerMode < 1 || param.nBayerMode > 4)
                    {
                        return FALSE;
                    }

                    // init
                    memset(&result, 0, sizeof(QUALCOMM_LSC_RESULT));
                    int nRawWidth = nWidth / 2;
                    int nRawHeight = nHeight / 2;
                    int nRawBlockWidth = param.nCenterBlockWidth / 2;
                    int nRawBlockHeight = param.nCenterBlockHeight / 2;
                    // 1:BGGR  2:RGGB  3:GBRG  4:GRBG
                    int R, Gr, Gb, B;
                    if (param.nBayerMode == 1)
                    {
                        B = 0; Gb = 1; Gr = 2; R = 3;
                    }
                    else if (param.nBayerMode == 2)
                    {
                        R = 0; Gr = 1; Gb = 2; B = 3;
                    }
                    else if (param.nBayerMode == 3)
                    {
                        Gb = 0; B = 1; R = 2; Gr = 3;
                    }
                    else if (param.nBayerMode == 4)
                    {
                        Gr = 0; R = 1; B = 2; Gb = 3;
                    }
                    unsigned char *pChannelBuffer[4];
                    for (int i = 0; i < 4; i++)
                    {
                        pChannelBuffer[i] = new unsigned char[nRawWidth * nRawHeight];
                        memset(pChannelBuffer[i], 0, nRawWidth * nRawHeight);
                    }

                    //-------------------------------------------------------------------------
                    // split buffer by channel
                    for (int y = 0; y < nRawHeight; y++)
                    {
                        for (int x = 0; x < nRawWidth; x++)
                        {
                            pChannelBuffer[0][y * nRawWidth + x] = pRawBuffer[(2 * y) * nWidth + (2 * x)];
                            pChannelBuffer[1][y * nRawWidth + x] = pRawBuffer[(2 * y) * nWidth + (2 * x + 1)];
                            pChannelBuffer[2][y * nRawWidth + x] = pRawBuffer[(2 * y + 1) * nWidth + (2 * x)];
                            pChannelBuffer[3][y * nRawWidth + x] = pRawBuffer[(2 * y + 1) * nWidth + (2 * x + 1)];
                        }
                    }

                    //-------------------------------------------------------------------------
                    // get block position
                    int nCenterLeft = (nRawWidth - nRawBlockWidth * param.nCenterBlockCountX) / 2;
                    int nCenterRight = nRawWidth - nCenterLeft;
                    int nCenterTop = (nRawHeight - nRawBlockHeight * param.nCenterBlockCountY) / 2;
                    int nCenterBottom = nRawHeight - nCenterTop;

                    // corner
                    // LeftTop
                    int nBlockIndex = 0;
                    result.arrBlockInfo[nBlockIndex].rcPos.left = 0;
                    result.arrBlockInfo[nBlockIndex].rcPos.top = 0;
                    result.arrBlockInfo[nBlockIndex].rcPos.right = nCenterLeft;
                    result.arrBlockInfo[nBlockIndex].rcPos.bottom = nCenterTop;
                    // RightTop
                    nBlockIndex = param.nCenterBlockCountX + 1;
                    result.arrBlockInfo[nBlockIndex].rcPos.left = nCenterRight;
                    result.arrBlockInfo[nBlockIndex].rcPos.top = 0;
                    result.arrBlockInfo[nBlockIndex].rcPos.right = nRawWidth;
                    result.arrBlockInfo[nBlockIndex].rcPos.bottom = nCenterTop;
                    // LeftBottom
                    nBlockIndex = (param.nCenterBlockCountX + 2) * (param.nCenterBlockCountY + 1);
                    result.arrBlockInfo[nBlockIndex].rcPos.left = 0;
                    result.arrBlockInfo[nBlockIndex].rcPos.top = nCenterBottom;
                    result.arrBlockInfo[nBlockIndex].rcPos.right = nCenterLeft;
                    result.arrBlockInfo[nBlockIndex].rcPos.bottom = nRawHeight;
                    // RightBottom
                    nBlockIndex = (param.nCenterBlockCountX + 2) * (param.nCenterBlockCountY + 2) - 1;
                    result.arrBlockInfo[nBlockIndex].rcPos.left = nCenterRight;
                    result.arrBlockInfo[nBlockIndex].rcPos.top = nCenterBottom;
                    result.arrBlockInfo[nBlockIndex].rcPos.right = nRawWidth;
                    result.arrBlockInfo[nBlockIndex].rcPos.bottom = nRawHeight;

                    // edge
                    for (int i = 1; i <= param.nCenterBlockCountX; i++)
                    {
                        // top
                        nBlockIndex = i;
                        result.arrBlockInfo[nBlockIndex].rcPos.left = nCenterLeft + (i - 1) * nRawBlockWidth;
                        result.arrBlockInfo[nBlockIndex].rcPos.right = result.arrBlockInfo[nBlockIndex].rcPos.left + nRawBlockWidth;
                        result.arrBlockInfo[nBlockIndex].rcPos.top = 0;
                        result.arrBlockInfo[nBlockIndex].rcPos.bottom = nCenterTop;
                        // bottom
                        nBlockIndex = (param.nCenterBlockCountX + 2) * (param.nCenterBlockCountY + 1) + i;
                        result.arrBlockInfo[nBlockIndex].rcPos.left = nCenterLeft + (i - 1) * nRawBlockWidth;
                        result.arrBlockInfo[nBlockIndex].rcPos.right = result.arrBlockInfo[nBlockIndex].rcPos.left + nRawBlockWidth;
                        result.arrBlockInfo[nBlockIndex].rcPos.top = nCenterBottom;
                        result.arrBlockInfo[nBlockIndex].rcPos.bottom = nRawHeight;
                    }
                    for (int j = 1; j <= param.nCenterBlockCountY; j++)
                    {
                        // left
                        nBlockIndex = j * (param.nCenterBlockCountX + 2);
                        result.arrBlockInfo[nBlockIndex].rcPos.left = 0;
                        result.arrBlockInfo[nBlockIndex].rcPos.right = nCenterLeft;
                        result.arrBlockInfo[nBlockIndex].rcPos.top = nCenterTop + (j - 1) * nRawBlockHeight;
                        result.arrBlockInfo[nBlockIndex].rcPos.bottom = result.arrBlockInfo[nBlockIndex].rcPos.top + nRawBlockHeight;
                        // right
                        nBlockIndex = j * (param.nCenterBlockCountX + 2) + param.nCenterBlockCountX + 1;
                        result.arrBlockInfo[nBlockIndex].rcPos.left = nCenterRight;
                        result.arrBlockInfo[nBlockIndex].rcPos.right = nRawWidth;
                        result.arrBlockInfo[nBlockIndex].rcPos.top = nCenterTop + (j - 1) * nRawBlockHeight;
                        result.arrBlockInfo[nBlockIndex].rcPos.bottom = result.arrBlockInfo[nBlockIndex].rcPos.top + nRawBlockHeight;
                    }

                    // center
                    for (int j = 1; j <= param.nCenterBlockCountY; j++)
                    {
                        for (int i = 1; i <= param.nCenterBlockCountX; i++)
                        {
                            nBlockIndex = j * (param.nCenterBlockCountX + 2) + i;
                            result.arrBlockInfo[nBlockIndex].rcPos.left = nCenterLeft + (i - 1) * nRawBlockWidth;
                            result.arrBlockInfo[nBlockIndex].rcPos.right = result.arrBlockInfo[nBlockIndex].rcPos.left + nRawBlockWidth;
                            result.arrBlockInfo[nBlockIndex].rcPos.top = nCenterTop + (j - 1) * nRawBlockHeight;
                            result.arrBlockInfo[nBlockIndex].rcPos.bottom = result.arrBlockInfo[nBlockIndex].rcPos.top + nRawBlockHeight;
                        }
                    }

                    for (int i = 0; i < (param.nCenterBlockCountX + 2) * (param.nCenterBlockCountY + 2); i++)
                    {
                        int nStartX = result.arrBlockInfo[i].rcPos.left;
                        int nStartY = result.arrBlockInfo[i].rcPos.top;
                        int nROIWidth = result.arrBlockInfo[i].rcPos.right - result.arrBlockInfo[i].rcPos.left;
                        int nROIHeight = result.arrBlockInfo[i].rcPos.bottom - result.arrBlockInfo[i].rcPos.top;
                        // Step. 1: measure R,Gr,Gb,B of Sq 1 ~ 221
                        result.arrBlockInfo[i].dRavg = GetOneChannelROIAvgY(pChannelBuffer[R], nRawWidth, nRawHeight, nStartX, nStartY, nROIWidth, nROIHeight);
                        result.arrBlockInfo[i].dGravg = GetOneChannelROIAvgY(pChannelBuffer[Gr], nRawWidth, nRawHeight, nStartX, nStartY, nROIWidth, nROIHeight);
                        result.arrBlockInfo[i].dGbavg = GetOneChannelROIAvgY(pChannelBuffer[Gb], nRawWidth, nRawHeight, nStartX, nStartY, nROIWidth, nROIHeight);
                        result.arrBlockInfo[i].dBavg = GetOneChannelROIAvgY(pChannelBuffer[B], nRawWidth, nRawHeight, nStartX, nStartY, nROIWidth, nROIHeight);

                        // Step. 2: R avg = Red average ¨C Black_level
                        result.arrBlockInfo[i].dRavg -= param.nBlackLevel;
                        result.arrBlockInfo[i].dGravg -= param.nBlackLevel;
                        result.arrBlockInfo[i].dGbavg -= param.nBlackLevel;
                        result.arrBlockInfo[i].dBavg -= param.nBlackLevel;
                    }

                    //-------------------------------------------------------------------------
                    // release
                    for (int i = 0; i < 4; i++)
                    {
                        RELEASE_ARRAY(pChannelBuffer[i]);
                    }
                    return nRet;
                }
            }   // namespace QualComm
        }   // namespace LSC
    }   // namespace Algorithm
}   // namespace UTS
