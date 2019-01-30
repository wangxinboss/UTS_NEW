#include "StdAfx.h"
#include "Algorithm.h"
#include "SFR_FuncDll.h"
#include "CtSFR3.h"

using namespace UTS::Algorithm::Image;

namespace UTS
{
    namespace Algorithm
    {
        namespace SFR
        {
            namespace SfrCalc
            {
				static double dividevar1,dividevar2;
				#define DIVIDE(a,b) (dividevar1 = (a),(dividevar2 = (b)) == 0.0 ? (dividevar1*10000):((dividevar1)/(dividevar2)))

				//-----------------------------------------------------
				void ahamming(int _nArraySize,double _Midpoint,double* _AhammingData)
				{
					memset(_AhammingData,0,sizeof(double)*_nArraySize);
					double wid1 = _Midpoint-1;
					double wid2 = double(_nArraySize)-_Midpoint;
					double wid = max(wid1, wid2);
					for (int i=0;i<_nArraySize;i++)
					{
						double arg = double(i+1)-_Midpoint;
						_AhammingData[i] = cos(PI*arg/(wid));
						_AhammingData[i] = 0.54 + 0.46*_AhammingData[i];
					}
				}

				//--------------------------------------------------------------------------
                //static double dividevar1,dividevar2;
                //#define DIVIDE(a,b) (dividevar1 = (a),(dividevar2 = (b)) == 0.0 ? (dividevar1*10000):((dividevar1)/(dividevar2)))
                void rotatev2(double* a,int &nlin, int &npix, bool &bflag)
                {
                    /*
                    nn = 3;  % Limits test area. Normally not a problem.
                    %Compute v, h ranges
                    testv = abs(mean(a(end-nn,:,mm))-mean(a(nn,:,mm)));
                    testh = abs(mean(a(:,end-nn,mm))-mean(a(:,nn,mm)));
                    */
                    int width = nlin;
                    int height = npix;
                    int nn=3;
                    double testv = 0;
                    double testh = 0;
                    double TVMean = 0;
                    double LVMean = 0;
                    double LHMean = 0;
                    double RHMean = 0;

                    for (int x=0;x<width;x++)
                    {
                        LVMean += (a[(height-1-nn)*width+x]);
                        TVMean += (a[(nn)*width+x]);
                    }
                    LVMean /= double(width);
                    TVMean /= double(width);

                    for (int y=0;y<height;y++)
                    {
                        LHMean += (a[y*width+(width-1-nn)]);
                        RHMean += (a[y*width+nn]);
                    }
                    LHMean /= double(height);
                    RHMean /= double(height);
                    testv = abs(LVMean-TVMean);
                    testh = abs(LHMean-RHMean);
                    /*
                    rflag =0;
                    if testv > testh
                    rflag =1;
                    a = rotate90(a);
                    temp=nlin;
                    nlin = npix;
                    npix = temp;
                    */
                    bflag = false;
                    if (testv >testh)
                    {		
                        bflag = true;
                        int tmp = nlin;
                        nlin = npix;
                        npix = tmp;
                        if((nlin == 0)||(npix ==0))
                        {
                            //foundation::DebugInfo(L"Width = %d,Height = %d",nlin,npix);
                        }

                        double* tmpBuffer = new double[nlin*npix];
                        for (int y=0;y<nlin;y++)
                        {
                            int X = y;
                            for (int x=0;x<npix;x++)
                            {
                                int Y= x;
                                tmpBuffer[y*npix+x] = a[Y*nlin+X];
                            }
                        }
                        for (int y=0;y<nlin;y++)
                        {
                            for (int x=0;x<npix;x++)
                            {
                                a[y*npix+x] = tmpBuffer[(nlin-1-y)*npix+x/*(npix-1-x)*/];
                            }
                        }

                        delete[] tmpBuffer;
                        tmpBuffer = 0;
                    }
                }
				//-----------------------------------------------------
				void conv(double* _pFilter,int _nFileSize,double* _pDataArray,int _nDataSize,double* _pConvData)
				{	
					//w(k) = u(j)*v(k-j+1)
					// 	if (_nFileSize%2 == 0)
					// 	{
					for (int k=0;k<_nDataSize;k++)
					{
						_pConvData[k] = 0;
						for (int j=0;j<_nFileSize;j++)
						{
							if (k-j >=0)
							{
								_pConvData[k] += _pFilter[j]*_pDataArray[k-j];
							}				
						}
					}
					if (_nFileSize == 2)
					{
						_pConvData[0] = _pConvData[1];
					}
					if (_nFileSize == 3)
					{
						_pConvData[0] = 0;
						_pConvData[1] = _pConvData[2];
					}
				}
				//-----------------------------------------------------
				void deriv1(double* _pDataArray,int _nLine,int _nPixels,double* _pFilter,int _nFileSize,double* _pDeriv1Data)
				{
					double* tmpArray = new double[_nPixels];
					memset(tmpArray,0,sizeof(double)*_nPixels);

					for (int i=0;i<_nLine;i++)
					{
						// conv(double* _pFilter,int _nFileSize,double* _pDataArray,int _nDataSize,double* _pConvData)
						conv(_pFilter,_nFileSize,&(_pDataArray[i*_nPixels]),_nPixels,tmpArray);
						for (int j=0;j<_nPixels;j++)
						{
							_pDeriv1Data[i*_nPixels+j] = tmpArray[j];
						}
						//_pDeriv1Data[i*_nPixels+0] = tmpArray[1];
					}

					delete[] tmpArray;
					tmpArray = 0;
				}

