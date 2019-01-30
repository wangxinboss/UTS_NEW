#include "StdAfx.h"
#include "Algorithm.h"

//-----------------------------------------------------------------------------
namespace UTS
{
	namespace Algorithm
	{
		namespace Correction_PD
		{
			int CalculateDPAFDefectTable
				(
				int _PitchX,
				int _PitchY,
				int _BlockNumX,
				int _BlockNumY,
				DefectPoint* _POS_L,
				int _PosLCount,
				DefectPoint* _POS_R,
				int _PosRCount,
				DefectPoint* PDAFDefectTable
				)
			{
				if ((nullptr == _POS_R)||(nullptr == _POS_L)||(nullptr == PDAFDefectTable))
				{
					return ((_BlockNumX*_BlockNumY)*(_PosLCount+_PosRCount));
				}
				int Totalcount = 0;
				for (int j=0;j<_BlockNumY;j++)
				{
					for (int i=0;i<_BlockNumX;i++)
					{
						for (int IndexL=0;IndexL<_PosLCount;IndexL++)
						{
							PDAFDefectTable[Totalcount].x = _POS_L[IndexL].x+_PitchX*i;
							PDAFDefectTable[Totalcount].y = _POS_L[IndexL].y+_PitchY*j;
							Totalcount++;
						}
						for (int IndexR=0;IndexR<_PosRCount;IndexR++)
						{
							PDAFDefectTable[Totalcount].x = _POS_R[IndexR].x+_PitchX*i;
							PDAFDefectTable[Totalcount].y = _POS_R[IndexR].y+_PitchY*j;
							Totalcount++;
						}
					}
				}
				return Totalcount;
			}
			//---------------------------------------------------------------------
			bool DefectPixelCorrection
				(
				unsigned char* _InOutRaw8Buffer,
				int _Width,
				int _Height,
				DefectPoint* DefectTable,
				int DefectCount
				)
			{
				bool bResult = true;
				int TotalCount = 0;
				for (int i = 0;i<DefectCount;i++)
				{
					DefectPoint point;
					point.x = DefectTable[i].x;
					point.y = DefectTable[i].y;
					if ((point.x < (_Width-2)) && (point.y < (_Height-2))&&((point.x>=2)&&(point.y>=2)))
					{
						int temp = _InOutRaw8Buffer[point.y * _Width + point.x] ;
						_InOutRaw8Buffer[point.y * _Width + point.x] = (_InOutRaw8Buffer[point.y * _Width + (point.x + 2)]+_InOutRaw8Buffer[point.y * _Width + (point.x - 2)]+_InOutRaw8Buffer[(point.y+2) * _Width + point.x]+_InOutRaw8Buffer[(point.y-2) * _Width + point.x])/4;
						TotalCount++;
					}
				}

				return bResult;
			}
			//---------------------------------------------------------------------

		}
	}
}