//#ifndef _LC4_H
//#define _LC4_H
//
//#ifdef LC4_API
//#else
//#define LC4_API  _declspec(dllimport)
//#endif

#ifdef LC4_API
#define LC4_API __declspec(dllexport)
#else
#define LC4_API extern "C" __declspec(dllimport)
#endif

//********************************************
//5650
#define IIC_ADDR_5650											0x6c
#define LENC_START_ADDR_5650						0x5800
#define LENC_START_ADDR_G_5650					0x5800
#define LENC_START_ADDR_B_5650					0x5824
#define LENC_START_ADDR_R_5650					0x583d
#define LENC_ADDR_LENGTH_5650					86
#define LENC_ADDR_LENGTH_G_5650				36
#define LENC_ADDR_LENGTH_B_5650				25
#define LENC_ADDR_LENGTH_R_5650				25
//5690
#define IIC_ADDR_5690											0x6c
#define LENC_START_ADDR_5690						0x5800
#define LENC_START_ADDR_G_5690					0x5800
#define LENC_START_ADDR_BR_5690				0x5824
#define LENC_START_ADDR_OFFSET_5690		0x583d
#define LENC_ADDR_LENGTH_5690					62
#define LENC_ADDR_LENGTH_G_5690				36
#define LENC_ADDR_LENGTH_BR_5690				25
//8820
#define IIC_ADDR_8820											0x6c
#define LENC_START_ADDR_8820						0x5800
#define LENC_START_ADDR_G_8820					0x5800
#define LENC_START_ADDR_BR_8820				0x5824
#define LENC_START_ADDR_OFFSET_8820		0x583d
#define LENC_ADDR_LENGTH_8820					62
#define LENC_ADDR_LENGTH_G_8820				36
#define LENC_ADDR_LENGTH_BR_8820				25
//8830
#define IIC_ADDR_8830											0x6c
#define LENC_START_ADDR_8830						0x5800
#define LENC_START_ADDR_G_8830					0x5800
#define LENC_START_ADDR_BR_8830				0x5824
#define LENC_START_ADDR_OFFSET_8830		0x583d
#define LENC_ADDR_LENGTH_8830					62
#define LENC_ADDR_LENGTH_G_8830				36
#define LENC_ADDR_LENGTH_BR_8830				25
//8850
#define IIC_ADDR_8850											0x6c
#define LENC_START_ADDR_8850						0x5800
#define LENC_START_ADDR_G_8850					0x5800
#define LENC_START_ADDR_BR_8850				0x5824
#define LENC_START_ADDR_OFFSET_8850		0x583d
#define LENC_ADDR_LENGTH_8850					62
#define LENC_ADDR_LENGTH_G_8850				36
#define LENC_ADDR_LENGTH_BR_8850				25
//14810
#define IIC_ADDR_14810											0x6c
#define LENC_START_ADDR_14810						0x5800
#define LENC_START_ADDR_G_14810				0x5800
#define LENC_START_ADDR_B_14810					0x5840
#define LENC_START_ADDR_R_14810				0x5864
#define LENC_ADDR_LENGTH_14810					136
#define LENC_ADDR_LENGTH_G_14810			64
#define LENC_ADDR_LENGTH_B_14810				36
#define LENC_ADDR_LENGTH_R_14810				36
//12830
#define IIC_ADDR_12830											0x6c
#define LENC_START_ADDR_12830						0x5800
#define LENC_START_ADDR_G_12830				0x5800
#define LENC_START_ADDR_BR_12830				0x5824
#define LENC_START_ADDR_OFFSET_12830	0x583d
#define LENC_ADDR_LENGTH_12830					62
#define LENC_ADDR_LENGTH_G_12830			36
#define LENC_ADDR_LENGTH_BR_12830			25
//13850
#define IIC_ADDR_13850											0x6c
#define LENC_START_ADDR_13850						0x5200
#define LENC_START_ADDR_G_13850				0x5200
#define LENC_START_ADDR_BR_13850				0x5224
#define LENC_START_ADDR_OFFSET_13850	0x523d
#define LENC_ADDR_LENGTH_13850					62
#define LENC_ADDR_LENGTH_G_13850			36
#define LENC_ADDR_LENGTH_BR_13850			25
//5647
#define IIC_ADDR_5647					0x6c
#define LENC_START_ADDR_5647			0x5800
#define LENC_START_ADDR_G_5647			0x5800
#define LENC_START_ADDR_BR_5647			0x5824
#define LENC_START_ADDR_OFFSET_5647		0x583d
#define LENC_ADDR_LENGTH_5647			62
#define LENC_ADDR_LENGTH_G_5647			36
#define LENC_ADDR_LENGTH_BR_5647		25
//5642
#define IIC_ADDR_5642				0x78
#define LENC_START_ADDR_5642		0x5800
#define LENC_START_ADDR_G_5642		0x5800
#define LENC_START_ADDR_B_5642		0x5840
#define LENC_START_ADDR_R_5642		0x5864
#define LENC_ADDR_LENGTH_5642		136
#define LENC_ADDR_LENGTH_G_5642		64
#define LENC_ADDR_LENGTH_B_5642		36
#define LENC_ADDR_LENGTH_R_5642		36
//5640
#define IIC_ADDR_5640					0x78
#define LENC_START_ADDR_5640			0x5800
#define LENC_START_ADDR_G_5640			0x5800
#define LENC_START_ADDR_BR_5640			0x5824
#define LENC_START_ADDR_OFFSET_5640		0x583d
#define LENC_ADDR_LENGTH_5640			62
#define LENC_ADDR_LENGTH_G_5640			36
#define LENC_ADDR_LENGTH_BR_5640		25
//660
#define IIC_ADDR_660				0x6a
#define LENC_START_ADDR_660			0x7200
#define LENC_START_ADDR_C_660		0x7200
#define LENC_START_ADDR_G_660		0x7240
#define LENC_START_ADDR_B_660		0x7280
#define LENC_START_ADDR_R_660		0x72c0
#define LENC_ADDR_LENGTH_660		256
#define LENC_ADDR_LENGTH_C_660		64
#define LENC_ADDR_LENGTH_G_660		64
#define LENC_ADDR_LENGTH_B_660		64
#define LENC_ADDR_LENGTH_R_660		64

