#include "StdAfx.h"
#include "Algorithm.h"
#include "core/core_c.h"
#include "imgproc/imgproc_c.h"

namespace UTS
{
    namespace Algorithm
    {
        void MeanFilter_CV(
            const unsigned char* pOneChannelBuffer,
            int nWidth,
            int nHeight,
            int nFilterWidth,
            int nFilterHeight,
            double* pFilterBuffer)
        {
            memset(pFilterBuffer, 0, sizeof(double) * nWidth * nHeight);

            CvSize size;
            size.width = nWidth;
            size.height = nHeight;
            int depth = IPL_DEPTH_8U;
            int channels = 1;

            IplImage* Image1 = cvCreateImage( /*CvSize*/ size, /*int*/ depth, /*int*/ channels);

            memcpy_s(Image1->imageData, nWidth * nHeight, pOneChannelBuffer, nWidth * nHeight);

            IplImage* Image2 = cvCreateImage( /*CvSize*/ size, /*int*/ depth, /*int*/ channels);

            cvSmooth(Image1, Image2, CV_BLUR, nFilterWidth, nFilterHeight, 0, 0);

            int index = 0;
            for (int y = 0; y < nHeight; y++)
            {
                for (int x = 0; x < nWidth; x++)
                {
                    /*int*/ index = (y * nWidth + x);
                    pFilterBuffer[index] = (unsigned char)Image2->imageData[index];
                }
            }

            cvReleaseImage(&Image1);
            cvReleaseImage(&Image2);
            //-----------------------------------------
        }

        //-------------------------------------------------------------------------
        // calculate block mean value, set into center Point
        void MeanFilter_CT(
            const unsigned char *pYBuffer,
            int nWidth,
            int nHeight,
            int nBlockWidth,
            int nBlockHeight,
            double *pdMeanBuffer)
        {
            //-------------------------------------------------------------------------
            // Param assert
            assert(nullptr != pYBuffer);
            assert(nullptr != pdMeanBuffer);
            assert(nWidth > 0);
            assert(nHeight > 0);
            assert(nBlockWidth > 0);
            assert(nBlockHeight > 0);
            assert(nBlockWidth % 2 == 1);
            assert(nBlockHeight % 2 == 1);

            int nHalfBlockWidth = nBlockWidth / 2;
            int nHalfBlockHeight = nBlockHeight / 2;
            for (int y = 0; y < nHeight; y++)
            {
                for (int x = 0; x < nWidth; x++)
                {
                    double dAvg = 0.0;

                    int nCount = 0;
                    int bys = ((y < nHalfBlockHeight) ? 0 : (y - nHalfBlockHeight));
                    int bye = ((y > nHeight - nHalfBlockHeight - 1) ? nHeight : (y + nHalfBlockHeight + 1));
                    int bxs = ((x < nHalfBlockWidth) ? 0 : (x - nHalfBlockWidth));
                    int bxe = ((x > nWidth - nHalfBlockWidth - 1) ? nWidth : (x + nHalfBlockWidth + 1));
                    for (int by = bys; by < bye; by++)
                    {
                        for (int bx = bxs; bx < bxe; bx++)
                        {
                            dAvg += pYBuffer[by * nWidth + bx];
                            nCount++;
                        }
                    }
                    dAvg /= nCount;

                    pdMeanBuffer[y * nWidth + x] = dAvg;
                }
            }
        }
        
        // 取得ROI中的平均RGB
        void GetROIAvgRGB(
            const unsigned char* pBmpBuffer,
            int nWidth,
            int nHeight,
            const RECT& rect,
            COLOR_RGB_DOUBLE &rgb)
        {
            long sumR = 0;
            long sumG = 0;
            long sumB = 0;
            for (int i = rect.left; i < rect.right; i++)
            {
                for (int j = rect.top; j < rect.bottom; j++)
                {
                    sumR += pBmpBuffer[PIXEL_R(i, j, nWidth, nHeight)];
                    sumG += pBmpBuffer[PIXEL_G(i, j, nWidth, nHeight)];
                    sumB += pBmpBuffer[PIXEL_B(i, j, nWidth, nHeight)];
                }
            }
            long totalCnt = (rect.right - rect.left) * (rect.bottom - rect.top);
            double avgR = (double)sumR / (double)totalCnt;
            double avgG = (double)sumG / (double)totalCnt;
            double avgB = (double)sumB / (double)totalCnt;
            rgb.dR = (avgR > 255.0 ? 255.0 : avgR);
            rgb.dG = (avgG > 255.0 ? 255.0 : avgG);
            rgb.dB = (avgB > 255.0 ? 255.0 : avgB);
        }

