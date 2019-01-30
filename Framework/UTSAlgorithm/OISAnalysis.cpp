#include "StdAfx.h"
#include "Algorithm.h"

#include <vector>
#include <numeric>

#include "../../../3rdparty/opencv231/include/opencv.hpp"

#pragma comment(lib,"../../../3rdparty/opencv231/lib/opencv_imgproc231.lib")
#pragma comment(lib,"../../../3rdparty/opencv231/lib/opencv_core231.lib")
#pragma comment(lib,"../../../3rdparty/opencv231/lib/opencv_highgui231.lib")
#pragma comment(lib,"../../../3rdparty/opencv231/lib/opencv_objdetect231.lib")


namespace UTS
{
	namespace Algorithm
	{
		OISAnalysis::OISAnalysis()
		{
			StaticCircleWidth = 0;
			StaticCircleHeight = 0;
			DynamicCircleWidth = 0;
			DynamicCircleHeight = 0;
			OISOFFCircleWidth = 0;
			OISOFFCircleHeight = 0;

			Width = 0;
			Height = 0;
			ROISize = 0;
			Level_A_Threshold = 0;
			Level_B_Threshold = 0;
			OIS_Threshold  = 0;
		}

		OISAnalysis::~OISAnalysis()
		{

		}
		void OISAnalysis::SetStaticImage
			(
			unsigned char* _pRGBStaticBuffer,
			int _Width,
			int _Height,
			int _ROISize,
			int _Level_A_Threshold,
			int _Level_B_Threshold
			)
		{
			Width = _Width;
			Height = _Height;
			StaticCircleWidth = 0;
			StaticCircleHeight = 0;
			ROISize = _ROISize;
			Level_A_Threshold = _Level_A_Threshold;
			Level_B_Threshold = _Level_B_Threshold;

			unsigned char* pStaticBuffer = new unsigned char[_Width*_Height];
			memset(pStaticBuffer,0,sizeof(unsigned char)*_Width*_Height);

			for (int y = 0;y < _Height;y++)
			{
				for (int x = 0;x < _Width;x++)
				{
					pStaticBuffer[y*_Width+x] = _pRGBStaticBuffer[(y*_Width+x)*3+1];
				}
			}

			Do(pStaticBuffer,_Width,_Height,StaticCircleWidth,StaticCircleHeight);

			if (pStaticBuffer!=nullptr)
			{
				delete[] pStaticBuffer;	
				pStaticBuffer = nullptr;
			}
		}

		void OISAnalysis::SetOISOFFImage
			(
			unsigned char* _pRGBStaticBuffer,
			int _Width,
			int _Height,
			int _ROISize)
		{
			Width = _Width;
			Height = _Height;

			ROISize = _ROISize;

			unsigned char* pStaticBuffer = new unsigned char[_Width*_Height];
			memset(pStaticBuffer,0,sizeof(unsigned char)*_Width*_Height);

			for (int y = 0;y < _Height;y++)
			{
				for (int x = 0;x < _Width;x++)
				{
					pStaticBuffer[y*_Width+x] = _pRGBStaticBuffer[(y*_Width+x)*3+1];
				}
			}

			Do(pStaticBuffer,_Width,_Height,OISOFFCircleWidth,OISOFFCircleHeight);

			if (pStaticBuffer!=nullptr)
			{
				delete[] pStaticBuffer;	
				pStaticBuffer = nullptr;
			}
		}

		bool OISAnalysis::Get_OIS_Status
			(
			unsigned char* _pRGBDynamicBuffer,			
			int &_StaticCircleWidth,
			int &_StaticCircleHeight,
			int &_OISOFFCircleWidth,
			int &_OISOFFCircleHeight,
			int &_DynamicCircleWidth,
			int &_DynamicCircleHeight,
			int _OIS_Threshold
			)
		{
			bool bRes= true;
			int _Width = Width;
			int _Height = Height;
			OIS_Threshold = _OIS_Threshold;

			unsigned char* pGrayDynamicBuffer = new unsigned char[_Width*_Height];
			memset(pGrayDynamicBuffer,0,sizeof(unsigned char)*_Width*_Height);

			for (int y = 0;y < _Height;y++)
			{
				for (int x = 0;x < _Width;x++)
				{
					pGrayDynamicBuffer[y*_Width+x] = _pRGBDynamicBuffer[(y*_Width+x)*3+1];
				}
			}
			DynamicCircleWidth = 0;
			DynamicCircleHeight = 0;

			Do(pGrayDynamicBuffer,_Width,_Height,DynamicCircleWidth,DynamicCircleHeight);

			_StaticCircleWidth = StaticCircleWidth;
			_StaticCircleHeight = StaticCircleHeight;

			_OISOFFCircleWidth  = OISOFFCircleWidth;
			_OISOFFCircleHeight = OISOFFCircleHeight;

			_DynamicCircleWidth = DynamicCircleWidth;
			_DynamicCircleHeight = DynamicCircleHeight;

			if (abs(_DynamicCircleWidth - _StaticCircleWidth) > OIS_Threshold)
			{
				bRes = false;
			}
			if (abs(_DynamicCircleHeight - _StaticCircleHeight) > OIS_Threshold)
			{
				bRes = false;
			}

			if (pGrayDynamicBuffer!=nullptr)
			{
				delete[] pGrayDynamicBuffer;
				pGrayDynamicBuffer = nullptr;
			}

			return bRes;
		}

