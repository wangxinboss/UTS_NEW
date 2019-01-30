#ifndef _PDAFCALIBRATIONTOOLSDLL_H_
#define _PDAFCALIBRATIONTOOLSDLL_H_

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef PDAFCALIBRATIONTOOLSDLL_EXPORTS
	#define PDAFCALIBRATIONTOOLSDLL_EXPORTS_DECLSPEC    __declspec( dllexport )
#else
	#define PDAFCALIBRATIONTOOLSDLL_EXPORTS_DECLSPEC
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint16_t Word16u;
typedef int16_t Word16;
//#include "BasicDataProc.h"

#define MAXLENGTH 256

typedef struct PDAFPatternParam
{
	int x_step;
	int y_step;
	int GlobalShift_x;
	int GlobalShift_y;
	int ImageBits;
	int BlackLevel;
	int SensorGain;
	int DSRatio;
}PDAFPattern;

typedef struct GainMapDataStructtag
{
	Word16u DSRatio;
	Word16u ActualLength;
	Word16u DSLength;
	Word16u Left_GainMap[MAXLENGTH];
	Word16u Right_GainMap[MAXLENGTH]; 
}GainMapDataStruct;

typedef struct VerificationStructtag
{
	int Verification_Lens_Position;
	int PD_ConversionCoeff;
	float Distance50cmTrueFocusPosition;
	float ToleranceError;
}VerificationStruct;

PDAFCALIBRATIONTOOLSDLL_EXPORTS_DECLSPEC int PDAF_Cal_GainMap(Word16 *RAWDATA,int nWidth, int nHeight, PDAFPattern PDAFPatternValue,GainMapDataStruct &GainMapData);

PDAFCALIBRATIONTOOLSDLL_EXPORTS_DECLSPEC int PDAF_Cal_PDConversionCoef(Word16 *FirstImg,Word16 *SecondImg,int nWidth, int nHeight,float FirstImg_LensPos, float SecondImg_LensPos,GainMapDataStruct GainMapData, PDAFPattern PDAFPatternValue);

PDAFCALIBRATIONTOOLSDLL_EXPORTS_DECLSPEC bool PDAF_Cal_Verification(Word16 *Distance50cmLensPosition9_Img,int nWidth, int nHeight,
	GainMapDataStruct GainMapData, PDAFPattern PDAFPatternValue,VerificationStruct VerificationValue);

#if defined(__cplusplus)
}
#endif

#endif

