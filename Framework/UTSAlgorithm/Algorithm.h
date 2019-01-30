#pragma once
#include <atltypes.h>
#include <vector>
#include <stdint.h>

using namespace std;

#ifdef UTSALGORITHM_EXPORTS
#define UTSALGORITHM_API __declspec(dllexport)
#else
#define UTSALGORITHM_API __declspec(dllimport)
#endif

namespace UTS
{
    namespace Algorithm
    {
		template <typename T>
		inline const T &fnMin(const T &a, const T &b) { return((a < b) ? (a) : (b)); }

		template <typename T>
		inline const T &fnMax(const T &a, const T &b) { return((a < b) ? (b) : (a)); }

		template <typename T>
		inline T fnRound(const T &a)
		{
			return(((a) >= (0.0f)) ? (int((a)+0.5f)):(int((a)-0.5f)));
		}

		template <typename T,typename T2,typename T3>
		inline const T &fnRange(const T &val, const T2 &min, const T3 &max)
		{
			return fnMax(static_cast<T>(min), fnMin(static_cast<T>(max), val));
		}

        typedef enum _e_corner_
        {
            Corner_LU = 0,
            Corner_RU,
            Corner_LD,
            Corner_RD,
            Corner_SIZES
        } eCorner;

        typedef struct AA_TwoDPoint
        {
            int x;
            int y;
            double value;
        } AA_TwoDPoint;

        typedef struct _color_rgb_double
        {
            double dR;
            double dG;
            double dB;
        } COLOR_RGB_DOUBLE;

		typedef struct color_rgrgbb_double
		{
			double R;
			double Gr;
			double Gb;
			double B;
		}COLOR_RGrGbB_DOUBLE;

		typedef struct color_rgrgbb_uShort
		{
			uint16_t R;
			uint16_t Gr;
			uint16_t Gb;
			uint16_t B;
		}COLOR_RGrGbB_USHORT;

		typedef struct color_rgrgbb_uchar
		{
			uint8_t R;
			uint8_t Gr;
			uint8_t Gb;
			uint8_t B;
		}COLOR_RGrGbB_UCHAR;

        //------------------------------------------------------------------------------
        // 图像变换
        namespace Image
        {
            class UTSALGORITHM_API CImageProc
            {
            public:
                static CImageProc& GetInstance();
                void RawToBmp(int nColorOrder, BYTE *pRawBuffer, BYTE *pBmpBuffer, int size_x, int size_y);
                void YCbCrITUToBmp(int iMode, BYTE *pYuvBuffer, BYTE *pBmpBuffer, int size_x, int size_y);
                void BufferConv_YUVRawToYUV24(
                    __in const BYTE *pYUVRawBuffer,
                    __in int nWidth,
                    __in int nHeight,
                    __in int nMode,
                    __out BYTE *pYUV24Buffer);
                void BufferConv_YUV24ToBmp(
                    __in const BYTE *pYUV24Buffer,
                    __in int nWidth,
                    __in int nHeight,
                    __in int nStandard, // 0: BT709
                    __out BYTE *pBmpBuffer);
                void BufferConv_BmpToYUV24(
                    __in const BYTE *pBmpBuffer,
                    __in int nWidth,
                    __in int nHeight,
                    __in int nStandard, // 0: BT709
                    __out BYTE *pYUV24Buffer);
                void SplitYUV24Buffer(
                    __in const BYTE *pYUV24Buffer,
                    __in int nWidth,
                    __in int nHeight,
                    __out BYTE *pYBuffer,
                    __out BYTE *pCbBuffer,
                    __out BYTE *pCrBuffer);
                void GetYUV24_YBuffer(
                    __in const BYTE *pYUV24Buffer,
                    __in int nWidth,
                    __in int nHeight,
                    __out BYTE *pYBuffer);
                void GetYUV24_CbBuffer(
                    __in const BYTE *pYUV24Buffer,
                    __in int nWidth,
                    __in int nHeight,
                    __out BYTE *pCbBuffer);
                void GetYUV24_CrBuffer(
                    __in const BYTE *pYUV24Buffer,
                    __in int nWidth,
                    __in int nHeight,
                    __out BYTE *pCrBuffer);
                void bit10_To_bit8(BYTE *pSrcBuffer, BYTE *pDestBuffer, unsigned int width, unsigned int height);		// v1.1.7 , Rickson
                void Shift10BitMode(unsigned char* pImage, unsigned char* pDest, unsigned int nWidth, unsigned int nHeight);
				void Make10BitMode(unsigned char* pImage,unsigned char* pDest,unsigned int _Width,unsigned int _Height);
                void Make10BitMode(unsigned char* pImage, WORD* pDest, unsigned int nWidth, unsigned int nHeight);				// v1.1.7 , Rickson
                void Shift12BitMode(unsigned char* pImage, unsigned char* pDest, unsigned int nWidth, unsigned int nHeight);
                void Make12BitMode(unsigned char* pImage, WORD* pDest, unsigned int nWidth, unsigned int nHeight);				// v1.1.7 , Rickson
                // 根据V5U读取的Buffer，转换出Raw8格式的和Bmp格式的Buffer
                BOOL GetRaw8AndBmpBuffer(
                    unsigned char *pDataBuffer,
                    UINT nImageWidth,
                    UINT nImageHeight,
                    UINT nDataFormat,
                    UINT nColorOrder,
                    UINT nMipiMode,
                    unsigned char *pRaw8Buffer,
                    unsigned char *pBmpBuffer);

                void GetFlipedBmpBuffer(
                    const unsigned char* pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    unsigned char* pFlipedBmpBuffer);

                void Cal_RGBtoYBuffer(
                    const unsigned char* pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    unsigned char* pYbuffer);

				void Cal_RGBtoGBuffer(
					const unsigned char* pBmpBuffer,
					int nWidth,
					int nHeight,
					unsigned char* pYbuffer);

				void Cal_RGBtoRBuffer(
					const unsigned char* pBmpBuffer,
					int nWidth,
					int nHeight,
					unsigned char* pYbuffer);

				void Cal_RGBtoBBuffer(
					const unsigned char* pBmpBuffer,
					int nWidth,
					int nHeight,
					unsigned char* pYbuffer);

                void Cal_YBuffertoGrayBmpBuffer(
                    const unsigned char* pYbuffer,
                    int nWidth,
                    int nHeight,
                    unsigned char* pBmpBuffer);
                
                void GetFlipedYBlockBuffer(
                    const unsigned char *pYBuffer,
                    unsigned char *pFlipedYBlockBuffer,
                    int _ImgWidth,
                    int _ImgHeight,
                    int _StartX,
                    int _StartY,
                    int _BlockWidth,
                    int _BlockHeight);

                void GetYBlockBuffer(
                    const unsigned char *pYBuffer,
                    unsigned char *pYBlockBuffer,
                    int _ImgWidth,
                    int _ImgHeight,
                    int _StartX,
                    int _StartY,
                    int _BlockWidth,
                    int _BlockHeight);