		void OISAnalysis::Do(unsigned char* _pGrayBuffer,int _Width,int _Height,int &_CircleWidth,int &_CircleHeight)
		{
			//二值化找中心（直接使用G Channel）
			//!!
			//double Threshold = 128;
			double XPosition = 0,YPosition = 0;
			double BlockCount = 0;
			for (int y = 0;y < _Height;y++)
			{
				for (int x = 0;x < _Width;x++)
				{
					if (_pGrayBuffer[(y*_Width+x)] < Level_A_Threshold)
					{
						XPosition += x;
						YPosition += y;
						BlockCount++;
					}
				}
			}
			XPosition /= BlockCount;
			YPosition /= BlockCount;

			//以上述中心，ROI特定區域
			//!!
			int ROIWidth = ROISize;
			int ROIHeight = ROISize;
			unsigned char* AnalysisBlock = new unsigned char[ROIWidth*ROIHeight];
			memset(AnalysisBlock,0,sizeof(unsigned char)*ROIWidth*ROIHeight);
			int StartX = int(XPosition) - ROIWidth/2;
			int StartY = int(YPosition) - ROIHeight/2;

			if(StartX <0) StartX = 0;
			if(StartY <0) StartY = 0;

			double Area=0;

			for (int y = 0;y < ROIHeight;y++)
			{
				int Y = StartY+y;
				for (int x = 0;x < ROIWidth;x++)
				{
					int X = StartX+x;
					AnalysisBlock[y*ROIWidth+x] = _pGrayBuffer[(Y*_Width+X)];
				}
			}
			//std::cout << "Start Analysis" << std::endl;
			//分析分析區域（需重新使用閥值）
			//!!
			//Threshold = 236;
			for (int y = 0;y < ROIHeight;y++)
			{
				for (int x = 0;x < ROIWidth;x++)
				{
					if (AnalysisBlock[y*ROIWidth+x] > Level_B_Threshold)
					{
						AnalysisBlock[y*ROIWidth+x] = 255;
					}
					else
					{
						AnalysisBlock[y*ROIWidth+x] = 0;
						Area++;
					}
				}
			}

			//20150108
			//////////////////////////////////////////////////////////////////////////
			ImageDilate(AnalysisBlock,ROIWidth,ROIHeight, 2);
			ImageErode(AnalysisBlock,ROIWidth,ROIHeight, 1);
			ImageDilate(AnalysisBlock,ROIWidth,ROIHeight, 3);
			ImageErode(AnalysisBlock,ROIWidth,ROIHeight, 2);
			//////////////////////////////////////////////////////////////////////////

			//std::cout << "Find edge Analysis" << std::endl;
			// 	int XP = int(XPosition)-StartX;
			// 	int YP = int(YPosition)-StartY;
			//std::cout << "XPosition : " << XP << std::endl;
			//std::cout << "YPosition : " << YP << std::endl;
			//發現上下、左右邊線
			//上
			int TopPositionY = _Height;
			//下
			int BottomPositionY = 0;
			//左
			int LeftPositionX = _Width;
			//右
			int RightPositionX = 0;


			for (int y = 0;y < ROIHeight;y++)
			{
				for (int x = 0;x < ROIWidth;x++)
				{
					if (AnalysisBlock[y*ROIWidth+x] == 0)
					{
						if (TopPositionY > y)
						{
							TopPositionY = y;
						}
						if (BottomPositionY < y)
						{
							BottomPositionY = y;
						}
						if (LeftPositionX > x)
						{
							LeftPositionX = x;
						}
						if (RightPositionX < x)
						{
							RightPositionX = x;
						}
					}
				}
			}

			//std::cout << "TopPosition Y = " << TopPositionY << std::endl;
			//std::cout << "BottomPosition Y = " << BottomPositionY << std::endl;
			//std::cout << "LeftPosition X = " << LeftPositionX << std::endl;
			//std::cout << "RightPosition X = " << RightPositionX << std::endl;

			//計算水平、垂直的量
			//重直距離
			int HLineLength = 0;
			HLineLength = RightPositionX - LeftPositionX;
			int VLineLength = 0;
			VLineLength = BottomPositionY - TopPositionY;

			//std::cout << "HLineLength = " << HLineLength << std::endl;
			//std::cout << "VLineLength = " << VLineLength << std::endl;

			_CircleWidth = HLineLength;
			_CircleHeight = VLineLength;

			CircleRect.left = LeftPositionX + StartX;
			CircleRect.right = RightPositionX + StartX;
			CircleRect.bottom = BottomPositionY +StartY;
			CircleRect.top = TopPositionY +StartY;


			// 	_CircleWidth = Area;
			// 	_CircleHeight = Area;

			if (AnalysisBlock!=nullptr)
			{
				delete[] AnalysisBlock;
				AnalysisBlock = nullptr;
			}
		}

