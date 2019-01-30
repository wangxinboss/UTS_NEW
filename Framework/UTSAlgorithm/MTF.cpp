#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::MTF;

namespace UTS
{
    namespace Algorithm
    {
        namespace MTF
        {
            namespace MtfCalc
            {
                void GetBlockWhiteYBlackY(
                    __in  const unsigned char *pYBuffer,
                    __in  int nWidth,
                    __in  int nHeight,
                    __in  const RECT &rcBlock,
                    __out double &dWhiteY,
                    __out double &dBlackY)
                {
                    assert(nullptr != pYBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);

                    dWhiteY = dBlackY = 0.0;

                    double dAvgY = 0.0;
                    GetBlockAvgY(pYBuffer, nWidth, nHeight, rcBlock, dAvgY);

                    int nBlockWidth = rcBlock.right - rcBlock.left;
                    int nBlockHeight = rcBlock.bottom - rcBlock.top;
                    int nPixelCount = nBlockWidth * nBlockHeight;
                    unsigned char *pBlockYBuffer = new unsigned char[nPixelCount];
                    Image::CImageProc::GetInstance().GetYBlockBuffer(
                        pYBuffer, pBlockYBuffer, nWidth, nHeight,
                        rcBlock.left, rcBlock.top, nBlockWidth, nBlockHeight);

                    int nWhiteCount = 0;
                    int nBlackCount = 0;
                    for (int i = 0; i < nPixelCount; i++)
                    {
                        double dY = pBlockYBuffer[i];
                        if (dY >= dAvgY)
                        {
                            dWhiteY += dY;
                            nWhiteCount++;
                        }
                        else
                        {
                            dBlackY += dY;
                            nBlackCount++;
                        }
                    }
                    dWhiteY /= nWhiteCount;
                    dBlackY /= nBlackCount;

                    RELEASE_ARRAY(pBlockYBuffer);
                }

                void CaculateYMTFValue(
                    const unsigned char *pBlockYBuffer,
                    int nROIWidth,
                    int nROIHeight,
                    double &dMTF)
                {
                    //-------------------------------------------------------------------------
                    // Param assert
                    assert(nullptr != pBlockYBuffer);
                    assert(nROIWidth > 0);
                    assert(nROIHeight > 0);

                    double dYavg = 0.0;
                    for (int i = 0; i < nROIWidth * nROIHeight; i++)
                    {
                        dYavg += pBlockYBuffer[i];
                    }
                    dYavg /= (nROIWidth * nROIHeight);

                    int nMinCount = 0;
                    int nMaxCount = 0;
                    double dYmax = 0.0;
                    double dYmin = 0.0;
                    for (int y = 0; y < nROIHeight; y++)
                    {
                        for (int x = 0; x < nROIWidth; x++)
                        {
                            double dY = pBlockYBuffer[y * nROIWidth + x];
                            if (dY >= dYavg)
                            {
                                dYmax += dY;
                                nMaxCount++;
                            }
                            else
                            {
                                dYmin += dY;
                                nMinCount++;
                            }
                        }
                    }
                    dYmax /= nMaxCount;
                    dYmin /= nMinCount;

                    dMTF = ((dYmax - dYmin) / (dYmax + dYmin)) * 100.0;
                }

                void GetYBlockMTF(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    const RECT &rcROI,
                    double &dMTF)
                {
                    //-------------------------------------------------------------------------
                    // Param assert
                    assert(nullptr != pYBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);

                    int nROIWidth = rcROI.right - rcROI.left;
                    int nROIHeight = rcROI.bottom - rcROI.top;
                    unsigned char *pBlockBuffer = new unsigned char[nROIWidth * nROIHeight];
                    CImageProc::GetInstance().GetFlipedYBlockBuffer(
                        pYBuffer,
                        pBlockBuffer,
                        nWidth,
                        nHeight,
                        rcROI.left,
                        rcROI.top,
                        nROIWidth,
                        nROIHeight);

                    CaculateYMTFValue(
                        pBlockBuffer,
                        nROIWidth,
                        nROIHeight,
                        dMTF);

                    RELEASE_ARRAY(pBlockBuffer);
                }
            }