                void GetBMPBlockBuffer(
                    const unsigned char *ImgSource,
                    unsigned char *ImgBlock,
                    int _ImgWidth,
                    int _ImgHeight,
                    int _StartX,
                    int _StartY,
                    int _BlockWidth,
                    int _BlockHeight);

                void GetBMPBlockBufferNoFlip(
                    const unsigned char *ImgSource,
                    unsigned char *ImgBlock,
                    int _ImgWidth,
                    int _ImgHeight,
                    int _StartX,
                    int _StartY,
                    int _BlockWidth,
                    int _BlockHeight);

                void ThreasholdYBuffer(
                    unsigned char* pYbuffer,
                    int nWidth,
                    int nHeight,
                    unsigned char cThreashold);

                void BMPBufferQuarteReduce(
                    unsigned char *pImgSource,
                    unsigned char *pImgReduce,
                    int nImgWidth,
                    int nImgHeight);

				void GetFlipedBmpBGRtoRGBBuffer(
					const unsigned char* pBmpBuffer,
					int nWidth,
					int nHeight,
					unsigned char* pFlipedBmpBuffer);

				void Shift10BitMode_2PD(
					unsigned char* pImage, 
					unsigned char* pDest, 
					unsigned int nWidth, 
					unsigned int nHeight);
            private:
                BYTE	RtblITU[256][256];
                BYTE	GtblITU[256][256][256];
                BYTE	BtblITU[256][256];

                CImageProc();
                CImageProc(const CImageProc&);
                CImageProc& operator = (const CImageProc&);
                ~CImageProc();

                void MakeSpaceTableITU(void);
                void GetYUV24_OneChannelBuffer(
                    __in const BYTE *pYUV24Buffer,
                    __in int nWidth,
                    __in int nHeight,
                    __in int nChannelOffset,
                    __out BYTE *pOneChannelBuffer);
				
            };
        }

        //------------------------------------------------------------------------------
        // 数学计算相关
        namespace Math
        {
            // 计算平面两点间的距离
            UTSALGORITHM_API double GetDistance2D(
                double x1, double y1,
                double x2, double y2);
            // 计算空间两点间的距离
            UTSALGORITHM_API double GetDistance3D(
                double x1, double y1, double z1,
                double x2, double y2, double z2);
            // 计算矩形的第四个点(逆时针传入四个点的坐标)
            UTSALGORITHM_API void GetLastPointOfRectangle(
                const POINT &ptA,
                const POINT &ptB,
                const POINT &ptC,
                POINT &ptD);
            // 计算方差
            UTSALGORITHM_API double Variance(double f[], int size);
            // 计算标准差
            UTSALGORITHM_API double StandardDeviation(double f[], int size);
            // 根据4对点组成的直线，求交点
            UTSALGORITHM_API void GetCrossPoint(
                int x0, int y0,   // 第1条直线上的第1个点
                int x1, int y1,   // 第1条直线上的第2个点
                int x2, int y2,   // 第2条直线上的第1个点
                int x3, int y3,   // 第2条直线上的第2个点
                LONG &x, LONG &y);
            // 根据中心取得rect
            UTSALGORITHM_API void GetRectByCenter(
                const POINT& ptCenter,
                const SIZE& sizeROI,
                RECT &rcReturn);

            // 计算曲线起始点
            UTSALGORITHM_API void FindStartXValue(
                int nSameDataCount,
                int nTotalDataCount,
                int *pXValue,
                double* pYValue,
                int &nStartXValue);

			UTSALGORITHM_API short Change2Complement(short value);
		

        }

        //------------------------------------------------------------------------------
        // 色彩空间转换
        namespace ColorSpace
        {
            typedef struct _color_lab
            {
                double dL;
                double dA;
                double dB;
            } COLOR_LAB;

            //-------------------------------------------------------------------------
            // RGB <===> Lab 色彩空间转换
            UTSALGORITHM_API void RGBInt2Lab(int R, int G, int B, double &L, double &a, double &b);
            UTSALGORITHM_API void RGBDouble2Lab(double R, double G, double B, double &L, double &a, double &b);
        }

        //------------------------------------------------------------------------------
        // 十字标记
        namespace CrossHair
        {
            typedef enum _e_crosshair_location_
            {
                Cross_Hair_Left = 0,	
                Cross_Hair_Right,	
                Cross_Hair_Top,
                Cross_Hair_Bottom,
                Cross_Hair_Num
            } _tageCrossHairLoc;

            class UTSALGORITHM_API CCrossHair
            {
            public:
                CCrossHair(void);
                ~CCrossHair(void);

                //------------------------------------------------------------------------------
                // Step1: 设置十字ROI
                void SetROI(int DisXfromCT, int DisYfromCT, int ROISize, double CodeVariation);

                //------------------------------------------------------------------------------
                // Step2: 计算
                BOOL Calculate(BYTE *pBmpBuffer, int nWidth, int nHeight);

                //------------------------------------------------------------------------------
                // Step3: 取得结果
                // Chart的中心
                POINT ptChartCenter;
                // 四个定位十字的中心
                POINT ptCrossHairCenter[Cross_Hair_Num];
                // 十字框的位置
                RECT rcCrossHairDetectROI[Cross_Hair_Num];
                //// 左右十字之间的距离
                //double dCrossHairWidth;
                //// 上下十字之间的距离
                //double dCrossHairHeight;

                POINT Cross_LinePosition(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    int xstart,
                    int ystart,
                    int boxwidth,
                    int boxheight,
                    float *crossMaxvalue);

            private:
                // 配置文件信息
                int	m_CrossHairDisXfromCT;
                int	m_CrossHairDisYfromCT;
                int	m_CrossHairROISize;
                double m_CrossHair_Code_Variation;
            };
        }

        namespace MTF
        {
            namespace MtfCalc
            {
                UTSALGORITHM_API void GetBlockWhiteYBlackY(
                    __in  const unsigned char *pYBuffer,
                    __in  int nWidth,
                    __in  int nHeight,
                    __in  const RECT &rcBlock,
                    __out double &dWhiteY,
                    __out double &dBlackY);

				UTSALGORITHM_API void GetYBlockMTF(
					const unsigned char *pYBuffer,
					int nWidth,
					int nHeight,
					const RECT &rcROI,
					double &dMTF);
            }

            namespace MtfStd
            {
                const int MAX_EDGE_FIELD_COUNT = 3;
                typedef enum _e_mtf_field_location_
                {
                    MtfFieldLoc_CT = 0,
                    MtfFieldLoc1_LU = 1,
                    MtfFieldLoc1_RU,
                    MtfFieldLoc1_RD,
                    MtfFieldLoc1_LD,
                    MtfFieldLoc2_LU = 5,
                    MtfFieldLoc2_RU,
                    MtfFieldLoc2_RD,
                    MtfFieldLoc2_LD,
                    MtfFieldLoc3_LU = 9,
                    MtfFieldLoc3_RU,
                    MtfFieldLoc3_RD,
                    MtfFieldLoc3_LD,
                    MtfFieldLoc_SIZES
                } eMtfFieldLoc;

                typedef struct _mtf_param_
                {
                    vector<double> vecField;
                    SIZE sizeMtfROI;
                } MTF_PARAM;

