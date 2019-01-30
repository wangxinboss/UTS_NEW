#include "StdAfx.h"
#include "Algorithm.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace WB
        {
            int WBCalibration(uint8_t* pRaw8, int width, int height, int rawBayerType, 
                int roiStartX, int roiStartY, int roiWidth, int roiHeight,
                double blackLvl, double* avgRGrGbB, uint8_t* RGrGbB
                )
            {
                int ret = -1;
                // Reset
                memset(avgRGrGbB,0,sizeof(double)*4);
                memset(RGrGbB,0,sizeof(uint8_t)*4);
                //-------------------------------------------------
                /*
                1. Region of interest(ROI) definition :
                Size : 1/5 Width_Resolution *1/5 Height_Resolution
                Position : Center
                */
                if (!pRaw8) return -1;

                if (roiWidth == 0 || roiHeight == 0)
                {
                    roiWidth = width/10;
                    roiHeight = height/10;
                    roiStartX = (width-roiWidth)/2;
                    roiStartY = (height-roiHeight)/2;
                }
                if (roiStartX & 0x01){roiStartX += 1;}
                if (roiStartY & 0x01){roiStartY += 1;}
                if (roiWidth & 0x01){roiWidth += 1;}
                if (roiHeight & 0x01){roiHeight += 1;}

                uint8_t* roiBuf = new uint8_t[roiWidth*roiHeight];
                memset(roiBuf, 0, roiWidth*roiHeight);
                for (int y=0;y<roiHeight;y++)
                {
                    for (int x=0;x<roiWidth;x++)
                    {
                        int X = x + roiStartX;
                        int Y = y + roiStartY;
                        roiBuf[y*roiWidth+x] = pRaw8[Y*width+X];
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
                        ChannelA += double(roiBuf[(y*roiWidth+x)]);
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


                /*
                5. Convert float to integer
                R/G = int(RG * 512 + 0.5)
                B/G = int(BG * 512 + 0.5)
                */
                unsigned char R = (unsigned char)fnRange(fnRound((avgRGrGbB[0])),0,255);
                unsigned char Gr = (unsigned char)fnRange(fnRound((avgRGrGbB[1])),0,255);
                unsigned char Gb = (unsigned char)fnRange(fnRound((avgRGrGbB[2])),0,255);
                unsigned char B = (unsigned char)fnRange(fnRound((avgRGrGbB[3])),0,255);
                RGrGbB[0] = R;
                RGrGbB[1] = Gr;
                RGrGbB[2] = Gb;
                RGrGbB[3] = B;
                ret = 0;
				RELEASE_ARRAY(roiBuf);

                return ret;
            }

			int WBCalibration(WORD* pRaw10, int width, int height, int rawBayerType, 
				int roiStartX, int roiStartY, int roiWidth, int roiHeight,
				double blackLvl, double* avgRGrGbB,  unsigned short* RGrGbB
				)
			{
				int ret = -1;
                // Reset
                memset(avgRGrGbB,0,sizeof(double)*4);
                memset(RGrGbB,0,sizeof(short)*4);
                //-------------------------------------------------
                /*
                1. Region of interest(ROI) definition :
                Size : 1/5 Width_Resolution *1/5 Height_Resolution
                Position : Center
                */
                if (!pRaw10) return -1;

                if (roiWidth == 0 || roiHeight == 0)
                {
                    roiWidth = width/10;
                    roiHeight = height/10;
                    roiStartX = (width-roiWidth)/2;
                    roiStartY = (height-roiHeight)/2;
                }
                if (roiStartX & 0x01){roiStartX += 1;}
                if (roiStartY & 0x01){roiStartY += 1;}
                if (roiWidth & 0x01){roiWidth += 1;}
                if (roiHeight & 0x01){roiHeight += 1;}

                WORD* roiBuf = new WORD[roiWidth*roiHeight];
                memset(roiBuf, 0, roiWidth*roiHeight);
                for (int y=0;y<roiHeight;y++)
                {
                    for (int x=0;x<roiWidth;x++)
                    {
                        int X = x + roiStartX;
                        int Y = y + roiStartY;
                        roiBuf[y*roiWidth+x] = pRaw10[Y*width+X];
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
                        ChannelA += double(roiBuf[(y*roiWidth+x)]);
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


                /*
                5. Convert float to integer
                R/G = int(RG * 512 + 0.5)
                B/G = int(BG * 512 + 0.5)
                */
                unsigned short R = (unsigned short)fnRange(fnRound((avgRGrGbB[0])),0,1023);
                unsigned short Gr = (unsigned short)fnRange(fnRound((avgRGrGbB[1])),0,1023);
                unsigned short Gb = (unsigned short)fnRange(fnRound((avgRGrGbB[2])),0,1023);
                unsigned short B = (unsigned short)fnRange(fnRound((avgRGrGbB[3])),0,1023);
                RGrGbB[0] = R;
                RGrGbB[1] = Gr;
                RGrGbB[2] = Gb;
                RGrGbB[3] = B;
                ret = 0;
				RELEASE_ARRAY(roiBuf);

                return ret;
			}
        }
    }
}