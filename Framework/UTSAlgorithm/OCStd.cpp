#include "StdAfx.h"
#include "Algorithm.h"
#include "ImageFile.h"
#include <list>

using namespace UTS::Algorithm::Image;


namespace UTS
{
    namespace Algorithm
    {
        namespace OC
        {
            namespace OC_Std
            {
                typedef struct Block_Member
                {
                    int x;
                    int y;
                    int width;
                    int height;
                    double value;
                } Block_Member;


                void OpticalCenter(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    POINT &ptCenter)
                {
                    //-------------------------
                    ptCenter.x = -1;
                    ptCenter.y = -1;

                    //	?算閥值
                    //圈選特定ROI
                    Block_Member ROI[5] = {0};

                    for (int i=0;i<5;i++)
                    {
                        ROI[i].width = 100;
                        ROI[i].height = 100;
                    }

                    int Diatance = min(nWidth,nHeight)-100;
                    int CenterX = nWidth/2;
                    int CenterY = nHeight/2;

                    ROI[0].x = CenterX - 50;
                    ROI[0].y = CenterY - Diatance/2 - 50;

                    ROI[1].x = CenterX + Diatance/2 - 50;
                    ROI[1].y = CenterY - 50;

                    ROI[2].x = CenterX - 50;
                    ROI[2].y = CenterY + Diatance/2 - 50;

                    ROI[3].x = CenterX - Diatance/2 - 50;
                    ROI[3].y = CenterY -50;

                    ROI[4].x = CenterX - 50;
                    ROI[4].y = CenterY - 50;

                    //double* _pTmp = new unsigned char[100*100];
                    for (int i=0;i<5;i++)
                    {
                        double AvgY = 0;
                        for (int y=0;y<100;y++)
                        {
                            for (int x=0;x<100;x++)
                            {
                                int X = ROI[i].x+x;
                                int Y = ROI[i].y+y;

                                AvgY += pYBuffer[Y*nWidth+X];
                            }
                        }
                        AvgY /= 10000.0;

                        ROI[i].value = AvgY;
                    }

                    //Threshold = (C+((E1+E2+E3+E4)/4))/2

                    double Threahold = (ROI[4].value+(ROI[0].value+ROI[1].value+ROI[2].value+ROI[3].value)/4.0)/2.0;

                    //二值化
                    unsigned char *BinaryBuffer = new unsigned char[nWidth*nHeight];
                    memset(BinaryBuffer,0,sizeof(unsigned char)*nWidth*nHeight);

                    for (int y=0;y<nHeight;y++)
                    {
                        for (int x=0;x<nWidth;x++)
                        {
                            int _tValue = pYBuffer[y*nWidth+x];
                            if (_tValue >  Threahold)
                            {
                                BinaryBuffer[y*nWidth+x] = 1;
                            }

                        }
                    }

                    //發現質心
                    double Centroid_X = 0;
                    double Centroid_Y = 0;
                    double CentroidCount = 0;
                    unsigned char *_pTmp = BinaryBuffer;

                    int MaxX =0;
                    int MinX = nWidth-1;
                    int MaxY = 0;
                    int MinY = nHeight-1;

                    for (int y=0;y<nHeight;y++)
                    {
                        for (int x=0;x<nWidth;x++)
                        {
                            if ((*(_pTmp++))  == 1)
                            {
                                Centroid_X += x;
                                Centroid_Y += y;
                                CentroidCount++;

                                if (MaxX < x)
                                {
                                    MaxX = x;
                                }
                                if (MinX > x)
                                {
                                    MinX = x;
                                }
                                if (MaxY < y)
                                {
                                    MaxY = y;
                                }
                                if (MinY > y)
                                {
                                    MinY = y;
                                }
                            }					
                        }
                    }

                    RELEASE_ARRAY(BinaryBuffer);

                    if (CentroidCount!=0)
                    {
                        Centroid_X /= CentroidCount;
                        Centroid_Y /= CentroidCount;
                    }
                    else
                    {
                        Centroid_X = 0;
                        Centroid_Y = 0;
                    }

                    if ((MaxX == nWidth-1)||(MinX == 0)||(MaxY == nHeight-1)||(MinY == 0))
                    {
                        ptCenter.x = -1;
                        ptCenter.y = -1;
                    }
                    else
                    {
                        ptCenter.x =(int) Centroid_X;
                        ptCenter.y =(int) Centroid_Y;
                    }
                }   // Cal_OpticalCenter

                void OpticalCenterRGB(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    POINT &ptCenter)
                {
                    //-------------------------------------------------------------------------
                    // RGB buffer => Y buffer
                    unsigned char *pYBuffer = new unsigned char[nWidth * nHeight];
                    memset(pYBuffer, 0, nWidth * nHeight);
                    CImageProc::GetInstance().Cal_RGBtoYBuffer(pBmpBuffer, nWidth, nHeight, pYBuffer);

                    //-------------------------------------------------------------------------
                    // Call Y buffer RI Algorithm
                    OpticalCenter(pYBuffer, nWidth, nHeight, ptCenter);
                    RELEASE_ARRAY(pYBuffer);

                    //-------------------------------------------------------------------------
                    // 将结果上下翻转
                    ptCenter.y = nHeight - ptCenter.y;
                }