				//-----------------------------------------------------
				double centroid(double* _Vector,int _nVectorSize)
				{
					double* n =new double[_nVectorSize];
					memset(n,0,sizeof(double)*_nVectorSize);
					double sumx = 0;
					for (int i=0;i<_nVectorSize;i++)
					{
						n[i] = i+1;
						sumx += _Vector[i];
					}
					double loc = 0;
					if (sumx <= 1/10000)
					{
						loc = 0;		
					}
					else
					{
						double sumnx = 0;
						for (int i=0;i<_nVectorSize;i++)
						{
							sumnx += n[i]*_Vector[i];
						}
						loc = DIVIDE(sumnx,sumx);
					}
					delete[] n;
					n=0;
					return loc;
				}
				//-----------------------------------------------------
				void fir2fix(int _FrequencyDataLength,int _FilterSize,double* _CorrectionFilter)
				{
					double n = double(_FrequencyDataLength);
					_CorrectionFilter[0] = 1.0;
					double m = double(_FilterSize-1);
					double scale = 1.0;

					for (int i=1;i<_FrequencyDataLength;i++)
					{
						_CorrectionFilter[i] = abs((PI*double(i+1)*m/(2.0*(n+1.0))) / sin(PI*double(i+1)*m/(2.0*(n+1.0))));
						_CorrectionFilter[i] = 1 + scale*(_CorrectionFilter[i]-1);
						if (_CorrectionFilter[i] > 10)
						{
							_CorrectionFilter[i] = 10;
						}
					}
				}
				//-----------------------------------------------------
				typedef struct AtCoordinate
				{
				public:
					double x;	//!< X畒夹
					double y;	//!< Y畒夹
				}
				AtCoordinate, AtPoint, AtVector;

				//-----------------------------------------------------
				bool LeastSquareLinearRegression(const AtPoint* const _PointSet, int _PointCount,double &_Slope,double &_Intercept)
				{
					//	﹍块挡狦
					bool bRes = true;
					_Slope = 0;
					_Intercept = 0;

					if (_PointSet == NULL || _PointCount == 0)
					{	
						bRes = false;
						return bRes;
					}

					double SUMx = 0;		//sum of x values
					double SUMy = 0;		//sum of y values
					double SUMxy = 0;		//sum of x * y
					double SUMxx = 0;		//sum of x^2
					double AVGy = 0;		//mean of y
					double AVGx = 0;		//mean of x

					//calculate various sums 
					for (int i = 0; i < _PointCount; i++)
					{
						//sum of x
						SUMx = SUMx + (_PointSet + i)->x;
						//sum of y
						SUMy = SUMy + (_PointSet + i)->y;
						//sum of squared x*y
						SUMxy = SUMxy + (_PointSet + i)->x * (_PointSet + i)->y;
						//sum of squared x
						SUMxx = SUMxx + (_PointSet + i)->x * (_PointSet + i)->x;
					}
					//calculate the means of x and y
					AVGy = SUMy / _PointCount;
					AVGx = SUMx / _PointCount;

					//slope or a1
					_Slope = (_PointCount * SUMxy - SUMx * SUMy) / (_PointCount * SUMxx - SUMx*SUMx);

					//y itercept or a0
					_Intercept = AVGy - _Slope * AVGx;

					return bRes;

				}
				void findedge(double* _CentroidArray,int _nArraySize,double &_Slope,double &_Intercept)
				{
					AtPoint* AnalysisArray = new AtPoint[_nArraySize];
					for (int i=0;i<_nArraySize;i++)
					{
						AnalysisArray[i].x = double(i);
						AnalysisArray[i].y = _CentroidArray[i];
					}
					LeastSquareLinearRegression(AnalysisArray,_nArraySize,_Slope,_Intercept);
					delete[] AnalysisArray;
					AnalysisArray = 0;
				}
				//-----------------------------------------------------
				void project(double* bb,int nlin,int npix,double loc, double slope, double fac,double* point)
				{
		#define MagicNum 100
 					double big = 0;
 					int nn = npix * int(fac) ;
 					//% smoothing window
 					/*
 					win = ahamming(nn, fac*loc(1, 1));
 
 					slope =  1/slope;
 					offset =  round(  fac*  (0  - (nlin - 1)/slope )   );
 
 					del = abs(offset);
 					if offset>0 offset=0;
 					end
 
 					barray = zeros(2, nn + del+100);
 					*/
 					double* win = new double[nn];
 					memset(win,0,sizeof(double)*nn);
 
 					ahamming(nn,fac*loc,win);
 					delete[] win;
 					win = 0;
 
 					slope = 1.0/slope;
 					double offset =  ROUND( fac*  (0.0  - double(nlin/*npix*/ - 1)/slope ) );
 					double del = abs(offset);
 					if (offset>0) offset=0;
 					
 					double* barray = new double[2*(nn + int(del)+MagicNum)];
 					memset(barray,0,sizeof(double)*2*(nn + int(del)+MagicNum));
 					/*
 					% Projection and binning
 						for n=1:npix;
 						for m=1:nlin;
 						x = n-1;
 						y = m-1;
 						ling =  ceil((x  - y/slope)*fac) + 1 - offset;
 						barray(1,ling) = barray(1,ling) + 1;
 						barray(2,ling) = barray(2,ling) + bb(m,n);
 						end;
 						end;
 
 						point = zeros(nn,1);
 						start = 1+round(0.5*del); 
 					*/
 					for (int n=0;n<npix;n++) //npix = width
 					{
 						for (int m=0;m<nlin;m++)
 						{
 							double y = m;
 							double x = n;
                     
 							int ling =  int(ceil((x - y/slope)*fac) + 1 - offset);
 							barray[0*(nn + int(del)+MagicNum)+ling] += 1;
 							barray[1*(nn + int(del)+MagicNum)+ling] += bb[m*npix+n];
 						}
 					}
 
		  #ifdef _DEBUG
		//               foundation::TextFile _tempFile(L"barray.csv");
		//   
		//               for (int n=0;n<npix;n++)
		//               {
		//                 for (int m=0;m<nlin;m++)
		//                 {
		//                   double x = n;
		//                   double y = m;
		//                   int ling =  int(ceil((x - y/slope)*fac) + 1 - offset);
		//                    _tempFile.Add(barray[1*(nn + int(del)+MagicNum)+ling]);
		//                    _tempFile.Add(L",");
		//                 }
		//                 _tempFile.Add(L"\n");
		//               }
		//               _tempFile.Add(L"\n");
		//               _tempFile.WriteText();
		  #endif
 					//point = zeros(nn,1);
 					int start = int(ROUND((0.5*del))); 
 					/*
 					%*********************************
 					% Check for zero counts
 						nz =0;
 						for i = start:start+nn-1; 
 						% ********************************
 
 						if barray(1, i) ==0;
 						nz = nz +1;
 						status = 0;  
 						if i==1;
 						barray(1, i) = barray(1, i+1);
 						else;
 						barray(1, i) = (barray(1, i-1) + barray(1, i+1))/2;
 						end;
 						end;
 						end;
 					*/
 					int nz = 0;
 					for (int i=start;i<(start+nn+1);i++)
 					{
 						if ((barray[1*(nn + int(del)+MagicNum)+(i)] == 0)&&(barray[0*(nn + int(del)+MagicNum)+(i)] == 0))
 						{
 							nz += 1;
 							int status =0;
 							if (i == 0)
 							{
 								barray[0*(nn + int(del)+MagicNum)+i] = barray[0*(nn + int(del)+MagicNum)+(i+1)];
								barray[1*(nn + int(del)+MagicNum)+i] = barray[1*(nn + int(del)+MagicNum)+(i+1)];
 							}
 							else
 							{
 								barray[0*(nn + int(del)+MagicNum)+i] = (barray[0*(nn + int(del)+MagicNum)+(i-1)] + barray[0*(nn + int(del)+MagicNum)+(i+1)])/2;
								barray[1*(nn + int(del)+MagicNum)+i] = (barray[1*(nn + int(del)+MagicNum)+(i-1)] + barray[1*(nn + int(del)+MagicNum)+(i+1)])/2;
 							}
 						}
 					}

 					/*
 					% Combine in single edge profile, point
 						for i = 0:nn-1; 
 						point(i+1) = barray(2, i+start)/ barray(1, i+start);
 						end;
 					*/
 					for (int i=0;i<nn;i++)
 					{
 						point[i] = DIVIDE(barray[1*(nn + int(del)+MagicNum)+(i+1+start)],barray[0*(nn + int(del)+MagicNum)+(i+1+start)]);
 					}
 
 					delete[] barray;
 					barray = 0;
				}
				//-----------------------------------------------------
				void cent(double* a,int n, double center,double* b)
				{
					memset(b,0,sizeof(double)*n);
					double mid = ROUND(double(n+1.0)/2.0);
					int del = int(ROUND(center-mid));
					if (del > 0)
					{
						for (int i=0;i<n-del/*+1*/;i++)
						{
							b[i] = a[i+del];
						}
					}
					else if(del < 1)
					{
						for (int i=-del;i<n-1;i++)
						{
							b[i+1] = a[i+del+1];
						}
					}
					else
					{
						for (int i=0;i<n;i++)
						{
							b[i] = a[i];
						}
					}
				}