		void OISAnalysis::GetCircleRECT(RECT &_CircleRect)
		{
			_CircleRect = CircleRect;
		}
		//-----------------------------------------------------------------------------
		bool OISAnalysis::SetPostImageValue2(unsigned char *_image,int width,int height, int x, int y, int _value)
		{
			if ((x<0)||(x>= width))  return false;
			if ((y<0)||(y>= height)) return false;

			_image[y*width + x] = _value;

			return true;
		}

		//-----------------------------------------------------------------------------
		void OISAnalysis::ImageDilate(unsigned char *_PostImage, int width,int height, int n)
		{
			int vec_x[9] = {-1,-1,-1, 0, 0, 1, 1, 1, 0};
			int vec_y[9] = {-1, 0, 1,-1, 1,-1, 0, 1, 0};

			unsigned char* _temp = new unsigned char[height*width];
			//memset(_temp,0,sizeof(unsigned char)*height*width);	

			unsigned char* _ptmp;

			for (int k=0; k<n; k++)
			{
				// 		for (int i=0; i<height*width; i++)
				// 		{
				// 			_temp[i] = 0;
				// 		}
				memset(_temp,0,sizeof(unsigned char)*height*width);

				_ptmp = _PostImage;

				for (int y=0; y<height; y++)
				{
					for (int x=0; x<width; x++)
					{
						/*if (_PostImage[y*width + x] > 0)*/
						if((*(_ptmp++)) > 0)
						{
							for (int i=0; i<9; i++)
							{
								SetPostImageValue2(_temp,width,height, x+vec_x[i], y+vec_y[i], 1);
							}					
						}
						//_ptmp++;
					}
				}

				memcpy(_PostImage,_temp,sizeof(unsigned char)*height*width);

				// 		for (int i=0; i<height*width; i++)
				// 		{
				// 			_PostImage[i] = _temp[i];
				// 		}
			}

			if (_temp!=nullptr)
			{
				delete[] _temp;
				_temp = nullptr;
			}
		}

		//-----------------------------------------------------------------------------
		void OISAnalysis::ImageErode(unsigned char *_PostImage, int width,int height, int n)
		{
			int vec_x[9] = {-1,-1,-1, 0, 0, 1, 1, 1, 0};
			int vec_y[9] = {-1, 0, 1,-1, 1,-1, 0, 1, 0};

			unsigned char* _temp = new unsigned char[height*width];
			//memset(_temp,0,sizeof(unsigned char)*height*width);
			
			unsigned char* _ptmp;

			for (int k=0; k<n; k++)
			{
				for (int i=0; i<height*width; i++)
				{
					_temp[i] = 1;
				}

				//memset(_temp,1,sizeof(unsigned char)*height*width);
				_ptmp = _PostImage;

				for (int y=0; y<height; y++)
				{
					for (int x=0; x<width; x++)
					{
						/*if (_PostImage[y*width + x] == 0)*/
						if((*(_ptmp++)) == 0)
						{
							for (int i=0; i<9; i++)
							{
								SetPostImageValue2(_temp,width,height, x+vec_x[i], y+vec_y[i], 0);
							}
						}
						//_ptmp++;
					}
				}

				// 		for (int i=0; i<height*width; i++)
				// 		{
				// 			_PostImage[i] = _temp[i];
				// 		}
				memcpy(_PostImage,_temp,sizeof(unsigned char)*height*width);
			}

			if (_temp!=nullptr)
			{
				delete[] _temp;
				_temp = nullptr;
			}
		}

