#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::Image;

namespace UTS
{
    namespace Algorithm
    {
        namespace ColorShading
        {
            namespace ColorShading_CenterCorner
            {
                void ColorShading(
                    const unsigned char* pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const CS_PARAM &param,
                    CS_RESULT &result)
                {
                    // Center
                    result.roi.rcCenter.left = (nWidth - param.sizeROI.cx) / 2;
                    result.roi.rcCenter.right = result.roi.rcCenter.left + param.sizeROI.cx;
                    result.roi.rcCenter.top = (nHeight - param.sizeROI.cy) / 2;
                    result.roi.rcCenter.bottom = result.roi.rcCenter.top + param.sizeROI.cy;
                    // Corner LU
                    result.roi.rcCorner[Corner_LU].left = 0;
                    result.roi.rcCorner[Corner_LU].right = param.sizeROI.cx;
                    result.roi.rcCorner[Corner_LU].top = 0;
                    result.roi.rcCorner[Corner_LU].bottom = param.sizeROI.cy;
                    // Corner LD
                    result.roi.rcCorner[Corner_LD].left = 0;
                    result.roi.rcCorner[Corner_LD].right = param.sizeROI.cx;
                    result.roi.rcCorner[Corner_LD].top = nHeight - param.sizeROI.cy;
                    result.roi.rcCorner[Corner_LD].bottom = nHeight;
                    // Corner RU
                    result.roi.rcCorner[Corner_RU].left = nWidth - param.sizeROI.cx;
                    result.roi.rcCorner[Corner_RU].right = nWidth;
                    result.roi.rcCorner[Corner_RU].top = 0;
                    result.roi.rcCorner[Corner_RU].bottom = param.sizeROI.cy;
                    // Corner RD
                    result.roi.rcCorner[Corner_RD].left = nWidth - param.sizeROI.cx;
                    result.roi.rcCorner[Corner_RD].right = nWidth;
                    result.roi.rcCorner[Corner_RD].top = nHeight - param.sizeROI.cy;
                    result.roi.rcCorner[Corner_RD].bottom = nHeight;

                    // Center
                    GetROIAvgRGB(
                        pBmpBuffer,
                        nWidth,
                        nHeight,
                        result.roi.rcCenter,
                        result.rgbCenter);
                    ColorSpace::RGBDouble2Lab(
                        result.rgbCenter.dR,
                        result.rgbCenter.dG,
                        result.rgbCenter.dB,
                        result.labCenter.dL,
                        result.labCenter.dA,
                        result.labCenter.dB);

                    // Corner
                    double dMinC = 999999.0;
                    double dMaxC = 0.0;
                    double dMinE = 999999.0;
                    double dMaxE = 0.0;
                    for (int i = 0; i < Corner_SIZES; i++)
                    {
                        GetROIAvgRGB(
                            pBmpBuffer,
                            nWidth,
                            nHeight,
                            result.roi.rcCorner[i],
                            result.rgbCorner[i]);
                        ColorSpace::RGBDouble2Lab(
                            result.rgbCorner[i].dR,
                            result.rgbCorner[i].dG,
                            result.rgbCorner[i].dB,
                            result.labCorner[i].dL,
                            result.labCorner[i].dA,
                            result.labCorner[i].dB);
                        result.dDeltaC[i] = Math::GetDistance2D(
                            result.labCenter.dA,
                            result.labCenter.dB,
                            result.labCorner[i].dA,
                            result.labCorner[i].dB);
                        result.dDeltaE[i] = Math::GetDistance3D(
                            result.labCenter.dL,
                            result.labCenter.dA,
                            result.labCenter.dB,
                            result.labCorner[i].dL,
                            result.labCorner[i].dA,
                            result.labCorner[i].dB);
                        dMinC = min(dMinC, result.dDeltaC[i]);
                        dMaxC = max(dMaxC, result.dDeltaC[i]);
                        dMinE = min(dMinE, result.dDeltaE[i]);
                        dMaxE = max(dMaxE, result.dDeltaE[i]);
                    }
                    result.dMaxDeltaC = dMaxC;
                    result.dMaxDeltaE = dMaxE;
                    result.dDeltaCDelta = abs(dMaxC - dMinC);
                    result.dDeltaEDelta = abs(dMaxE - dMinE);
                }
            }
		}

