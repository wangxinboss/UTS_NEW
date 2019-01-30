//#ifndef _LC44_H
//#define _LC44_H
//
//#ifdef LC44_API
//#else
//#define LC44_API  _declspec(dllimport)
//#endif

#ifdef LC44_EXPORTS
	#define LC44_API __declspec(dllexport)
#else
	#define LC44_API extern __declspec(dllimport)
#endif

//********************************************
//8858 R2A
#define IIC_ADDR_8858				0x6c
#define LENC_START_ADDR_8858		0x5800
#define LENC_START_ADDR_G_8858		0x5800
#define LENC_START_ADDR_B_8858		0x5850
#define LENC_START_ADDR_R_8858		0x58a0
#define LENC_ADDR_LENGTH_8858		240
#define LENC_ADDR_LENGTH_G_8858		80
#define LENC_ADDR_LENGTH_B_8858		80
#define LENC_ADDR_LENGTH_R_8858		80

//13850 R2A
#define IIC_ADDR_13850				0x6c
#define LENC_START_ADDR_13850		0x5200
#define LENC_START_ADDR_G_13850		0x5200
#define LENC_START_ADDR_B_13850		0x5278
#define LENC_START_ADDR_R_13850		0x52f0
#define LENC_ADDR_LENGTH_13850		360
#define LENC_ADDR_LENGTH_G_13850	120
#define LENC_ADDR_LENGTH_B_13850	120
#define LENC_ADDR_LENGTH_R_13850	120

//13860
#define IIC_ADDR_13860				0x6c
#define LENC_START_ADDR_13860		0x5400
#define LENC_START_ADDR_G_13860		0x5400
#define LENC_START_ADDR_B_13860		0x5478
#define LENC_START_ADDR_R_13860		0x54f0
#define LENC_ADDR_LENGTH_13860		360
#define LENC_ADDR_LENGTH_G_13860	120
#define LENC_ADDR_LENGTH_B_13860	120
#define LENC_ADDR_LENGTH_R_13860	120

//2740 R2A
#define IIC_ADDR_2740				0x6c
#define LENC_START_ADDR_2740		0x5800
#define LENC_START_ADDR_G_2740		0x5800
#define LENC_START_ADDR_B_2740		0x5850
#define LENC_START_ADDR_R_2740		0x58a0
#define LENC_ADDR_LENGTH_2740		240
#define LENC_ADDR_LENGTH_G_2740		80
#define LENC_ADDR_LENGTH_B_2740		80
#define LENC_ADDR_LENGTH_R_2740		80

//16850
#define IIC_ADDR_16850				0x6c
#define LENC_START_ADDR_16850		0x5400
#define LENC_START_ADDR_G_16850		0x5400
#define LENC_START_ADDR_B_16850		0x5478
#define LENC_START_ADDR_R_16850		0x54f0
#define LENC_ADDR_LENGTH_16850		360
#define LENC_ADDR_LENGTH_G_16850	120
#define LENC_ADDR_LENGTH_B_16850	120
#define LENC_ADDR_LENGTH_R_16850	120

//23850
#define IIC_ADDR_23850				0x6c
#define LENC_START_ADDR_23850		0x2800
#define LENC_START_ADDR_G_23850		0x2800
#define LENC_START_ADDR_B_23850		0x2900
#define LENC_START_ADDR_R_23850		0x2a00
#define LENC_ADDR_LENGTH_23850		768
#define LENC_ADDR_LENGTH_G_23850	256
#define LENC_ADDR_LENGTH_B_23850	256
#define LENC_ADDR_LENGTH_R_23850	256

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
LC44_API bool LenC_Cal_8858R2A(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_13850R2A(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_13850R2A_C(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_13860(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_2740(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_16850(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_23850(__in unsigned char *BmpBuffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);

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
LC44_API bool LenC_Cal_8858R2A_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_13850R2A_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_13850R2A_Raw8_C(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_13860_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_2740_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_16850_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
LC44_API bool LenC_Cal_23850_Raw8(__in unsigned char *Raw8Buffer, __in int ImageWidth, __in int ImageHeight, __in int percent, __in unsigned char offset, __out unsigned char *pLenCReg, __out int *pLen);
//********************************************

/*Decode function
argument definition:
__in unsigned char *pInBuf: input 186 bytes buffer;
__out unsigned char *pOutBuf: output 360 bytes buffer;
*/
LC44_API bool Decode_13850R2A(unsigned char*pInBuf, unsigned char* pOutBuf);
//sample code
/*

unsigned char* pPixel;
int nWidth;
int nHeight;
int nLenCReg[360];
int nLenCRegCount;
if (LenC_Cal_13850R2A_NC(pPixel, nWidth, nHeight, 60, 16, nLenCReg, &nLenCRegCount)
	&& m_nLenCRegCount) {
	//add your code here;
}
else if(LenC_Cal_13850R2A_C(pPixel, nWidth, nHeight, 60, 16, nLenCReg, &nLenCRegCount)
	&& m_nLenCRegCount)
	{
		Decode_13850R2A(nLenCReg, nLenCRegOut)
		//add your code here;
	}
*/

//#endif