                typedef struct _mtf_result_
                {
                    RECT rcArrROI[MtfFieldLoc_SIZES];
                    POINT ptArrROICenter[MtfFieldLoc_SIZES];
                    double dArrMTF[MtfFieldLoc_SIZES];
					double dArrMTFDelta[MAX_EDGE_FIELD_COUNT];
                } MTF_RESULT;


                UTSALGORITHM_API void GetAllMTF_Y(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    const MTF_PARAM &param,
                    MTF_RESULT &result);

                UTSALGORITHM_API void GetAllMTF(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const MTF_PARAM &param,
                    MTF_RESULT &result);

            }
        }

        namespace SFR
        {
            //------------------------------------------------------------------------------
            // SFR 计算
            namespace SfrCalc
            {
                typedef enum _e_sfr_channel_
                {
                    SFR_ORG = 0,
                    SFR_R,
                    SFR_G,
                    SFR_B,
                    SFR_LUMN,
                    SFR_SIZES
                } eSFRChannel;

                typedef enum _e_sfr_frq_
                {
                    SFR_Frq_Lo = 0,
                    SFR_Frq_Hi,	
                    SFR_Frq_SIZES
                } eSfrFrq;

				//SFRmat 3
				class  SFR3Mat
				{
				public:
					SFR3Mat(void);
					~SFR3Mat(void);
					bool Run(double* _pSrcBuffer,int _Width,int _Height);
					double GetSFRValue(double _Ny);
					void GetAllSFR(double *_SFRArray,double *_FreqList);
					int GetSFRArraySize(void);

				protected:
				private:
					double fil1[2];
					double fil2[3];
					bool bOldFlag;

					double del;	// frequency is given in cy/pixel
					int nBin;	// = 4;

					double* SFRArray;
					double* FreqList;
					int ArraySize;
				};
				//-------------------------------------------------
                UTSALGORITHM_API void GetBlockSFR(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const RECT &rcROI,
                    double dFrqHiNum,
                    double dFrqLoNum,
                    double &dHiSFR,
                    double &dLoSFR);

                UTSALGORITHM_API void CaculateSFRValue(
                    UCHAR* _InImage,
                    double *_SFR_HiFrq_Value,
                    double *_SFR_LoFrq_Value,
                    double _HighFrqNum,
                    double _LowFrqNum,
                    int _SFRType,
                    int _ROIWidth,
                    int _ROIHeight);



                // 根据SFR分数，返回对应频率的版本
                UTSALGORITHM_API void GetBlockSFRWithFrq(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const RECT &rcROI,
                    double dFrqHiNum,
                    double dFrqLoNum,
                    double dSFRNum,
                    double &dHiSFR,
                    double &dLoSFR,
                    double &dFrqNum);

                // 根据SFR分数，返回对应频率的版本
                UTSALGORITHM_API void CaculateSFRValueWithFrq(
                    UCHAR* _InImage,
                    double *_SFR_HiFrq_Value,
                    double *_SFR_LoFrq_Value,
                    double *pdFrqNum,
                    double _HighFrqNum,
                    double _LowFrqNum,
                    double dSFRNum,
                    int _SFRType,
                    int _ROIWidth,
                    int _ROIHeight);

				//  [10/13/2017 CJ] debug SFR = 0 when degree error 
				UTSALGORITHM_API void CaculateSFR3Value(
					unsigned char* _InImage,
					int _Width, 
					int _Height, 
					int _SFRType,
					double BLC,
					double _HighFrqNum,
					double _LowFrqNum,
					double &_SFR_HiFrq_Value,
					double &_SFR_LoFrq_Value);


            }

            //------------------------------------------------------------------------------
            // 标准SFR Chart
            namespace SfrStd
            {
                const int FIELD_SIZES = 4;

                typedef enum _e_sfr_field_location_
                {
                    SfrFieldLoc_LU = 0,
                    SfrFieldLoc_U,
                    SfrFieldLoc_RU,
                    SfrFieldLoc_R,
                    SfrFieldLoc_RD,
                    SfrFieldLoc_D,
                    SfrFieldLoc_LD,
                    SfrFieldLoc_L,
                    SfrFieldLoc_SIZES
                } eSfrFieldLoc;

                typedef enum _e_enable_mask_
                {
                    MaskL  = 0x01,
                    MaskLD = 0x02,
                    MaskD  = 0x04,
                    MaskRD = 0x08,
                    MaskR  = 0x10,
                    MaskRU = 0x20,
                    MaskU  = 0x40,
                    MaskLU = 0x80,
                } eEnableMask;

                //-------------------------------------------------------------------------
                // ROI
                typedef struct _roi
                {
                    // 中心视场: 1个
                    POINT ptCenter;
                    RECT rcCenter;
                    // 边缘每个视场: 4 * 8 = 32个
                    POINT ptEdge[FIELD_SIZES][SfrFieldLoc_SIZES];
                    RECT rcEdge[FIELD_SIZES][SfrFieldLoc_SIZES];
                } ROI;

                typedef struct _SFR_PARAM
                {
                    double dFrqNum[SfrCalc::SFR_Frq_SIZES];      // 每个频率的数值
                    POINT ptChartCenter;                // Chart的中心
                    POINT ptCrossHairCenter[CrossHair::Cross_Hair_Num]; // 每个十字的中心
                    double dCrossHairOffset;            // 十字所在的视场的数值
                    SIZE SFR_ROI;                       // SFR的ROI大小
                    BYTE FieldTestEnable[FIELD_SIZES];  // 每个视场8个bit: LU, U, RU, R, RD, D, LD, L
                    double dFieldOffset[FIELD_SIZES];   // 每个边缘视场的数值
                } SFR_PARAM;

                typedef struct _SFR_RESULT
                {
                    ROI roi;
                    double dCenterSFR[SfrCalc::SFR_Frq_SIZES];
                    double dEdgeSFR[FIELD_SIZES][SfrFieldLoc_SIZES][SfrCalc::SFR_Frq_SIZES];
                } SFR_RESULT;

                /*
                function: Get the whole bmp image SFRs
                */
                UTSALGORITHM_API void GetAllSFR(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const SFR_PARAM &param,
                    SFR_RESULT &result);
            }

            //------------------------------------------------------------------------------
            // SFR Plus Chart
            namespace SfrPlus
            {
                const int SFR_PLUS_MAX_ROI = 256;

                //-------------------------------------------------------------------------
                // ROI
                typedef struct _roi
                {
                    POINT ptCenter[SFR_PLUS_MAX_ROI];
                    RECT rcRect[SFR_PLUS_MAX_ROI];
                } ROI;

                typedef struct _sfr_plus_param_static_
                {
                    double dCrossHairField;                 // 十字交点所在的视场的数值
                    double dFrqNum[SfrCalc::SFR_Frq_SIZES]; // 计算哪些频率的SFR
                    double dSFRNum;                         // 计算哪个SFR的频率和Nq(开启会影响效率，如果案子不用，请设为-1.0)
                    SIZE SFR_ROI;                           // SFR的ROI大小
                    int nSfrCount;                          // 启用的SFR数
                    double dROIField[SFR_PLUS_MAX_ROI];     // ROI中心点所在视场
                    double dROIAngle[SFR_PLUS_MAX_ROI];     // ROI中心点的极坐标角度(0-359.9999)
                } SFRPLUS_PARAM_STATIC;