		// 取得ROI中的平均RGB
		void GetROIAvgRGB(
			const unsigned char *bmp,
			int nWidth,
			int nHeight,
			int filter,
			const RECT& rect,
			RGBTRIPLE& rgb)
		{
			int totalCnt = 0;
			double sumR = 0, sumG = 0, sumB = 0;
			for (int i = rect.left; i < rect.right; i++) {
				for (int j = rect.top; j < rect.bottom; j++) {
					uint8_t r = bmp[PIXEL_R(i, j, nWidth, nHeight)];
					uint8_t g = bmp[PIXEL_G(i, j, nWidth, nHeight)];
					uint8_t b = bmp[PIXEL_B(i, j, nWidth, nHeight)];

					double Y = YVALUE(b,g,r);
					if (Y < filter) continue;
					totalCnt++;
					sumR += r; sumG += g; sumB += b;
				}
			}
			if (totalCnt == 0) {
				rgb.rgbtRed = 0; rgb.rgbtGreen = 0; rgb.rgbtBlue = 0;
				return;
			}
			int avgR = DOUBLE2INT(sumR / totalCnt);
			int avgG = DOUBLE2INT(sumG / totalCnt);
			int avgB = DOUBLE2INT(sumB / totalCnt);
			rgb.rgbtRed = (avgR > 255 ? 255 : (BYTE)avgR);
			rgb.rgbtGreen = (avgG > 255 ? 255 : (BYTE)avgG);
			rgb.rgbtBlue = (avgB > 255 ? 255 : (BYTE)avgB);
		}

		// 取得ROI中的平均RGrGbB
		void GetROIAvgRGrGbB(
			WORD* pRaw10,
			int rawBayerType,
			double blackLvl,
			int nWidth,
			int nHeight,
			const RECT& rect,
			color_rgrgbb_uShort &rgb)
		{
			double avgRGrGbB[4] ={0.0};
			unsigned short RGrGbB[4] ={0};

                //-------------------------------------------------
                /*
                1. Region of interest(ROI) definition :
                Size : 1/5 Width_Resolution *1/5 Height_Resolution
                Position : Center
                */

			int roiWidth,roiHeight;

			roiWidth = (int)(rect.right - rect.left + 1);
			roiHeight = (int)(rect.bottom - rect.top + 1);

            WORD* roiBuf = new WORD[roiHeight * roiWidth];

            for (int y=0;y<roiHeight;y++)
            {
                for (int x=0;x<roiWidth;x++)
                {
                    int X = x + rect.left;
                    int Y = y + rect.top;
                    roiBuf[y*roiWidth+x] = pRaw10[Y*nWidth+X];
                }
            }
            //-------------------------------------------------
            /*
            2. Get average R, G, B value in the ROI window.
            R_Avg, B_Avg, G_Avg
            */		
            double ChannelA = 0;
            double ChannelB = 0;
            double ChannelC = 0;
            double ChannelD = 0;				

            int Count = 0;
            for (int y = 0; y< roiHeight; y+=2)
            {
                for (int x = 0; x< roiWidth; x+=2)
                {
                    ChannelA += double(roiBuf[(y*roiWidth + x)]);
                    Count++;
                }
            }
            ChannelA /= double(Count);

            Count = 0;
            for (int y = 0; y< roiHeight; y+=2)
            {
                for (int x = 1; x< roiWidth; x+=2)
                {
                    ChannelB += double(roiBuf[(y*roiWidth+x)]);
                    Count++;
                }
            }
            ChannelB /= double(Count);

            Count = 0;
            for (int y = 1; y< roiHeight; y+=2)
            {
                for (int x = 0; x< roiWidth; x+=2)
                {
                    ChannelC += double(roiBuf[(y*roiWidth+x)]);
                    Count++;
                }
            }
            ChannelC /= double(Count);

            Count = 0;
            for (int y = 1; y< roiHeight; y+=2)
            {
                for (int x = 1; x< roiWidth; x+=2)
                {
                    ChannelD += double(roiBuf[(y*roiWidth+x)]);
                    Count++;
                }
            }
            ChannelD /= double(Count);

            double RGGBValue[4] = {0.0};
            //RGGBValue[0]:R, RGGBValue[1]:Gr, RGGBValue[2]:Gb, RGGBValue[3]:B
            switch (rawBayerType)
            {
            case 1://B Gb Gr R
                RGGBValue[0] = ChannelD;
                RGGBValue[1] = ChannelC;
                RGGBValue[2] = ChannelB;
                RGGBValue[3] = ChannelA;
                break;

            case 2://R Gr Gb B
                RGGBValue[0] = ChannelA;
                RGGBValue[1] = ChannelB;
                RGGBValue[2] = ChannelC;
                RGGBValue[3] = ChannelD;
                break;

            case 3://Gb B R Gr
                RGGBValue[0] = ChannelC;
                RGGBValue[1] = ChannelD;
                RGGBValue[2] = ChannelA;
                RGGBValue[3] = ChannelB;
                break;

            case 4://Gr R B Gb
                RGGBValue[0] = ChannelB;
                RGGBValue[1] = ChannelA;
                RGGBValue[2] = ChannelD;
                RGGBValue[3] = ChannelC;
                break;
            }

            avgRGrGbB[0] = RGGBValue[0]-blackLvl;
            avgRGrGbB[1] = RGGBValue[1]-blackLvl;
            avgRGrGbB[2] = RGGBValue[2]-blackLvl;
            avgRGrGbB[3] = RGGBValue[3]-blackLvl;

            unsigned short R = (unsigned short) (avgRGrGbB[0] > 1023.0 ? 1023.0 : avgRGrGbB[0]);
            unsigned short Gr = (unsigned short)(avgRGrGbB[1] > 1023.0 ? 1023.0 : avgRGrGbB[1]);
            unsigned short Gb = (unsigned short)(avgRGrGbB[2] > 1023.0 ? 1023.0 : avgRGrGbB[2]);
            unsigned short B = (unsigned short) (avgRGrGbB[3] > 1023.0 ? 1023.0 : avgRGrGbB[3]);

            rgb.R = R;
            rgb.Gr = Gr;
            rgb.Gb = Gb;
            rgb.B = B;

		}

