#include "StdAfx.h"
#include "Algorithm.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace DefectLine
        {
            namespace DefectLineStd
            {
                void DefectLine(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    double dThreshold,
                    vector<int> &vecHDefectLine,
                    vector<int> &vecVDefectLine)
                {
                    //-------------------------------------------------------------------------
                    // Param assert
                    assert(nullptr != pBmpBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);
                    assert(dThreshold > 0);

                    //-------------------------------------------------------------------------
                    // init result
                    vecHDefectLine.clear();
                    vecVDefectLine.clear();
                    
                    // Get Y buffer
                    unsigned char *pYBuffer = new unsigned char[nWidth * nHeight];
                    memset(pYBuffer, 0, nWidth * nHeight);
                    UTS::Algorithm::Image::CImageProc::GetInstance().Cal_RGBtoYBuffer(
                        pBmpBuffer,
                        nWidth,
                        nHeight,
                        pYBuffer);

                    // get AvgY Row
                    double *pdAvgYRow = new double[nHeight];
                    for (int j = 0; j < nHeight; j++)
                    {
                        double dAvgRow = 0.0;
                        for (int i = 0; i < nWidth; i++)
                        {
                            dAvgRow += pYBuffer[j * nWidth + i];
                        }
                        dAvgRow /= nWidth;
                        pdAvgYRow[j] = dAvgRow;
                    }

                    // get AvgY Col
                    double *pdAvgYCol = new double[nWidth];
                    for (int i = 0; i < nWidth; i++)
                    {
                        double dAvgCol = 0.0;
                        for (int j = 0; j < nHeight; j++)
                        {
                            dAvgCol += pYBuffer[i * nHeight + j];
                        }
                        dAvgCol /= nHeight;
                        pdAvgYCol[i] = dAvgCol;
                    }

                    // HDefectLine
                    for (int i = 1; i < nHeight; i++)
                    {
                        if (abs(pdAvgYRow[i] - pdAvgYRow[i - 1]) > dThreshold)
                        {
                            vecHDefectLine.push_back(i);
                        }
                    }
                    // VDefectLine
                    for (int i = 1; i < nWidth; i++)
                    {
                        if (abs(pdAvgYCol[i] - pdAvgYCol[i - 1]) > dThreshold)
                        {
                            vecVDefectLine.push_back(i);
                        }
                    }

                    RELEASE_ARRAY(pdAvgYCol);
                    RELEASE_ARRAY(pdAvgYRow);
                    RELEASE_ARRAY(pYBuffer);
                }
            }
        }
    }
}
