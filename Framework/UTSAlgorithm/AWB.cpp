#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::Math;

#define ABS(a)	   (((a) < 0) ? -(a) : (a))	

namespace UTS
{
    namespace Algorithm
    {
        namespace AWB
        {
            namespace AWB_Altek
			{
				typedef enum
				{
					UL		=0,
					UR		=1,
					BL		=2,
					BR		=3,
					CT		=4,
					SIZES	=5	
				}tagShd;
				
				void AWBY(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
					int nROIWidth,
					int nROIHeight,
                    AWBInfo &result)
				{
					CPoint Block_Pos[SIZES];
					double	m_dBlockRGRatio[SIZES] = {0};
					double	m_dBlockBGRatio[SIZES] = {0};
					memset(&result,0,sizeof(AWBInfo));

					double dMinRG= 65535,dMaxRG=-1,dMinBG= 65535,dMaxBG=-1,dRGRatio,dBGRatio;
					int	nR,nG,nB,i,x,y,m_ShdProduct;
					LPBYTE	pTmp[SIZES];

					Block_Pos[UL].x = 0;
					Block_Pos[UL].y = 0;
					Block_Pos[UR].x = nWidth- nROIWidth;
					Block_Pos[UR].y = 0;		
					Block_Pos[BL].x = 0;
					Block_Pos[BL].y = nHeight- nROIHeight;
					Block_Pos[BR].x = nWidth- nROIWidth;
					Block_Pos[BR].y = nHeight- nROIHeight;
					Block_Pos[CT].x = nWidth/2- nROIWidth/2;
					Block_Pos[CT].y = nHeight/2- nROIHeight/2;

					m_ShdProduct=nROIHeight*nROIWidth;
					
					for (i=0 ; i < SIZES ; i++)
					{
						for (y=0; y<nROIHeight;y++)
						{
							pTmp[i]=pBmpBuffer+((y+Block_Pos[i].y)*nWidth+Block_Pos[i].x)*3;
							for(x=0 ; x<nROIWidth ; x++)
							{
								nR=pTmp[i][2];
								nG=pTmp[i][1];
								nB=pTmp[i][0];

								dRGRatio=(double)nR/nG;
								dBGRatio=(double)nB/nG;
								m_dBlockRGRatio[i]+=dRGRatio;
								m_dBlockBGRatio[i]+=dBGRatio;
								pTmp[i]+=3;
							}
						}
						m_dBlockRGRatio[i]/=m_ShdProduct;
						m_dBlockBGRatio[i]/=m_ShdProduct;

						result.dRG[i] = m_dBlockRGRatio[i];
						result.dBG[i] = m_dBlockBGRatio[i];
					}
					result.dRGCenter=m_dBlockRGRatio[CT];
					result.dBGCenter=m_dBlockBGRatio[CT];
					for (i=0 ; i < 4 ; i ++) 
					{
						if ( m_dBlockRGRatio[i] < dMinRG ) 
							dMinRG = m_dBlockRGRatio[i]; 
						if ( m_dBlockRGRatio[i] > dMaxRG ) 
							dMaxRG = m_dBlockRGRatio[i]; 

						if ( m_dBlockBGRatio[i] < dMinBG ) 
							dMinBG = m_dBlockBGRatio[i]; 
						if ( m_dBlockBGRatio[i] > dMaxBG ) 
							dMaxBG = m_dBlockBGRatio[i];
					}
					result.dRGMax=dMaxRG;
					result.dBGMax=dMaxBG;
					result.dRGDelta=ABS(result.dRGMax-result.dRGCenter)/result.dRGCenter;
					result.dBGDelta=ABS(result.dBGMax-result.dBGCenter)/result.dBGCenter;
				}
			}
		}
	}
}