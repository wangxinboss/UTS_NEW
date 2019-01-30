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
		namespace Shading_Smartisan
		{
			void Shading_Y_Smartisan(
				const unsigned char *pYBuffer,
				int nWidth,
				int nHeight,
				Shading_RESULT &result)
			{
				//------------------------------------------------------------------------------
				// 初始化结果
				for (int i = 0; i < 25; i++)
				{
					result.dShadingCorner[i] = 0.0;
				}

				result.dShadingDelta = 0.0;

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

			//	double dAvgY[25];

				double MaxCorner = 0;
				double MinCorner = 65536;

				GetBlockAvgY(pYBuffer, nWidth, nHeight, rcROI[12], result.dAvgY[12]);

				for (int i = 0; i < 25; i++)
				{
					GetBlockAvgY(pYBuffer, nWidth, nHeight, rcROI[i], result.dAvgY[i]);  

					if(result.dAvgY[12] != 0.0)
					result.dShadingCorner[i] = result.dAvgY[i]/result.dAvgY[12]; 
					else
					result.dShadingCorner[i] = 0.0;

					if (MaxCorner < result.dShadingCorner[i])
					{
						MaxCorner = result.dShadingCorner[i];
					}
					if (MinCorner > result.dShadingCorner[i])
					{
						MinCorner = result.dShadingCorner[i];
					}
				}

				result.dShadingDelta = (MaxCorner - MinCorner);
				//result.dWorstRatio = MinCorner;

				// 将结果上下翻转
				SWAP(double, result.dShadingCorner[0], result.dShadingCorner[20]);
				SWAP(double, result.dShadingCorner[1], result.dShadingCorner[21]);
				SWAP(double, result.dShadingCorner[2], result.dShadingCorner[22]);
				SWAP(double, result.dShadingCorner[3], result.dShadingCorner[23]);
				SWAP(double, result.dShadingCorner[4], result.dShadingCorner[24]);
				SWAP(double, result.dShadingCorner[5], result.dShadingCorner[15]);
				SWAP(double, result.dShadingCorner[6], result.dShadingCorner[16]);
				SWAP(double, result.dShadingCorner[7], result.dShadingCorner[17]);
				SWAP(double, result.dShadingCorner[8], result.dShadingCorner[18]);
				SWAP(double, result.dShadingCorner[9], result.dShadingCorner[19]);

			}
		}
	}
}