				unsigned char GetNOrderValue(const unsigned char* const ValueArray, const int &ArrayLength,const int &OrderNum)
				{
					std::list <unsigned char> c1;
					std::list <unsigned char>::iterator c1_Iter;

					c1.assign(ValueArray,ValueArray+ArrayLength);
					c1.sort( std::greater<unsigned char>() );

					unsigned char A;
					int i=0;	
					for (c1_Iter =  c1.begin();c1_Iter != c1.end();c1_Iter++)
					{
						if (i == OrderNum-1)
						{
							A = *c1_Iter;
							break;
						}
						i++;
					}	
					return A;
				}

				void Histogram(unsigned char* const ValueArray,long *HistogramArray,int ArrayLength)
				{
					unsigned char value = 0 ;
					for (int i = 0 ;i < ArrayLength ;i++)
					{
						value = ValueArray[i];
						HistogramArray[value] ++;
					}
				}

				int GetHistogramH(long *HistogramArray,int OrderNum)
				{
					long TotalCount = 0;
					int i = 255;
					for (i = 255 ;i >= 0 ;i--)
					{
						TotalCount += HistogramArray[i];

						if(TotalCount > OrderNum) break;
					}

					return i;
				}


				void OpticalCenterRAW(
					unsigned char *_pRAWImage,
					int nWidth,
					int nHeight,
					int rawBayerType,
					POINT *ptCenter)
				{
					int RawWidth = nWidth/2;
					int RawHeight = nHeight/2;
					long *HistogramArray = new long[256];

					unsigned char* Plane1	= new unsigned char[RawWidth*RawHeight];
					memset(Plane1,0,sizeof(unsigned char)*RawWidth*RawHeight);
					unsigned char* Plane2	= new unsigned char[RawWidth*RawHeight];
					memset(Plane2,0,sizeof(unsigned char)*RawWidth*RawHeight);
					unsigned char* Plane3	= new unsigned char[RawWidth*RawHeight];
					memset(Plane3,0,sizeof(unsigned char)*RawWidth*RawHeight);
					unsigned char* Plane4	= new unsigned char[RawWidth*RawHeight];
					memset(Plane4,0,sizeof(unsigned char)*RawWidth*RawHeight);

					for (int y=0;y<RawHeight;y++)
					{
						for (int x=0;x<RawWidth;x++)
						{
							Plane1[y*RawWidth+x]	= _pRAWImage[2*y*nWidth+2*x];
							Plane2[y*RawWidth+x]	= _pRAWImage[2*y*nWidth+(2*x+1)];
							Plane3[y*RawWidth+x]	= _pRAWImage[(2*y+1)*nWidth+2*x];
							Plane4[y*RawWidth+x]	= _pRAWImage[(2*y+1)*nWidth+(2*x+1)];
						}
					}

					///unsigned char Top10_Plane1 =  GetNOrderValue(Plane1, int(RawWidth*RawHeight),int(RawWidth*RawHeight*0.1));
					//unsigned char Top10_Plane2 =  GetNOrderValue(Plane2, int(RawWidth*RawHeight),int(RawWidth*RawHeight*0.1));
					//unsigned char Top10_Plane3 =  GetNOrderValue(Plane3, int(RawWidth*RawHeight),int(RawWidth*RawHeight*0.1));
					//unsigned char Top10_Plane4 =  GetNOrderValue(Plane4, int(RawWidth*RawHeight),int(RawWidth*RawHeight*0.1));

					memset(HistogramArray,0,sizeof(long)*256);
					Histogram(Plane1,HistogramArray,RawWidth*RawHeight);
					unsigned char Top10_Plane1 = GetHistogramH(HistogramArray,int(RawWidth*RawHeight*0.1));
					memset(HistogramArray,0,sizeof(long)*256);
					Histogram(Plane2,HistogramArray,RawWidth*RawHeight);
					unsigned char Top10_Plane2 = GetHistogramH(HistogramArray,int(RawWidth*RawHeight*0.1));
					memset(HistogramArray,0,sizeof(long)*256);
					Histogram(Plane3,HistogramArray,RawWidth*RawHeight);
					unsigned char Top10_Plane3 = GetHistogramH(HistogramArray,int(RawWidth*RawHeight*0.1));
					memset(HistogramArray,0,sizeof(long)*256);
					Histogram(Plane4,HistogramArray,RawWidth*RawHeight);
					unsigned char Top10_Plane4 = GetHistogramH(HistogramArray,int(RawWidth*RawHeight*0.1));

					double sumS_Plane1 = 0 , sumS_Plane2 = 0 ,sumS_Plane3 = 0 ,sumS_Plane4 = 0 ;
					double sumXS_Plane1 = 0 , sumXS_Plane2 = 0 ,sumXS_Plane3 = 0 , sumXS_Plane4 = 0;
					double sumYS_Plane1 = 0 , sumYS_Plane2 = 0 ,sumYS_Plane3 = 0 , sumYS_Plane4 = 0;

					for (int y=0;y<RawHeight;y++)
					{
						for (int x=0;x<RawWidth;x++)
						{
							if (Plane1[y*RawWidth+x] > Top10_Plane1)
							{
								sumS_Plane1 += Plane1[y*RawWidth+x];
								sumXS_Plane1 += (2*x * Plane1[y*RawWidth+x]);
								sumYS_Plane1 += (2*y * Plane1[y*RawWidth+x]);
							}

							if (Plane2[y*RawWidth+x] > Top10_Plane2)
							{
								sumS_Plane2 += Plane2[y*RawWidth+x];
								sumXS_Plane2 += ((2*x+1) * Plane2[y*RawWidth+x]);
								sumYS_Plane2 += (2*y * Plane2[y*RawWidth+x]);
							}

							if (Plane3[y*RawWidth+x] > Top10_Plane3)
							{
								sumS_Plane3 += Plane3[y*RawWidth+x];
								sumXS_Plane3 += (2*x * Plane3[y*RawWidth+x]);
								sumYS_Plane3 += ((2*y+1) * Plane3[y*RawWidth+x]);
							}

							if (Plane4[y*RawWidth+x] > Top10_Plane4)
							{
								sumS_Plane4 += Plane4[y*RawWidth+x];
								sumXS_Plane4 += ((2*x+1) * Plane4[y*RawWidth+x]);
								sumYS_Plane4 += ((2*y+1) * Plane4[y*RawWidth+x]);
							}
						}
					}

					//ptCenter[0]:R, ptCenter[1]:Gr, ptCenter[2]:Gb, ptCenter[3]:B
					switch (rawBayerType)
					{
					case 1://B Gb Gr R
						ptCenter[0].x = int(sumXS_Plane4/sumS_Plane4);
						ptCenter[0].y = int(sumYS_Plane4/sumS_Plane4);
						ptCenter[1].x = int(sumXS_Plane3/sumS_Plane3);
						ptCenter[1].y = int(sumYS_Plane3/sumS_Plane3);
						ptCenter[2].x = int(sumXS_Plane2/sumS_Plane2);
						ptCenter[2].y = int(sumYS_Plane2/sumS_Plane2);
						ptCenter[3].x = int(sumXS_Plane1/sumS_Plane1);
						ptCenter[3].y = int(sumYS_Plane1/sumS_Plane1);
						break;

					case 2://R Gr Gb B
						ptCenter[0].x = int(sumXS_Plane1/sumS_Plane1);
						ptCenter[0].y = int(sumYS_Plane1/sumS_Plane1);
						ptCenter[1].x = int(sumXS_Plane2/sumS_Plane2);
						ptCenter[1].y = int(sumYS_Plane2/sumS_Plane2);
						ptCenter[2].x = int(sumXS_Plane3/sumS_Plane3);
						ptCenter[2].y = int(sumYS_Plane3/sumS_Plane3);
						ptCenter[3].x = int(sumXS_Plane4/sumS_Plane4);
						ptCenter[3].y = int(sumYS_Plane4/sumS_Plane4);
						break;

					case 3://Gb B R Gr
						ptCenter[0].x = int(sumXS_Plane3/sumS_Plane3);
						ptCenter[0].y = int(sumYS_Plane3/sumS_Plane3);
						ptCenter[1].x = int(sumXS_Plane4/sumS_Plane4);
						ptCenter[1].y = int(sumYS_Plane4/sumS_Plane4);
						ptCenter[2].x = int(sumXS_Plane1/sumS_Plane1);
						ptCenter[2].y = int(sumYS_Plane1/sumS_Plane1);
						ptCenter[3].x = int(sumXS_Plane2/sumS_Plane2);
						ptCenter[3].y = int(sumYS_Plane2/sumS_Plane2);
						break;

					case 4://Gr R B Gb
						ptCenter[0].x = int(sumXS_Plane2/sumS_Plane2);
						ptCenter[0].y = int(sumYS_Plane2/sumS_Plane2);
						ptCenter[1].x = int(sumXS_Plane1/sumS_Plane1);
						ptCenter[1].y = int(sumYS_Plane1/sumS_Plane1);
						ptCenter[2].x = int(sumXS_Plane4/sumS_Plane4);
						ptCenter[2].y = int(sumYS_Plane4/sumS_Plane4);
						ptCenter[3].x = int(sumXS_Plane3/sumS_Plane3);
						ptCenter[3].y = int(sumYS_Plane3/sumS_Plane3);
						break;
					}

					RELEASE_ARRAY(Plane1);
					RELEASE_ARRAY(Plane2);
					RELEASE_ARRAY(Plane3);
					RELEASE_ARRAY(Plane4);
					RELEASE_ARRAY(HistogramArray);
				}
            }   // OC_Std
        } // OC
    }   // Algorithm
}   // UTS