				double interp1(double* _Y,double* _X,int _VectorSize,double _Index)
				{
					double dRes = 0;
					//h = diff(xCol);
					double* h = new double[_VectorSize-1];
					memset(h,0,sizeof(double)*(_VectorSize-1));
					for (int i=0;i<(_VectorSize-1);i++)
					{
						h[i] = _X[i+1]-_X[i];
						if (i!=0)
						{
							if (abs(h[i]-h[i-1]) > 0.00001)
							{
								delete[] h;
								h = 0;
								return dRes;//Fail
							}
						}
					}
					double diff = h[0];
					delete[] h;
					h = 0;
					if (diff==0)
					{
						dRes = 0;
					}
					else
					{
					//	k = min(max(1+floor((xiCol-xCol(1))/h),1),n-1);
					//	т程綼竚	
					int k = int(min(max(floor((_Index-_X[0])/diff),0),(_VectorSize-2)));
					double s = (_Index - _X[k])/diff;
					/*
					for j = 1:prodDs
					yiMat(p,j) = yMat(k,j) + s.*(yMat(k+1,j)-yMat(k,j));
					end
					*/
					double yiMat = 0;
				// 	for (int i=0;i<1;i++)
				// 	{
						yiMat = _Y[k]+s*(_Y[k+1]-_Y[k]);
				/*	}*/
					dRes = yiMat;
					}

					return dRes;
				}

                bool IsOKSFRBlock(double* _pBuffer,int _Width,int _Height)
                {
                    bool bRes= true;
                    double ULMean = 0;
                    double URMean = 0;
                    double BLMean = 0;
                    double BRMean = 0;
                    int analysize = 5;
                    for (int y=0;y<analysize;y++)
                    {
                        for (int x=0;x<analysize;x++)
                        {
                            ULMean += _pBuffer[y*_Width+x];
                            URMean += _pBuffer[y*_Width+((_Width-analysize)+x)];
                            BLMean += _pBuffer[((_Height-analysize)+y)*_Width+x];
                            BRMean += _pBuffer[((_Height-analysize)+y)*_Width+((_Width-analysize)+x)];
                        }
                    }
                    ULMean /= double(analysize*analysize);
                    URMean /= double(analysize*analysize);
                    BLMean /= double(analysize*analysize);
                    BRMean /= double(analysize*analysize);
                    
                    double TestULR = fabs((ULMean-URMean)/MAKESURE_NOT_ZERO(ULMean+URMean)); //?題：算的是比值
                    double TestBLR = fabs((BLMean-BRMean)/MAKESURE_NOT_ZERO(BLMean+BRMean)); //?題：算的是比值
                    double TestLUB = fabs((ULMean-BLMean));
                    double TestRUB = fabs((URMean-BRMean));
                    double diffTestULR = fabs(ULMean-URMean); //左右黑白差值
                    double diffTestBLR = fabs(BLMean-BRMean); //左右黑白差值
                    //左右黑差值應大於莫?數值(如16)，同時原先黑白對比依舊大於0.2
                    if ((TestULR < 0.2) || (TestBLR < 0.2) || 
						(TestLUB >= 16) || (TestRUB >= 16) || 
						(diffTestULR < 16) || (diffTestBLR < 16)) //左右黑
				//	if ((TestULR < 0.2) || (TestBLR < 0.2) ||
				//		(diffTestULR < 16) || (diffTestBLR < 16)) //左右黑
                    {
                        bRes = false;
                    }

                    return bRes;
                }

