#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::Math;

namespace UTS
{
    namespace Algorithm
    {
        namespace DarkNoise
        {
            namespace DarkNoise_WholeImage
            {
                void DarkNoiseY(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    double &dStdDevY)
                {
                    //-------------------------------------------------------------------------
                    // Param assert
                    assert(nullptr != pYBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);

                    dStdDevY = 0.0;

                    double *pdArrY = new double[nWidth * nHeight];
                    for (int j = 0; j < nHeight; j++)
                    {
                        for (int i = 0; i < nWidth; i++)
                        {
                            pdArrY[j * nWidth + i] = (double)(pYBuffer[j * nWidth + i]);
                        }
                    }
                    dStdDevY = Math::StandardDeviation(pdArrY, nWidth * nHeight);
                    RELEASE_ARRAY(pdArrY);
                }

                void DarkNoiseRGB(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    double &dStdDevY)
                {
                    //-------------------------------------------------------------------------
                    // Param assert
                    assert(nullptr != pBmpBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);

                    unsigned char *pYBuffer = new unsigned char[nWidth * nHeight];
                    CImageProc::GetInstance().Cal_RGBtoYBuffer(pBmpBuffer, nWidth, nHeight, pYBuffer);
                    DarkNoiseY(pYBuffer, nWidth, nHeight, dStdDevY);
                    RELEASE_ARRAY(pYBuffer);
                }

				void DarkNoiseAltek(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    double &dAvgR,
					double &dAvgG,
					double &dAvgB)
                {
					assert(nullptr != pBmpBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);

					LPBYTE pDataIndex=pBmpBuffer;
					int LoopLimit = nHeight*nWidth;

					dAvgR=dAvgG=dAvgB=0.0;

					for (int i=0 ; i<LoopLimit ; i++) 		
					{
						dAvgR += pDataIndex[2];
						dAvgG += pDataIndex[1];
						dAvgB += pDataIndex[0];
						pDataIndex+=3;
					}
					dAvgR=dAvgR/LoopLimit;
					dAvgG=dAvgG/LoopLimit;
					dAvgB=dAvgB/LoopLimit;
				}
            }
        }
    }
}