		namespace ColorShading_NOKIA
		{
			void ColorShadingRatio(
				unsigned char *pRaw8Buffer,
				int nWidth,
				int nHeight,
				int nBayerType,
				double blackLvl,
				int nROISizeX,
				int nROISizeY,
				CS_RATIO_NOKIA_RESULT &result)
			{
				COLOR_RGrGbB_DOUBLE dRGrGbB[5];
				COLOR_RGrGbB_UCHAR   RGrGbB[5];

				int X[5] ={-1,1,0,-1,1};
				int Y[5] ={-1,-1,0,1,1};

				int startx,starty;

				double data_d[4];
				uint8_t data_u[4];

				for(int i = 0 ; i < 5 ; i++)
				{
					startx = (nWidth/2) + (nWidth/2 - (nROISizeX/2))*X[i] - (nROISizeX/2);
					starty = (nHeight/2)+ (nHeight/2 -(nROISizeY/2))*Y[i] - (nROISizeY/2);

					UTS::Algorithm::WB::WBCalibration(pRaw8Buffer, 
						nWidth, nHeight, nBayerType,
						startx, starty, nROISizeX, nROISizeY,
						blackLvl, data_d, 
						data_u);

					memcpy(&dRGrGbB[i],data_d,sizeof(double)*4);
					memcpy(&RGrGbB[i],data_u,sizeof(uint8_t)*4);
				}

				result.dCenterRatioRG = dRGrGbB[2].R/((dRGrGbB[2].Gb + dRGrGbB[2].Gr)/2.0);
				result.dCenterRatioBG = dRGrGbB[2].B/((dRGrGbB[2].Gb + dRGrGbB[2].Gr)/2.0);
				result.dCenterRatioGrGb = dRGrGbB[2].Gr / dRGrGbB[2].Gb;
				result.dCornerRatioRG[0] = dRGrGbB[0].R/((dRGrGbB[0].Gb + dRGrGbB[0].Gr)/2.0);
				result.dCornerRatioBG[0] = dRGrGbB[0].B/((dRGrGbB[0].Gb + dRGrGbB[0].Gr)/2.0);
				result.dCornerRatioRG[1] = dRGrGbB[1].R/((dRGrGbB[1].Gb + dRGrGbB[1].Gr)/2.0);
				result.dCornerRatioBG[1] = dRGrGbB[1].B/((dRGrGbB[1].Gb + dRGrGbB[1].Gr)/2.0);
				result.dCornerRatioRG[2] = dRGrGbB[3].R/((dRGrGbB[3].Gb + dRGrGbB[3].Gr)/2.0);
				result.dCornerRatioBG[2] = dRGrGbB[3].B/((dRGrGbB[3].Gb + dRGrGbB[3].Gr)/2.0);
				result.dCornerRatioRG[3] = dRGrGbB[4].R/((dRGrGbB[4].Gb + dRGrGbB[4].Gr)/2.0);
				result.dCornerRatioBG[3] = dRGrGbB[4].B/((dRGrGbB[4].Gb + dRGrGbB[4].Gr)/2.0);
			}