                void CaculateSFRValue(
                    UCHAR* _InImage,
                    double *_SFR_HiFrq_Value,
                    double *_SFR_LoFrq_Value,
                    double _HighFrqNum,
                    double _LowFrqNum,
                    int _SFRType,
                    int _ROIWidth,
                    int _ROIHeight)
                {
                    *_SFR_HiFrq_Value = 0.0;
                    *_SFR_LoFrq_Value = 0.0;
#if 0
                    // 调查计算SFR会挂掉的问题
                    static int i = 0;
                    CString strIndex;
                    strIndex.Format(_T("%d"), i++);
                    UTS::ImageFile::SaveBmpFile(strIndex, _InImage, _ROIWidth, _ROIHeight);
                    if (i > 100)
                    {
                        i = 0;
                    }
#endif

                    double *m_pY = new double[_ROIWidth * _ROIHeight];
                    for (int y = 0; y < _ROIHeight; y++)
                    {
                        for (int x = 0; x < _ROIWidth; x++)
                        {

                            UCHAR	R	= _InImage[(y*_ROIWidth+x)*3+2];
                            UCHAR	G	= _InImage[(y*_ROIWidth+x)*3+1];
                            UCHAR	B	= _InImage[(y*_ROIWidth+x)*3+0];
                            double	l;	
                            switch ( _SFRType ) 
                            {
                            case SFR_ORG:
                                l = 0.299 * R + 0.587 * G + 0.114 * B;
                                break;
                                //case SFR_LUMN:
                                //    l = double(giSSFR_R)/100 * R + double(giSSFR_G)/100 * G + double(giSSFR_B)/100 * B;
                                //    break;
                            case SFR_R:
                                l = R;
                                break;
                            case SFR_G:
                                l = G;
                                break;
                            case SFR_B:
                                l = B;
                                break;			
                            default:
                                l =0 ;
                                break;
                            }	
                            *(m_pY+x+y*_ROIWidth)	= l;
                        }
                    }

                    double	dSFRH=0.0;
                    double	dSFRL=0.0;

                    bool brotatev2 = false;
                    rotatev2(m_pY, _ROIWidth, _ROIHeight,brotatev2);
                    if (true == IsOKSFRBlock(m_pY, _ROIWidth, _ROIHeight))
                    {
                        CtSFR3		m_CtSFR3;
                        m_CtSFR3.Run(m_pY, _ROIWidth, _ROIHeight);



                        dSFRH = m_CtSFR3.GetSFRValue(_HighFrqNum);
                        dSFRL = m_CtSFR3.GetSFRValue(_LowFrqNum);
                        if(!((dSFRH >= 0) && (dSFRH < 1)))
                            dSFRH = 0;

                        if(!((dSFRL >= 0) && (dSFRL < 1)))
                            dSFRL = 0;
                        *_SFR_HiFrq_Value = dSFRH*100;
                        *_SFR_LoFrq_Value = dSFRL*100;
                    }

                    //	DoSFR(m_pY,_ROIWidth,_ROIHeight,dSFRL,dSFRH);			
                    delete []	m_pY;
                }

				void /*ALGORITHM_API*/ CaculateSFR3Value(unsigned char* _InImage,int _Width, int _Height, int _SFRType,double BLC,double _HighFrqNum,double _LowFrqNum,double &_SFR_HiFrq_Value,double &_SFR_LoFrq_Value)
				{
					double *m_pY = new double[_Width*_Height];
					memset(m_pY,0,sizeof(double)*_Width*_Height);
					for(int y=0;y<_Height;y++)
					{
						for(int x=0;x<_Width;x++)
						{

							double	R	= _InImage[(y*_Width+x)*3+ 2];
							double	G	= _InImage[(y*_Width+x)*3+ 1];
							double	B	= _InImage[(y*_Width+x)*3+ 0];

							double	l;	
							switch ( _SFRType ) 
							{
							case SFR_ORG:
								l = 0.299 * R + 0.587 * G + 0.114 * B;
								break;
							case SFR_R:
								l = R;
								break;
							case SFR_G:
								l = G;
								break;
							case SFR_B:
								l = B;
								break;			
							default:
								l =0 ;
								break;
							}
							if(l<BLC)
							{
								l=BLC;
							}
							*(m_pY+x+y*_Width)	= unsigned char(fnRange(fnRound(l),0,255));
						}
					}

					double	dSFRH=0.0;
					double	dSFRL=0.0;

					//	DoSFR(m_pY,_ROIWidth,_ROIHeight,dSFRL,dSFRH);
					SFR3Mat m_CtSFR3;
					m_CtSFR3.Run(m_pY, _Width, _Height);

					dSFRH = m_CtSFR3.GetSFRValue(_HighFrqNum);
					dSFRL = m_CtSFR3.GetSFRValue(_LowFrqNum);
					if(dSFRH < 0)
					{
						dSFRH = 0;
					}
					if(dSFRL < 0)
					{
						dSFRL = 0;
					}
					_SFR_HiFrq_Value = dSFRH*100;
					_SFR_LoFrq_Value = dSFRL*100;

					delete []	m_pY;
					m_pY = 0;
				}

                void GetBlockSFR(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const RECT &rcROI,
                    double dFrqHiNum,
                    double dFrqLoNum,
                    double &dHiSFR,
                    double &dLoSFR)
                {
                    int nROIWidth = rcROI.right - rcROI.left;
                    int nROIHeight = rcROI.bottom - rcROI.top;
                    unsigned char *pBlockBuffer = new unsigned char[nROIWidth * nROIHeight * 3];
                    CImageProc::GetInstance().GetBMPBlockBuffer(
                        pBmpBuffer,
                        pBlockBuffer,
                        nWidth,
                        nHeight,
                        rcROI.left,
                        rcROI.top,
                        nROIWidth,
                        nROIHeight);
//                     CaculateSFRValue(
//                         pBlockBuffer,
//                         &dHiSFR,
//                         &dLoSFR,
//                         dFrqHiNum,
//                         dFrqLoNum,
//                         SFR_ORG,
//                         nROIWidth,
//                         nROIHeight);

					CaculateSFR3Value(pBlockBuffer,
						nROIWidth, nROIHeight,
						SFR_ORG,0,dFrqHiNum,dFrqLoNum,dHiSFR, dLoSFR);


                    RELEASE_ARRAY(pBlockBuffer);
                }