		//-----------------------------------------------------------------------------
		void UcharToMat(cv::Mat &mSrc, UCHAR *_RGBImage_ori, int iType, UINT _Width, UINT _Height);
		void GetThick(cv::Mat msrc, cv::Rect inRect, int mode, double &_thr, UINT percent, UINT offset);
		void GetHull(cv::Mat msrc, int &tLine, int sType);
		void GetPeaks(std::vector<std::vector<int>> &vec, int &high, int &low);
		void GetRangeThld(std::vector<std::vector<int>> vec, int &high, int &low, int right, int left, int percent, int offset);
		void DrawWhite(cv::Mat mgry, cv::Mat &mdrw, int tPixel);
		void MatToUchar(cv::Mat mSrc, UCHAR *_RGBImage_out, int iType);
		//-----------------------------------------------------------------------------
		void OISAnalysis::GetImageThreshold(unsigned char *_RGBImage_ori, int iType, int _inWidth, int _inHeight, unsigned char *_RGBImage_ok, double &_aThr, UINT _percent, UINT _offset)
		{
			cv::Mat mSrc;
			if(iType==3)
				mSrc.create(_inHeight, _inWidth, CV_8UC3);
			else if(iType==1)
				mSrc.create(_inHeight, _inWidth, CV_8UC1);
	
			UcharToMat(mSrc, _RGBImage_ori, iType, _inWidth, _inHeight);

			//運算於此
			//*************************
			//-- Set calculate ROI
			float rFactor = 10;
			int rShift = int(mSrc.cols / rFactor);
			int rX = int((mSrc.cols/2)-rShift);

			//分上半與下半計算
			cv::Rect upRect = cv::Rect( rX, 0, rShift*2, mSrc.rows/2);
			cv::Rect lowRect = cv::Rect( rX, mSrc.rows/2, rShift*2, mSrc.rows/2);
			GetThick(mSrc, upRect, 1, _aThr, _percent, _offset);   //1 for up

			wchar_t m_reportFileName[256];
			swprintf_s(m_reportFileName, L"%f", _aThr);
			wprintf(L"%s\n", m_reportFileName);
	
			double nDouble = 0;
			int nInt = 0;
			int nPer = 0;
			GetThick(mSrc, lowRect, 2, nDouble, nPer, nInt);  //2 for low

			//************************

			//如果要回傳繪圖
			if(_RGBImage_ok!=NULL)
				MatToUchar(mSrc, _RGBImage_ok, iType);
		
		}

		void UcharToMat(cv::Mat &mSrc, UCHAR *_RGBImage_ori, int iType, UINT _Width, UINT _Height)
		{
			if(iType==3)
			{
				cv::Mat mRGB(_Height, _Width, CV_8UC3);

				for(int j=0; j<mRGB.rows; j++)
				{
					for(int i=0; i<mRGB.cols; i++)
					{
						mRGB.ptr<cv::Vec3b>(j)[i][2] = *_RGBImage_ori++; //R
						mRGB.ptr<cv::Vec3b>(j)[i][1] = *_RGBImage_ori++; //G
						mRGB.ptr<cv::Vec3b>(j)[i][0] = *_RGBImage_ori++; //B
					}

				}

				mRGB.copyTo(mSrc);

			}
			else if(iType==1)
			{
				cv::Mat mGry(_Height, _Width, CV_8UC1);

				for(int j=0; j<mGry.rows; j++)
				{
					for(int i=0; i<mGry.cols; i++)
					{
						mGry.at<uchar>(j,i) = *_RGBImage_ori++;
					}

				}

				mGry.copyTo(mSrc);
			}

		}