			void ColorShadingDelta(
				unsigned char* pBmpBuffer,
				int nWidth,
				int nHeight,
				int nROISizeX,
				int nROISizeY,
				double *MeanA,
				double *MeanB,
				CS_DELTA_NOKIA_RESULT &result)
			{
				int X[5] ={-1,1,0,-1,1};
				int Y[5] ={-1,-1,0,1,1};

				int startx,starty;
				COLOR_RGB_DOUBLE rgb;
				RECT rect;
				double dMaxC = 0.0;

				for(int i = 0 ; i < 5 ; i++)
				{
					startx = (nWidth/2) + (nWidth/2 - (nROISizeX/2))*X[i] - (nROISizeX/2);
					starty = (nHeight/2)+ (nHeight/2 -(nROISizeY/2))*Y[i] - (nROISizeY/2);

					rect.left = startx;
					rect.top =  starty;
					rect.right = rect.left + nROISizeX;
					rect.bottom = rect.top + nROISizeY;

					GetROIAvgRGB(
						pBmpBuffer,
						nWidth,
						nHeight,
						rect,
						rgb);

					ColorSpace::RGBDouble2Lab(
						rgb.dR,
						rgb.dG,
						rgb.dB,
						result.lab[i].dL,
						result.lab[i].dA,
						result.lab[i].dB);

					result.dDeltaC[i] = Math::GetDistance2D(
						result.lab[i].dA,
						result.lab[i].dB,
						MeanA[i],
						MeanB[i]);

					 dMaxC = max(dMaxC, result.dDeltaC[i]);
				}

				result.dMaxDeltaC = dMaxC;
			}
		}
		namespace ColorShadingA
		{
			namespace ColorShading_Matrix
			{
				//void ColorShading(const unsigned char* pBmpBuffer,int nWidth,int nHeight,const CS_PARAM &param,	CS_RESULT &result)
				void ColorShading(
					unsigned char* pBmpBuffer, 
					int nWidth,
					int nHeight,
					int nHBlockCount,
					int nVBlockCount,
					double &MaxDeltaC)
				{

					//	每一塊ROI的尺寸（忽略無法整除的問題）
					int ROISizeW = nWidth/nHBlockCount;
					int ROISizeH = nHeight/nVBlockCount;

					double* BlockRGB= new double[nHBlockCount*nVBlockCount*3];
					memset(BlockRGB,0,sizeof(double)*nHBlockCount*nVBlockCount*3);

					double* BlockLab = new double[nHBlockCount*nVBlockCount*3];
					memset(BlockLab,0,sizeof(double)*nHBlockCount*nVBlockCount*3);

					for (int IndexH = 0;IndexH<nVBlockCount;IndexH++)
					{
						for (int IndexW = 0;IndexW<nHBlockCount;IndexW++)
						{
							int BlockIndex = (IndexH*nHBlockCount+IndexW)*3;		
							for (int y=0;y<ROISizeH;y++)
							{
								for (int x=0;x<ROISizeW;x++)
								{											
									int X = (IndexW*ROISizeW)+x;
									int Y = (IndexH*ROISizeH)+y;
									int ImageIndex = (Y*nWidth+X)*3;
									BlockRGB[BlockIndex+0] += pBmpBuffer[ImageIndex+0];
									BlockRGB[BlockIndex+1] += pBmpBuffer[ImageIndex+1];
									BlockRGB[BlockIndex+2] += pBmpBuffer[ImageIndex+2];
								}
							}
							BlockRGB[BlockIndex+0] /= double(ROISizeW*ROISizeH);
							BlockRGB[BlockIndex+1] /= double(ROISizeW*ROISizeH);
							BlockRGB[BlockIndex+2] /= double(ROISizeW*ROISizeH);


							double dRGB[3] = {0};
							dRGB[0] = BlockRGB[BlockIndex+0];
							dRGB[1] = BlockRGB[BlockIndex+1];
							dRGB[2] = BlockRGB[BlockIndex+2];

							double dLab[3] = {0};
							ColorSpace::RGBDouble2Lab(
								dRGB[0],
								dRGB[1],
								dRGB[2],
								dLab[0],
								dLab[1],
								dLab[2]);


							BlockLab[BlockIndex+0] = dLab[0];
							BlockLab[BlockIndex+1] = dLab[1];
							BlockLab[BlockIndex+2] = dLab[2];
						}
					}
					//計算C，以便求得Max△C
					/*double*/ MaxDeltaC = 0;

					for (int IndexH = 0;IndexH<nVBlockCount;IndexH++)
					{
						for (int IndexW = 0;IndexW<nHBlockCount;IndexW++)
						{
							int BlockIndex = (IndexH*nHBlockCount+IndexW)*3;
							double a1 = BlockLab[BlockIndex+1];
							double b1 = BlockLab[BlockIndex+2];

							for (int IndexH2 = IndexH;IndexH2<nVBlockCount;IndexH2++)
							{
								for (int IndexW2 =IndexW;IndexW2<nHBlockCount;IndexW2++)
								{
									int BlockIndex2 = (IndexH2*nHBlockCount+IndexW2)*3;
									double a2 = BlockLab[BlockIndex2+1];
									double b2 = BlockLab[BlockIndex2+2];

									double DeltaC = pow(((a1-a2)*(a1-a2)+(b1-b2)*(b1-b2)),0.5);
									if (MaxDeltaC < DeltaC)
									{
										MaxDeltaC = DeltaC;
									}

								}
							}
						}
					}
					RELEASE_ARRAY(BlockRGB);
					RELEASE_ARRAY(BlockLab);
				}
			}
		}
    }
}