                void GetFrqBySFR(CtSFR3& objCtSFR3, double dSFRNum, double *pdFrqNum)
                {
                    const int POINT_COUNT = 3200;
                    const double PRECISION = 0.5 / POINT_COUNT;

                    dSFRNum /= 100.0;
                    *pdFrqNum = -1.0;
                    double arrSFR[POINT_COUNT];
                    memset(arrSFR, 0, sizeof(arrSFR));
                    for (int i = 0; i < POINT_COUNT; i++)
                    {
                        arrSFR[i] = objCtSFR3.GetSFRValue(PRECISION * i);
                        if (i != 0)
                        {
                            double dStartSFR = arrSFR[i - 1];
                            double dEndSFR = arrSFR[i];
                            if (dEndSFR <= dSFRNum && dSFRNum <= dStartSFR)
                            {
                                // StartSFR = k * StartFrq + b
                                // EndSFR = k * EndFrq + b
                                // k = (StartSFR - EndSFR) / (StartFrq - EndFrq)
                                // b = StartSFR - k * StartFrq
                                // FrqNum = (SFRNum - b) / k
                                double k = (dStartSFR - dEndSFR) / PRECISION;
                                double b = dStartSFR - k * (i - 1) * PRECISION;
                                *pdFrqNum = (dSFRNum - b) / k;
                                return;
                            }
                        }
                    }
                }

                //------------------------------------------------------------------------------
                // 根据SFR分数，返回对应频率的版本
                void CaculateSFRValueWithFrq(
                    UCHAR* _InImage,
                    double *_SFR_HiFrq_Value,
                    double *_SFR_LoFrq_Value,
                    double *pdFrqNum,
                    double _HighFrqNum,
                    double _LowFrqNum,
                    double dSFRNum,
                    int _SFRType,
                    int _ROIWidth,
                    int _ROIHeight)
                {
                    *_SFR_HiFrq_Value = 0.0;
                    *_SFR_LoFrq_Value = 0.0;
                    *pdFrqNum = 0.0;
#if 0
                    // 调查计算SFR会挂掉的问题
                    static int i = 0;
                    CString strIndex;
                    strIndex.Format(_T("%d"), i++);
                    UTS::ImageFile::SaveBmpFile(strIndex, _InImage, _ROIWidth, _ROIHeight);
                    if (i > 100)
                    {
                        i = 0;
                    }
#endif

                    double *m_pY = new double[_ROIWidth * _ROIHeight];
                    for (int y = 0; y < _ROIHeight; y++)
                    {
                        for (int x = 0; x < _ROIWidth; x++)
                        {

                            UCHAR	R	= _InImage[(y*_ROIWidth+x)*3+2];
                            UCHAR	G	= _InImage[(y*_ROIWidth+x)*3+1];
                            UCHAR	B	= _InImage[(y*_ROIWidth+x)*3+0];
                            double	l;	
                            switch ( _SFRType ) 
                            {
                            case SFR_ORG:
                                l = 0.299 * R + 0.587 * G + 0.114 * B;
                                break;
                                //case SFR_LUMN:
                                //    l = double(giSSFR_R)/100 * R + double(giSSFR_G)/100 * G + double(giSSFR_B)/100 * B;
                                //    break;
                            case SFR_R:
                                l = R;
                                break;
                            case SFR_G:
                                l = G;
                                break;
                            case SFR_B:
                                l = B;
                                break;			
                            default:
                                l =0 ;
                                break;
                            }	
                            *(m_pY+x+y*_ROIWidth)	= l;
                        }
                    }

                    double	dSFRH=0.0;
                    double	dSFRL=0.0;

                    bool brotatev2 = false;
                    rotatev2(m_pY, _ROIWidth, _ROIHeight,brotatev2);
                    if (true == IsOKSFRBlock(m_pY, _ROIWidth, _ROIHeight))
                    {
                        CtSFR3		m_CtSFR3;
                        m_CtSFR3.Run(m_pY, _ROIWidth, _ROIHeight);

                        dSFRH = m_CtSFR3.GetSFRValue(_HighFrqNum);
                        dSFRL = m_CtSFR3.GetSFRValue(_LowFrqNum);
                        if(!((dSFRH >= 0) && (dSFRH < 1)))
                            dSFRH = 0;

                        if(!((dSFRL >= 0) && (dSFRL < 1)))
                            dSFRL = 0;
                        *_SFR_HiFrq_Value = dSFRH*100;
                        *_SFR_LoFrq_Value = dSFRL*100;

                        GetFrqBySFR(m_CtSFR3, dSFRNum, pdFrqNum);
                    }

                    //	DoSFR(m_pY,_ROIWidth,_ROIHeight,dSFRL,dSFRH);			
                    delete []	m_pY;
                }

