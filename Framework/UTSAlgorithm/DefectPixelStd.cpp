#include "StdAfx.h"
#include "Algorithm.h"
#include "CtLibrary.h"

using namespace UTS::Algorithm::Math;
using namespace UTS::Algorithm::Image;

namespace UTS
{
    namespace Algorithm
    {
        namespace DefectPixel
        {
            namespace DefectPixelStd
            {
                void DefectPixels(
                    unsigned char *_pRAWImage,
                    int Width,
                    int Height,
                    double DefThreshold,
                    int ROIWidth,
                    int ROIHeight,
                    int AA_DefectPixel3ChannelEn,
                    H4ISPDefectInfo &DefectCount,
                    AA_TwoDPoint *DefectTable,
                    int RAWBayerType)
                {
                    //-----------------------------------------------------------------
                    int TotalSingleDefectCount = 0;
                    int TotalDoubleDefectCount = 0;
                    int TotalMultipleDefectCount = 0;
                    int CDefectCount = 0;
                    int H4ISPDefectPixelCount = 0;
                    double deviation30x40 = 0;
                    //-------------------------------------------------------------------------
                    int CutImageWidth = Width;
                    int CutImageHeight = Height;
                    int StartX = 0;
                    int StartY = 0;
                    //	演算基礎，需有一? Defect Table（已標?所有Defect Point的狀況，應為一0/1 Table）
                    //	建立 Defect Table
                    //	先不管G1、G2的一致性，?成不同的Channel
                    //	RGRG
                    //	GBGB
                    int RawWidth = CutImageWidth/2;
                    int RawHeight = CutImageHeight/2;

                    unsigned char* RPlane	= new unsigned char[RawWidth*RawHeight];
                    memset(RPlane,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    unsigned char* G1Plane	= new unsigned char[RawWidth*RawHeight];
                    memset(G1Plane,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    unsigned char* G2Plane	= new unsigned char[RawWidth*RawHeight];
                    memset(G2Plane,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    unsigned char* BPlane	= new unsigned char[RawWidth*RawHeight];
                    memset(BPlane,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            RPlane[y*RawWidth+x]	= _pRAWImage[2*y*CutImageWidth+2*x];
                            G1Plane[y*RawWidth+x]	= _pRAWImage[2*y*CutImageWidth+(2*x+1)];
                            G2Plane[y*RawWidth+x]	= _pRAWImage[(2*y+1)*CutImageWidth+2*x];
                            BPlane[y*RawWidth+x]	= _pRAWImage[(2*y+1)*CutImageWidth+(2*x+1)];
                        }
                    }

                    //	full Image
                    int* DefTable = new int[CutImageWidth*CutImageHeight];
                    memset(DefTable,0,sizeof(int)*CutImageWidth*CutImageHeight);

                    //	R Channel Defect
                    double* R_AvgTable = new double[RawWidth*RawHeight];
                    memset(R_AvgTable,0,sizeof(double)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::MeanFilter2(RPlane,RawWidth,RawHeight,ROIWidth,ROIHeight,R_AvgTable);
                    unsigned char* RDefTable = new unsigned char[RawWidth*RawHeight];
                    memset(RDefTable,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    //	G1 Channel Defect
                    double* G1_AvgTable = new double[RawWidth*RawHeight];
                    memset(G1_AvgTable,0,sizeof(double)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::MeanFilter2(G1Plane,RawWidth,RawHeight,ROIWidth,ROIHeight,G1_AvgTable);
                    unsigned char* G1DefTable = new unsigned char[RawWidth*RawHeight];
                    memset(G1DefTable,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    //	G2 Channel Defect
                    double* G2_AvgTable = new double[RawWidth*RawHeight];
                    memset(G2_AvgTable,0,sizeof(double)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::MeanFilter2(G2Plane,RawWidth,RawHeight,ROIWidth,ROIHeight,G2_AvgTable);
                    unsigned char* G2DefTable = new unsigned char[RawWidth*RawHeight];
                    memset(G2DefTable,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    //	B Channel Defect
                    double* B_AvgTable = new double[RawWidth*RawHeight];
                    memset(B_AvgTable,0,sizeof(double)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::MeanFilter2(BPlane,RawWidth,RawHeight,ROIWidth,ROIHeight,B_AvgTable);
                    unsigned char* BDefTable = new unsigned char[RawWidth*RawHeight];
                    memset(BDefTable,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    double* ImDeff = new double[CutImageWidth*CutImageHeight];
                    memset(ImDeff,0,sizeof(double)*CutImageWidth*CutImageHeight);

                    //	判定 Defect
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            double DeviatesRate = 0;
                            int index = y*RawWidth+x;
                            double DiffValue = 0;
                            double _A = 0;
                            double _B = 0;

                            // R
                            _A = double(RPlane[index]);
                            _B = double(R_AvgTable[index]);
                            DiffValue = (((_A-_B) < 0)?(_B-_A):(_A-_B));
                            if (DefThreshold >= 1)
                            {
                                DeviatesRate = _A;    // Jola ,1.1.26
                            }
                            else
                            {
                                DeviatesRate = DiffValue/_B;
                            }													
                            if (DeviatesRate > DefThreshold)
                            {
                                RDefTable[index] = 1;		
                                DefTable[2*y*CutImageWidth+2*x] = 1;				
                            }
                            ImDeff[2*y*CutImageWidth+2*x] = DeviatesRate;

                            // G1
                            _A = double(G1Plane[index]);
                            _B = double(G1_AvgTable[index]);
                            DiffValue = (((_A-_B) < 0)?(_B-_A):(_A-_B));
                            if (DefThreshold >= 1)
                            {
                                DeviatesRate = _A;   // Jola ,1.1.26
                            }
                            else
                            {
                                DeviatesRate = DiffValue/_B;
                            }			
                            if (DeviatesRate > DefThreshold)
                            {
                                G1DefTable[index] = 1;	
                                DefTable[2*y*CutImageWidth+(2*x+1)] = 1;
                            }
                            ImDeff[2*y*CutImageWidth+(2*x+1)] = DeviatesRate;

                            // G2
                            _A = double(G2Plane[index]);
                            _B = double(G2_AvgTable[index]);
                            DiffValue = (((_A-_B) < 0)?(_B-_A):(_A-_B));
                            if (DefThreshold >= 1)
                            {
                                DeviatesRate = _A;   // Jola ,1.1.26
                            }
                            else
                            {
                                DeviatesRate = DiffValue/_B;
                            }			
                            if (DeviatesRate > DefThreshold)
                            {
                                G2DefTable[index] = 1;	
                                DefTable[(2*y+1)*CutImageWidth+2*x] = 1;
                            }
                            ImDeff[(2*y+1)*CutImageWidth+2*x] = DeviatesRate;

                            // B
                            _A = double(BPlane[index]);
                            _B = double(B_AvgTable[index]);
                            DiffValue = (((_A-_B) < 0)?(_B-_A):(_A-_B));
                            if (DefThreshold >= 1)
                            {
                                DeviatesRate = _A;    // Jola ,1.1.26
                            }
                            else
                            {
                                DeviatesRate = DiffValue/_B;
                            }				
                            if (DeviatesRate > DefThreshold)
                            {
                                BDefTable[index] = 1;	
                                DefTable[(2*y+1)*CutImageWidth+(2*x+1)] = 1;
                            }
                            ImDeff[(2*y+1)*CutImageWidth+(2*x+1)] = DeviatesRate;
                        }
                    }
                    RELEASE_ARRAY(RPlane);
                    RELEASE_ARRAY(G1Plane);
                    RELEASE_ARRAY(G2Plane);
                    RELEASE_ARRAY(BPlane);

                    //-------------------------------------------------------------------------
                    unsigned char* TDefectTable = new unsigned char[Width*Height];
                    memset(TDefectTable,0,sizeof(unsigned char)*Width*Height);
                    //-------------------------------------------------------------------------
                    //-------------------------------------------------------------------------
                    // 判定 Single Defect
                    //-------------------------------------------------------------------------
                    unsigned char* RSingleDefectTable = new unsigned char[RawWidth*RawHeight];
                    memset(RSingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::SumFilter(RDefTable,RawWidth,RawHeight,3,3,RSingleDefectTable);
                    int RSingleDefectCount = 0;
                    int RDoubleDefectCount = 0;
                    int RMultipleDefectCount = 0;
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            if (RSingleDefectTable[y*RawWidth+x] == 1)
                            {
                                RSingleDefectCount++;
                            }
                            else if (RSingleDefectTable[y*RawWidth+x] == 2)
                            {
                                RDoubleDefectCount++;
                            }
                            else if (RSingleDefectTable[y*RawWidth+x] >= 3)
                            {
                                RMultipleDefectCount++;
                            }
                            TDefectTable[(2*y+0)*Width+(2*x+0)] = RSingleDefectTable[y*RawWidth+x];
                        }
                    }
                    //-------------------------------------------------------------------------
                    unsigned char* G1SingleDefectTable = new unsigned char[RawWidth*RawHeight];
                    memset(G1SingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::SumFilter(G1DefTable,RawWidth,RawHeight,3,3,G1SingleDefectTable);
                    int G1SingleDefectCount = 0;
                    int G1DoubleDefectCount = 0;
                    int G1MultipleDefectCount = 0;
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            if (G1SingleDefectTable[y*RawWidth+x] == 1)
                            {
                                G1SingleDefectCount++;
                            }
                            else if (G1SingleDefectTable[y*RawWidth+x] == 2)
                            {
                                G1DoubleDefectCount++;
                            }
                            else if (G1SingleDefectTable[y*RawWidth+x] >= 3)
                            {
                                G1MultipleDefectCount++;
                            }
                            TDefectTable[(2*y+0)*Width+(2*x+1)] = G1SingleDefectTable[y*RawWidth+x];
                        }
                    }
                    //-------------------------------------------------------------------------
                    unsigned char* G2SingleDefectTable = new unsigned char[RawWidth*RawHeight];
                    memset(G2SingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::SumFilter(G2DefTable,RawWidth,RawHeight,3,3,G2SingleDefectTable);
                    int G2SingleDefectCount = 0;
                    int G2DoubleDefectCount = 0;
                    int G2MultipleDefectCount = 0;
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            if (G2SingleDefectTable[y*RawWidth+x] == 1)
                            {
                                G1SingleDefectCount++;
                            }
                            else if (G2SingleDefectTable[y*RawWidth+x] == 2)
                            {
                                G2DoubleDefectCount++;
                            }
                            else if (G2SingleDefectTable[y*RawWidth+x] >= 3)
                            {
                                G2MultipleDefectCount++;
                            }
                            TDefectTable[(2*y+1)*Width+(2*x+0)] = G2SingleDefectTable[y*RawWidth+x];
                        }
                    }
                    //-------------------------------------------------------------------------
                    unsigned char* BSingleDefectTable = new unsigned char[RawWidth*RawHeight];
                    memset(BSingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::SumFilter(BDefTable,RawWidth,RawHeight,3,3,BSingleDefectTable);
                    int BSingleDefectCount = 0;
                    int BDoubleDefectCount = 0;
                    int BMultipleDefectCount = 0;
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            if (BSingleDefectTable[y*RawWidth+x] == 1)
                            {
                                BSingleDefectCount++;
                            }
                            else if (BSingleDefectTable[y*RawWidth+x] == 2)
                            {
                                BDoubleDefectCount++;
                            }
                            else if (BSingleDefectTable[y*RawWidth+x] >= 3)
                            {
                                BMultipleDefectCount++;
                            }
                            TDefectTable[(2*y+1)*Width+(2*x+1)] = BSingleDefectTable[y*RawWidth+x];
                        }
                    }

                    //-------------------------------------------------------------------------
                    // 20130902-Rod G1/G2 and R/B combined defect calculate - create combined G and RB defect table.
                    unsigned char* CGDefTable = new unsigned char[RawWidth*RawHeight*2];
                    memset(CGDefTable,0,sizeof(unsigned char)*RawWidth*RawHeight*2);
                    unsigned char* RBDefTable = new unsigned char[RawWidth*RawHeight*2];
                    memset(RBDefTable,0,sizeof(unsigned char)*RawWidth*RawHeight*2);

                    // 20130902-Rod G1/G2 and R/B combined defect calculate - duplicate G1/G2 and R/B to combined G and RB defect table by different RAW bayer type.
                    if ((RAWBayerType == 1) || (RAWBayerType == 2))
                    {
                        for (int y=0;y<RawHeight;y++)
                        {
                            for (int x=0;x<RawWidth;x++)
                            {
                                CGDefTable[(2*y)*RawWidth+x] = G1DefTable[y*RawWidth+x];
                                CGDefTable[(2*y+1)*RawWidth+x] = G2DefTable[y*RawWidth+x];
                                RBDefTable[(2*y)*RawWidth+x] = RDefTable[y*RawWidth+x];		///v1.1.19 , Rickson
                                RBDefTable[(2*y+1)*RawWidth+x] = BDefTable[y*RawWidth+x];	///v1.1.19 , Rickson
                            }
                        }
                    }
                    else if ((RAWBayerType == 3) || (RAWBayerType == 4))
                    {
                        for (int y=0;y<RawHeight;y++)
                        {
                            for (int x=0;x<RawWidth;x++)
                            {
                                CGDefTable[(2*y)*RawWidth+x] = RDefTable[y*RawWidth+x];
                                CGDefTable[(2*y+1)*RawWidth+x] = BDefTable[y*RawWidth+x];
                                RBDefTable[(2*y)*RawWidth+x] = RDefTable[y*RawWidth+x];		///v1.1.19 , Rickson	
                                RBDefTable[(2*y+1)*RawWidth+x] = BDefTable[y*RawWidth+x];	///v1.1.19 , Rickson
                            }
                        }
                    }	

                    int CGSingleDefectCount = 0;				/// v1.1.18 , Rickson
                    int CGDoubleDefectCount = 0;				/// v1.1.18 , Rickson
                    int CGMultipleDefectCount = 0;				/// v1.1.18 , Rickson
                    int RBSingleDefectCount = 0;				/// v1.1.19 , Rickson
                    int RBDoubleDefectCount = 0;				/// v1.1.19 , Rickson
                    int RBMultipleDefectCount = 0;				/// v1.1.19 , Rickson
                    if ( AA_DefectPixel3ChannelEn == 1)			/// v1.1.18 , Rickson
                    {
                        // 20130902-Rod G1/G2 combined defect calculate - count defect type on combined G channel
                        unsigned char* CGSingleDefectTable = new unsigned char[RawWidth*RawHeight*2];
                        memset(CGSingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight*2);
                        CtLibrary::CtProcess::SumFilter(CGDefTable,RawWidth,RawHeight*2,3,3,CGSingleDefectTable);
                        unsigned char* RBSingleDefectTable = new unsigned char[RawWidth*RawHeight*2];				/// v1.1.19 , Rickson
                        memset(RBSingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight*2);					/// v1.1.19 , Rickson
                        CtLibrary::CtProcess::SumFilter(RBDefTable,RawWidth,RawHeight*2,3,3,RBSingleDefectTable);	/// v1.1.19 , Rickson
                        for (int y=0;y<RawHeight*2;y++)
                        {
                            for (int x=0;x<RawWidth;x++)
                            {
                                //Defect count on G plane
                                if (CGSingleDefectTable[y*RawWidth+x] == 1)
                                {
                                    CGSingleDefectCount++;
                                }
                                else if (CGSingleDefectTable[y*RawWidth+x] == 2)
                                {
                                    CGDoubleDefectCount++;
                                }
                                else if (CGSingleDefectTable[y*RawWidth+x] >= 3)
                                {
                                    CGMultipleDefectCount++;
                                }			
                                //Defect count on RB plane
                                if (RBSingleDefectTable[y*RawWidth+x] == 1)
                                {
                                    RBSingleDefectCount++;							
                                }
                                else if (RBSingleDefectTable[y*RawWidth+x] == 2)		/// v1.1.19 , Rickson
                                {
                                    RBDoubleDefectCount++;
                                }
                                else if (RBSingleDefectTable[y*RawWidth+x] >= 3)		/// v1.1.19 , Rickson
                                {
                                    RBMultipleDefectCount++;
                                }
                            }
                        }
                        RELEASE_ARRAY(CGSingleDefectTable);
                        RELEASE_ARRAY(RBSingleDefectTable);								/// v1.1.19 , Rickson
                        // 20130902-Rod G1/G2 combined defect calculate - output combined G defect count.
                        DefectCount.CGSingleDefectCount = CGSingleDefectCount;
                        DefectCount.CGDoubleDefectCount = CGDoubleDefectCount;
                        DefectCount.CGMultipleDefectCount = CGMultipleDefectCount;	

                        //-------------------------------------------------------------------------
                    }
                    RELEASE_ARRAY(CGDefTable);
                    RELEASE_ARRAY(RBDefTable);										/// v1.1.19 , Rickson

                    if ( AA_DefectPixel3ChannelEn == 1 )	/// v1.1.18 , Rickson
                    {
                        TotalSingleDefectCount = RSingleDefectCount+CGSingleDefectCount+BSingleDefectCount+RBSingleDefectCount;
                        TotalDoubleDefectCount = RDoubleDefectCount+CGDoubleDefectCount+BDoubleDefectCount+RBDoubleDefectCount;
                        TotalMultipleDefectCount = RMultipleDefectCount+CGMultipleDefectCount+BMultipleDefectCount+RBMultipleDefectCount;
                    }
                    else
                    {
                        TotalSingleDefectCount = RSingleDefectCount+G1SingleDefectCount+G2SingleDefectCount+BSingleDefectCount;
                        TotalDoubleDefectCount = RDoubleDefectCount+G1DoubleDefectCount+G2DoubleDefectCount+BDoubleDefectCount;
                        TotalMultipleDefectCount = RMultipleDefectCount+G1MultipleDefectCount+G2MultipleDefectCount+BMultipleDefectCount;
                    }
                    //-----------------------------------------------------------------

                    RELEASE_ARRAY(RSingleDefectTable);
                    RELEASE_ARRAY(G1SingleDefectTable);
                    RELEASE_ARRAY(G2SingleDefectTable);
                    RELEASE_ARRAY(BSingleDefectTable);

                    if ((TDefectTable!=0)&&(DefectTable!=0))
                    {
                        int k=0;
                        for (int y=0;y<Height;y++)
                        {
                            for (int x=0;x<Width;x++)
                            {
                                if (TDefectTable[y*Width+x] > 0)
                                {
                                    DefectTable[k].x = x;
                                    DefectTable[k].y = y;
                                    DefectTable[k].value = TDefectTable[y*Width+x];
                                    /*DefectRate[k] = ImDeff[y*Width+x];*/
                                    k++;
                                    if (k >= DEFECT_COUNT_MAX)      //breeze V1.T.9
                                    {
                                        y = Height;    //breeze V1.T.9
                                        break;
                                    }
                                }					
                            }
                        }
                        //AA_Defi = k;      /// Tiger , v1.1.28
                    }


                    RELEASE_ARRAY(TDefectTable);


                    //	已有Full Defect Table，即可判定是否有?題
                    //	9×9的Mask，確認符合條件（原已判定Mutiple）
                    //	以左上角為基準，?mask中如有其他
                    POINT* CorrectionDefect = new POINT[CutImageWidth*CutImageHeight];
                    memset(CorrectionDefect,0,sizeof(POINT)*CutImageWidth*CutImageHeight);
                    /*int */CDefectCount = 0;
                    for (int y=2;y<CutImageHeight-2;y++)
                    {
                        for (int x=2;x<CutImageWidth-2;x++)
                        {
                            int index=y*CutImageWidth+x;
                            if (DefTable[index] == 1)
                            {
                                //	判定周圍5×5的區域中有無其他defect（除了上下行僅1顆）
                                int Count = 0;
                                for (int j=-2;j<=2;j++)
                                {
                                    for (int i=-2;i<=2;i++)
                                    {
                                        int X = (((x+i)<0)?(-1):(((x+i)>=CutImageWidth)?(-1):(x+i)));
                                        int Y = (((y+j)<0)?(-1):(((y+j)>=CutImageHeight)?(-1):(y+j)));
                                        if ((X!=-1)&&(Y!=-1))
                                        {
                                            if (DefTable[Y*CutImageWidth+X] == 1)
                                            {
                                                Count++;
                                            }
                                        }
                                    }
                                }
                                if (Count > 2)
                                {
                                    CorrectionDefect[CDefectCount].x = StartX+x;
                                    CorrectionDefect[CDefectCount].y = StartY+y;
                                    CDefectCount++;
                                }
                                else if (Count == 2)	//	 Pass - Only one Case
                                {
                                    bool PassCase = false;

                                    for (int i=-2;i<=2;i++)
                                    {
                                        int X = (((x+i)<0)?(0):(((x+i)>=CutImageWidth)?(CutImageWidth-1):(x+i)));
                                        int Y1 = (((y-1) < 0)?(0):(y-1));
                                        int Y2 = (((y+1) >=CutImageHeight)?(CutImageHeight-1):(y+1));

                                        if ((DefTable[Y1*CutImageWidth+X] == 1)||(DefTable[Y2*CutImageWidth+X] == 1))
                                        {
                                            PassCase = true;
                                        }
                                    }

                                    if (PassCase == false)
                                    {
                                        CorrectionDefect[CDefectCount].x = StartX+x;
                                        CorrectionDefect[CDefectCount].y = StartY+y;
                                        CDefectCount++;
                                    }
                                }
                            }
                        }
                    }

                    RELEASE_ARRAY(R_AvgTable);
                    RELEASE_ARRAY(RDefTable);

                    RELEASE_ARRAY(G1_AvgTable);
                    RELEASE_ARRAY(G1DefTable);

                    RELEASE_ARRAY(G2_AvgTable);
                    RELEASE_ARRAY(G2DefTable);

                    RELEASE_ARRAY(B_AvgTable);
                    RELEASE_ARRAY(BDefTable);

                    /*
                    //	Identify pixels whose defect threshold is outside of H4ISP's correctable range
                    double * _RawWindowsBuffer = new double[5*5];
                    memset(_RawWindowsBuffer,0,sizeof(double)*25);
                    //	額外定義閥值
                    double THR_WHITE = 0;
                    double THR_BLACK = 0;
                    // 			THR_BLACK = 32*2.7;     // Black pixel detection threshold based on avg 182 DN on Gb, Gr for center 50 pixels
                    // 			THR_WHITE = 12*2.7;     // White pixel detection threshold based on avg 182 DN on Gb, Gr for center 50 pixels
                    THR_WHITE = H4ISPWThreshold;
                    THR_BLACK = H4ISPBThreshold;
                    //--------------------------------------------------------------
                    double white_detect = 0;
                    double black_detect = 0;
                    double white_delta = 0;
                    double black_delta = 0;

                    H4ISPDPInfo* H4ISPDefectPixelTable = new H4ISPDPInfo[CutImageWidth*CutImageHeight];
                    memset(H4ISPDefectPixelTable,0,sizeof(H4ISPDPInfo)*CutImageWidth*CutImageHeight);

                    int index = 0;
                    for (int y=2;y<CutImageHeight-2;y++)
                    {
                    for (int x=2;x<CutImageWidth-2;x++)
                    {
                    if (DefTable[y*CutImageWidth+x] == 1)
                    {
                    ///double white_detect = 0;
                    ///double black_detect = 0;
                    ///double white_delta = 0;
                    ///double black_delta = 0;
                    //5*5的區域
                    int X = x-2;
                    int Y = y-2;
                    for (int j=0;j<5;j++)
                    {
                    for (int i=0;i<5;i++)
                    {
                    _RawWindowsBuffer[j*5+i] = _pRAWImage[(Y+j)*CutImageWidth+(X+i)];
                    }
                    }
                    H4ISPDefectPixel(_RawWindowsBuffer,THR_WHITE,THR_BLACK,white_detect,black_detect,white_delta,black_delta);
                    if ((white_detect != 1)&&(black_detect != 1))
                    {
                    H4ISPDefectPixelTable[index].x = StartX+x;
                    H4ISPDefectPixelTable[index].y = StartY+y;
                    H4ISPDefectPixelTable[index].white_delta = white_delta;
                    H4ISPDefectPixelTable[index].black_delta = black_delta;
                    index++;
                    }
                    }
                    }
                    }

                    RELEASE(_RawWindowsBuffer);
                    /// int H4ISPDefectPixelCount = index;
                    if (index>0)
                    {
                    deviation30x40 = ImDeff[(H4ISPDefectPixelTable[0].y-StartY)*CutImageWidth+(H4ISPDefectPixelTable[0].x-StartX)];
                    }
                    RELEASE(H4ISPDefectPixelTable);

                    */
                    RELEASE_ARRAY(DefTable);
                    RELEASE_ARRAY(CorrectionDefect);	
                    RELEASE_ARRAY(ImDeff);
                    //-----------------------------------------------------------------
                    //最終?出結果
                    DefectCount.TotalSingleDefectCount = TotalSingleDefectCount;
                    DefectCount.TotalDoubleDefectCount = TotalDoubleDefectCount;
                    DefectCount.TotalMultipleDefectCount = TotalMultipleDefectCount;
                    DefectCount.CDefectCount = CDefectCount;
                    DefectCount.H4ISPDefectPixelCount= H4ISPDefectPixelCount;
                }

                void DefectPixelsRGB(
                    unsigned char *_pBmpImage,
                    int Width,
                    int Height,
                    double DefThreshold,
                    int ROIWidth,
                    int ROIHeight,
                    H4ISPDefectInfo &DefectCount,
                    AA_TwoDPoint *DefectTable)
                {
                    //-------------------------------------------------------------------------
                    // 整个画面的Y值Table
                    unsigned char *pYbuffer = new unsigned char[Width * Height];
                    memset(pYbuffer, 0, sizeof(unsigned char) * Width * Height);
                    CImageProc::GetInstance().Cal_RGBtoYBuffer(_pBmpImage, Width, Height, pYbuffer);

                    //-------------------------------------------------------------------------
                    // 整个画面的DefectTable(0/1 Table)
                    unsigned char* DefTable = new unsigned char[Width * Height];
                    memset(DefTable, 0, sizeof(unsigned char) * Width * Height);

                    //-------------------------------------------------------------------------
                    // 整个画面的Y值AvgTable
                    double *pYAvgTable = new double[Width * Height];
                    memset(pYAvgTable, 0, sizeof(double) * Width * Height);
                    CtLibrary::CtProcess::MeanFilter2(pYbuffer, Width, Height, ROIWidth, ROIHeight, pYAvgTable);

                    //	判定 Defect
                    for (int y = 0; y < Height; y++)
                    {
                        for (int x = 0; x < Width; x++)
                        {
                            double DeviatesRate = 0;
                            int index = y * Width + x;
                            double DiffValue = 0;
                            double _A = 0;
                            double _B = 0;

                            // R
                            _A = double(pYbuffer[index]);
                            _B = double(pYAvgTable[index]);
                            DiffValue = (((_A - _B) < 0) ? (_B - _A) : (_A - _B));
                            if (DefThreshold >= 1)
                            {
                                DeviatesRate = _A;
                            }
                            else
                            {
                                DeviatesRate = DiffValue / _B;
                            }													
                            if (DeviatesRate > DefThreshold)
                            {	
                                DefTable[y * Width + x] = 1;				
                            }
                        }
                    }
                    RELEASE_ARRAY(pYbuffer);

                    //-------------------------------------------------------------------------
                    // 整个画面的Defect类型Table(1:Single, 2:Double, >=3:Multiple)
                    unsigned char* TDefectTable = new unsigned char[Width * Height];
                    memset(TDefectTable, 0, sizeof(unsigned char) * Width * Height);

                    //-------------------------------------------------------------------------
                    // 判定 Defect
                    //-------------------------------------------------------------------------
                    CtLibrary::CtProcess::SumFilter(DefTable, Width, Height, 3, 3, TDefectTable);
                    int SingleDefectCount = 0;
                    int DoubleDefectCount = 0;
                    int MultipleDefectCount = 0;
                    for (int y = 0; y < Height; y++)
                    {
                        for (int x = 0; x < Width; x++)
                        {
                            if (TDefectTable[y * Width + x] == 1)
                            {
                                SingleDefectCount++;
                            }
                            else if (TDefectTable[y * Width + x] == 2)
                            {
                                DoubleDefectCount++;
                            }
                            else if (TDefectTable[y * Width + x] >= 3)
                            {
                                MultipleDefectCount++;
                            }
                        }
                    }

                    if ((TDefectTable != NULL) && (DefectTable != NULL))
                    {
                        int k = 0;
                        for (int y = 0; y < Height; y++)
                        {
                            for (int x = 0; x < Width; x++)
                            {
                                if (TDefectTable[y * Width + x] > 0)
                                {
                                    DefectTable[k].x = x;
                                    DefectTable[k].y = y;
                                    DefectTable[k].value = TDefectTable[y * Width + x];
                                    /*DefectRate[k] = ImDeff[y*Width+x];*/
                                    k++;
                                    if (k >= DEFECT_COUNT_MAX)      //breeze V1.T.9
                                    {
                                        y = Height;      //breeze V1.T.9
                                        break;
                                    }
                                }					
                            }
                        }
                        //AA_Defi = k;      /// Tiger , v1.1.28
                    }
                    RELEASE_ARRAY(TDefectTable);
                    RELEASE_ARRAY(DefTable);
                    RELEASE_ARRAY(pYAvgTable);

                    //-----------------------------------------------------------------
                    // 最终输出结果
                    DefectCount.TotalSingleDefectCount = SingleDefectCount;
                    DefectCount.TotalDoubleDefectCount = DoubleDefectCount;
                    DefectCount.TotalMultipleDefectCount = MultipleDefectCount;
                    //DefectCount.CDefectCount = CDefectCount;
                    //DefectCount.H4ISPDefectPixelCount= H4ISPDefectPixelCount;
                }
               void DefectPixelDarkDead(
                    unsigned char *_pRAWImage,
                    int Width,
                    int Height,
                    double DefThreshold,
                    int ROIWidth,
                    int ROIHeight,
                    int AA_DefectPixel3ChannelEn,
                    H4ISPDefectInfo &DefectCount,
                    AA_TwoDPoint *DefectTable,
                    int RAWBayerType)
                {
                    //-----------------------------------------------------------------
                    int TotalSingleDefectCount = 0;
                    int TotalDoubleDefectCount = 0;
                    int TotalMultipleDefectCount = 0;
                    int CDefectCount = 0;
                    int H4ISPDefectPixelCount = 0;
                    double deviation30x40 = 0;
                    //-------------------------------------------------------------------------
                    int CutImageWidth = Width;
                    int CutImageHeight = Height;
                    int StartX = 0;
                    int StartY = 0;
                    //	RGRG
                    //	GBGB
                    int RawWidth = CutImageWidth/2;
                    int RawHeight = CutImageHeight/2;

                    unsigned char* RPlane	= new unsigned char[RawWidth*RawHeight];
                    memset(RPlane,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    unsigned char* G1Plane	= new unsigned char[RawWidth*RawHeight];
                    memset(G1Plane,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    unsigned char* G2Plane	= new unsigned char[RawWidth*RawHeight];
                    memset(G2Plane,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    unsigned char* BPlane	= new unsigned char[RawWidth*RawHeight];
                    memset(BPlane,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            RPlane[y*RawWidth+x]	= _pRAWImage[2*y*CutImageWidth+2*x];
                            G1Plane[y*RawWidth+x]	= _pRAWImage[2*y*CutImageWidth+(2*x+1)];
                            G2Plane[y*RawWidth+x]	= _pRAWImage[(2*y+1)*CutImageWidth+2*x];
                            BPlane[y*RawWidth+x]	= _pRAWImage[(2*y+1)*CutImageWidth+(2*x+1)];
                        }
                    }

                    //	full Image
                    int* DefTable = new int[CutImageWidth*CutImageHeight];
                    memset(DefTable,0,sizeof(int)*CutImageWidth*CutImageHeight);

                    //	R Channel Defect
                    double* R_AvgTable = new double[RawWidth*RawHeight];
                    memset(R_AvgTable,0,sizeof(double)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::MeanFilter2(RPlane,RawWidth,RawHeight,ROIWidth,ROIHeight,R_AvgTable);
                    unsigned char* RDefTable = new unsigned char[RawWidth*RawHeight];
                    memset(RDefTable,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    //	G1 Channel Defect
                    double* G1_AvgTable = new double[RawWidth*RawHeight];
                    memset(G1_AvgTable,0,sizeof(double)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::MeanFilter2(G1Plane,RawWidth,RawHeight,ROIWidth,ROIHeight,G1_AvgTable);
                    unsigned char* G1DefTable = new unsigned char[RawWidth*RawHeight];
                    memset(G1DefTable,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    //	G2 Channel Defect
                    double* G2_AvgTable = new double[RawWidth*RawHeight];
                    memset(G2_AvgTable,0,sizeof(double)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::MeanFilter2(G2Plane,RawWidth,RawHeight,ROIWidth,ROIHeight,G2_AvgTable);
                    unsigned char* G2DefTable = new unsigned char[RawWidth*RawHeight];
                    memset(G2DefTable,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    //	B Channel Defect
                    double* B_AvgTable = new double[RawWidth*RawHeight];
                    memset(B_AvgTable,0,sizeof(double)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::MeanFilter2(BPlane,RawWidth,RawHeight,ROIWidth,ROIHeight,B_AvgTable);
                    unsigned char* BDefTable = new unsigned char[RawWidth*RawHeight];
                    memset(BDefTable,0,sizeof(unsigned char)*RawWidth*RawHeight);

                    double* ImDeff = new double[CutImageWidth*CutImageHeight];
                    memset(ImDeff,0,sizeof(double)*CutImageWidth*CutImageHeight);

                    //	判定 Defect
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            double DeviatesRate = 0;
                            int index = y*RawWidth+x;
                            double DiffValue = 0;
                            double _A = 0;
                            double _B = 0;

                            // R
                            _A = double(RPlane[index]);
                            _B = double(R_AvgTable[index]);
                            DiffValue = (((_A-_B) < 0)?(_B-_A):(_A-_B));
                            		
                            if (DiffValue > DefThreshold)
                            {
                                RDefTable[index] = 1;		
                                DefTable[2*y*CutImageWidth+2*x] = 1;				
                            }
                            ImDeff[2*y*CutImageWidth+2*x] = DeviatesRate;

                            // G1
                            _A = double(G1Plane[index]);
                            _B = double(G1_AvgTable[index]);
                            DiffValue = (((_A-_B) < 0)?(_B-_A):(_A-_B));
	
							if (DiffValue > DefThreshold)
                            {
                                G1DefTable[index] = 1;	
                                DefTable[2*y*CutImageWidth+(2*x+1)] = 1;
                            }
                            ImDeff[2*y*CutImageWidth+(2*x+1)] = DeviatesRate;

                            // G2
                            _A = double(G2Plane[index]);
                            _B = double(G2_AvgTable[index]);
                            DiffValue = (((_A-_B) < 0)?(_B-_A):(_A-_B));
			
							if (DiffValue > DefThreshold)
                            {
                                G2DefTable[index] = 1;	
                                DefTable[(2*y+1)*CutImageWidth+2*x] = 1;
                            }
                            ImDeff[(2*y+1)*CutImageWidth+2*x] = DeviatesRate;

                            // B
                            _A = double(BPlane[index]);
                            _B = double(B_AvgTable[index]);
                            DiffValue = (((_A-_B) < 0)?(_B-_A):(_A-_B));
				
							if (DiffValue > DefThreshold)
                            {
                                BDefTable[index] = 1;	
                                DefTable[(2*y+1)*CutImageWidth+(2*x+1)] = 1;
                            }
                            ImDeff[(2*y+1)*CutImageWidth+(2*x+1)] = DeviatesRate;
                        }
                    }
                    RELEASE_ARRAY(RPlane);
                    RELEASE_ARRAY(G1Plane);
                    RELEASE_ARRAY(G2Plane);
                    RELEASE_ARRAY(BPlane);

                    //-------------------------------------------------------------------------
                    unsigned char* TDefectTable = new unsigned char[Width*Height];
                    memset(TDefectTable,0,sizeof(unsigned char)*Width*Height);
                    //-------------------------------------------------------------------------
                    //-------------------------------------------------------------------------
                    // 判定 Single Defect
                    //-------------------------------------------------------------------------
                    unsigned char* RSingleDefectTable = new unsigned char[RawWidth*RawHeight];
                    memset(RSingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::SumFilter(RDefTable,RawWidth,RawHeight,3,3,RSingleDefectTable);
                    int RSingleDefectCount = 0;
                    int RDoubleDefectCount = 0;
                    int RMultipleDefectCount = 0;
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            if (RSingleDefectTable[y*RawWidth+x] == 1)
                            {
                                RSingleDefectCount++;
                            }
                            else if (RSingleDefectTable[y*RawWidth+x] == 2)
                            {
                                RDoubleDefectCount++;
                            }
                            else if (RSingleDefectTable[y*RawWidth+x] >= 3)
                            {
                                RMultipleDefectCount++;
                            }
                            TDefectTable[(2*y+0)*Width+(2*x+0)] = RSingleDefectTable[y*RawWidth+x];
                        }
                    }
                    //-------------------------------------------------------------------------
                    unsigned char* G1SingleDefectTable = new unsigned char[RawWidth*RawHeight];
                    memset(G1SingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::SumFilter(G1DefTable,RawWidth,RawHeight,3,3,G1SingleDefectTable);
                    int G1SingleDefectCount = 0;
                    int G1DoubleDefectCount = 0;
                    int G1MultipleDefectCount = 0;
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            if (G1SingleDefectTable[y*RawWidth+x] == 1)
                            {
                                G1SingleDefectCount++;
                            }
                            else if (G1SingleDefectTable[y*RawWidth+x] == 2)
                            {
                                G1DoubleDefectCount++;
                            }
                            else if (G1SingleDefectTable[y*RawWidth+x] >= 3)
                            {
                                G1MultipleDefectCount++;
                            }
                            TDefectTable[(2*y+0)*Width+(2*x+1)] = G1SingleDefectTable[y*RawWidth+x];
                        }
                    }
                    //-------------------------------------------------------------------------
                    unsigned char* G2SingleDefectTable = new unsigned char[RawWidth*RawHeight];
                    memset(G2SingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::SumFilter(G2DefTable,RawWidth,RawHeight,3,3,G2SingleDefectTable);
                    int G2SingleDefectCount = 0;
                    int G2DoubleDefectCount = 0;
                    int G2MultipleDefectCount = 0;
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            if (G2SingleDefectTable[y*RawWidth+x] == 1)
                            {
                                G1SingleDefectCount++;
                            }
                            else if (G2SingleDefectTable[y*RawWidth+x] == 2)
                            {
                                G2DoubleDefectCount++;
                            }
                            else if (G2SingleDefectTable[y*RawWidth+x] >= 3)
                            {
                                G2MultipleDefectCount++;
                            }
                            TDefectTable[(2*y+1)*Width+(2*x+0)] = G2SingleDefectTable[y*RawWidth+x];
                        }
                    }
                    //-------------------------------------------------------------------------
                    unsigned char* BSingleDefectTable = new unsigned char[RawWidth*RawHeight];
                    memset(BSingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight);
                    CtLibrary::CtProcess::SumFilter(BDefTable,RawWidth,RawHeight,3,3,BSingleDefectTable);
                    int BSingleDefectCount = 0;
                    int BDoubleDefectCount = 0;
                    int BMultipleDefectCount = 0;
                    for (int y=0;y<RawHeight;y++)
                    {
                        for (int x=0;x<RawWidth;x++)
                        {
                            if (BSingleDefectTable[y*RawWidth+x] == 1)
                            {
                                BSingleDefectCount++;
                            }
                            else if (BSingleDefectTable[y*RawWidth+x] == 2)
                            {
                                BDoubleDefectCount++;
                            }
                            else if (BSingleDefectTable[y*RawWidth+x] >= 3)
                            {
                                BMultipleDefectCount++;
                            }
                            TDefectTable[(2*y+1)*Width+(2*x+1)] = BSingleDefectTable[y*RawWidth+x];
                        }
                    }

                    //-------------------------------------------------------------------------
                    // 20130902-Rod G1/G2 and R/B combined defect calculate - create combined G and RB defect table.
                    unsigned char* CGDefTable = new unsigned char[RawWidth*RawHeight*2];
                    memset(CGDefTable,0,sizeof(unsigned char)*RawWidth*RawHeight*2);
                    unsigned char* RBDefTable = new unsigned char[RawWidth*RawHeight*2];
                    memset(RBDefTable,0,sizeof(unsigned char)*RawWidth*RawHeight*2);

                    // 20130902-Rod G1/G2 and R/B combined defect calculate - duplicate G1/G2 and R/B to combined G and RB defect table by different RAW bayer type.
                    if ((RAWBayerType == 1) || (RAWBayerType == 2))
                    {
                        for (int y=0;y<RawHeight;y++)
                        {
                            for (int x=0;x<RawWidth;x++)
                            {
                                CGDefTable[(2*y)*RawWidth+x] = G1DefTable[y*RawWidth+x];
                                CGDefTable[(2*y+1)*RawWidth+x] = G2DefTable[y*RawWidth+x];
                                RBDefTable[(2*y)*RawWidth+x] = RDefTable[y*RawWidth+x];		///v1.1.19 , Rickson
                                RBDefTable[(2*y+1)*RawWidth+x] = BDefTable[y*RawWidth+x];	///v1.1.19 , Rickson
                            }
                        }
                    }
                    else if ((RAWBayerType == 3) || (RAWBayerType == 4))
                    {
                        for (int y=0;y<RawHeight;y++)
                        {
                            for (int x=0;x<RawWidth;x++)
                            {
                                CGDefTable[(2*y)*RawWidth+x] = RDefTable[y*RawWidth+x];
                                CGDefTable[(2*y+1)*RawWidth+x] = BDefTable[y*RawWidth+x];
                                RBDefTable[(2*y)*RawWidth+x] = RDefTable[y*RawWidth+x];		///v1.1.19 , Rickson	
                                RBDefTable[(2*y+1)*RawWidth+x] = BDefTable[y*RawWidth+x];	///v1.1.19 , Rickson
                            }
                        }
                    }	

                    int CGSingleDefectCount = 0;				/// v1.1.18 , Rickson
                    int CGDoubleDefectCount = 0;				/// v1.1.18 , Rickson
                    int CGMultipleDefectCount = 0;				/// v1.1.18 , Rickson
                    int RBSingleDefectCount = 0;				/// v1.1.19 , Rickson
                    int RBDoubleDefectCount = 0;				/// v1.1.19 , Rickson
                    int RBMultipleDefectCount = 0;				/// v1.1.19 , Rickson
                    if ( AA_DefectPixel3ChannelEn == 1)			/// v1.1.18 , Rickson
                    {
                        // 20130902-Rod G1/G2 combined defect calculate - count defect type on combined G channel
                        unsigned char* CGSingleDefectTable = new unsigned char[RawWidth*RawHeight*2];
                        memset(CGSingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight*2);
                        CtLibrary::CtProcess::SumFilter(CGDefTable,RawWidth,RawHeight*2,3,3,CGSingleDefectTable);
                        unsigned char* RBSingleDefectTable = new unsigned char[RawWidth*RawHeight*2];				/// v1.1.19 , Rickson
                        memset(RBSingleDefectTable,0,sizeof(unsigned char)*RawWidth*RawHeight*2);					/// v1.1.19 , Rickson
                        CtLibrary::CtProcess::SumFilter(RBDefTable,RawWidth,RawHeight*2,3,3,RBSingleDefectTable);	/// v1.1.19 , Rickson
                        for (int y=0;y<RawHeight*2;y++)
                        {
                            for (int x=0;x<RawWidth;x++)
                            {
                                //Defect count on G plane
                                if (CGSingleDefectTable[y*RawWidth+x] == 1)
                                {
                                    CGSingleDefectCount++;
                                }
                                else if (CGSingleDefectTable[y*RawWidth+x] == 2)
                                {
                                    CGDoubleDefectCount++;
                                }
                                else if (CGSingleDefectTable[y*RawWidth+x] >= 3)
                                {
                                    CGMultipleDefectCount++;
                                }			
                                //Defect count on RB plane
                                if (RBSingleDefectTable[y*RawWidth+x] == 1)
                                {
                                    RBSingleDefectCount++;							
                                }
                                else if (RBSingleDefectTable[y*RawWidth+x] == 2)		/// v1.1.19 , Rickson
                                {
                                    RBDoubleDefectCount++;
                                }
                                else if (RBSingleDefectTable[y*RawWidth+x] >= 3)		/// v1.1.19 , Rickson
                                {
                                    RBMultipleDefectCount++;
                                }
                            }
                        }
                        RELEASE_ARRAY(CGSingleDefectTable);
                        RELEASE_ARRAY(RBSingleDefectTable);								/// v1.1.19 , Rickson
                        // 20130902-Rod G1/G2 combined defect calculate - output combined G defect count.
                        DefectCount.CGSingleDefectCount = CGSingleDefectCount;
                        DefectCount.CGDoubleDefectCount = CGDoubleDefectCount;
                        DefectCount.CGMultipleDefectCount = CGMultipleDefectCount;	

                        //-------------------------------------------------------------------------
                    }
                    RELEASE_ARRAY(CGDefTable);
                    RELEASE_ARRAY(RBDefTable);										/// v1.1.19 , Rickson

                    if ( AA_DefectPixel3ChannelEn == 1 )	/// v1.1.18 , Rickson
                    {
                        TotalSingleDefectCount = RSingleDefectCount+CGSingleDefectCount+BSingleDefectCount+RBSingleDefectCount;
                        TotalDoubleDefectCount = RDoubleDefectCount+CGDoubleDefectCount+BDoubleDefectCount+RBDoubleDefectCount;
                        TotalMultipleDefectCount = RMultipleDefectCount+CGMultipleDefectCount+BMultipleDefectCount+RBMultipleDefectCount;
                    }
                    else
                    {
                        TotalSingleDefectCount = RSingleDefectCount+G1SingleDefectCount+G2SingleDefectCount+BSingleDefectCount;
                        TotalDoubleDefectCount = RDoubleDefectCount+G1DoubleDefectCount+G2DoubleDefectCount+BDoubleDefectCount;
                        TotalMultipleDefectCount = RMultipleDefectCount+G1MultipleDefectCount+G2MultipleDefectCount+BMultipleDefectCount;
                    }
                    //-----------------------------------------------------------------

                    RELEASE_ARRAY(RSingleDefectTable);
                    RELEASE_ARRAY(G1SingleDefectTable);
                    RELEASE_ARRAY(G2SingleDefectTable);
                    RELEASE_ARRAY(BSingleDefectTable);

                    if ((TDefectTable!=0)&&(DefectTable!=0))
                    {
                        int k=0;
                        for (int y=0;y<Height;y++)
                        {
                            for (int x=0;x<Width;x++)
                            {
                                if (TDefectTable[y*Width+x] > 0)
                                {
                                    DefectTable[k].x = x;
                                    DefectTable[k].y = y;
                                    DefectTable[k].value = TDefectTable[y*Width+x];
                                    /*DefectRate[k] = ImDeff[y*Width+x];*/
                                    k++;
                                    if (k >= DEFECT_COUNT_MAX)      //breeze V1.T.9
                                    {
                                        y = Height;    //breeze V1.T.9
                                        break;
                                    }
                                }					
                            }
                        }
                        //AA_Defi = k;      /// Tiger , v1.1.28
                    }


                    RELEASE_ARRAY(TDefectTable);



                    POINT* CorrectionDefect = new POINT[CutImageWidth*CutImageHeight];
                    memset(CorrectionDefect,0,sizeof(POINT)*CutImageWidth*CutImageHeight);
                    /*int */CDefectCount = 0;
                    for (int y=2;y<CutImageHeight-2;y++)
                    {
                        for (int x=2;x<CutImageWidth-2;x++)
                        {
                            int index=y*CutImageWidth+x;
                            if (DefTable[index] == 1)
                            {
                                int Count = 0;
                                for (int j=-2;j<=2;j++)
                                {
                                    for (int i=-2;i<=2;i++)
                                    {
                                        int X = (((x+i)<0)?(-1):(((x+i)>=CutImageWidth)?(-1):(x+i)));
                                        int Y = (((y+j)<0)?(-1):(((y+j)>=CutImageHeight)?(-1):(y+j)));
                                        if ((X!=-1)&&(Y!=-1))
                                        {
                                            if (DefTable[Y*CutImageWidth+X] == 1)
                                            {
                                                Count++;
                                            }
                                        }
                                    }
                                }
                                if (Count > 2)
                                {
                                    CorrectionDefect[CDefectCount].x = StartX+x;
                                    CorrectionDefect[CDefectCount].y = StartY+y;
                                    CDefectCount++;
                                }
                                else if (Count == 2)	//	 Pass - Only one Case
                                {
                                    bool PassCase = false;

                                    for (int i=-2;i<=2;i++)
                                    {
                                        int X = (((x+i)<0)?(0):(((x+i)>=CutImageWidth)?(CutImageWidth-1):(x+i)));
                                        int Y1 = (((y-1) < 0)?(0):(y-1));
                                        int Y2 = (((y+1) >=CutImageHeight)?(CutImageHeight-1):(y+1));

                                        if ((DefTable[Y1*CutImageWidth+X] == 1)||(DefTable[Y2*CutImageWidth+X] == 1))
                                        {
                                            PassCase = true;
                                        }
                                    }

                                    if (PassCase == false)
                                    {
                                        CorrectionDefect[CDefectCount].x = StartX+x;
                                        CorrectionDefect[CDefectCount].y = StartY+y;
                                        CDefectCount++;
                                    }
                                }
                            }
                        }
                    }

                    RELEASE_ARRAY(R_AvgTable);
                    RELEASE_ARRAY(RDefTable);

                    RELEASE_ARRAY(G1_AvgTable);
                    RELEASE_ARRAY(G1DefTable);

                    RELEASE_ARRAY(G2_AvgTable);
                    RELEASE_ARRAY(G2DefTable);

                    RELEASE_ARRAY(B_AvgTable);
                    RELEASE_ARRAY(BDefTable);


                    RELEASE_ARRAY(DefTable);
                    RELEASE_ARRAY(CorrectionDefect);	
                    RELEASE_ARRAY(ImDeff);
                    //-----------------------------------------------------------------
                    DefectCount.TotalSingleDefectCount = TotalSingleDefectCount;
                    DefectCount.TotalDoubleDefectCount = TotalDoubleDefectCount;
                    DefectCount.TotalMultipleDefectCount = TotalMultipleDefectCount;
                    DefectCount.CDefectCount = CDefectCount;
                    DefectCount.H4ISPDefectPixelCount= H4ISPDefectPixelCount;
                }
            }
        }
    }
}