        void GetBlockAvgY(
            __in  const unsigned char *pYBuffer,
            __in  int nWidth,
            __in  int nHeight,
            __in  const RECT &rcBlock,
            __out double &dYAvg)
        {
            assert(nullptr != pYBuffer);
            assert(nWidth > 0);
            assert(nHeight > 0);

            dYAvg = 0.0;

            int nBlockWidth = rcBlock.right - rcBlock.left;
            int nBlockHeight = rcBlock.bottom - rcBlock.top;
            int nPixelCount = nBlockWidth * nBlockHeight;
            unsigned char *pBlockYBuffer = new unsigned char[nPixelCount];
            Image::CImageProc::GetInstance().GetYBlockBuffer(
                pYBuffer, pBlockYBuffer, nWidth, nHeight,
                rcBlock.left, rcBlock.top, nBlockWidth, nBlockHeight);
            for (int i = 0; i < nPixelCount; i++)
            {
                dYAvg += pBlockYBuffer[i];
            }
            dYAvg /= nPixelCount;
            RELEASE_ARRAY(pBlockYBuffer);
        }

		void GetBlockMinY(
			__in  const unsigned char *pYBuffer,
			__in  int nWidth,
			__in  int nHeight,
			__in  const RECT &rcBlock,
			__out double &dYMin)
		{
			assert(nullptr != pYBuffer);
			assert(nWidth > 0);
			assert(nHeight > 0);

			dYMin = 255.0;

			int nBlockWidth = rcBlock.right - rcBlock.left;
			int nBlockHeight = rcBlock.bottom - rcBlock.top;
			int nPixelCount = nBlockWidth * nBlockHeight;
			unsigned char *pBlockYBuffer = new unsigned char[nPixelCount];
			Image::CImageProc::GetInstance().GetYBlockBuffer(
				pYBuffer, pBlockYBuffer, nWidth, nHeight,
				rcBlock.left, rcBlock.top, nBlockWidth, nBlockHeight);
			for (int i = 0; i < nPixelCount; i++)
			{
				if (pBlockYBuffer[i] < dYMin)
					dYMin = pBlockYBuffer[i];
			}
			
			RELEASE_ARRAY(pBlockYBuffer);
		}

