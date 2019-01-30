#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::Image;

typedef struct CtRectangle
{
	int x;
	int y;
	int width;
	int height;
}
CtRectangle;
//!	定義二維『RGB24影像』的結構
typedef struct CtRGBImg
{
	int width;
	int height;
	unsigned char* img;
}
CtRGBImg;

namespace UTS
{
	namespace Algorithm
	{
		namespace ColorUniformity
		{
			namespace CtColorUniformity
			{

				//-----------------------------------------------------------------------------
				BOOL GetDivideMethodA(/*unsigned char *_pImageBuffer,*/int _Width,int _Height, int _HBlockCount, int _VBlockCount,CtRectangle *_ROIInfoArray);
				BOOL RGBImageGetROI(unsigned char *_pImageBuffer,int _Width,int _Height, CtRectangle _ROIInfo, unsigned char *_pROIBuffer);
				BOOL DivideToSubImageA(unsigned char *_pImageBuffer,int _Width,int _Height, int _HBlockCount, int _VBlockCount,CtRectangle *_ROIInfoArray, CtRGBImg *_SubImage);
				BOOL GetAverageRBRatio(unsigned char *_pRGBBuffer, int _Width,int _Height,double &RBRatio);
				//-----------------------------------------------------------------------------
				

				BOOL CtColorUniformity(unsigned char *_RGBBuffer, int _Width, int _Height, int _HBlockCount, int _VBlockCount,double *RBRatioArray, double &_CU,double &_CUMaxMin)
				{
					BOOL Res = TRUE;
					//-------------------------
					_CU = 0;
					_CUMaxMin = 0;

					CtRectangle *ROIInfoArray = new CtRectangle[_HBlockCount*_VBlockCount];
					memset(ROIInfoArray,0,sizeof(CtRectangle)*_HBlockCount*_VBlockCount);
					/*CtProcess::*/GetDivideMethodA(/*_RGBBuffer,*/_Width,_Height,_HBlockCount,_VBlockCount,ROIInfoArray);

					CtRGBImg *SubImage = new CtRGBImg[_HBlockCount*_VBlockCount];
					for (int y=0;y<_VBlockCount;y++)
					{
						for (int x=0;x<_HBlockCount;x++)
						{
							int _tIndex = y*_HBlockCount+x;
							int _tW = ROIInfoArray[_tIndex].width;
							int _tH = ROIInfoArray[_tIndex].height;
							SubImage[_tIndex].width = _tW;
							SubImage[_tIndex].height = _tH;
							SubImage[_tIndex].img = new unsigned char[_tW*_tH*3];
							memset(SubImage[_tIndex].img,0,sizeof(unsigned char)*_tW*_tH*3);
						}
					}

					/*CtProcess::*/DivideToSubImageA(_RGBBuffer,_Width,_Height,_HBlockCount,_VBlockCount,ROIInfoArray,SubImage);

					RELEASE_ARRAY(ROIInfoArray);

					//	計算Y Channel
					//double *RBRatioArray = new double[_HBlockCount*_VBlockCount];
					memset(RBRatioArray,0,sizeof(double)*_HBlockCount*_VBlockCount);

					double MaxRBRatio = -65536;
					double MinRBRatio = 65536;

					for (int y=0;y<_VBlockCount;y++)
					{
						for (int x=0;x<_HBlockCount;x++)
						{
							int _tIndex = y*_HBlockCount+x;
							/*CtProcess::*/GetAverageRBRatio(SubImage[_tIndex].img,SubImage[_tIndex].width,SubImage[_tIndex].height,RBRatioArray[_tIndex]);
							if (MaxRBRatio < RBRatioArray[_tIndex])
							{
								MaxRBRatio = RBRatioArray[_tIndex];
							}
							else if (MinRBRatio > RBRatioArray[_tIndex])
							{
								MinRBRatio = RBRatioArray[_tIndex];
							}
						}
					}

					for (int y=0;y<_VBlockCount;y++)
					{
						for (int x=0;x<_HBlockCount;x++)
						{
							int _tIndex = y*_HBlockCount+x;
							RELEASE_ARRAY(SubImage[_tIndex].img);
						}
					}
					RELEASE_ARRAY(SubImage);
					//-------------------------

					double MaxRatio = -65536;
					for (int y=0;y<_VBlockCount;y++)
					{
						for (int x=0;x<_HBlockCount;x++)
						{
							int X = x;
							int Y = y;
							for (int j=-1;j<2;j++)
							{
								for (int i=-1;i<2;i++)
								{
									int _X = X+i;
									int _Y = Y+j;
									if (_X<0)
									{
										_X = X;
									}
									if (_Y<0)
									{
										_Y = Y;
									}
									if (_X>=_HBlockCount)
									{
										_X = X;
									}
									if (_Y>=_VBlockCount)
									{
										_Y = Y;
									}
									double Ratio = abs(RBRatioArray[Y*_HBlockCount+X]-RBRatioArray[_Y*_HBlockCount+_X])/RBRatioArray[Y*_HBlockCount+X];
									if (MaxRatio < Ratio)
									{
										MaxRatio = Ratio;
									}
								}
							}
						}
					}

					_CU = MaxRatio*100;
					_CUMaxMin = (MaxRBRatio - MinRBRatio)*100;

					return Res;
				}
				//-----------------------------------------------------------------------------
				//-----------------------------------------------------------------------------
				BOOL GetDivideMethodA(/*unsigned char *_pImageBuffer,*/int _Width,int _Height, int _HBlockCount, int _VBlockCount,CtRectangle *_ROIInfoArray)
				{
					BOOL Res = TRUE;
					//-----------------------------------------
					//	清空輸出Buffer
					memset(_ROIInfoArray,0,sizeof(CtRectangle)*_HBlockCount*_VBlockCount);

					for(int y=0;y<_VBlockCount;y++)
					{
						for(int x=0;x<_HBlockCount;x++)
						{
							_ROIInfoArray[y*_HBlockCount+x].x = ROUND((double)_Width/(double)_HBlockCount*(double)x);
							_ROIInfoArray[y*_HBlockCount+x].y = ROUND((double)_Height/(double)_VBlockCount*(double)y);
							_ROIInfoArray[y*_HBlockCount+x].width = ROUND((double)_Width/(double)_HBlockCount*(double)(x+1))-ROUND((double)_Width/(double)_HBlockCount*(double)x);
							_ROIInfoArray[y*_HBlockCount+x].height = ROUND((double)_Height/(double)_VBlockCount*(double)(y+1))-ROUND((double)_Height/(double)_VBlockCount*(double)y);
						}
					}

					//-----------------------------------------
					return Res;
				}