                typedef struct _sfr_plus_param_dynamic_
                {
                    POINT ptChartCenter;                                // Chart的中心
                    POINT ptCrossHairCenter[CrossHair::Cross_Hair_Num]; // 每个十字的中心
                } SFRPLUS_PARAM_DYNAMIC;

                typedef struct _sfr_plus_result
                {
                    ROI roi;
                    double dSFR[SFR_PLUS_MAX_ROI][SfrCalc::SFR_Frq_SIZES];  // 根据指定频率计算的SFR数值
                    double dFrq[SFR_PLUS_MAX_ROI];  // 根据指定SFR计算的频率
                    double dNq[SFR_PLUS_MAX_ROI];   // 根据指定的SFR计算的Nq
                } SFRPLUS_RESULT;
                
                class UTSALGORITHM_API CSfrPlus
                {
                public:
                    /*
                    Set param for calc SFR
                    */
                    void SetParam(const SFRPLUS_PARAM_STATIC &staticParam);

                    /*
                    Get the whole bmp image SFRs
                    */
                    void CalculateSFR(
                        unsigned char *pBmpBuffer,
                        int nWidth,
                        int nHeight,
                        const SFRPLUS_PARAM_DYNAMIC &dynamicParam,
                        SFRPLUS_RESULT &result);

                private:
                    double m_dSinValue[SFR_PLUS_MAX_ROI];
                    double m_dCosValue[SFR_PLUS_MAX_ROI];

                    int m_nType[SFR_PLUS_MAX_ROI];
                    SFRPLUS_PARAM_STATIC m_staticParam;
                };
            }   // namespace SfrPlus
        }   // namespace SFR

        namespace RI
        {
            typedef struct _ri_result_
            {
                POINT ptCenterXY;
                double dRICorner[Corner_SIZES];
                double dRI;
                double dRIDelta;
            } RI_RESULT;

            namespace RI_OpticalCenter
            {
                UTSALGORITHM_API void RI_Y(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result);

                UTSALGORITHM_API void RI_Raw(
                    unsigned char *pRawBuffer,
                    int nWidth,
                    int nHeight,
                    int nBayerType,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result);

                UTSALGORITHM_API void RI_RGB(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result);
            }

            namespace RI_LogicalCenter
            {
                UTSALGORITHM_API void RI_Y(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result);

                UTSALGORITHM_API void RI_Raw(
                    unsigned char *pRawBuffer,
                    int nWidth,
                    int nHeight,
                    int nBayerType,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result);

                UTSALGORITHM_API void RI_RGB(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    int nROISizeX,
                    int nROISizeY,
                    RI_RESULT &result);
            }
        }

        namespace DefectPixel
        {
            namespace DefectPixelStd
            {
                const int DEFECT_COUNT_MAX = 1000;

                typedef struct  
                {
                    int TotalSingleDefectCount;
                    int TotalDoubleDefectCount;
                    int TotalMultipleDefectCount;
                    int CDefectCount;
                    int H4ISPDefectPixelCount;
                    //-----------------------------------------------------------------
                    // 20130902-Rod G1/G2 combined defect calculate - create combined G defect count.
                    int CGSingleDefectCount;
                    int CGDoubleDefectCount;
                    int CGMultipleDefectCount;
                    //-----------------------------------------------------------------
                    int VertDefLineCount_CH0;		///  v1.1.24 , Rickson
                    int HoriDefLineCount_CH0;		///  v1.1.24 , Rickson
                    int VertDefLineCount_CH1;		///  v1.1.24 , Rickson
                    int HoriDefLineCount_CH1;		///  v1.1.24 , Rickson
                    int VertDefLineCount_CH2;		///  v1.1.24 , Rickson
                    int HoriDefLineCount_CH2;		///  v1.1.24 , Rickson
                    int VertDefLineCount_CH3;		///  v1.1.24 , Rickson
                    int HoriDefLineCount_CH3;		///  v1.1.24 , Rickson
                    int VertTotal;					///  v1.1.24 , Rickson
                    int HoriTotal;					///  v1.1.24 , Rickson
                } H4ISPDefectInfo;

                UTSALGORITHM_API void DefectPixels(
                    unsigned char *_pRAWImage,
                    int Width,
                    int Height,
                    double DefThreshold,
                    int ROIWidth,
                    int ROIHeight,
                    int AA_DefectPixel3ChannelEn,
                    H4ISPDefectInfo &DefectCount,
                    AA_TwoDPoint *DefectTable,
                    int RAWBayerType);

                UTSALGORITHM_API void DefectPixelsRGB(
                    unsigned char *_pBmpImage,
                    int Width,
                    int Height,
                    double DefThreshold,
                    int ROIWidth,
                    int ROIHeight,
                    H4ISPDefectInfo &DefectCount,
                    AA_TwoDPoint *DefectTable);
                    
                UTSALGORITHM_API void DefectPixelDarkDead(
                    unsigned char *_pRAWImage,
                    int Width,
                    int Height,
                    double DefThreshold,
                    int ROIWidth,
                    int ROIHeight,
                    int AA_DefectPixel3ChannelEn,
                    H4ISPDefectInfo &DefectCount,
                    AA_TwoDPoint *DefectTable,
                    int RAWBayerType);
            }
        }

        namespace DefectLine
        {
            namespace DefectLineStd
            {
                UTSALGORITHM_API void DefectLine(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    double dThreshold,
                    vector<int> &vecHDefectLine,
                    vector<int> &vecVDefectLine);
            }
        }

        namespace ColorChart
        {
            const int MAX_COLOR_NUM = 24;
            //const int ROW_NUM = 4;
            //const int COL_NUM = 6;

            typedef struct _color_chart_param_
            {
                int nRowNum;        // 行数
                int nColNum;        // 列数
                int nChartMargin;   // 黑边的宽度
                int nYThreshold;    // 亮度阀值
                CRect rcChartROI;   // 整个Chart的ROI
            } COLOR_CHART_PARAM;

            UTSALGORITHM_API void GetChartRect(
                const unsigned char *pBmpBuffer,
                int nWidth,
                int nHeight,
                const COLOR_CHART_PARAM& param,
                RECT &rcChart);

            UTSALGORITHM_API void GetColorCenterPoint(
                const RECT *prcChart,
                int nRow,
                int nCol,
                POINT *pColorPoint);
        }

        namespace CR    // ColorReproducibility | ColorAccuracy | ColorFidelity
        {
            namespace CR_Std
            {
                typedef struct _color_info
                {
                    int nColorIndex;    // 从上到下，从左到右。 范围是0 ~ 23的整数。
                    ColorSpace::COLOR_LAB stColorLab;
                } COLOR_INFO;