		void GetBlockMaxY(
			__in  const unsigned char *pYBuffer,
			__in  int nWidth,
			__in  int nHeight,
			__in  const RECT &rcBlock,
			__out double &dYMax)
		{
			assert(nullptr != pYBuffer);
			assert(nWidth > 0);
			assert(nHeight > 0);

			dYMax = 0.0;

			int nBlockWidth = rcBlock.right - rcBlock.left;
			int nBlockHeight = rcBlock.bottom - rcBlock.top;
			int nPixelCount = nBlockWidth * nBlockHeight;
			unsigned char *pBlockYBuffer = new unsigned char[nPixelCount];
			Image::CImageProc::GetInstance().GetYBlockBuffer(
				pYBuffer, pBlockYBuffer, nWidth, nHeight,
				rcBlock.left, rcBlock.top, nBlockWidth, nBlockHeight);
			for (int i = 0; i < nPixelCount; i++)
			{
				if (pBlockYBuffer[i] > dYMax)
					dYMax = pBlockYBuffer[i];
			}
			
			RELEASE_ARRAY(pBlockYBuffer);
		}


        void CalYavg(const unsigned char* pBmpBuffer, int Width, int Height, double &_Yavg)
        {
            double temp_Yavg = 0;
            for (int y = 0; y < Height; y++)
            {
                for (int x = 0; x < Width; x++)
                {
                    int _index = (y * Width + x) * 3;
                    double _Y = YVALUE(pBmpBuffer[_index+0], pBmpBuffer[_index+1], pBmpBuffer[_index+2]);
                    _Y = GET_VALUE_IN_RANGE(_Y, 0, 255);
                    temp_Yavg = temp_Yavg + _Y;
                }
            }
            _Yavg = temp_Yavg / (Width * Height);
        }

        BOOL CalYavgExp(const unsigned char* pBmpBuffer, int Width, int Height, double &_Yavg)
        {
            BOOL bIsExp = FALSE;
            const int Y_EXP_VALUE = 220;
            double temp_Yavg = 0;
            double temp_RYavg = 0;
            double temp_GYavg = 0;
            double temp_BYavg = 0;
            for (int y = 0; y < Height; y++)
            {
                for (int x = 0; x < Width; x++)
                {
                    int _index = (y * Width + x) * 3;
                    double _Y = YVALUE(pBmpBuffer[_index + 0], pBmpBuffer[_index + 1], pBmpBuffer[_index + 2]);
                    _Y = GET_VALUE_IN_RANGE(_Y, 0, 255);		
                    temp_Yavg = temp_Yavg + _Y;	
                    temp_BYavg = temp_BYavg + pBmpBuffer[_index + 0];
                    temp_GYavg = temp_GYavg + pBmpBuffer[_index + 1];
                    temp_RYavg = temp_RYavg + pBmpBuffer[_index + 2];
                }
            }

            _Yavg = temp_Yavg / (Width * Height);
            temp_BYavg = temp_BYavg / (Width * Height);
            temp_GYavg = temp_GYavg / (Width * Height);
            temp_RYavg = temp_RYavg / (Width * Height);

            if (temp_RYavg > Y_EXP_VALUE || temp_GYavg > Y_EXP_VALUE || temp_BYavg > Y_EXP_VALUE)
            {
                bIsExp = TRUE;
            }
            return bIsExp;
        }

        BOOL IsBlockPartical(
            const int *pParticalTable,
            int nWidth,
            int nHeight,
            int nParticalSizeX,
            int nParticalSizeY,
            const POINT &ptStartPoint)
        {
            int i, j;
            for (j = 0; j < nParticalSizeY; j++)
            {
                for (i = 0; i < nParticalSizeX; i++)
                {
                    int x = ptStartPoint.x + i;
                    int y = ptStartPoint.y + j;
                    if (x >= nWidth)
                    {
                        continue;
                    }
                    if (y >= nHeight)
                    {
                        continue;
                    }
                    if (0 == pParticalTable[y * nWidth + x])
                    {
                        return FALSE;
                    }
                }
            }
            return TRUE;
        }

        void ReverseBlock(
            int *pParticalTable,
            int nWidth,
            int nHeight,
            int nParticalSizeX,
            int nParticalSizeY,
            const POINT &ptStartPoint)
        {
            int i, j;
            for (j = 0; j < nParticalSizeY; j++)
            {
                for (i = 0; i < nParticalSizeX; i++)
                {
                    int x = ptStartPoint.x + i;
                    int y = ptStartPoint.y + j;
                    if (x >= nWidth)
                    {
                        continue;
                    }
                    if (y >= nHeight)
                    {
                        continue;
                    }
                    pParticalTable[y * nWidth + x] = 0;
                }
            }
        }