            namespace MtfStd
            {
                void GetAllMTF_Y(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    const MTF_PARAM &param,
                    MTF_RESULT &result)
                {
                    //-------------------------------------------------------------------------
                    // Param assert
                    assert(nullptr != pYBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);
                    assert(0 < param.vecField.size() && param.vecField.size() <= MAX_EDGE_FIELD_COUNT);
                    for (size_t i = 0; i < param.vecField.size(); i++)
                    {
                        assert(0.0 <= param.vecField[i] && param.vecField[i] <= 1.0);
                    }
                    
                    double dHalfWidth = nWidth / 2.0;
                    double dHalfHeight = nHeight / 2.0;
                    // Center
                    result.ptArrROICenter[MtfFieldLoc_CT].x = DOUBLE2LONG(dHalfWidth);
                    result.ptArrROICenter[MtfFieldLoc_CT].y = DOUBLE2LONG(dHalfHeight);
                    for (size_t i = 0; i < param.vecField.size(); i++)
                    {
                        // LU
                        result.ptArrROICenter[MtfFieldLoc1_LU + i * 4].x = DOUBLE2LONG(dHalfWidth - param.vecField[i] * dHalfWidth);
                        result.ptArrROICenter[MtfFieldLoc1_LU + i * 4].y = DOUBLE2LONG(dHalfHeight - param.vecField[i] * dHalfHeight);
                        // RU
                        result.ptArrROICenter[MtfFieldLoc1_RU + i * 4].x = DOUBLE2LONG(dHalfWidth + param.vecField[i] * dHalfWidth);
                        result.ptArrROICenter[MtfFieldLoc1_RU + i * 4].y = DOUBLE2LONG(dHalfHeight - param.vecField[i] * dHalfHeight);
                        // LD
                        result.ptArrROICenter[MtfFieldLoc1_LD + i * 4].x = DOUBLE2LONG(dHalfWidth - param.vecField[i] * dHalfWidth);
                        result.ptArrROICenter[MtfFieldLoc1_LD + i * 4].y = DOUBLE2LONG(dHalfHeight + param.vecField[i] * dHalfHeight);
                        // RD
                        result.ptArrROICenter[MtfFieldLoc1_RD + i * 4].x = DOUBLE2LONG(dHalfWidth + param.vecField[i] * dHalfWidth);
                        result.ptArrROICenter[MtfFieldLoc1_RD + i * 4].y = DOUBLE2LONG(dHalfHeight + param.vecField[i] * dHalfHeight);
                    }

                    for (int i = 0; i < MtfFieldLoc_SIZES; i++)
                    {
                        if (i > (int)(param.vecField.size() * 4))
                        {
                            break;
                        }
                        Math::GetRectByCenter(result.ptArrROICenter[i], param.sizeMtfROI, result.rcArrROI[i]);
                        MtfCalc::GetYBlockMTF(pYBuffer, nWidth, nHeight, result.rcArrROI[i], result.dArrMTF[i]);
                    }

					for (int i = 0; i < (int)(param.vecField.size()); i++)
					{
						result.dArrMTFDelta[i] = max(max(result.dArrMTF[MtfFieldLoc1_LU + i * 4],result.dArrMTF[MtfFieldLoc1_RU + i * 4]),
							                         max(result.dArrMTF[MtfFieldLoc1_LD + i * 4],result.dArrMTF[MtfFieldLoc1_RD + i * 4]))
										       - min(min(result.dArrMTF[MtfFieldLoc1_LU + i * 4],result.dArrMTF[MtfFieldLoc1_RU + i * 4]),
												     min(result.dArrMTF[MtfFieldLoc1_LD + i * 4],result.dArrMTF[MtfFieldLoc1_RD + i * 4]));

					}


                }

                void GetAllMTF(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const MTF_PARAM &param,
                    MTF_RESULT &result)
                {
                    //-------------------------------------------------------------------------
                    // Param assert
                    assert(nullptr != pBmpBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);
                    assert(0 < param.vecField.size() && param.vecField.size() <= MAX_EDGE_FIELD_COUNT);
                    for (size_t i = 0; i < param.vecField.size(); i++)
                    {
                        assert(0.0 <= param.vecField[i] && param.vecField[i] <= 1.0);
                    }

                    unsigned char *pYBuffer = new unsigned char[nWidth * nHeight];
                    CImageProc::GetInstance().Cal_RGBtoYBuffer(pBmpBuffer, nWidth, nHeight, pYBuffer);
                    GetAllMTF_Y(pYBuffer, nWidth, nHeight, param, result);
                    RELEASE_ARRAY(pYBuffer);
                }
            }   // namespace MtfStd
        }   // namespace Mtf
    }   // namespace Algorithm
}   // namespace UTS
