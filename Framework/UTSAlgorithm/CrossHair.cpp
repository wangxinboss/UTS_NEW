#include "StdAfx.h"
#include "Algorithm.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace CrossHair
        {
            CCrossHair::CCrossHair(void)
            {
            }

            CCrossHair::~CCrossHair(void)
            {
            }

            //-------------------------------------------------------------------------
            // 设置十字ROI
            void CCrossHair::SetROI(
                int DisXfromCT,
                int DisYfromCT,
                int ROISize,
                double CodeVariation)
            {
                m_CrossHairDisXfromCT = DisXfromCT;
                m_CrossHairDisYfromCT = DisYfromCT;
                m_CrossHairROISize = ROISize;
                m_CrossHair_Code_Variation = CodeVariation;
            }

            //-------------------------------------------------------------------------
            // 返回单个十字的中心
            POINT CCrossHair::Cross_LinePosition(
                const unsigned char *pBmpBuffer,
                int nWidth,
                int nHeight,
                int xstart,
                int ystart,
                int boxwidth,
                int boxheight,
                float *crossMaxvalue)
            {
                POINT		return_pos;
                BYTE		pixel;
                int			horizontal_sum1=0, horizontal_sum2=0, sum_avg=0;
                int			vertical_sum1=0, vertical_sum2=0;
                float		avg=0;
                float		vmax_r=0,vmax_f=0, hmax_r=0,  hmax_f=0;
                float		vmax_value=0.0, hmax_value=0.0;
                int			horizontal_line1=-1, vertical_line1=-1,horizontal_line2=-1, vertical_line2=-1;
                int			i, j, count=0;
                float		vertical_result=-1, horizontal_result=-1;
                double		code_variation;

                *crossMaxvalue = 0.0;

                // average green
                for (i=ystart; i<ystart+boxheight;i++)
                {
                    for (j=xstart; j<xstart+boxwidth; j++)
                    {
                        pixel = pBmpBuffer[PIXEL_G(j,i,nWidth,nHeight)];
                        sum_avg += pixel;
                        count++;
                    }
                }
                avg = sum_avg/(float)count;

                //code_variation = 3; 
                //code_variation = m_CrossHair_Code_Variation;		// default = 0.5 , v1.1.2 , Cindy
                code_variation = 0;
                //horizontal
                for (i=ystart; i<ystart+boxheight-3;i++)
                {
                    for (j=xstart; j<xstart+boxwidth; j++)
                    {
                        //pixel = (int)(.299 * (double)(pBmp[PIXEL_R(j,i,width,height)]) + .587 * (double)(pBmp[PIXEL_G(j,i,width,height)]) + .114 * (double)(pBmp[PIXEL_B(j,i,width,height)]));
                        pixel = pBmpBuffer[PIXEL_G(j,i,nWidth,nHeight)];
                        horizontal_sum1 += pixel;
                        //pixel = (int)(.299 * (double)(pBmp[PIXEL_R(j,i+3,width,height)]) + .587 * (double)(pBmp[PIXEL_G(j,i+3,width,height)]) + .114 * (double)(pBmp[PIXEL_B(j,i+3,width,height)]));
                        pixel = pBmpBuffer[PIXEL_G(j,i+3,nWidth,nHeight)];
                        horizontal_sum2 += pixel;
                    }

                    //falling
                    if (hmax_f < (horizontal_sum1 - horizontal_sum2))
                    {
                        hmax_f = (float)abs(horizontal_sum1 - horizontal_sum2);
                        if (hmax_f >= (nWidth/2)*code_variation ) horizontal_line1 = i; //1200
                    }

                    //rising
                    if (hmax_r < (horizontal_sum2 - horizontal_sum1)) {
                        hmax_r = (float)abs(horizontal_sum2 - horizontal_sum1);
                        if (hmax_r >= (nWidth/2)*code_variation ) {
                            horizontal_line2 = i+3;
                            hmax_value = hmax_r;  //horizon max save
                        }
                    }
                    horizontal_sum1 = horizontal_sum2 = 0;
                }
                if (horizontal_line1 == -1 || horizontal_line2 == -1  ) horizontal_result=0;
                else
                {
                    int center_line = nHeight/2;
                    horizontal_result = (float)((horizontal_line1+horizontal_line2)/2.0);
                }

                //vertical
                for (i=xstart; i<xstart+boxwidth-1;i++) {
                    for (j=ystart; j<ystart+boxheight; j++) {
                        //pixel = (int)(.299 * (double)(pBmp[PIXEL_R(i,j,width,height)]) + .587 * (double)(pBmp[PIXEL_G(i,j,width,height)]) + .114 * (double)(pBmp[PIXEL_B(i,j,width,height)]));
                        pixel = pBmpBuffer[PIXEL_G(i,j,nWidth,nHeight)];
                        vertical_sum1 += pixel;
                        //pixel = (int)(.299 * (double)(pBmp[PIXEL_R(i+3,j,width,height)]) + .587 * (double)(pBmp[PIXEL_G(i+3,j,width,height)]) + .114 * (double)(pBmp[PIXEL_B(i+3,j,width,height)]));
                        pixel = pBmpBuffer[PIXEL_G(i+3,j,nWidth,nHeight)];
                        vertical_sum2 += pixel;
                    }
                    //falling
                    if (vmax_f < (vertical_sum1 - vertical_sum2)) {
                        vmax_f = (float)abs(vertical_sum1 - vertical_sum2);
                        if (vmax_f >= (nHeight/2)*code_variation ) vertical_line1 = i; //1200
                    }
                    //rising
                    if (vmax_r < (vertical_sum2 - vertical_sum1)) {
                        vmax_r = (float)abs(vertical_sum1 - vertical_sum2);
                        if (vmax_r >= (nHeight/2)*code_variation ) {
                            vertical_line2 = i+3;
                            vmax_value = vmax_r;
                        }
                    }
                    vertical_sum1 = vertical_sum2 = 0;
                }

                if (vertical_line1 == -1 || vertical_line2 == -1) vertical_result=0;
                else
                {
                    int center_line = nWidth/2;

                    vertical_result =(float)((vertical_line1+vertical_line2)/2.0);
                }

                if (vertical_line1 == -1 || horizontal_line1 == -1 || vertical_line2 == -1 || horizontal_line2 == -1){
                    return_pos.x = -1;		
                    return_pos.y = -1;		
                    *crossMaxvalue = 0.0;   
                }
                else
                {
                    return_pos.x =(long)vertical_result;
                    return_pos.y = (long)horizontal_result;
                    *crossMaxvalue = (hmax_value+vmax_value);
                }
                return return_pos;
            }

            BOOL CCrossHair::Calculate(BYTE *pBmpBuffer, int nWidth, int nHeight)
            {
                BOOL bResult = TRUE;

                //-------------------------------------------------------------------------
                // 取得十字的ROI(BMP基准)
                int signXY[Cross_Hair_Num][2] = { {-1,0} , {1,0} , {0,-1} , {0,1} }; //left, right, top, bottom
                for (int i = 0; i < Cross_Hair_Num; i++)   
                {
                    int boxX = nWidth / 2 + (m_CrossHairDisXfromCT) * signXY[i][0]; 
                    int boxY = nHeight / 2 + (m_CrossHairDisYfromCT) * signXY[i][1];
                    rcCrossHairDetectROI[i].left = boxX - m_CrossHairROISize / 2;
                    rcCrossHairDetectROI[i].top = boxY - m_CrossHairROISize / 2;
                    rcCrossHairDetectROI[i].right = boxX + m_CrossHairROISize / 2;
                    rcCrossHairDetectROI[i].bottom = boxY + m_CrossHairROISize / 2;
                }
                // 	WhitePatchROI.left=centerX-AA_WhitePatchDisXfromCT;
                // 	WhitePatchROI.top=centerY-AA_WhitePatchDisYfromCT;
                // 	WhitePatchROI.right=WhitePatchROI.left+AA_WhitePatchROISize;
                // 	WhitePatchROI.bottom=WhitePatchROI.top+AA_WhitePatchROISize;

                for (int i = 0; i < Cross_Hair_Num; i++)
                {
                    int boxwidth = m_CrossHairROISize;
                    int boxheight = m_CrossHairROISize;
                    //first check suitable range or not
                    if (boxwidth < 1 || boxheight < 1) 
                        continue;
                    if (rcCrossHairDetectROI[i].left < 1 || rcCrossHairDetectROI[i].top < 1) 
                        continue;
                    if (rcCrossHairDetectROI[i].right >= nWidth || rcCrossHairDetectROI[i].bottom >= nHeight) 
                        continue;

                    //-------------------------------------------------------------------------
                    //Find Scan Box CrossHairCenter Pisition Jason 
                    float crossvalue;
                    POINT patternPos = Cross_LinePosition(
                        pBmpBuffer,
                        nWidth,
                        nHeight,
                        rcCrossHairDetectROI[i].left,
                        rcCrossHairDetectROI[i].top,
                        boxwidth,
                        boxheight,
                        &crossvalue);  
                    //patternPos = Cross_Search_LinePosition_PMX(pBmp, width, height, crossHairScanBox[i].l, crossHairScanBox[i].t, boxwidth, boxheight, &crossvalue);	// Rickson , 6/29
                    if (patternPos.x == -1 || patternPos.y == -1) 
                    {
                        ptCrossHairCenter[i].x = rcCrossHairDetectROI[i].left + boxwidth / 2;
                        ptCrossHairCenter[i].y = rcCrossHairDetectROI[i].top + boxheight / 2;
                        bResult = FALSE;
                    }
                    else 
                    {
                        ptCrossHairCenter[i].x = patternPos.x;    //crossHairCenterXY[i][0]  x Position [i][1] y Position  CrossHairCenterPositon Result(x,y) 
                        ptCrossHairCenter[i].y = patternPos.y;
                    }

                }

                //-------------------------------------------------------------------------
                // 算法改进，以前的算法只能算矩形
                //AA_ChartRealCenterX = (int)(
                //       ( AA_CrossHairCenterPos[Cross_Hair_Left][0]
                //       + AA_CrossHairCenterPos[Cross_Hair_Right][0]
                //       + AA_CrossHairCenterPos[Cross_Hair_Top][0]
                //       + AA_CrossHairCenterPos[Cross_Hair_Bottom][0]) / 4.0 + 0.5);
                //AA_ChartRealCenterY = (int)(
                //       ( AA_CrossHairCenterPos[Cross_Hair_Left][1]
                //       + AA_CrossHairCenterPos[Cross_Hair_Right][1]
                //       + AA_CrossHairCenterPos[Cross_Hair_Top][1]
                //       + AA_CrossHairCenterPos[Cross_Hair_Bottom][1]) / 4.0 + 0.5);
                Algorithm::Math::GetCrossPoint(
                    ptCrossHairCenter[Cross_Hair_Left].x, ptCrossHairCenter[Cross_Hair_Left].y,
                    ptCrossHairCenter[Cross_Hair_Right].x, ptCrossHairCenter[Cross_Hair_Right].y,
                    ptCrossHairCenter[Cross_Hair_Top].x, ptCrossHairCenter[Cross_Hair_Top].y,
                    ptCrossHairCenter[Cross_Hair_Bottom].x, ptCrossHairCenter[Cross_Hair_Bottom].y,
                    ptChartCenter.x, ptChartCenter.y);

                //dCrossHairWidth = Algorithm::Math::GetDistance2D(
                //    (double)ptCrossHairCenter[Cross_Hair_Left].x,
                //    (double)ptCrossHairCenter[Cross_Hair_Left].y,
                //    (double)ptCrossHairCenter[Cross_Hair_Right].x,
                //    (double)ptCrossHairCenter[Cross_Hair_Right].y);
                //dCrossHairHeight = Algorithm::Math::GetDistance2D(
                //    (double)ptCrossHairCenter[Cross_Hair_Top].x,
                //    (double)ptCrossHairCenter[Cross_Hair_Top].y,
                //    (double)ptCrossHairCenter[Cross_Hair_Bottom].x,
                //    (double)ptCrossHairCenter[Cross_Hair_Bottom].y);
                return bResult;
            }
        }   // namespace CrossHair
    }   // namespace Algorithm
}   // namespace UTS