        BOOL FindOnePartical(
            const int *pParticalTable,
            int nWidth,
            int nHeight,
            int nParticalSizeX,
            int nParticalSizeY,
            POINT &ptStartPoint)
        {
            int i, j;
            int nMinSize = min(nParticalSizeX, nParticalSizeY);
            for (j = 0; j < nHeight - nMinSize; j++)
            {
                for (i = 0; i < nWidth - nMinSize; i++)
                {
                    ptStartPoint.x = i;
                    ptStartPoint.y = j;
                    if (IsBlockPartical(
                        pParticalTable,
                        nWidth,
                        nHeight,
                        nParticalSizeX,
                        nParticalSizeY,
                        ptStartPoint))
                    {
                        return TRUE;
                    }

                    if (IsBlockPartical(
                        pParticalTable,
                        nWidth,
                        nHeight,
                        nParticalSizeY,
                        nParticalSizeX,
                        ptStartPoint))
                    {
                        return TRUE;
                    }
                }
            }
            return FALSE;
        }


        BOOL FindMultiPartical(
            const int *pParticalTable,
            int nWidth,
            int nHeight,
            int nParticalSizeX,
            int nParticalSizeY,
            int nMaxCount,
            vector<POINT> &vecPtPartical)
        {
            BOOL bFind = FALSE;
            POINT ptStartPoint;
            int i, j;
            int nMinSize = min(nParticalSizeX, nParticalSizeY);
            int *pTable = new int[nWidth * nHeight];
            memcpy(pTable, pParticalTable, sizeof(int) * nWidth * nHeight);

            for (j = 0; j < nHeight - nMinSize; j++)
            {
                for (i = 0; i < nWidth - nMinSize; i++)
                {
                    ptStartPoint.x = i;
                    ptStartPoint.y = j;
                    if (IsBlockPartical(
                        pTable,
                        nWidth,
                        nHeight,
                        nParticalSizeX,
                        nParticalSizeY,
                        ptStartPoint))
                    {
                        bFind = TRUE;
                        vecPtPartical.push_back(ptStartPoint);
                        // 抹掉
                        ReverseBlock(
                            pTable,
                            nWidth,
                            nHeight,
                            nParticalSizeX,
                            nParticalSizeY,
                            ptStartPoint);
                        if ((int)(vecPtPartical.size()) >= nMaxCount)
                        {
                            goto end;
                        }
                    }

                    if (nParticalSizeX != nParticalSizeY)
                    {
                        if (IsBlockPartical(
                            pTable,
                            nWidth,
                            nHeight,
                            nParticalSizeY,
                            nParticalSizeX,
                            ptStartPoint))
                        {
                            bFind = TRUE;
                            vecPtPartical.push_back(ptStartPoint);
                            // 抹掉
                            ReverseBlock(
                                pTable,
                                nWidth,
                                nHeight,
                                nParticalSizeY,
                                nParticalSizeX,
                                ptStartPoint);
                            if ((int)(vecPtPartical.size()) >= nMaxCount)
                            {
                                goto end;
                            }
                        }
                    }
                }
            }

end:
            RELEASE_ARRAY(pTable);
            return bFind;
        }

		void Sleep(int msec)
		{
			clock_t start_time,end_time,total_time; 

			start_time = clock();
			while((clock() - start_time) < (msec * CLOCKS_PER_SEC/1000))
			{ 
				MSG msg;
				while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if(msg.message==WM_QUIT) return ;
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				};
			}
			end_time = clock();
			total_time=end_time-start_time;	
			/*
			你的Thread是收发室老头儿，老头儿的任务就是接受信件(GetMessage),然后立即送出(DispatchMessage)。
			邮递员不停地来送信(SendMessage), 你这个函数，老头决定休息一会儿，不送信了（DispatchMessage),但是不能让邮递员干等，人还要干别的事情。
			所以老头还是瞄了一眼有没有邮递员（PeekMessage），有的话就把信拿到收发室里。 
			这样邮递员就走了，信留在收发室(Message Queue)。
			这时候因为不想送信（DispatchMessage)，所以一定不能把信给扔了（PM_NOREMOVE),否则信丢了。
			等老头休息完了，就可以把信从收发室拿出来(GetMessage)，再Dispatch出去。
			所以PeekMessage如果后面没有紧跟DispatchMessage， 一定得用PM_NOREMOVE（除非老头儿想过滤，比如把骚扰的信扔了）.
			*/
		}
    }   // namespace Algorithm
}   // namespace UTS
