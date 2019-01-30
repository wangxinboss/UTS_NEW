#include "StdAfx.h"
#include "Algorithm.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace SFRBlock
        {
            //-------------------------------------------------------------------------
			bool ROI_SFR09D(unsigned char* RGBImage,
				int Width,int Height,
				int RoiW,int RoiH,
				SFRROI* Block)
			{
				bool Res = true;

				unsigned char* CBlock = new unsigned char[RoiW*RoiH*4];

				//切割四個角落
				int X=0;
				int Y=0;
				for (int y=0;y<RoiH;y++)
				{
					for (int x=0;x<RoiW;x++)
					{
						int _tShift = (y*RoiW+x)*4;
						//LL			
						X = 0+x;
						Y = 0+y;
						CBlock[_tShift+0] = RGBImage[(Y*Width+X)*3+1];

						//LR			
						X = (Width-1-RoiW)+x;
						Y = 0+y;
						CBlock[_tShift+1] = RGBImage[(Y*Width+X)*3+1];

						//BL			
						X = 0+x;
						Y = (Height-1-RoiH)+y;
						CBlock[_tShift+2] = RGBImage[(Y*Width+X)*3+1];

						//BR			
						X = (Width-1-RoiW)+x;
						Y = (Height-1-RoiH)+y;
						CBlock[_tShift+3] = RGBImage[(Y*Width+X)*3+1];
					}
				}

				int MaxV[4];
				memset(MaxV,0,sizeof(int)*4);
				int MinV[4];
				memset(MinV,256,sizeof(int)*4);

				int Threshold[4];
				memset(Threshold,256,sizeof(int)*4);

				int StartX[4];
				memset(StartX,0,sizeof(int)*4);
				int StartY[4];
				memset(StartY,0,sizeof(int)*4);

				for (int index=0;index<4;index++)
				{
					MinV[index] = 255;
					for (int y=0;y<RoiH;y++)
					{
						for (int x=0;x<RoiW;x++)
						{
							int _tShift = (y*RoiW+x)*4;
							MaxV[index] = max(MaxV[index],CBlock[_tShift+index]);
							MinV[index] = min(MinV[index],CBlock[_tShift+index]);
						}
					}

					Threshold[index] = (MaxV[index]-MinV[index])/4+MinV[index];

					int X = 0;
					int Y = 0;

					//發現四個區域的質心
					double Centroid_X = 0;
					double Centroid_Y = 0;
					double CentroidCount = 0;		

					int _tShift;
					for (int y=0;y<RoiH;y++)
					{
						for (int x=0;x<RoiW;x++)
						{
							_tShift = (y*RoiW+x)*4;
							X = x;
							Y = y;
							if (CBlock[_tShift+index] <= Threshold[index])
							{
								Centroid_X += x;
								Centroid_Y += y;
								CentroidCount++;
							}
						}
					}

					if (CentroidCount!=0)
					{
						Centroid_X /= CentroidCount;
						Centroid_Y /= CentroidCount;
					}
					else
					{
						Centroid_X = 0;
						Centroid_Y = 0;
						Res = false;
					}

					StartX[index] = (int )Centroid_X;
					StartY[index] = (int )Centroid_Y;

				}
				//實際啟始座標

				//LL
				StartX[0] += 0;
				StartY[0] += 0;
				//LR
				StartX[1] += (Width-1-RoiW);
				StartY[1] += 0;
				//BL
				StartX[2] += 0;
				StartY[2] += (Height-1-RoiH);
				//BR
				StartX[3] += (Width-1-RoiW);
				StartY[3] += (Height-1-RoiH);

				//四個方向搜尋，確認可能的區塊大小
				//啟始X、Y
				int XS[4];
				int YS[4];
				int XE[4];
				int YE[4];

				SFRROI BlockPoint[4];

				for (int i=0;i<4;i++)
				{
					int X = 0;
					int Y = 0;
					//上
					for (int y=StartY[i];y>=0;y--)
					{
						int x = StartX[i]; 
						int _tShift = (y*Width+x)*3;
						if (RGBImage[_tShift+1] >= Threshold[i])
						{
							X = x;
							Y = y;
							break;
						}
					}
					YS[i] =  Y;

					//下
					for (int y=StartY[i];y<Height;y++)
					{
						int x = StartX[i]; 
						int _tShift = (y*Width+x)*3;
						X = x;
						Y = y;
						if (RGBImage[_tShift+1] >= Threshold[i])
						{
							break;
						}
					}
					YE[i] =  Y;

					//左
					for (int x=StartX[i];x>=0;x--)
					{
						int y = StartY[i]; 
						int _tShift = (y*Width+x)*3;
						X = x;
						Y = y;
						if (RGBImage[_tShift+1] >= Threshold[i])
						{
							break;
						}
					}

					XS[i] =  X;

					//右
					for (int x=StartX[i];x<Width;x++)
					{
						int y = StartY[i]; 
						int _tShift = (y*Width+x)*3;
						X = x;
						Y = y;
						if (RGBImage[_tShift+1] >= Threshold[i])
						{
							break;
						}
					}
					XE[i] =  X;

					//放大1.2倍當作擷取範圍

					int A = XS[i]-ROUND(double(XE[i]-XS[i])*0/*.2*/);
					int B = YS[i]-ROUND(double(YE[i]-YS[i])*0/*.2*/);
					if (A < 0)
					{
						A = 0;
					}
					if (B < 0)
					{
						B = 0;
					}

					int A1 = XE[i]+ROUND(double(XE[i]-XS[i])*0/*.2*/);
					int B1 = YE[i]+ROUND(double(YE[i]-YS[i])*0/*.2*/);

					if (A1 >= Width)
					{
						A1 = Width-1;
					}
					if (B1 >= Height)
					{
						B1 = Height-1;
					}

					// 		BlockPoint[i].x = XS[i];
					// 		BlockPoint[i].y = YS[i];
					// 		BlockPoint[i].width  = XE[i]-XS[i];
					// 		BlockPoint[i].height = YE[i]-YS[i];
					BlockPoint[i].x = A;
					BlockPoint[i].y = B;
					BlockPoint[i].width  = A1-A;
					BlockPoint[i].height = B1-B;
				}

				int FinalX[4];
				int FinalY[4];
				//重新找質心
				for (int i=0;i<4;i++)
				{
					//發現四個區域的質心
					double Centroid_X = 0;
					double Centroid_Y = 0;
					double CentroidCount = 0;		

					int RoiWidth =BlockPoint[i].width;
					int RoiHeight =BlockPoint[i].height;
					int RoiStartX = BlockPoint[i].x;
					int RoiStartY = BlockPoint[i].y;

					for (int y=0;y<RoiHeight;y++)
					{
						for (int x=0;x<RoiWidth;x++)
						{				
							X = x+RoiStartX;
							Y = y+RoiStartY;
							if (RGBImage[(Y*Width+X)*3+1] <= Threshold[i])
							{
								Centroid_X += X;
								Centroid_Y += Y;
								CentroidCount++;
							}
						}
					}

					if (CentroidCount!=0)
					{
						Centroid_X /= CentroidCount;
						Centroid_Y /= CentroidCount;
					}
					else
					{
						Centroid_X = 0;
						Centroid_Y = 0;
						Res = false;
					}

					FinalX[i] = (int )Centroid_X;
					FinalY[i] = (int )Centroid_Y;

				}

				//先試試看直接抓邊是否可行。

				//四個方向搜尋，確認可能的區塊大小
				//啟始X、Y
				int XS2[4];
				int YS2[4];
				int XE2[4];
				int YE2[4];

				SFRROI BlockPoint2[4];

				for (int i=0;i<4;i++)
				{
					int X = 0;
					int Y = 0;
					//上
					for (int y=FinalY[i];y>=0;y--)
					{
						int x = FinalX[i]; 
						int _tShift = (y*Width+x)*3;
						if (RGBImage[_tShift+1] >= Threshold[i])
						{
							X = x;
							Y = y;
							break;
						}
					}
					YS2[i] =  Y;

					//下
					for (int y=FinalY[i];y<Height;y++)
					{
						int x = FinalX[i]; 
						int _tShift = (y*Width+x)*3;
						X = x;
						Y = y;
						if (RGBImage[_tShift+1] >= Threshold[i])
						{
							break;
						}
					}
					YE2[i] =  Y;

					//左
					for (int x=FinalX[i];x>=0;x--)
					{
						int y = FinalY[i]; 
						int _tShift = (y*Width+x)*3;
						X = x;
						Y = y;
						if (RGBImage[_tShift+1] >= Threshold[i])
						{
							break;
						}
					}

					XS2[i] =  X;

					//右
					for (int x=FinalX[i];x<Width;x++)
					{
						int y = FinalY[i]; 
						int _tShift = (y*Width+x)*3;
						X = x;
						Y = y;
						if (RGBImage[_tShift+1] >= Threshold[i])
						{
							break;
						}
					}
					XE2[i] =  X;

					BlockPoint2[i].x = XS2[i];
					BlockPoint2[i].y = YS2[i];
					BlockPoint2[i].width  = XE2[i]-XS2[i];
					BlockPoint2[i].height = YE2[i]-YS2[i];

				}

				//最終結果
				for (int i=0;i<4;i++)
				{
					if ((i==0)||(i==2))
					{
						Block[i].height = BlockPoint2[i].height/2;
						Block[i].width = BlockPoint2[i].width*3/2;
						if (Block[i].width > Block[i].height)
						{
							Block[i].width = Block[i].height;
						}

						Block[i].x = BlockPoint2[i].x + BlockPoint2[i].width-Block[i].width/2;
						Block[i].y = BlockPoint2[i].y + BlockPoint2[i].height/2-Block[i].height/2;
					}
					if ((i==1)||(i==3))
					{
						Block[i].height = BlockPoint2[i].height/2;
						Block[i].width = BlockPoint2[i].width*3/2;

						if (Block[i].width > Block[i].height)
						{
							Block[i].width = Block[i].height;
						}

						Block[i].x = BlockPoint2[i].x -Block[i].width/2;
						Block[i].y = BlockPoint2[i].y + BlockPoint2[i].height/2 -Block[i].height/2;
					}
				}
		
				for (int i=0;i<4;i++)
				{
					Block[i].x = RANGE(Block[i].x,0,Width);
					Block[i].y = RANGE(Block[i].y,0,Height);
					if ((Block[i].x+Block[i].width)>=Width)
					{
						Block[i].width = Width-Block[i].x-1;
					}
					if ((Block[i].y+Block[i].height)>=Height)
					{
						Block[i].height = Height-Block[i].y-1;
					}

				}

				RELEASE_ARRAY(CBlock);

				return Res;
			}
       
			bool ROI_SFR(unsigned char* CBlock,
				int RoiW,int RoiH,
				SFRROI* BlockPoint2)
			{
				bool Res = true;

				int MaxV = 0;
				int MinV = 256;
		
				int Threshold = 256;
		
				int StartX = 0;
				int StartY = 0;
				double CentroidCount = 0;	
				double value = 0;

				for (int y=0;y<RoiH;y++)
				{
					for (int x=0;x<RoiW;x++)
					{
						int _tShift = (y*RoiW+x)*3 + 1;
						MaxV = max(MaxV,CBlock[_tShift]);
						MinV = min(MinV,CBlock[_tShift]);

						value += CBlock[_tShift];
						CentroidCount++;
					}
				}

				//value /= CentroidCount;
				Threshold = (MaxV-MinV)/4+MinV;
				//Threshold = (int)value;

				int X = 0;
				int Y = 0;

				//發現四個區域的質心
				double Centroid_X = 0;
				double Centroid_Y = 0;
				CentroidCount = 0;	

				int _tShift;
				for (int y=0;y<RoiH;y++)
				{
					for (int x=0;x<RoiW;x++)
					{
						_tShift = (y*RoiW+x)*3 + 1;
						X = x;
						Y = y;
						if (CBlock[_tShift] <= Threshold)
						{
							Centroid_X += x;
							Centroid_Y += y;
							CentroidCount++;
						}
					}
				}

				if (CentroidCount!=0)
				{
					Centroid_X /= CentroidCount;
					Centroid_Y /= CentroidCount;
				}
				else
				{
					Centroid_X = 0;
					Centroid_Y = 0;
					Res = false;
				}

				StartX = (int)Centroid_X;
				StartY = (int)Centroid_Y;

				//實際啟始座標
				//LL
				StartX += 0;
				StartY += 0;
			
				//四個方向搜尋，確認可能的區塊大小
				//啟始X、Y
				int XS;
				int YS;
				int XE;
				int YE;

				SFRROI BlockPoint;
			
				//上
				Y = 0;
				for (int y=StartY ; y >= 0 ; y--)
				{
					int x = StartX; 
					int _tShift = (y*RoiW+x)*3 + 1;
					if (CBlock[_tShift] >= Threshold)
					{
						X = x;
						Y = y;
						break;
					}
				}
				YS =  Y;

				//下
				for (int y=StartY;y<RoiH;y++)
				{
					int x = StartX; 
					int _tShift = (y*RoiW+x)*3 + 1;
					X = x;
					Y = y;
					if (CBlock[_tShift] >= Threshold)
					{
						break;
					}
				}
				YE =  Y;

				//左
				for (int x=StartX ; x>=0 ; x--)
				{
					int y = StartY; 
					int _tShift = (y*RoiW+x)*3 + 1;
					X = x;
					Y = y;
					if (CBlock[_tShift] >= Threshold)
					{
						break;
					}
				}

				XS =  X;

				//右
				for (int x=StartX;x<RoiW;x++)
				{
					int y = StartY; 
					int _tShift = (y*RoiW+x)*3 + 1;
					X = x;
					Y = y;
					if (CBlock[_tShift] >= Threshold)
					{
						break;
					}
				}
				XE =  X;

				//放大1.2倍當作擷取範圍

				int A = XS-ROUND(double(XE-XS)*0/*.2*/);
				int B = YS-ROUND(double(YE-YS)*0/*.2*/);
				if (A < 0)
				{
					A = 0;
				}
				if (B < 0)
				{
					B = 0;
				}

				int A1 = XE+ROUND(double(XE-XS)*0/*.2*/);
				int B1 = YE+ROUND(double(YE-YS)*0/*.2*/);

				if (A1 >= RoiW)
				{
					A1 = RoiW-1;
				}
				if (B1 >= RoiH)
				{
					B1 = RoiH-1;
				}

				BlockPoint.x = A;
				BlockPoint.y = B;
				BlockPoint.width  = A1-A;
				BlockPoint.height = B1-B;
				
				int FinalX;
				int FinalY;
				//重新找質心
				
				//發現四個區域的質心
				Centroid_X = 0;
				Centroid_Y = 0;
				CentroidCount = 0;		

				int RoiWidth =BlockPoint.width;
				int RoiHeight =BlockPoint.height;
				int RoiStartX = BlockPoint.x;
				int RoiStartY = BlockPoint.y;

				for (int y=0;y<RoiHeight;y++)
				{
					for (int x=0;x<RoiWidth;x++)
					{				
						X = x+RoiStartX;
						Y = y+RoiStartY;
						if (CBlock[(Y*RoiW+X)*3+1] <= Threshold)
						{
							Centroid_X += X;
							Centroid_Y += Y;
							CentroidCount++;
						}
					}
				}

				if (CentroidCount!=0)
				{
					Centroid_X /= CentroidCount;
					Centroid_Y /= CentroidCount;
				}
				else
				{
					Centroid_X = 0;
					Centroid_Y = 0;
					Res = false;
				}

				FinalX = (int )Centroid_X;
				FinalY = (int )Centroid_Y;

				//四個方向搜尋，確認可能的區塊大小
				//啟始X、Y
				int XS2;
				int YS2;
				int XE2;
				int YE2;

				X = 0;
				Y = 0;
				//上
				for (int y=FinalY;y>=0;y--)
				{
					int x = FinalX; 
					int _tShift = (y*RoiW+x)*3+ 1;
					if (CBlock[_tShift] >= Threshold)
					{
						X = x;
						Y = y;
						break;
					}
				}
				YS2 =  Y;

				//下
				for (int y=FinalY;y<RoiH;y++)
				{
					int x = FinalX; 
					int _tShift = (y*RoiW+x)*3+ 1;
					X = x;
					Y = y;
					if (CBlock[_tShift] >= Threshold)
					{
						break;
					}
				}
				YE2 =  Y;

				//左
				for (int x=FinalX;x>=0;x--)
				{
					int y = FinalY; 
					int _tShift = (y*RoiW+x)*3+ 1;
					X = x;
					Y = y;
					if (CBlock[_tShift] >= Threshold)
					{
						break;
					}
				}

				XS2 =  X;

				//右
				for (int x=FinalX;x<RoiW;x++)
				{
					int y = FinalY; 
					int _tShift = (y*RoiW+x)*3+ 1;
					X = x;
					Y = y;
					if (CBlock[_tShift] >= Threshold)
					{
						break;
					}
				}
				XE2 =  X;

				BlockPoint2->x = FinalX;
				BlockPoint2->y = FinalY;
				BlockPoint2->width  = XE2-XS2;
				BlockPoint2->height = YE2-YS2;

				return Res;
			}//ROI_SFR

			bool ROI_FindCenetr(unsigned char* CBlock,
				int RoiW,int RoiH,
				POINT* CenterPoint)
			{
				bool Res = true;

				int Threshold = 256;

				int StartX = 0;
				int StartY = 0;
				double CentroidCount = 0;	
				double value = 0;
				int MaxV = 0;
				int MinV = 256;

				for (int y=0;y<RoiH;y++)
				{
					for (int x=0;x<RoiW;x++)
					{
						int _tShift = (y*RoiW+x)*3 + 1;
						MaxV = max(MaxV,CBlock[_tShift]);
						MinV = min(MinV,CBlock[_tShift]);

						value += CBlock[_tShift];
						CentroidCount++;
					}
				}

				//value /= CentroidCount;
				Threshold = (MaxV-MinV)/2;

				//四個方向搜尋，確認可能的區塊大小
				//
				int UStart,UEnd,BStart,BEnd,LStart,LEnd,RStart,REnd;

				//UP 
				for(int x = 0 ;x < RoiW ;x++)
				{
					int _tShift = (0*RoiW+x)*3 + 1;
					if(CBlock[_tShift] > Threshold) 
					{
						UStart = x;
						break;
					}
				}

				for(int x = (RoiW-1) ;x > 0 ;x--)
				{
					int _tShift = (0*RoiW+x)*3 + 1;
					if(CBlock[_tShift] > Threshold) 
					{
						UEnd = x;
						break;
					}
				}

				//Bottom
				for(int x = 0 ;x < RoiW ;x++)
				{
					int _tShift = ((RoiH-1)*RoiW+x)*3 + 1;
					if(CBlock[_tShift] > Threshold) 
					{
						BStart = x;
						break;
					}
				}

				for(int x = (RoiW-1) ;x > 0 ;x--)
				{
					int _tShift = ((RoiH-1)*RoiW+x)*3 + 1;
					if(CBlock[_tShift] > Threshold) 
					{
						BEnd = x;
						break;
					}
				}

				//Left
				for (int y = 0;y<RoiH;y++)
				{
					int _tShift = (y*RoiW + 0)*3+ 1;
					if (CBlock[_tShift] > Threshold)
					{
						LStart = y;
						break;
					}
				}
				for (int y = (RoiH-1);y > 0;y--)
				{
					int _tShift = (y*RoiW + 0)*3+ 1;
					if (CBlock[_tShift] > Threshold)
					{
						LEnd = y;
						break;
					}
				}

				//Right
				for (int y = 0;y<RoiH;y++)
				{
					int _tShift = (y*RoiW + (RoiW-1))*3+ 1;
					if (CBlock[_tShift] > Threshold)
					{
						RStart = y;
						break;
					}
				}

				for (int y = (RoiH -1);y > 0;y--)
				{
					int _tShift = (y*RoiW + (RoiW-1))*3+ 1;
					if (CBlock[_tShift] > Threshold)
					{
						REnd = y;
						break;
					}
				}

				if(UStart > UEnd || BStart > BEnd ||
				   LStart > UEnd || RStart > REnd)
				{
					Res = false;
					CenterPoint->x = 0;
					CenterPoint->y = 0;
				}

				CenterPoint->x = (UStart + UEnd + BStart + BEnd)/4;
				CenterPoint->y = (LStart + LEnd + RStart + REnd)/4;

				return Res;
			}
		}   // namespace SFRBlock
    }   // namespace Algorithm
}   // namespace UTS
