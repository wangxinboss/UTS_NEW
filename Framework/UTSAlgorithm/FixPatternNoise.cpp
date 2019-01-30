#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::Math;

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define ABS(a)	   (((a) < 0) ? -(a) : (a))	
#define RELEASE(a) (((a)!=0) ? (delete[] (a),(a) = 0):((a)=0))

namespace UTS
{
    namespace Algorithm
    {
        namespace FixPatternNoise
        {
            namespace FixPatternNoise_WD
            {
				void FixPatternNoiseY(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    double &dRowDifMaxMean,
					double &dColDifMaxMean)
                {
					assert(nullptr != pBmpBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);

					int m_nLoopSize=nWidth*nHeight;
					double *YArray = new double[nWidth*nHeight];
					memset(YArray,0,sizeof(double)*nWidth*nHeight);
					
					int	m_nR=0;
					int m_nG=0;
					int m_nB=0;
					int i=0;
					double *RowMeanArray=new double[nHeight];
					double *ColMeanArray=new double[nWidth];
					double  m_dRowDifMax=0.0;
					double  m_dColDifMax=0.0;
					memset(RowMeanArray,0,sizeof(double)*nHeight);
					memset(ColMeanArray,0,sizeof(double)*nWidth);
					LPBYTE pImageBuffer=pBmpBuffer;

					for (i=0;i<m_nLoopSize;i++)
					{
						m_nR=pImageBuffer[2];
						m_nG=pImageBuffer[1];
						m_nB=pImageBuffer[0];
						YArray[i] = YVALUE(m_nB,m_nG,m_nR);
						pImageBuffer+=3;
					}
					i=0;
					for (int y=0;y<nHeight;y++)
					{
						for (int x=0;x<nWidth;x++)
						{
							RowMeanArray[y]+=YArray[i]; 
							ColMeanArray[x]+=YArray[i]; 
							i++;
						}
					}
					for (int y=0;y<nHeight-1;y++)
					{
						m_dRowDifMax = MAX(m_dRowDifMax,ABS((RowMeanArray[y+1]-RowMeanArray[y])/nWidth));
					}

					for (int x=0;x<nWidth-1;x++)
					{
						m_dColDifMax = MAX(m_dColDifMax,ABS((ColMeanArray[x+1]-ColMeanArray[x])/nHeight));
					}
					dRowDifMaxMean=m_dRowDifMax;
					dColDifMaxMean=m_dColDifMax;

					RELEASE(YArray);
					RELEASE(RowMeanArray);
					RELEASE(ColMeanArray);
				}
			}
		}
	}
}