                typedef struct _cr_param_
                {
                    ColorChart::COLOR_CHART_PARAM stColorParam;           // 24 color chart 抓取需要用到的参数
                    SIZE sizeColorROI;                                    // 抓取单个颜色的ROI
                    int nColorCount;                                      // 抓取Color数
                    COLOR_INFO stArrColorInfo[ColorChart::MAX_COLOR_NUM]; // Color信息
                } CR_PARAM;

                typedef struct _cr_roi_
                {
                    RECT rcChartRect;
                    POINT ptColorCenter[ColorChart::MAX_COLOR_NUM];
                    RECT rcColorRect[ColorChart::MAX_COLOR_NUM];
                } CR_ROI;

                typedef struct _cr_result_
                {
                    CR_ROI roi;
                    COLOR_RGB_DOUBLE rgbAvg[ColorChart::MAX_COLOR_NUM];
                    ColorSpace::COLOR_LAB labVal[ColorChart::MAX_COLOR_NUM];
                    double dDeltaE[ColorChart::MAX_COLOR_NUM];
                    double dDeltaC[ColorChart::MAX_COLOR_NUM];
                } CR_RESULT;

                UTSALGORITHM_API void ColorReproducibility(
                    unsigned char* pBmpBuffer,
                    int width,
                    int height,
                    const CR_PARAM &param,
                    CR_RESULT &result);
            }
        }

        namespace ColorShading
        {
            //-------------------------------------------------------------------------
            // DeltaC =Sqrt[(a*-a*center)2+( b*-b*center)2]
            namespace ColorShading_CenterCorner
            {
                typedef struct _cs_param_
                {
                    SIZE sizeROI;
                } CS_PARAM;

                typedef struct _cs_roi_
                {
                    RECT rcCenter;
                    RECT rcCorner[Corner_SIZES];
                } CS_ROI;

                typedef struct _cs_result_
                {
                    CS_ROI roi;
                    COLOR_RGB_DOUBLE rgbCenter;
                    COLOR_RGB_DOUBLE rgbCorner[Corner_SIZES];
                    ColorSpace::COLOR_LAB labCenter;
                    ColorSpace::COLOR_LAB labCorner[Corner_SIZES];
                    double dDeltaC[Corner_SIZES];
                    double dMaxDeltaC;
                    double dDeltaCDelta;
                    double dDeltaE[Corner_SIZES];
                    double dMaxDeltaE;
                    double dDeltaEDelta;
                } CS_RESULT;

				
                UTSALGORITHM_API void ColorShading(
                    const unsigned char* pBmpBuffer,
                    int width,
                    int height,
                    const CS_PARAM &param,
                    CS_RESULT &result);
            }
		}

		namespace ColorShading_NOKIA
		{
			typedef struct _cs_ratio_nokia_result_
			{
				double dCenterRatioBG;
				double dCenterRatioRG;
				double dCenterRatioGrGb;
				double dCornerRatioBG[Corner_SIZES];
				double dCornerRatioRG[Corner_SIZES];
			} CS_RATIO_NOKIA_RESULT;

			typedef struct _cs_delta_nokia_result_
			{
				double dMaxDeltaC;
				ColorSpace::COLOR_LAB lab[5];
				double dDeltaC[5];
			} CS_DELTA_NOKIA_RESULT;

			UTSALGORITHM_API void ColorShadingRatio(
				unsigned char *pRaw8Buffer,
				int nWidth,
				int nHeight,
				int nBayerType,
				double blackLvl,
				int nROISizeX,
				int nROISizeY,
				CS_RATIO_NOKIA_RESULT &result);

			UTSALGORITHM_API void ColorShadingDelta(
				unsigned char* pBmpBuffer,
				int nWidth,
				int nHeight,
				int nROISizeX,
				int nROISizeY,
				double *MeanA,
				double *MeanB,
				CS_DELTA_NOKIA_RESULT &result);
		}

		namespace ColorShadingA
		{
			//-------------------------------------------------------------------------
			// DeltaC =Sqrt[(a*-a*center)2+( b*-b*center)2]
			namespace ColorShading_Matrix
			{
				UTSALGORITHM_API void ColorShading(
					unsigned char* pBmpBuffer, 
					int nWidth,
					int nHeight,
					int nHBlockCount,
					int nVBlockCount,
					double &MaxDeltaC);
			}
		}
		
        namespace OC
        {
            namespace OC_Std
            {
                UTSALGORITHM_API void OpticalCenter(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    POINT &ptCenter);

                UTSALGORITHM_API void OpticalCenterRGB(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    POINT &ptCenter);

				UTSALGORITHM_API void OpticalCenterRGB(
					const unsigned char *pBmpBuffer,
					int nWidth,
					int nHeight,
					POINT &ptCenter);

				UTSALGORITHM_API void OpticalCenterRAW(
					unsigned char *_pRAWImage,
					int nWidth,
					int nHeight,
					int rawBayerType,
					POINT *ptCenter);
            }
        }

        namespace Blemish
        {
            namespace Blemish_Std
            {
                #define DEF_BLEMISH_MAXCOUNT    10

                UTSALGORITHM_API void Blemish(
                    unsigned char* RGBImage,
                    int Width,
                    int Height,
                    int WCutLowRange,
                    int WCutHighRange,
                    int HCutLowRange,
                    int HCutHighRange,
                    int &BlemishCount,
                    AA_TwoDPoint *BlemishBlockPoint);
            }

            namespace Blemish_Circle
            {
                //------------------------------------------------------------------------------
                // param struct
                typedef struct _blemish_circle_param_
                {
                    int nAngleStep;
                    int nRadiusStep;
                    int nMoveStep;
                    int nMaxRadius;    // max blemish radius
                    int nMinRadius;    // min blemish radius
                    double dThreshold; // threshold
                } BlemishCircleParam;

                //------------------------------------------------------------------------------
                // result struct
                typedef struct _circle_
                {
                    int x;
                    int y;
                    int r;
                } Circle;

                //------------------------------------------------------------------------------
                // find blemish from rgb buffer
                UTSALGORITHM_API void CircleBlemish_RGB(
                    __in  unsigned char *pBmpBuffer,
                    __in  int nWidth,
                    __in  int nHeight,
                    __in  const BlemishCircleParam &param,
                    __out std::vector<Circle> &vResult);

                //------------------------------------------------------------------------------
                // find blemish from Y buffer
                UTSALGORITHM_API void CircleBlemish_Y(
                    __in  unsigned char *pYBuffer,
                    __in  int nWidth,
                    __in  int nHeight,
                    __in  const BlemishCircleParam &param,
                    __out std::vector<Circle> &vResult);
            }
        }

        namespace DarkNoise
        {
            namespace DarkNoise_WholeImage
            {
                UTSALGORITHM_API void DarkNoiseY(
                    const unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    double &dStdDevY);

                UTSALGORITHM_API void DarkNoiseRGB(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    double &dStdDevY);

				UTSALGORITHM_API void DarkNoiseAltek(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    double &dAvgR,
					double &dAvgG,
					double &dAvgB);
            }
        }

		namespace FixPatternNoise
        {
            namespace FixPatternNoise_WD
            {
                typedef struct
				{
					double  m_dRowDifMax;
					double  m_dColDifMax;
					double *YArray;
					double *RowMeanArray;
					double *ColMeanArray;
					LPBYTE	pImageBuffer;
					int		m_nLoopSize;
					int		m_nR;
					int		m_nG;
					int		m_nB;
				}FPNInfo;

