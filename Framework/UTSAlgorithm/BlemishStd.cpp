#include "StdAfx.h"
#include "Algorithm.h"
#include "fftw3.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace Blemish
        {
            namespace Blemish_Std
            {
                #define DEF_DUST_THRESHOLD      (-3)

                void Get_border(double* data, int width, int &border_left, int &border_right)
                {
                    border_left = -1;
                    border_right = -1;
                    //

                    double idata;
                    double i1data;
                    double i2data;
                    for (int i=1; i<width-1; i++)
                    {
                        idata = data[i];
                        i1data = data[i-1];
                        i2data = data[i+1];
                        //if (((data[i]>data[i-1])&&(data[i]>data[i+1]))||((data[i]<data[i-1])&&(data[i]<data[i+1])))
                        if (((idata>i1data)&&(i1data>i2data))||((idata<i1data)&&(idata<i2data)))
                        {
                            if (border_left == -1)
                            {
                                border_left = i;
                            }
                            else
                            {
                                border_right = i;
                            }
                        }
                    }
                }

                bool SetPostImageValue2(unsigned char *_image,int width,int height, int x, int y, int _value)
                {
                    if ((x<0)||(x>= width))  return false;
                    if ((y<0)||(y>= height)) return false;

                    _image[y*width + x] = _value;

                    return true;
                }

                void ImageErode(unsigned char *_PostImage, int width,int height, int n)
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
                        // 			_temp[i] = 1;
                        // 		}
                        memset(_temp,1,sizeof(unsigned char)*height*width);

                        _ptmp = _PostImage;

                        for (int y=0; y<height; y++)
                        {
                            for (int x=0; x<width; x++)
                            {
                                /*if (_PostImage[y*width + x] == 0)*/
                                if ((*(_ptmp++)) == 0)
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

                    RELEASE_ARRAY(_temp);
                }

                void ImageDilate(unsigned char *_PostImage, int width,int height, int n)
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
                                if ((*(_ptmp++)) > 0)
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

                    RELEASE_ARRAY(_temp);
                }

                void HoriBandPass(unsigned char* _Image,unsigned char* _PosHoriImage,int width,int height,int _cutLowowFrequency,int _cutHighFrequency)
                {
                    //將使用下列fftw函式
                    double* _inData = new double[width];
                    memset(_inData,0,sizeof(double)*width);

                    double* _outData = new double[width];
                    memset(_outData,0,sizeof(double)*width);

                    double* _outData2 = new double[width];
                    memset(_outData2,0,sizeof(double)*width);
                    //to compensate to Matlab

                    double k_dct_1  = /*1.0/*/(pow(width*1.0,-0.5)/2);
                    double k_dct_2  = /*1.0/*/(pow(width*1.0/2.0,-0.5)/2);
                    //double k_idct_1 = 1/sqrtf(2*width/*, 0.5*/);
                    double k_idct_1 = pow(2*width, -0.5);
                    double k_idct_2 = 0;

                    double _compensate = (pow(width, -0.5) - pow(2*width, -0.5));

                    unsigned char* _ptmpImg;

                    //一行一行進行轉換
                    for (int y=0; y<height; y++)    //Y座標
                    {
                        //填入資料
                        _ptmpImg = &_Image[(y*width*3)];
                        for (int x=0; x<width; x++) //X座標			
                        {
                            int r = *_ptmpImg++;
                            int g = *_ptmpImg++;
                            int b = *_ptmpImg++;

                            _inData[x] = g;//_Image[(y*width+x)*3+1];	///<原本是定義取得最大Channel Value。get_pixel_max(j,i);
                        }

                        //開始進行單行的一階轉換（正轉）
                        fftw_plan plan1;
                        plan1 = fftw_plan_r2r_1d(width, _inData,_outData,FFTW_REDFT10,FFTW_ESTIMATE);
                        fftw_execute(plan1);
                        fftw_destroy_plan(plan1);

                        //compensate dct to MatLab
                        _outData[0] *= k_dct_1;

                        for (int x=_cutLowowFrequency; x<_cutHighFrequency; x++)
                        {
                            _outData[x] *= k_dct_2;
                        }
                        //	帶通濾波器（band pass filter）是指能通過某一頻率?圍內的頻率分量、但將其他?圍的頻率分量衰減到極低水平的濾波器
                        //	只留下Cutoff Frequency(截頻點)左右頻?圍
                        for (int x=0; x<_cutLowowFrequency; x++)
                        {
                            _outData[x] = 0;
                        }
                        for (int x=_cutHighFrequency; x<width; x++)
                        {
                            _outData[x] = 0;
                        }

                        //開始進行單行的一階轉換（反轉）
                        fftw_plan plan2;
                        plan2 = fftw_plan_r2r_1d(width, _outData, _outData2, FFTW_REDFT01, FFTW_ESTIMATE);
                        fftw_execute(plan2);
                        fftw_destroy_plan(plan2);

                        //compensate idct to MatLab
                        //  [5/14/2010 CTC]
                        //k_idct_2 = _outData[0]*(pow(width, -0.5) - pow(2*width, -0.5));
                        k_idct_2 = _outData[0]*_compensate;
                        //---------------------------------------------------------------------------
                        for (int x=0; x<width; x++)
                        {
                            _outData2[x] = _outData2[x]*k_idct_1 + k_idct_2;
                        }

                        //Mark the hole of blemish
                        for (int x=0; x<width; x++)
                        {
                            if (_outData2[x] < DEF_DUST_THRESHOLD)
                            {
                                SetPostImageValue2(_PosHoriImage,width,height, x, y, 1);
                            }
                        }

                        //eliminate border effect
                        int LeftBorder = 0, RightBorder = 0;
                        Get_border(_outData2, width, LeftBorder, RightBorder);	///<得到??
                        if (LeftBorder > width)
                        {
                            LeftBorder = width;
                        }
                        for (int x=0; x<=LeftBorder; x++)
                        {
                            SetPostImageValue2(_PosHoriImage,width,height, x, y, 0);
                        }
                        for (int x=RightBorder; x<width; x++)
                        {
                            SetPostImageValue2(_PosHoriImage,width,height, x, y, 0);
                        }
                    }

                    RELEASE_ARRAY(_inData);
                    RELEASE_ARRAY(_outData);
                    RELEASE_ARRAY(_outData2);
                }

                void VertBandPass(const unsigned char* _Image,unsigned char* _PosVertImage,int width,int height,int _cutLowowFrequency,int _cutHighFrequency)
                {
                    //將使用下列fftw函式
                    double* _inData = new double[height];
                    memset(_inData,0,sizeof(double)*height);

                    double* _outData = new double[height];
                    memset(_outData,0,sizeof(double)*height);

                    double* _outData2 = new double[height];
                    memset(_outData2,0,sizeof(double)*height);
                    //to compensate to Matlab
                    // 	double k_dct_1  = 1.0/(sqrtf(height*1.0/*,0.5*/)*2);
                    // 	double k_dct_2  = 1.0/(sqrtf(height*1.0/2.0/*,0.5*/)*2);
                    double k_dct_1  = /*1.0/*/(pow(height*1.0,-0.5)/2);
                    double k_dct_2  = /*1.0/*/(pow(height*1.0/2.0,-0.5)/2);
                    //double k_idct_1 = 1.0/sqrtf(2*height/*, 0.5*/);
                    double k_idct_1 = pow(2*height, -0.5);
                    double k_idct_2 = 0;

                    double _compensate = (pow(height, -0.5) - pow(2*height, -0.5));

                    //一行一行進行轉換
                    for (int x=0; x<width; x++)    //Y座標
                    {
                        //填入資料
                        for (int y=0; y<height; y++) //X座標
                        {
                            _inData[y] = _Image[(y*width+x)*3+1];	///<原本是定義取得最大Channel Value。get_pixel_max(j,i);
                        }

                        //開始進行單行的一階轉換（正轉）
                        fftw_plan plan1;
                        plan1 = fftw_plan_r2r_1d(height, _inData,_outData,FFTW_REDFT10,FFTW_ESTIMATE);
                        fftw_execute(plan1);
                        fftw_destroy_plan(plan1);

                        //compensate dct to MatLab
                        _outData[0] *= k_dct_1;
                        for (int y=_cutLowowFrequency; y<_cutHighFrequency; y++)
                        {
                            _outData[y] *= k_dct_2;
                        }
                        //	帶通濾波器（band pass filter）是指能通過某一頻率?圍內的頻率分量、但將其他?圍的頻率分量衰減到極低水平的濾波器
                        //	只留下Cutoff Frequency(截頻點)左右頻?圍
                        for (int y=0; y<_cutLowowFrequency; y++)
                        {
                            _outData[y] = 0;
                        }
                        for (int y=_cutHighFrequency; y<height; y++)
                        {
                            _outData[y] = 0;
                        }

                        //開始進行單行的一階轉換（反轉）
                        fftw_plan plan2;
                        plan2 = fftw_plan_r2r_1d(height, _outData, _outData2, FFTW_REDFT01, FFTW_ESTIMATE);
                        fftw_execute(plan2);
                        fftw_destroy_plan(plan2);

                        //compensate idct to MatLab
                        //  [5/14/2010 CTC]
                        //k_idct_2 = _outData[0]*(pow(height, -0.5) - pow(2*height, -0.5));
                        k_idct_2 = _outData[0]*_compensate;
                        //---------------------------------------------------------------------
                        for (int y=0; y<height; y++)
                        {
                            _outData2[y] = _outData2[y]*k_idct_1 + k_idct_2;
                        }

                        //Mark the hole of blemish
                        for (int y=0; y<height; y++)
                        {
                            if (_outData2[y] < DEF_DUST_THRESHOLD)
                            {
                                SetPostImageValue2(_PosVertImage,width,height, x, y, 1);
                            }
                        }

                        //eliminate border effect
                        int LeftBorder = 0, RightBorder = 0;
                        Get_border(_outData2, height, LeftBorder, RightBorder);	///<得到??
                        if (LeftBorder > height)
                        {
                            LeftBorder = height;
                        }
                        // 		if (RightBorder >width)
                        // 		{
                        // 		}

                        for (int y=0; y<=LeftBorder; y++)
                        {
                            SetPostImageValue2(_PosVertImage,width,height, x, y, 0);
                        }
                        for (int y=RightBorder; y<height; y++)
                        {
                            SetPostImageValue2(_PosVertImage,width,height, x, y, 0);
                        }
                    }

                    RELEASE_ARRAY(_inData);
                    RELEASE_ARRAY(_outData);
                    RELEASE_ARRAY(_outData2);
                }

                bool FindFirstWhitePoint(unsigned char *_PostImage, int width,int height, POINT &_point)
                {
                    unsigned char* _ptmp;
                    _ptmp = _PostImage;
                    for (int y=0; y<height; y++)
                    {
                        //int _index = y*width;
                        for (int x=0; x<width; x++)
                        {
                            //if (_PostImage[_index + x] > 0)
                            if ((*(_ptmp++)) > 0)
                            {
                                _point.x = x;
                                _point.y = y;
                                return true;
                            }
                            //_ptmp++;
                        }
                    }
                    _point.x = _point.y = -1;
                    return false;
                }

                void FindBlemishBlock(unsigned char *_PostImage, int width,int height,AA_TwoDPoint* _BlemishBlockPoint,int &_BlemishCount)
                {
                    POINT BlockPoint;
                    _BlemishCount = 0;

                    //AA_BLMWidth = width;     /// Tiger , v1.1.28 
                    //AA_BLMHeight = height;    /// Tiger , v1.1.28

                    /////--------------------Tiger v1.1.28---------------------
                    //for (int i = 0;i < DEF_BLEMISH_MAXCOUNT;i++)
                    //{
                    //    AA_Coor_XMax[i] = -1;
                    //    AA_Coor_YMax[i] = -1;
                    //    AA_Coor_XMin[i] = width;
                    //    AA_Coor_YMin[i] = height;
                    //}
                    /////--------------------Tiger v1.1.28---------------------

                    //需先備份_PostImage，以便搜尋時能將搜尋過的抹去。
                    int TotalSize = width*height;
                    unsigned char *_cpyPostImage = new unsigned char[TotalSize];
                    //memset(_cpyPostImage,0,sizeof(unsigned char)*width*height);
                    memcpy(_cpyPostImage,_PostImage,sizeof(unsigned char)*TotalSize);	

                    //設定空圖，用以描繪區塊??
                    unsigned char *_nulPostImage = new unsigned char[TotalSize];
                    memset(_nulPostImage,0,sizeof(unsigned char)*TotalSize);

                    //?碼搜尋表
                    int vec_x[8] = {-1,	 0,	 1,	1,	1,	0,	-1,	-1};
                    int vec_y[8] = {-1,	-1,	-1,	0,	1,	1,	1,	 0};

                    while (FindFirstWhitePoint(_cpyPostImage,width,height,BlockPoint)&&(_BlemishCount<DEF_BLEMISH_MAXCOUNT))
                    {
                        POINT _Point;
                        memset(&_Point,0,sizeof(POINT));

                        //重新清空，以利後面?算
                        memset(_nulPostImage,0,sizeof(unsigned char)*TotalSize);

                        bool _bContinue = true;
                        int _count = 0;
                        //取得第一?亮像素

                        _nulPostImage[BlockPoint.y*width+BlockPoint.x] = 1;

                        _Point.x = BlockPoint.x;
                        _Point.y = BlockPoint.y;

                        int _tmp1 = 0,_tmp2 = 0;
                        int index = 0;

                        int ErrorCount = 0;

                        while (_bContinue == true)
                        {
                            if (_count == 0)
                            {
                                _tmp1 = _tmp2 = 0;
                            }
                            else
                            {
                                _tmp1 = _tmp2-3;
                            }

                            for (int i=0; i<8; i++)
                            {
                                index = ((_tmp1+i)>=8)? (_tmp1+i-8):(((_tmp1+i)<0)?(8+(_tmp1+i)):(_tmp1+i));
                                //  [7/16/2010 CTC]
                                int _iy = _Point.y + vec_y[index];
                                int _ix = _Point.x + vec_x[index];
                                if ((_iy < height)&&(_iy >= 0)&&(_ix < width)&&(_ix >= 0))
                                {
                                    int _tIndex = _iy*width+_ix;
                                    //-------------------------------------------------------------
                                    if (_cpyPostImage[_tIndex] == 1)
                                    {
                                        _tmp1 = index;

                                        if (abs(_tmp1-_tmp2)!=4)
                                        {
                                            if (_nulPostImage[_tIndex] != 1)
                                            {

                                                _nulPostImage[_tIndex] = 1;
                                                _Point.x = (_Point.x + vec_x[index]);
                                                _Point.y = (_Point.y + vec_y[index]);

                                                _tmp2 =_tmp1;
                                                _count++;

                                                if (_count==26)
                                                {
                                                    _count =_count;
                                                }

                                                break;
                                            }
                                            else
                                            {
                                                _bContinue = false;
                                                break;
                                            }
                                        }

                                    }					
                                }

                            }
                        }

                        //將此區塊明確標示（塗白），?算其中心與尺寸
                        bool _bStartDraw = false;
                        bool _bEndDraw = false;
                        POINT* _iStatrPoint = new POINT[height];
                        memset(_iStatrPoint,0,sizeof(POINT)*height);
                        POINT* _iEndPoint = new POINT[height];
                        memset(_iEndPoint,0,sizeof(POINT)*height);

                        unsigned char* _nulPostImage2 =new unsigned char[TotalSize];
                        memset(_nulPostImage2,0,sizeof(unsigned char)*TotalSize);

                        unsigned char* _nulPostImage3 =new unsigned char[TotalSize];
                        memset(_nulPostImage3,0,sizeof(unsigned char)*TotalSize);

                        for (int y=0;y<height;y++)
                        {
                            _bStartDraw = false;
                            _bEndDraw = false;

                            if ((_bStartDraw == false))
                            {
                                for (int x1=0;x1<width;x1++)
                                {
                                    //假如是第一?白點
                                    if ((_nulPostImage[y*width+x1] == 1)/*&&(_bStartDraw == false)*/)
                                    {
                                        _iStatrPoint[y].x = x1;
                                        _iStatrPoint[y].y = y;
                                        _bStartDraw = true;
                                        break;
                                    }
                                }
                            }

                            if (_bEndDraw ==false)
                            {
                                for (int x2 = width-1;x2>=0;x2--)
                                {
                                    //假如是最後一?白點
                                    if ((_nulPostImage[y*width+x2] == 1)/*&&(_bEndDraw ==false)*/)
                                    {
                                        _iEndPoint[y].x = x2+1;
                                        _iEndPoint[y].y = y;
                                        _bEndDraw = true;
                                        break;
                                    }
                                }
                            }


                            for (int x3 = _iStatrPoint[y].x; x3<_iEndPoint[y].x;x3++)
                            {
                                _nulPostImage2[y*width+x3] = 1;
                            }

                        }
                        POINT* _jStatrPoint = new POINT[width];
                        memset(_jStatrPoint,0,sizeof(POINT)*width);
                        POINT* _jEndPoint = new POINT[width];
                        memset(_jEndPoint,0,sizeof(POINT)*width);

                        for (int x=0;x<width;x++)
                        {
                            _bStartDraw = false;
                            _bEndDraw = false;

                            if ((_bStartDraw == false))
                            {
                                for (int y1=0;y1<height;y1++)
                                {
                                    //假如是第一?白點
                                    if ((_nulPostImage[y1*width+x] == 1)/*&&(_bStartDraw == false)*/)
                                    {
                                        _jStatrPoint[x].x = x;
                                        _jStatrPoint[x].y = y1;
                                        _bStartDraw = true;
                                        break;
                                    }
                                }
                            }

                            if (_bEndDraw ==false)
                            {
                                for (int y2 = height-1;y2>=0;y2--)
                                {
                                    //假如是最後一?白點
                                    if ((_nulPostImage[y2*width+x] == 1)/*&&(_bEndDraw ==false)*/)
                                    {
                                        _jEndPoint[x].x = x;
                                        _jEndPoint[x].y = y2+1;
                                        _bEndDraw = true;
                                        break;
                                    }
                                }
                            }

                            for (int y3 = _jStatrPoint[x].y; y3<_jEndPoint[x].y;y3++)
                            {
                                _nulPostImage3[y3*width+x] = 1;
                            }

                        }
                        //疊合，同時?算尺寸於中心
                        int _blemishCount = 0;
                        POINT _tmpCorrdiation;
                        _tmpCorrdiation.x = 0;
                        _tmpCorrdiation.y = 0;
                        for (int y=0;y<height;y++)
                        {
                            for (int x=0;x<width;x++)
                            {
                                int _index = y*width+x;
                                if ((_nulPostImage2[_index]==1)&&(_nulPostImage3[_index]==1))
                                {
                                    _nulPostImage[_index] =1;
                                    _blemishCount++;
                                    _tmpCorrdiation.x += x;
                                    _tmpCorrdiation.y += y;

                                    ///-------------------------------Tiger v1.1.28-----------------------------
                                    //if (x > AA_Coor_XMax[_BlemishCount]) AA_Coor_XMax[_BlemishCount] = x;
                                    //if (y > AA_Coor_YMax[_BlemishCount]) AA_Coor_YMax[_BlemishCount] = y;
                                    //if (x < AA_Coor_XMin[_BlemishCount]) AA_Coor_XMin[_BlemishCount] = x;
                                    //if (y < AA_Coor_YMin[_BlemishCount]) AA_Coor_YMin[_BlemishCount] = y;
                                    ///-------------------------------Tiger v1.1.28-----------------------------
                                }
                            }
                        }
                        _tmpCorrdiation.x /=_blemishCount;
                        _tmpCorrdiation.y /=_blemishCount;

                        _BlemishBlockPoint[_BlemishCount].value = _blemishCount;
                        _BlemishBlockPoint[_BlemishCount].x = _tmpCorrdiation.x;
                        _BlemishBlockPoint[_BlemishCount].y = _tmpCorrdiation.y;

                        //掩滅已發現的Blemish
                        for (int y=0;y<height;y++)
                        {
                            for (int x=0;x<width;x++)
                            {
                                int _index = y*width+x;
                                if ((_cpyPostImage[_index]==1)&&(_nulPostImage[_index]==1))
                                {
                                    _cpyPostImage[_index] =0;
                                }
                            }
                        }

                        //	繼續發現下一?起始點
                        _BlemishCount++;

                        //RELEASE(_Point);
                        RELEASE_ARRAY(_iStatrPoint);
                        RELEASE_ARRAY(_iEndPoint);
                        RELEASE_ARRAY(_nulPostImage2);
                        RELEASE_ARRAY(_nulPostImage3);
                        RELEASE_ARRAY(_jStatrPoint);
                        RELEASE_ARRAY(_jEndPoint);
                    }

                    RELEASE_ARRAY(_cpyPostImage);
                    RELEASE_ARRAY(_nulPostImage);
                }

                void Blemish(unsigned char* RGBImage,
                    int Width,
                    int Height,
                    int WCutLowRange,
                    int WCutHighRange,
                    int HCutLowRange,
                    int HCutHighRange,
                    int &BlemishCount,
                    AA_TwoDPoint *BlemishBlockPoint)
                {
                    BlemishCount = 0;
                    //----------------------------------------------
                    //	這裡的截止頻率?對的是BandPass Filter的設定，其一般為浮點數（介於0～1之間）
                    //	然?際使用時，轉換為長度＊頻率的整數索引值
                    int CutLowFrequency = 0;
                    int CutHighFrequency = 0;

                    int ImageSize = Width*Height;

                    unsigned char* PostHoriImage = new unsigned char[ImageSize];
                    memset(PostHoriImage,0,sizeof(unsigned char)*ImageSize);

                    unsigned char* PostVertImage = new unsigned char[ImageSize];
                    memset(PostVertImage,0,sizeof(unsigned char)*ImageSize);

                    unsigned char* PostImage = new unsigned char[ImageSize];
                    memset(PostImage,0,sizeof(unsigned char)*ImageSize);

                    //DCT -> iDCT, Bandpass-------------------------
                    //	這裡是?入RGB影像，下面?理時會自行取出單Channel來分析
                    //	這裡附加Band Pass Filter?理。
                    //	最終?出是一二元影像，其內容即為Blemish之標?
                    CutLowFrequency = WCutLowRange;
                    CutHighFrequency = WCutHighRange;

                    // 	CutLowFrequency = int(double(Width)*CutLowFrequency);
                    // 	CutHighFrequency = int(double(Width)*CutHighFrequency);
                    HoriBandPass(RGBImage,PostHoriImage,Width,Height,CutLowFrequency,CutHighFrequency);


                    CutLowFrequency = HCutLowRange;
                    CutHighFrequency = HCutHighRange;

                    // 	CutLowFrequency = int(double(Height)*CutLowFrequency);
                    // 	CutHighFrequency = int(double(Height)*CutHighFrequency);
                    VertBandPass(RGBImage,PostVertImage,Width,Height,CutLowFrequency,CutHighFrequency);

                    unsigned char* _tmpP = PostImage;
                    unsigned char* _tmpPH = PostHoriImage;
                    unsigned char* _tmpPV = PostVertImage;

                    for (int i=0; i<Width*Height; i++)
                    {
                        //-------------------------------------------------------------
                        if (((*_tmpPH)>0)&&((*_tmpPV)>0))
                        {
                            (*_tmpP) = 1;
                        }
                        else
                        {
                            (*_tmpP) = 0;
                        }
                        _tmpP++;
                        _tmpPH++;
                        _tmpPV++;

                        //-------------------------------------------------------------
                    }

                    RELEASE_ARRAY(PostVertImage);
                    RELEASE_ARRAY(PostHoriImage);

                    //----------------------------------------------
                    ImageDilate(PostImage,Width,Height, 2);
                    ImageErode(PostImage,Width,Height, 1);
                    ImageDilate(PostImage,Width,Height, 3);
                    ImageErode(PostImage,Width,Height, 2);

                    memset(BlemishBlockPoint,0,sizeof(AA_TwoDPoint)*DEF_BLEMISH_MAXCOUNT);
                    FindBlemishBlock(PostImage, Width, Height,BlemishBlockPoint,BlemishCount);

                    RELEASE_ARRAY(PostImage);
                }
            }
        }
    }
}
