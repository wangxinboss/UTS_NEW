#include "StdAfx.h"
#include "Algorithm.h"
//#include "Algorithm_Smartisan.h"

#include "UTS.h"

using namespace std;
using namespace UTS::Algorithm;
using namespace UTS::Algorithm::Image;


namespace UTS
{
	namespace Algorithm_Smartisan
	{
		
		namespace ColorShading_Smartisan
		{
			void ColorShading_Y_Smartisan(
				WORD* pRaw10,
				int rawBayerType,
				double blackLvl,
				int nWidth,
				int nHeight,
				ColorShading_RESULT &result)
			{
				//------------------------------------------------------------------------------
				// 初始化结果
				for (int i = 0; i < 25; i++)
				{
					result.dRatioRG[i] = 0.0;
					result.dRatioBG[i] = 0.0;
					result.dAvg4chanel[i].R = 0;
					result.dAvg4chanel[i].Gr = 0;
					result.dAvg4chanel[i].Gb = 0;
					result.dAvg4chanel[i].B = 0;
				}

				//?算
				RECT rcROI[25];
				
				SIZE sizeROI;
			//	color_rgrgbb_double dAvgCenter;
			//	color_rgrgbb_double dAvgCorner[Corner_SIZES];

				sizeROI.cx = nWidth/5;
				sizeROI.cy = nHeight/5;

				// roi
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						rcROI[i*5+j].left   = j * sizeROI.cx;
						rcROI[i*5+j].right  = j * sizeROI.cx + sizeROI.cx - 1;
						rcROI[i*5+j].top    = i * sizeROI.cy;
						rcROI[i*5+j].bottom = i * sizeROI.cy + sizeROI.cy - 1;

					}
				}

				uint16_t dAve_G[25];
				double rg[25],bg[25];

				GetROIAvgRGrGbB(pRaw10, rawBayerType, blackLvl, nWidth, nHeight, rcROI[12], result.dAvg4chanel[12]); //flip bmp 

				dAve_G[12] = (result.dAvg4chanel[12].Gr + result.dAvg4chanel[12].Gb)/2;								
				rg[12] = 1.0 * result.dAvg4chanel[12].R / dAve_G[12];
				bg[12] = 1.0 * result.dAvg4chanel[12].B / dAve_G[12];

				result.dWorstRatioBG = 256.0;
				result.dWorstRatioRG = 256.0;


				
				for (int i = 0; i < 25; i++)
				{
					GetROIAvgRGrGbB(pRaw10, rawBayerType, blackLvl, nWidth, nHeight, rcROI[i], result.dAvg4chanel[i]); //flip bmp 
					
					dAve_G[i] = (result.dAvg4chanel[i].Gr + result.dAvg4chanel[i].Gb)/2;
					
					rg[i] = 1.0 * result.dAvg4chanel[i].R / dAve_G[i];
					bg[i] = 1.0 * result.dAvg4chanel[i].B / dAve_G[i];

					result.dRatioRG[i] = rg[i]/rg[12];
					result.dRatioBG[i] = bg[i]/bg[12];
					//result.dRatioRG[i] = (result.dAvg4chanel[i].R/dAve_G[i])/(result.dAvg4chanel[12].R/dAve_G[12]);
					//result.dRatioBG[i] = (result.dAvg4chanel[i].B/dAve_G[i])/(result.dAvg4chanel[12].B/dAve_G[12]);

					if(result.dWorstRatioRG > result.dRatioRG[i]) result.dWorstRatioRG = result.dRatioRG[i];
					if(result.dWorstRatioBG > result.dRatioBG[i]) result.dWorstRatioBG = result.dRatioBG[i];
				}
			}
		}
	}
}