				UTSALGORITHM_API void FixPatternNoiseY(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    double &dRowDifMaxMean,
					double &dColDifMaxMean);
			}
		}

		namespace AWB
		{
			namespace AWB_Altek
			{
				typedef struct
				{
					double dRG[4];
					double dBG[4];
					double dRGCenter;
					double dBGCenter;
					double dRGMax;
					double dBGMax;
					double dRGDelta;
					double dBGDelta;
				} AWBInfo;

				UTSALGORITHM_API void AWBY(
					unsigned char *pBmpBuffer,
					int nWidth,
					int nHeight,
					int nCenterWidth,
					int nCenterHeight,
					AWBInfo &result);
			}
		}

        namespace RectangleChart
        {
            typedef struct _rect_chart_param_
            {
                SIZE sizeROI;
            } RECT_CHART_PARAM;

            typedef struct _rect_chart_result_
            {
                RECT rcROI[Corner_SIZES + 2];               // +1: Top  +2: Bottom
                POINT ptRectChartPoint[Corner_SIZES + 2];   // +1: Top  +2: Bottom
            } RECT_CHART_RESULT;

            void GetRectChartPoints(
                const unsigned char *pBmpBuffer,
                int nWidth,
                int nHeight,
                const RECT_CHART_PARAM &param,
                RECT_CHART_RESULT &result);
        }

        namespace TV_Distortion
        {
            namespace TV_Distortion_Rectangle
            {
                typedef struct _tvd_param_
                {
                    SIZE sizeROI;
                } TVD_PARAM;

                typedef struct _tvd_roi_
                {
                    RECT rcCorner[Corner_SIZES];
                    POINT ptCorner[Corner_SIZES];
                    RECT rcTop;
                    POINT ptTop;
                    RECT rcBottom;
                    POINT ptBottom;
                } TVD_ROI;

                typedef struct _tvd_result_
                {
                    TVD_ROI roi;
                    double dALeft;
                    double dARight;
                    double dB;
                    double dTvDistortion;   // ((ALeft + ARight) / 2 - B) / (2 * B) * 100%，注意：未取绝对值
                } TVD_RESULT;

                UTSALGORITHM_API void TvDistortion(
                    const unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const TVD_PARAM &param,
                    TVD_RESULT &result);
            }
        }

        namespace DFOV
        {
            namespace DFOV_Rectangle
            {
				typedef struct _fov_result_
				{
					double hfov;
					double vfov;
					double dfov;
				}FOV_RESULT;

				#define SFOV_RESULT_CATCHED_MAX  5
				struct SFOV_smooth_data
				{
					int cache_count;
					int cache_idx;
					int fov_cache_num;
					FOV_RESULT cache[SFOV_RESULT_CATCHED_MAX];
				};

				typedef struct _fov_param_
				{
					SFOV_smooth_data fov_smooth_data;
					POINT  ptCross_hair[Corner_SIZES];
					double dHDistance;//Chart Height distance
					double dWDistance;//Chart width distance
					double dModule2Chart_dist; //Module to chart dist;
					int nwidth;
					int nheight;
				} FOV_PARAM;

				UTSALGORITHM_API void CaculateFOV(
					FOV_PARAM *param,
					FOV_RESULT *res);

				UTSALGORITHM_API void SmoothFOV(
					FOV_PARAM *param,
					FOV_RESULT *res);
            }
        }

        namespace PreFocus
        {
            namespace PreFocus_BigStepThrough
            {
                typedef void (*LPFN_SET_VCM_MOVE)(__in int nCurrentDac);
                typedef double (*LPFN_GET_SFR_VALUE)(__in int nCurrentDac);

                typedef enum _e_pre_throughfocus_errorcode_
                {
                    PTE_OK = 0,
                    PTE_INPUT_RANGE_TOO_NARROW,
                    PTE_FUNCTION_POINT_NULL,
                } ePreThroughfocusErrorcode;

                typedef struct _pre_throughfocus_param_
                {
                    struct _pre_throughfocus_param_()
                    {
                        nMinDac = 100;
                        nMaxDac = 700;
                        nStepDac = 4;
                        nStepSleepTime = 0;
                        nDivideCount = 11;
                        dDownPersent = 0.45;
                        dPeakMin = 20;
                    };

                    int nMinDac;        // 最小Dac
                    int nMaxDac;        // 最大Dac
                    int nStepDac;       // 步长
                    int nStepSleepTime; // 每步等待时间
                    int nDivideCount;   // 切分大步数
                    double dDownPersent;// 拐点下降百分比
                    double dPeakMin;    // Peak最小值
                    LPFN_SET_VCM_MOVE lpfnSetVcmMove;   // 移动VCM方法的函数指针
                    LPFN_GET_SFR_VALUE lpfnGetSfrValue; // 取得SFR值的函数指针
                } PRE_THROUGHFOCUS_PARAM;

                typedef struct _pre_throughfocus_result_
                {
                    int nFixedMinDac;   // 修正后最小Dac
                    int nFixedMaxDac;   // 修正后最大Dac
                    int nPeakDac;       // 大步搜索过程中的峰值点
                    double dMinSfr;     // 大步搜索过程中的最小SFR
                    double dMaxSfr;     // 大步搜索过程中的最大SFR
                } PRE_THROUGHFOCUS_RESULT;

                UTSALGORITHM_API int PreThroughFocus_FindPeakRange(
                    __in const PRE_THROUGHFOCUS_PARAM &param,
                    __out PRE_THROUGHFOCUS_RESULT &result);

                UTSALGORITHM_API int PreThroughFocus_FindTurnRange(
                    __in const PRE_THROUGHFOCUS_PARAM &param,
                    __out PRE_THROUGHFOCUS_RESULT &result);
            }
        }

        namespace ThroughFocus
        {
            namespace ThroughFocus_Std
            {
                const int SFR_MAX_ROI = 256;

                typedef enum _e_throughfocus_errorcode_
                {
                    TFE_OK = 0,
                    TFE_USER_STOP,
                    TFE_PARAM_ERROR,
                    TFE_PRE_FOCUS_ERROR,
                    TFE_DO_STEP_SFR_ERROR,
                } eThroughfocusErrorcode;

                typedef enum _e_throughfocus_phase_
                {
                    TFP_PRE_FOCUS = 0,
                    TFP_DETAIL_FOCUS,
                    TFP_FIND_BEST,
                } eThroughfocusPhase;

                typedef void (*LPFN_SET_VCM_MOVE)(
                    __in int nCurrentDac);
                typedef BOOL (*LPFN_DO_STEP_SFR)(
                    __in eThroughfocusPhase ePhase,
                    __in int nCurrentDac,
                    __out double &dKeySfr);
                typedef void (*LPFN_LOG_OUT)(
                    __in LPCTSTR lpLogText);