				BOOL RGBImageGetROI(unsigned char *_pImageBuffer,int _Width,int _Height, CtRectangle _ROIInfo, unsigned char *_pROIBuffer)
				{
					BOOL Res = TRUE;
					//-------------------------------------------------------------
					if ((_pImageBuffer == 0)||(_pROIBuffer == 0))
					{
						Res = false;
						return Res;
					}
					//-------------------------------------------------------------
					//-----------------------------------------
					int StartX = _ROIInfo.x;
					int StartY = _ROIInfo.y;
					int ROIWidth = _ROIInfo.width;
					int ROIHeight = _ROIInfo.height;
					memset(_pROIBuffer,0,sizeof(unsigned char)*ROIWidth*ROIHeight*3);

					if ((StartX < 0)||(StartX >=_Width)||(StartY < 0)||(StartY >= _Height))
					{
						Res = false;
						return Res;
					}
					if (((StartX+ROIWidth) > _Width)||(ROIWidth < 0)||((StartY+ROIHeight) > _Height)||(ROIHeight < 0))
					{
						Res = false;
						return Res;
					}

					//	清空輸出Buffer
					for (int y=0;y<ROIHeight;y++)
					{
						for (int x=0;x<ROIWidth;x++)
						{
							int index = (y*ROIWidth+x)*3;
							int index2 = ((StartY+y)*_Width+(StartX+x))*3;
							_pROIBuffer[index+0] = _pImageBuffer[index2+0];
							_pROIBuffer[index+1] = _pImageBuffer[index2+1];
							_pROIBuffer[index+2] = _pImageBuffer[index2+2];
						}
					}
					//-----------------------------------------
					return Res;
				}
				BOOL DivideToSubImageA(unsigned char *_pImageBuffer,int _Width,int _Height, int _HBlockCount, int _VBlockCount,CtRectangle *_ROIInfoArray, CtRGBImg *_SubImage)
				{
					BOOL Res = TRUE;
					//-----------------------------------------
					//	清空輸出Buffer
					for(int y=0;y<_VBlockCount;y++)
					{
						for(int x=0;x<_HBlockCount;x++)
						{
							_SubImage[y*_HBlockCount+x].width = _ROIInfoArray[y*_HBlockCount+x].width;
							_SubImage[y*_HBlockCount+x].height = _ROIInfoArray[y*_HBlockCount+x].height;
							memset(_SubImage[y*_HBlockCount+x].img,0,_SubImage[y*_HBlockCount+x].width*_SubImage[y*_HBlockCount+x].height*3);
						}
					}

					for(int y=0;y<_VBlockCount;y++)
					{
						for(int x=0;x<_HBlockCount;x++)
						{
							int StartX = _ROIInfoArray[y*_HBlockCount+x].x;
							int StartY = _ROIInfoArray[y*_HBlockCount+x].y;
							int ROIWidth	= _ROIInfoArray[y*_HBlockCount+x].width;
							int ROIHeight = _ROIInfoArray[y*_HBlockCount+x].height;

							unsigned char *_pROIImage = new unsigned char[ROIWidth*ROIHeight*3];
							memset(_pROIImage,0,sizeof(unsigned char)*ROIWidth*ROIHeight*3);
							RGBImageGetROI(_pImageBuffer,_Width,_Height,_ROIInfoArray[y*_HBlockCount+x],_pROIImage);

							_SubImage[y*_HBlockCount+x].width = ROIWidth;
							_SubImage[y*_HBlockCount+x].height = ROIHeight;
							memcpy(_SubImage[y*_HBlockCount+x].img,_pROIImage,sizeof(unsigned char)*ROIWidth*ROIHeight*3);
							RELEASE_ARRAY(_pROIImage);
						}				
					}
					//-----------------------------------------
					return Res;
				}
				BOOL GetAverageRBRatio(unsigned char *_pRGBBuffer, int _Width,int _Height,double &RBRatio)
				{
					BOOL Res = TRUE;
					//-------------------------------------
					RBRatio = 0;

					double RAvg = 0;
					double BAvg = 0;

					for (int y=0;y<_Height;y++)
					{
						double _tRAvg = 0;
						double _tBAvg = 0;
						for (int x=0;x<_Width;x++)
						{
							int _tIndex = (y*_Width+x)*3;
							double r = _pRGBBuffer[_tIndex+0];
							double b = _pRGBBuffer[_tIndex+2];

							_tRAvg += r;
							_tBAvg += b;
						}
						_tRAvg /= double(_Width);
						_tBAvg /= double(_Width);

						RAvg += _tRAvg;
						BAvg += _tBAvg;
					}
					RAvg /= double(_Height);
					BAvg /= double(_Height);

					RBRatio = RAvg/BAvg;
					//-------------------------------------
					return Res;
				}
			}
		}
	}
}