//8858
#define IIC_ADDR_8858					0x6c
#define LENC_START_ADDR_8858			0x5800
#define LENC_START_ADDR_G_8858			0x5800
#define LENC_START_ADDR_B_8858		0x5824
#define LENC_START_ADDR_R_8858		0x5848
#define LENC_START_ADDR_B_OFFSET_8858	0x586c
#define LENC_START_ADDR_R_OFFSET_8858	0x586d
#define LENC_ADDR_LENGTH_8858			110
#define LENC_ADDR_LENGTH_G_8858		36
#define LENC_ADDR_LENGTH_B_8858		36
#define LENC_ADDR_LENGTH_R_8858		36
/*
argument definition:
__in unsigned char *BmpBuffer: input 24-bit bmp buffer; raw data should be interpolated into 24-bit color bmp, and this bmp buffer should have vertical flip;
__in int ImageWidth: input image width;
__in int ImageHeight: input image height;
__in int percent; shading percentage; usually 70(depending on backend requirement);
__in unsigned char offset: BL offset; usually 16(should be 10-bit mode);
__out int *pLenCReg: output register array; recommend to use 256 units of array, like reg[256];
__out int *pLen: output actual register count;
*/
LC4_API bool LenC_Cal_5650_V4(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_5690_V4(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_8820_V4(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
/* OV8865, OV13850 also call this function LenC_Cal_8830() */
LC4_API bool LenC_Cal_8830_V4(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_8850_V4(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_8858_V4(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_14810_V4(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_12830_V4(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
/*
argument definition:
__in unsigned char *Raw8Buffer: input 8-bit raw buffer;
__in int ImageWidth: input image width;
__in int ImageHeight: input image height;
__in int percent; shading percentage; usually 70(depending on backend requirement);
__in unsigned char offset: BL offset; usually 16(should be 10-bit mode);
__out int *pLenCReg: output register array; recommend to use 256 units of array, like reg[256];
__out int *pLen: output actual register count;
*/
LC4_API bool LenC_Cal_5650_Raw8_V4(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_5690_Raw8_V4(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_8820_Raw8_V4(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
/* OV8865, OV13850 also call this function LenC_Cal_8830_Raw8() */
LC4_API bool LenC_Cal_8830_Raw8_V4(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_8850_Raw8_V4(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_8858_Raw8_V4(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_14810_Raw8_V4(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC4_API bool LenC_Cal_12830_Raw8_V4(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);


//********************************************

//sample code
/*
unsigned char* pPixel;
int nWidth;
int nHeight;
int nLenCReg[256];
int nLenCRegCount;
if (LenC_Cal_5650_V4(pPixel, nWidth, nHeight, 70, 16, nLenCReg, &nLenCRegCount)
	&& m_nLenCRegCount) {
	//add your code here;
	
}
*/

//#endif