                void GetBlockSFRWithFrq(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const RECT &rcROI,
                    double dFrqHiNum,
                    double dFrqLoNum,
                    double dSFRNum,
                    double &dHiSFR,
                    double &dLoSFR,
                    double &dFrqNum)
                {
                    int nROIWidth = rcROI.right - rcROI.left;
                    int nROIHeight = rcROI.bottom - rcROI.top;
                    unsigned char *pBlockBuffer = new unsigned char[nROIWidth * nROIHeight * 3];
                    CImageProc::GetInstance().GetBMPBlockBuffer(
                        pBmpBuffer,
                        pBlockBuffer,
                        nWidth,
                        nHeight,
                        rcROI.left,
                        rcROI.top,
                        nROIWidth,
                        nROIHeight);
                    CaculateSFRValueWithFrq(
                        pBlockBuffer,
                        &dHiSFR,
                        &dLoSFR,
                        &dFrqNum,
                        dFrqHiNum,
                        dFrqLoNum,
                        dSFRNum,
                        SFR_ORG,
                        nROIWidth,
                        nROIHeight);
                    RELEASE_ARRAY(pBlockBuffer);
                }
				SFR3Mat::SFR3Mat()
					:bOldFlag(false),del(1.0),nBin(4),SFRArray(NULL),FreqList(NULL),ArraySize(0)
				{
					fil1[0] = 0.5;
					fil1[1] = -0.5;

					fil2[0] = 0.5;
					fil2[1] = 0.0;
					fil2[2] = -0.5;
				}
				SFR3Mat::~SFR3Mat()
				{
					if (SFRArray!=NULL)
					{
						delete[] SFRArray;
						SFRArray = NULL;
					}
					if (FreqList!=NULL)
					{
						delete[] FreqList;
						FreqList = NULL;
					}
				}
				bool SFR3Mat::Run(double* _pSrcBuffer,int _Width,int _Height)
				{
 					bool bRes = true;
					bOldFlag = false;
					del = 1.0;
					nBin = 4;

					if (SFRArray!=NULL)
					{
						delete[] SFRArray;
						SFRArray = NULL;
					}
					if (FreqList!=NULL)
					{
						delete[] FreqList;
						FreqList = NULL;
					}
					ArraySize = 0;

 					int RawBufferWidth = _Width;
 					int RawBufferHeight = _Height;
 					double* DataBuffer = new double[RawBufferWidth*RawBufferHeight];
 					memset(DataBuffer,0,sizeof(double)*RawBufferWidth*RawBufferHeight);
 					for (int y=0;y<RawBufferHeight;y++)
 					{
 						for (int x=0;x<RawBufferWidth;x++)
 						{
 							DataBuffer[y*RawBufferWidth+x] = _pSrcBuffer[y*RawBufferWidth+x];
 						}
 					}
 					//-------------------------------------
 					bool bflag = false;
 					rotatev2(DataBuffer,RawBufferWidth,RawBufferHeight,bflag);

// 		#ifdef _DEBUG
// 					unsigned char* rotatevbuffer = new unsigned char[RawBufferWidth*RawBufferHeight];
// 					for (int i=0;i<RawBufferWidth*RawBufferHeight;i++)
// 					{
// 						rotatevbuffer[i] = RANGE(ROUND(DataBuffer[i]),0,255);
// 					}
// 					foundation::image::WriteBMPImage(L"rotatev2.bmp",rotatevbuffer,RawBufferWidth,RawBufferHeight,1);
// 					RELEASE(rotatevbuffer);
// 		#endif
 					//-------------------------------------
 					double tleft = 0;
 					double tright = 0;
 					for (int y=0;y<RawBufferHeight;y++)
 					{
 						for (int x=0;x<5;x++)
 						{
 							tleft += DataBuffer[y*RawBufferWidth+x];
 							tright += DataBuffer[y*RawBufferWidth+(RawBufferWidth-1-x)];
 						}
 					}
 					tleft /= double(5*RawBufferHeight);
 					tright /= double(5*RawBufferHeight);
 					if (tleft>tright)
 					{		
 						fil1[0] = -0.5;
 						fil1[1] = 0.5;
 
 						fil2[0] = -0.5;
 						fil2[1] = 0.0;
 						fil2[2] = 0.5;
 					}
					else
					{
						fil1[0] = 0.5;
						fil1[1] = -0.5;

						fil2[0] = 0.5;
						fil2[1] = 0.0;
						fil2[2] = -0.5;
					}

 					//double test = abs( (tleft-tright)/(tleft+tright) );
					double test = abs(DIVIDE((tleft-tright),(tleft+tright)));
 					if ((test < 0.2) || (false == IsOKSFRBlock(DataBuffer,RawBufferWidth,RawBufferHeight)))
			//		if ((test < 0.2))
 					{
 						//** WARNING: Edge contrast is less that 20%, this can lead to high error in the SFR measurement.
					//	foundation::DebugInfo(L"WARNING: Edge contrast is less that 0.2, this can lead to high error in the SFR measurement.");
						int nn2 =  int(floor(double(int(floor(double(RawBufferWidth*nBin))))/2.0) + 1);

						this->ArraySize = nn2;
						this->SFRArray = new double[this->ArraySize];
						memset(this->SFRArray,0,sizeof(double)*(this->ArraySize));
						this->FreqList = new double[this->ArraySize];
						memset(this->FreqList,0,sizeof(double)*(this->ArraySize));

						for (int i=0;i<nn2;i++)
						{
							this->SFRArray[i] = 0;
							this->FreqList[i] = 0;
						}

						delete[] DataBuffer;
						DataBuffer = 0;

						bRes=false;
						return bRes;

 					}
					//-------------------
					// Edge estimation
					//-------------------
					/*
						% Smoothing window for first part of edge location estimation to be used on each line of ROI
						win1 = ahamming(npix, (npix+1)/2);    % Symmetric window
					*/
					double* AhammingData = new double[RawBufferWidth];
					memset(AhammingData,0,sizeof(double)*RawBufferWidth);
					//ahamming(RawBufferWidth,(RawBufferWidth+1)/2,AhammingData);
					ahamming(RawBufferWidth,double(RawBufferWidth+1.0)/2.0,AhammingData);

					double *Deriv1Data = new double[RawBufferHeight*RawBufferWidth];
					memset(Deriv1Data,0,sizeof(double)*RawBufferHeight*RawBufferWidth);
					deriv1(DataBuffer,RawBufferHeight,RawBufferWidth,fil1,2,Deriv1Data);
					/*
						% compute centroid for derivative array for each line in ROI. NOTE WINDOW array 'win'
						for n=1:nlin
						loc(color, n) = centroid( c(n, 1:npix )'.*win1) - 0.5;   % -0.5 shift for FIR phase
						end;
					*/
					double* LocData = new double[RawBufferHeight];
					memset(LocData,0,sizeof(double)*RawBufferHeight);	
					for (int y=0;y<RawBufferHeight;y++)
					{
						// c(n, 1:npix )'.*win1
						double* _tmp = new double[RawBufferWidth];
						for (int x = 0;x<RawBufferWidth;x++)
						{
							_tmp[x] = Deriv1Data[y*RawBufferWidth+x]*AhammingData[x];
						}
						LocData[y] = centroid(_tmp,RawBufferWidth) - 0.5;
						delete[] _tmp;
						_tmp = 0;		
					}
					delete[] AhammingData;
					AhammingData = 0;
					/*
						% mgamadia 10.11.2011: add fitstats output to compute R^2
						[fitme(color,1:2) fitstats] = findedge(loc(color,:), nlin);
						place = zeros(nlin,1);
						for n=1:nlin;
						place(n) = fitme(color,2) + fitme(color,1)*n;
						win2 = ahamming(npix, place(n));
						loc(color, n) = centroid( c(n, 1:npix )'.*win2) -0.5;
						end;
					*/
					double Fitme[3] = {0};
					findedge(LocData,RawBufferHeight,Fitme[0],Fitme[1]);
					
					double* Place = new double[RawBufferHeight];
					memset(Place,0,sizeof(double)*RawBufferHeight);

					for (int y=0;y<RawBufferHeight;y++)
					{
						Place[y] = Fitme[1]+Fitme[0]*double((y+1));
						double* AhammingData2 = new double[RawBufferWidth];
						memset(AhammingData2,0,sizeof(double)*RawBufferWidth);
						ahamming(RawBufferWidth,Place[y],AhammingData2);

						// c(n, 1:npix )'.*win2
						double* _tmp = new double[RawBufferWidth];
						for (int x = 0;x<RawBufferWidth;x++)
						{
							_tmp[x] = Deriv1Data[y*RawBufferWidth+x]*AhammingData2[x];
						}
						LocData[y] = centroid(_tmp,RawBufferWidth) - 0.5;
						delete[] _tmp;
						_tmp = 0;
						delete[] AhammingData2;
						AhammingData2 = 0;
					}
					delete[] Deriv1Data;
					Deriv1Data = 0;
					delete[] Place;
					Place = 0;
					//---------------------------------
					/*
					% mgamadia 10.11.2011: add fitstats output to compute R^2
					[fitme(color,1:2) fitstats] = findedge(loc(color,:), nlin);
					*/	
					memset(Fitme,0,sizeof(double)*3);
					findedge(LocData,RawBufferHeight,Fitme[0],Fitme[1]);
					/*
					% mgamadia 10.11.2011: compute LSF line fit slope and R^2 for each color
						est_slope(color) = fitme(1,1);
					est_slope_deg(color) = 180*atan(abs(est_slope(color)))/pi;
					fitrsquared(color) = 1-(fitstats.normr^2 / norm(loc(color,:)-mean(loc(color,:)))^2);
					*/
					double Est_Slope = Fitme[0];
					double Est_Slope_Deg = 180*atan(abs(Est_Slope))/PI;
					//fitrsquared(color) = 1-(fitstats.normr^2 / norm(loc(color,:)-mean(loc(color,:)))^2);
					//---------------------------------
					/*
					% Full linear fit is available as variable fitme. Note that the fit is for the projection onto the X-axis,
					%       x = fitme(color, 1) y + fitme(color, 2)
					% so the slope is the inverse of the one that you may expect

					% Limit number of lines to integer(npix*line slope as per ISO algorithm except if processing as 'sfrmat2'
					if oldflag ~=1
					%   disp(['Input lines: ',num2str(nlin)])
					nlin1 = round(floor(nlin*abs(fitme(1,1)))/abs(fitme(1,1)));
					%   disp(['Integer cycle lines: ',num2str(nlin1)])
					a = a(1:nlin1, :, 1:ncol);
					end
					*/
					//-----------------------------------------------------	
					double* AnalysisBuffer;
					if (bOldFlag != true)
					{
						RawBufferHeight = int(ROUND(floor(double(RawBufferHeight)/abs(Fitme[0]))*abs(Fitme[0])));
						//RawBufferHeight = int(ROUND(floor(double(RawBufferHeight)*abs(Fitme[0]))/abs(Fitme[0])));
						if (RawBufferHeight == 0)
						{
 							//** WARNING: Edge contrast is less that 20%, this can lead to high error in the SFR measurement.
							int nn2 =  int(floor(double(int(floor(double(RawBufferWidth*nBin))))/2.0) + 1);

							this->ArraySize = nn2;
							this->SFRArray = new double[this->ArraySize];
							memset(this->SFRArray,0,sizeof(double)*(this->ArraySize));
							this->FreqList = new double[this->ArraySize];
							memset(this->FreqList,0,sizeof(double)*(this->ArraySize));

							for (int i=0;i<nn2;i++)
							{
								this->SFRArray[i] = 0;
								this->FreqList[i] = 0;
							}

							delete[] DataBuffer;
							DataBuffer = 0;

							delete[] LocData;
							LocData = 0;
							

							bRes=false;
							return bRes;
						}

						AnalysisBuffer = new double[RawBufferHeight*RawBufferWidth];
						for (int y=0;y<RawBufferHeight;y++)
						{
							for (int x=0;x<RawBufferWidth;x++)
							{
								AnalysisBuffer[y*RawBufferWidth+x] = DataBuffer[y*RawBufferWidth+x];
							}			
						}
					}
					delete[] DataBuffer;
					DataBuffer = 0;
					//---------------------------------
					/*
					vslope = fitme(1,1);
					slope_deg= 180*atan(abs(vslope))/pi;
					*/
					double VSlope = Fitme[0];
					double Slope_Deg= 180.0*atan(abs(VSlope))/PI;
					//-----------------------------------------------------
					/*
					del2=0;
					if oldflag ~= 1;
					%Correct sampling inverval for sampling parallel to edge
					delfac = cos(atan(vslope));
					del = del*delfac;
					del2 = del/nbin;
					end
					*/
					double del2 = 0;
					if (bOldFlag != true)
					{
						double delfac = cos(atan(VSlope));
						del = del*delfac;
						del2 = del/nBin;
					}
					//-----------------------------------------------------
					int npix = RawBufferWidth;
					int nbin = nBin;
					int ncol = 1;
					//-----------------------------------------------------
					/*
					nn =   floor(npix *nbin);
					mtf =  zeros(nn, ncol);
					nn2 =  floor(nn/2) + 1;
					*/
					int nn = int(floor(double(npix*nbin)));
				// 	double* mtf = new double(nn*ncol);
				// 	memset(mtf,0,sizeof(double)*nn*ncol);
					int nn2 =  int(floor(double(nn)/2.0) + 1);
					/*
					if oldflag ~=1;
					disp('Derivative correction')
					dcorr = fir2fix(nn2, 3);    % dcorr corrects SFR for response of FIR filter
					end
					*/
					double* CorrectionFilter = new double[nn2];
					memset(CorrectionFilter,0,sizeof(double)*nn2);
					if (bOldFlag != true)
					{
						fir2fix(nn2,3,CorrectionFilter);
					}
					/*
					freq = zeros(nn, 1);
					for n=1:nn;
					freq(n) = nbin*(n-1)/(del*nn);
					end;
					*/
					double* freq = new double[nn];
					for (int i=0;i<nn;i++)
					{
						freq[i] = double(nbin*(i))/(del*double(nn));
					}
					/*
					freqlim = 1;
					if nbin == 1;
						freqlim = 2;
					end
					nn2out = round(nn2*freqlim/2);
					nfreq = n/(2*del*nn);    % half-sampling frequency
					win = ahamming(nbin*npix,(nbin*npix+1)/2);      % centered Hamming window
					*/
					double freqlim = 1;
					if (nbin == 1)
					{
						freqlim =2;
					}
					double nn2out = ROUND(double(nn2)*freqlim/2.0);
					double nfreq = double(nn)/(2*del*double(nn));

					double* win = new double[nbin*npix];
					memset(win,0,sizeof(double)*nbin*npix);
					ahamming(nbin*npix,double(nbin*npix+1)/2,win);

					/*
					%-------------------
					% Edge spread function (edge profile)
					% -> Point spread function (psf)
					% -> fft -> mtf
					%-------------------
					*/
					//	esf = zeros(nn,ncol);
					double* esf = new double[nn*ncol];
					memset(esf,0,sizeof(double)*nn*ncol);
					/*
					% project and bin data in 4x sampled array
					[point, ~] = project(a(:,:,color), loc(color, 1), fitme(color,1), nbin);
					esf(:,color) = point;
					*/
					double* point = new double[nn];
					memset(point,0,sizeof(double)*nn);
					project(AnalysisBuffer,RawBufferHeight,RawBufferWidth,LocData[0],Fitme[0], nbin,point);

					delete[] LocData;
					LocData = 0;
					delete[] AnalysisBuffer;
					AnalysisBuffer = 0;

					for (int i=0;i<nn;i++)
					{
						esf[i] = point[i];
					}
		
					delete[] esf;
					esf = 0;
					/*
					% compute first derivative via FIR (1x3) filter fil
					c = deriv1(point', 1, nn, fil2);
					c = c';
					*/
					double* C448 = new double[1*nn];
					memset(C448,0,sizeof(double)*1*nn);
					deriv1(point,1,nn,fil2,3,C448);
					delete[] point;
					point = 0;
 
					/*
					% mgamadia 10.11.2011: uncommented to save supersampled lsf per color
					psf(:,color) = c;

					mid = centroid(c);
					temp = cent(c, round(mid));              % shift array so it is centered
					c = temp;
					clear temp;

					% apply window (symmetric Hamming)
					c = win.*c;
					*/
					double* psf = new double[nn];
					for (int i=0;i<nn;i++)
					{
						psf[i] = C448[i];
					}
					delete[] psf;
					psf = 0;

					double mid = centroid(C448,nn);
					double* tmp = new double[nn];
					memset(tmp,0,sizeof(double)*nn);
					cent(C448,nn,ROUND(mid),tmp);
					for (int i=0;i<nn;i++)
					{
						C448[i] = tmp[i];
					}
					delete[] tmp;
					tmp = 0;

					for (int i=0;i<nn;i++)
					{
						C448[i] *= win[i];
					}
					delete[] win;
					win = 0;

					/*

					%%%%
					% Transform, scale and correct for FIR filter response

					temp = abs(fft(c, nn));
					mtf(1:nn2, color) = temp(1:nn2)/temp(1);
					if oldflag ~=1;
					mtf(1:nn2, color) = mtf(1:nn2, color).*dcorr;
					end
					*/

					//  [1/20/2016 Chijen.Liao]
					double** tmpA = new double*[nn];
					for (int i=0;i<nn;i++)
					{
						tmpA[i] = new double[2];
					}

					FastFouriertTansform::GetInstance()->dft_r2c_1d(nn,C448,tmpA);

			// 		fftw_complex * _outData = new fftw_complex[nn];
			// 		memset(_outData,0,sizeof(fftw_complex)*nn);
			// 
			// 		fftw_plan plan1;
			// 		plan1 = fftw_plan_dft_r2c_1d(nn,C448,_outData,FFTW_ESTIMATE);
			// 		fftw_execute(plan1);
			// 		fftw_destroy_plan(plan1);

					delete[] C448;
					C448 = 0;

					/*double**/ tmp = new double[nn*2];
					for (int i=0;i<nn;i++)
					{
						tmp[i] = sqrt(tmpA[i][0]*tmpA[i][0]+tmpA[i][1]*tmpA[i][1]);
					}

					//  [1/20/2016 Chijen.Liao]
					for (int i=0;i<nn;i++)
					{
						delete[] tmpA[i];
					}
					delete[] tmpA;
					tmpA = 0;
					//---------------------------
		// 			delete[] _outData;
		// 			_outData = 0;

					double* mtf = new double[nn2];
					memset(mtf,0,sizeof(double)*nn2);
					for (int i=0;i<nn2;i++)
					{
						mtf[i] = tmp[i]/tmp[0];
					}
					if (bOldFlag != true)
					{
						for (int i=0;i<nn2;i++)
						{
							mtf[i] *= CorrectionFilter[i];
						}
					}
					delete[] tmp;
					tmp = 0;

					delete[] CorrectionFilter;
					CorrectionFilter = 0;

					// OUTPUT
					this->ArraySize = nn2;
					this->SFRArray = new double[this->ArraySize];
					memset(this->SFRArray,0,sizeof(double)*(this->ArraySize));
					this->FreqList = new double[this->ArraySize];
					memset(this->FreqList,0,sizeof(double)*(this->ArraySize));

					for (int i=0;i<nn2;i++)
					{
						this->SFRArray[i] = mtf[i];
						this->FreqList[i] = freq[i];
					}

					delete[] freq;
					freq = 0;
					delete[] mtf;
					mtf = 0;
					
 					return bRes;
				}
				double SFR3Mat::GetSFRValue(double _Ny)
				{
					double SFRValue =  interp1(this->SFRArray,this->FreqList,this->ArraySize,_Ny);
					return SFRValue;
				}
				void SFR3Mat::GetAllSFR(double *_SFRArray,double *_FreqList)
				{
					memcpy(_SFRArray,this->SFRArray,this->ArraySize);
					memcpy(_FreqList,this->FreqList,this->ArraySize);
				}
				int SFR3Mat::GetSFRArraySize(void)
				{
					return this->ArraySize;
				}
            }
        }   // namespace SFR
    }   // namespace Algorithm
}   // namespace UTS