                typedef struct _throughfocus_param_
                {
                    LPFN_SET_VCM_MOVE lpfnSetVcmMove;   // 移动VCM方法的函数指针
                    LPFN_DO_STEP_SFR lpfnDoStepSfr;     // 取得SFR值的函数指针
                    LPFN_LOG_OUT lpfnLogOut;            // 输出Log的函数指针
                    int nMinDac;
                    int nMaxDac;
                    int nDetailStepDac;
                    int nDetailStepTime;
                    int nDetailReverseThroughEn;
                    int nSuperStepTime;
                    int nPreFocusEn;
                    int nPreFocusStepTime;
                    int nPreFocusStepCount;
//Add by Spac @20170811 For SFR issue
					int nSfrType;
					int nSfrStackCount;
//End
                    double dPreFocusDownRate;
                    double dPreFocusPeakMin;
                } THROUGHFOCUS_PARAM;

                //-------------------------------------------------------------------------
                // 注意：此方法线程不安全，不要用于多线程环境
                UTSALGORITHM_API int DoThroughFocus(
                    __in BOOL *pbIsRunning,
                    __inout THROUGHFOCUS_PARAM &param);
            }
        }

        namespace WB
        {
            UTSALGORITHM_API int WBCalibration(
                uint8_t* pRaw8, int width, int height, int rawBayerType, 
                int roiStartX, int roiStartY, int roiWidth, int roiHeight,
                double blackLvl, double* avgRGrGbB, uint8_t* RGrGbB);

			UTSALGORITHM_API int WBCalibration(
				WORD* pRaw10, int width, int height, int rawBayerType, 
				int roiStartX, int roiStartY, int roiWidth, int roiHeight,
				double blackLvl, double* avgRGrGbB,  unsigned short* RGrGbB);

        }

        namespace LSC
        {
            namespace QualComm
            {
                const int MAX_BLOCK_COUNT = 256;
                typedef struct _qualcomm_lsc_param_
                {
                    int nCenterBlockWidth;
                    int nCenterBlockHeight;
                    int nCenterBlockCountX;
                    int nCenterBlockCountY;
                    int nBlackLevel;
                    int nBayerMode;         //1:BGGR  2:RGGB  3:GBRG  4:GRBG
                } QUALCOMM_LSC_PARAM;

                typedef struct _block_info_
                {
                    RECT rcPos;
                    double dRavg;
                    double dGravg;
                    double dGbavg;
                    double dBavg;
                } BLOCK_INFO;

                typedef struct _qualcomm_lsc_result_
                {
                    BLOCK_INFO arrBlockInfo[MAX_BLOCK_COUNT];
                } QUALCOMM_LSC_RESULT;

                UTSALGORITHM_API BOOL LSC_Cali(
                    const unsigned char* pRawBuffer,
                    int nWidth,
                    int nHeight,
                    __in const QUALCOMM_LSC_PARAM &param,
                    __out QUALCOMM_LSC_RESULT &result);
            }
        }

        UTSALGORITHM_API void MeanFilter_CV(
            const unsigned char* pOneChannelBuffer,
            int nWidth,
            int nHeight,
            int nFilterWidth,
            int nFilterHeight,
            double* pFilterBuffer);

        //-------------------------------------------------------------------------
        // calculate block mean value, set into center Point
        UTSALGORITHM_API void MeanFilter_CT(
            const unsigned char *pYBuffer,
            int nWidth,
            int nHeight,
            int nBlockWidth,
            int nBlockHeight,
            double *pdMeanBuffer);

        UTSALGORITHM_API void GetROIAvgRGB(
            const unsigned char* pBmpBuffer,
            int nWidth,
            int nHeight,
            const RECT& rect,
            COLOR_RGB_DOUBLE &rgb);

		UTSALGORITHM_API void GetROIAvgRGB(
			const unsigned char *pBmpBuffer,
			int nWidth,
			int nHeight,
			int filter,
			const RECT& rect,
			RGBTRIPLE& rgb);

		UTSALGORITHM_API void GetROIAvgRGrGbB(
			WORD* pRaw10,
			int rawBayerType,
			double blackLvl,
			int nWidth,
			int nHeight,
			const RECT& rect,
			color_rgrgbb_uShort &rgb);

        UTSALGORITHM_API void GetBlockAvgY(
            __in  const unsigned char *pYBuffer,
            __in  int nWidth,
            __in  int nHeight,
            __in  const RECT &rcBlock,
            __out double &dYAvg);

		UTSALGORITHM_API void GetBlockMaxY(
			__in  const unsigned char *pYBuffer,
			__in  int nWidth,
			__in  int nHeight,
			__in  const RECT &rcBlock,
			__out double &dYMax);

		UTSALGORITHM_API void GetBlockMinY(
			__in  const unsigned char *pYBuffer,
			__in  int nWidth,
			__in  int nHeight,
			__in  const RECT &rcBlock,
			__out double &dYMin);

        UTSALGORITHM_API void CalYavg(
            const unsigned char* pBmpBuffer,
            int Width,
            int Height,
            double &_Yavg);

        UTSALGORITHM_API BOOL CalYavgExp(
            const unsigned char* pBmpBuffer,
            int Width,
            int Height,
            double &_Yavg);

        UTSALGORITHM_API BOOL IsBlockPartical(
            const int *pParticalTable,
            int nWidth,
            int nHeight,
            int nParticalSizeX,
            int nParticalSizeY,
            const POINT &ptStartPoint);

        UTSALGORITHM_API void ReverseBlock(
            int *pParticalTable,
            int nWidth,
            int nHeight,
            int nParticalSizeX,
            int nParticalSizeY,
            const POINT &ptStartPoint);

        UTSALGORITHM_API BOOL FindOnePartical(
            const int *pParticalTable,
            int nWidth,
            int nHeight,
            int nParticalSizeX,
            int nParticalSizeY,
            POINT &ptStartPoint);

        UTSALGORITHM_API BOOL FindMultiPartical(
            const int *pParticalTable,
            int nWidth,
            int nHeight,
            int nParticalSizeX,
            int nParticalSizeY,
            int nMaxCount,
            vector<POINT> &vecPtPartical);
    

		namespace ColorUniformity
		{
			namespace CtColorUniformity
			{
				UTSALGORITHM_API BOOL CtColorUniformity(
					unsigned char *_RGBBuffer, 
					int _Width, 
					int _Height, 
					int _HBlockCount, 
					int _VBlockCount,
					double *RBRatioArray, 
					double &_CU,
					double &_CUMaxMin);
			}
		}

		class UTSALGORITHM_API OISAnalysis
		{
		public:
			OISAnalysis();
			~OISAnalysis();

			 void SetStaticImage
				(
				unsigned char* _pRGBStaticBuffer,
				int _Width,
				int _Height,
				int _ROISize =180,
				int _Level_A_Threshold = 128,
				int _Level_B_Threshold = 236
				);

			 void SetOISOFFImage
				(
				unsigned char* _pRGBStaticBuffer,
				int _Width,
				int _Height,
				int _ROISize =180);