		void GetThick(cv::Mat msrc, cv::Rect inRect, int mode, double &_thr, UINT percent, UINT offset)
		{
			cv::Mat mCut = msrc(inRect);
			cv::Mat mGry;
			//cvtColor(mCut, mGry, cv::COLOR_BGR2GRAY);
			mGry = mCut;

			//-- Histogram
			cv::vector<cv::vector<int>> hVec(256);
			for(int j=0; j<mGry.rows; j++)
			{
				for(int i=0; i<mGry.cols; i++)
				{
					int val = mGry.at<uchar>(j,i);
					hVec[val].push_back(1);
				}
			}

			//-- Get max & second peak, define to right or left (or, left right)
			int right_idx = 0; //right peak
			int left_idx = 0; //left peak
			GetPeaks(hVec, right_idx, left_idx);

			//-- Get range between two peaks
			int hThr = 0; //high(right) of range-two peaks
			int lThr = -99; //low(left) of range-two peaks
			GetRangeThld(hVec, hThr, lThr, right_idx, left_idx, percent, offset);

			//給OIS_Pixel_Dist試驗
			if(hThr!=0 || lThr!=(-99))
			{
				if(hThr>lThr)
					_thr = hThr;
				else
					_thr = lThr;
			}

			//-- Draw testing
			//Mat mDrw; //mat for drawing
			//mCut.copyTo(mDrw);
			//DrawMat(mGry, mDrw, hThr, lThr); //畫出pixel分布
			/*
			//-- Draw white (high low) for convex hull
			cv::Mat mHigh(mCut.rows, mCut.cols, CV_8UC1);
			mHigh.setTo(cv::Scalar(0)); 
			cv::Mat mLow(mCut.rows, mCut.cols, CV_8UC1);
			mLow.setTo(cv::Scalar(0)); 
			DrawWhite(mGry, mHigh, hThr);
			DrawWhite(mGry, mLow, lThr);

			int yUp = 0;//找到的上中線
			int yLow= 0; //找到的下中線

			if(mode==1)
			{
				GetHull(mHigh, yUp, 1);
				GetHull(mLow, yLow, 2);
			}
			else if(mode==2)
			{
				GetHull(mHigh, yLow, 2);
				GetHull(mLow, yUp, 1);
			}

			return fabs(float(yUp-yLow));
			*/
		}

		void MatToUchar(cv::Mat mSrc, UCHAR *_RGBImage_out, int iType)
		{
			if(iType==3)
			{
				for(int j=0; j<mSrc.rows; j++)
				{
					for(int i=0; i<mSrc.cols; i++)
					{
						*_RGBImage_out++ = mSrc.ptr<cv::Vec3b>(j)[i][2]; //R
						*_RGBImage_out++ = mSrc.ptr<cv::Vec3b>(j)[i][1]; //G
						*_RGBImage_out++ = mSrc.ptr<cv::Vec3b>(j)[i][0]; //B
					}
				}
			}
			else if(iType==1)
			{
				for(int j=0; j<mSrc.rows; j++)
				{
					for(int i=0; i<mSrc.cols; i++)
					{
						*_RGBImage_out++ = mSrc.at<uchar>(j,i);
					}
				}
			}
		}

		void GetHull(cv::Mat msrc, int &tLine, int sType) //type=1: 上半部, type=2: 下半部
		{
			//imshow("msrc",msrc);waitKey();
			int thresh = 100;
			int max_thresh = 255;
			cv::Mat threshold_output;
			cv::vector<cv::vector<cv::Point> > contours;
			cv::vector<cv::Vec4i> hierarchy;

			//-- Detect edges
			threshold( msrc, threshold_output, thresh, 255, CV_THRESH_BINARY );

			//-- Find contours
			findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

			if(contours.size()>0)
			{
				if(sType==1)//上半部, 找最低y
				{
					tLine = contours[0][0].y;
				}
				else if(sType==2)//下半部, 找最高y
				{
					tLine = contours[contours.size()-1][0].y;
				}
			}

		}

