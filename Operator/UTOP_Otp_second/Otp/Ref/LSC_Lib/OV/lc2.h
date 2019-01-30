// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LC2_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LC2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LC2_EXPORTS
#define LC2_API __declspec(dllexport)
#else
#define LC2_API __declspec(dllimport)
#endif


//********************************************
//5650
#define IIC_ADDR_5650				0x6c
#define LENC_START_ADDR_5650		0x5800
#define LENC_START_ADDR_G_5650		0x5800
#define LENC_START_ADDR_B_5650		0x5824
#define LENC_START_ADDR_R_5650		0x583d
#define LENC_ADDR_LENGTH_5650		86
#define LENC_ADDR_LENGTH_G_5650		36
#define LENC_ADDR_LENGTH_B_5650		25
#define LENC_ADDR_LENGTH_R_5650		25
//5690
#define IIC_ADDR_5690					0x6c
#define LENC_START_ADDR_5690			0x5800
#define LENC_START_ADDR_G_5690			0x5800
#define LENC_START_ADDR_BR_5690			0x5824
#define LENC_START_ADDR_OFFSET_5690		0x583d
#define LENC_ADDR_LENGTH_5690			62
#define LENC_ADDR_LENGTH_G_5690			36
#define LENC_ADDR_LENGTH_BR_5690		25
//8820
#define IIC_ADDR_8820					0x6c
#define LENC_START_ADDR_8820			0x5800
#define LENC_START_ADDR_G_8820			0x5800
#define LENC_START_ADDR_BR_8820			0x5824
#define LENC_START_ADDR_OFFSET_8820		0x583d
#define LENC_ADDR_LENGTH_8820			62
#define LENC_ADDR_LENGTH_G_8820			36
#define LENC_ADDR_LENGTH_BR_8820		25
//8830
#define IIC_ADDR_8830					0x6c
#define LENC_START_ADDR_8830			0x5800
#define LENC_START_ADDR_G_8830			0x5800
#define LENC_START_ADDR_BR_8830			0x5824
#define LENC_START_ADDR_OFFSET_8830		0x583d
#define LENC_ADDR_LENGTH_8830			62
#define LENC_ADDR_LENGTH_G_8830			36
#define LENC_ADDR_LENGTH_BR_8830		25
//8850
#define IIC_ADDR_8850					0x6c
#define LENC_START_ADDR_8850			0x5800
#define LENC_START_ADDR_G_8850			0x5800
#define LENC_START_ADDR_BR_8850			0x5824
#define LENC_START_ADDR_OFFSET_8850		0x583d
#define LENC_ADDR_LENGTH_8850			62
#define LENC_ADDR_LENGTH_G_8850			36
#define LENC_ADDR_LENGTH_BR_8850		25
//14810
#define IIC_ADDR_14810					0x6c
#define LENC_START_ADDR_14810			0x5800
#define LENC_START_ADDR_G_14810			0x5800
#define LENC_START_ADDR_B_14810			0x5840
#define LENC_START_ADDR_R_14810			0x5864
#define LENC_ADDR_LENGTH_14810			136
#define LENC_ADDR_LENGTH_G_14810		64
#define LENC_ADDR_LENGTH_B_14810		36
#define LENC_ADDR_LENGTH_R_14810		36
//12830
#define IIC_ADDR_12830					0x6c
#define LENC_START_ADDR_12830			0x5800
#define LENC_START_ADDR_G_12830			0x5800
#define LENC_START_ADDR_BR_12830		0x5824
#define LENC_START_ADDR_OFFSET_12830	0x583d
#define LENC_ADDR_LENGTH_12830			62
#define LENC_ADDR_LENGTH_G_12830		36
#define LENC_ADDR_LENGTH_BR_12830		25
//13850
#define IIC_ADDR_13850					0x6c
#define LENC_START_ADDR_13850			0x5200
#define LENC_START_ADDR_G_13850			0x5200
#define LENC_START_ADDR_BR_13850		0x5224
#define LENC_START_ADDR_OFFSET_13850	0x523d
#define LENC_ADDR_LENGTH_13850			62
#define LENC_ADDR_LENGTH_G_13850		36
#define LENC_ADDR_LENGTH_BR_13850		25

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

LC2_API bool LenC_Cal_5650(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_5690(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_8820(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
/* OV8865, OV13850 also call this function LenC_Cal_8830() */
LC2_API bool LenC_Cal_8830(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_8850(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_14810(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_12830(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);

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

LC2_API bool LenC_Cal_5650_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_5690_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_8820_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
/* OV8865, OV13850 also call this function LenC_Cal_8830_Raw8() */
LC2_API bool LenC_Cal_8830_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_8850_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_14810_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC2_API bool LenC_Cal_12830_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);


//********************************************

//sample code
/*
unsigned char* pPixel;
int nWidth;
int nHeight;
int nLenCReg[256];
int nLenCRegCount;
if (LenC_Cal_5650(pPixel, nWidth, nHeight, 70, 16, nLenCReg, &nLenCRegCount)
	&& m_nLenCRegCount) {
	//add your code here;
	
}
*/