			 bool Get_OIS_Status
				(
				unsigned char* _pRGBDynamicBuffer,			
				int &_StaticCircleWidth,
				int &_StaticCircleHeight,
				int &_OISOFFCircleWidth,
				int &_OISOFFCircleHeight,
				int &_DynamicCircleWidth,
				int &_DynamicCircleHeight,
				int _OIS_Threshold = 2
				);
			void GetCircleRECT(RECT &_CircleRect);

			bool SetPostImageValue2(unsigned char *_image,int width,int height, int x, int y, int _value);
			void ImageErode(unsigned char *_PostImage, int width,int height, int n);
			void ImageDilate(unsigned char *_PostImage, int width,int height, int n);

			void Do
				(
				unsigned char* _pGrayBuffer,
				int _Width,
				int _Height,
				int &_CircleWidth,
				int &_CircleHeight
				);
			void GetImageThreshold(
				unsigned char *_RGBImage_ori, 
				int iType, 
				int _inWidth, 
				int _inHeight, 
				unsigned char *_RGBImage_ok, 
				double &_aThr, 
				UINT _percent, 
				UINT _offset);

			int Level_A_Threshold;
			int Level_B_Threshold ;

		protected:
			int StaticCircleWidth ;
			int StaticCircleHeight ;
			int DynamicCircleWidth ;
			int DynamicCircleHeight ;
			int OISOFFCircleWidth ;
			int OISOFFCircleHeight ;

			int Width ;
			int Height;
			int ROISize ;
			int OIS_Threshold ;
			RECT CircleRect;

		};

		namespace SobelFocus
		{
			BOOL UTSALGORITHM_API CalImageSobel(unsigned char* _YBuffer,int Width,int Height,double &SobelValue);
			BOOL UTSALGORITHM_API CalImageSobelX(unsigned char* _YBuffer,int Width,int Height,double &SobelValue);
		};

		UTSALGORITHM_API void Sleep(int msec);

		//-------------------------------------------------------------------------
		namespace Correction_PD
		{
			//---------------------------------------------------------------------
			typedef struct DefectPoint
			{
				int x;
				int y;
			}DefectPoint;
			//---------------------------------------------------------------------
			int UTSALGORITHM_API CalculateDPAFDefectTable
				(
				int _PitchX,
				int _PitchY,
				int _BlockNumX,
				int _BlockNumY,
				DefectPoint* _POS_L,
				int _PosLCount,
				DefectPoint* _POS_R,
				int _PosRCount,
				DefectPoint* PDAFDefectTable
				);
			bool UTSALGORITHM_API DefectPixelCorrection
				(
				unsigned char* _InOutRaw8Buffer,
				int _Width,
				int _Height,
				DefectPoint* DefectTable,
				int DefectCount
				);
			//---------------------------------------------------------------------
			bool UTSALGORITHM_API GetPDPattern_Qualcomm
				(
				wchar_t* _PatternFilePath,
				int &PDLength,
				DefectPoint* PD_L_POS = nullptr,
				DefectPoint* PD_R_POS = nullptr
				);
			int UTSALGORITHM_API CalculateDPAFDefectTable_Qualcomm
				(
				int _GlobalShift_X,
				int _GlobalShift_Y,
				int _X_Step,
				int _Y_Step,
				int _BlockNumX,
				int _BlockNumY,
				DefectPoint* _POS_L,
				int _PosLCount,
				DefectPoint* _POS_R,
				int _PosRCount,
				DefectPoint* PDAFDefectTable
				);
			int UTSALGORITHM_API CalculateDPAFDefectTable_Qualcomm
				(
				wchar_t PatternFileName[255],
				int _GlobalShift_X,
				int _GlobalShift_Y,
				int _X_Step,
				int _Y_Step,
				int _ImageSizeX,
				int _ImageSizeY,
				DefectPoint* PDAFDefectTable
				);
		}

		//------------------------------------------------------------------------------
		// INI文件
		class UTSALGORITHM_API IniFile
		{
		public:
			IniFile(LPCTSTR lpFileName);
			~IniFile();

			int ReadValue(LPCTSTR lpAppName, LPCTSTR lpKey, int nDefault);
			double ReadValue(LPCTSTR lpAppName, LPCTSTR lpKey, double dDefault);
			char* ReadValue(LPCTSTR lpAppName, LPCTSTR lpKey, LPCTSTR lpDefault);

			BOOL INIWrite_File(LPCTSTR lpAppName, LPCTSTR lpKey, int nData);
			BOOL INIWrite_File(LPCTSTR lpAppName, LPCTSTR lpKey, float fdata);
			BOOL INIWrite_File(LPCTSTR lpAppName, LPCTSTR lpKey, double ddata);
			BOOL INIWrite_File(LPCTSTR lpAppName, LPCTSTR lpKey, LPCWSTR str);

			LPCTSTR ReadBlock(LPCTSTR lpStartMark, LPCTSTR lpEndMark);
			BOOL GetAllBlockStartMark(vector<CString> &vecBlockStartMark);

		private:
			LPCTSTR m_strFileName;
		};

		namespace SFRBlock
		{
			#define RANGE(a,min,max) (((a) >= (max))?(max):(((a) <= (min))?(min):(a)))
			//-------------------------------------------------------------------------
			typedef struct Rectangle
			{
				int x;
				int y;
				int width;
				int height;		
			}
			SFRROI;
			//---------------------------------------------------------------------

			bool UTSALGORITHM_API ROI_SFR09D(unsigned char* RGBImage,
										 int Width,int Height,
										 int RoiW,int RoiH,
										 SFRROI* Block);
			bool UTSALGORITHM_API ROI_SFR(unsigned char* RGBImage,
										int Width,int Height,
										SFRROI* Block);
			bool UTSALGORITHM_API ROI_FindCenetr(unsigned char* RGBImage,
										int Width,int Height,
										POINT* CenterPoint);
		}

		class UTSALGORITHM_API FastFouriertTansform
		{
		public:
			~FastFouriertTansform(); 
			static FastFouriertTansform* GetInstance();

			bool dft_r2c_1d(int _n,double *_in,double** _out);
		protected:
		private:
			FastFouriertTansform();
			class FFTImplement;
			std::tr1::shared_ptr<FFTImplement> FFTImpl;
		};


	}

	namespace Algorithm_Smartisan
	{
		namespace Shading_Smartisan
		{
			typedef struct _shading_result_
			{
				double dShadingCorner[25];
				double dShadingDelta;
				double dWorstRatio;
				double dAvgY[25];
			} Shading_RESULT;

			void UTSALGORITHM_API Shading_Y_Smartisan(
				const unsigned char *pYBuffer,
				int nWidth,
				int nHeight,
				Shading_RESULT &result);
		}

		namespace ColorShading_Smartisan
		{
			typedef struct _colorshading_result_
			{
				double dRatioRG[25];
				double dRatioBG[25];
				double dWorstRatioRG;
				double dWorstRatioBG;
				UTS::Algorithm::color_rgrgbb_uShort dAvg4chanel[25];
			} ColorShading_RESULT;

			void UTSALGORITHM_API ColorShading_Y_Smartisan(
				WORD* pRaw10,
				int rawBayerType,
				double blackLvl,
				int nWidth,
				int nHeight,
				ColorShading_RESULT &result);

		}
	}
}