		void GetPeaks(std::vector<std::vector<int>> &vec, int &high, int &low)
		{
			//-- Erase empty element from the end
			for(int i=vec.size()-1; i>0; i--)
			{
				if(vec[i].size()==0)
				{
					vec.erase(vec.begin()+i);
				}
				else if(vec[i].size()!=0)
					break;
			}

			//-- size 0的idx給予1, 避免中間的空數
			for(int unsigned i=0; i<vec.size(); i++)
			{
				if(vec[i].size()==0)
				{
					vec[i].push_back(1);
				}
			}

			int mIdx = vec.size()/2; //middle idx of two peaks

			high = distance(vec.begin(), max_element(vec.begin(), vec.end()));//最高峰之索引值

			int low1 = 0; //最高峰往右找的最高峰
			int low2 = 0; //最高峰往左找的最高峰
			UINT temp = 0;
			int vSize = vec.size();

			int srhRight = 0; //往右搜尋起點
			if(high+mIdx >= vSize)
			{
				srhRight = (vec.size()-high)/2+high;
			}
			else
				srhRight = high+mIdx;

			for(int unsigned i=srhRight; i<vec.size(); i++) //由最高峰往右找
			{
				vec[i].size();
				if(vec[i].size()>temp)
				{
					temp = vec[i].size();
					low1 = i;
				}
			}
			low1;

			int srhLeft = 0; //往左搜尋起點
			if(high-mIdx <= vSize)
			{
				srhLeft = high/2;
			}
			else
				srhLeft = high-mIdx;

			temp = 0;
			for(int i=srhLeft; i>0; i--)//由最高峰往左找
			{
				if(vec[i].size()>temp)
				{
					temp = vec[i].size();
					low2 = i;
				}
			}
			low2;

			vec[low1];
			vec[low2];

			if(vec[low1] > vec[low2])
				low = low1;
			else
				low = low2;

			if(high<low)
			{
				temp = high;
				high = low;
				low = temp;
			}

		}

		void GetRangeThld(std::vector<std::vector<int>> vec, int &high, int &low, int right, int left, int percent, int offset)
		{
			//-- Get valley
			int min_idx = 0; //valley(min) between max peak and second peak
			min_element(vec.begin()+left, vec.begin()+right);
			min_idx = distance(vec.begin(), min_element(vec.begin()+left, vec.begin()+right));

			int rRange = int((right-left)*percent/100.0+left);

			double tWSlope = 1.5;
			double tBSlope = 1.3;
			std::vector<float> vTemp;

			//-- Sarching for right slope
			for(UINT i=min_idx+1; i<vec.size(); i++)
			{
				float slope = 0.0;
				slope = ( (float)vec[i].size()-(float)vec[min_idx].size() ) / (float)( (i)-(min_idx) ); //當前點與min_idx之斜率

				if(!_isnan(slope))
				{

					vTemp.push_back(slope);
					if(vTemp.size()>=2)
					{
						if(vTemp.size()==5)
						{
							high = i;
							if( high>rRange ) //找到的high 必須在range內
							{
								high = i-offset;
								break;
							}
						}
						if(vTemp.size()==2)
						{
							if((float)vTemp[vTemp.size()-2]>=slope)
								vTemp.erase(vTemp.begin(), vTemp.begin()+vTemp.size()-1);
						}
						else
						{
							if((float)vTemp[vTemp.size()-2]*tWSlope>=slope)
								vTemp.erase(vTemp.begin(), vTemp.begin()+vTemp.size()-1);
						}
					}
				}
				else
					vTemp.clear();
			}
			vTemp.clear();

			//-- Sarching for left slope
			for(int i=min_idx; i>0; i--)
			{
				float slope = 0.0;
				slope = ( (float)vec[i].size()-(float)vec[min_idx].size() ) / (float)( (i)-(min_idx) ); //當前點與min_idx之斜率

				if(!_isnan(slope))
				{
					vTemp.push_back(slope);
					if(vTemp.size()>=2)
					{
						if(vTemp.size()==5)
						{
							low = i;
							if( abs(low-left)<10) //若找到的low 距離left peak大於10(dn), 繼續往下找
								break;
						}
						if((float)vTemp[vTemp.size()-2]*tBSlope<slope)
							vTemp.erase(vTemp.begin(), vTemp.begin()+vTemp.size()-1);
					}
				}
				else
					vTemp.clear();
			}
		}

		void DrawWhite(cv::Mat mgry, cv::Mat &mdrw, int tPixel)
		{
			IplImage *iSrc = &IplImage(mdrw);
			IplImage *iDet = &IplImage(mgry);

			int dPoint = 0; //若當前tPixel沒紀錄(dPoint==0), 則遞減sampling pixel value(tPixel)
			for(int k=0; k<4; k++)
			{
				if(dPoint==0)
				{
					for(int i=0; i<iDet->height; i++)
						for(int j=0; j<iDet->width; j++)
						{
							CvScalar mpG;
							mpG = cvGet2D(iDet, i, j);

							CvScalar mpD;
							mpD = cvGet2D(iSrc, i, j);

							if(mpG.val[0]==tPixel-k)
							{
								mpD.val[0] = 255; //B
								dPoint++;
							}
							cvSet2D(iSrc, i, j, mpD);
						}
				}
			}
		}
	}
}