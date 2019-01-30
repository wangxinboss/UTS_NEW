// 다음 ifdef 블록은 DLL에서 내보내기하는 작업을 쉽게 해 주는 매크로를 만드는 
// 표준 방식입니다. 이 DLL에 들어 있는 파일은 모두 명령줄에 정의된 _EXPORTS 기호로
// 컴파일되며, 다른 프로젝트에서는 이 기호를 정의할 수 없습니다.
// 이렇게 하면 소스 파일에 이 파일이 들어 있는 다른 모든 프로젝트에서는 
// SKHYNIX_OTP_LIB_API 함수를 DLL에서 가져오는 것으로 보고, 이 DLL은
// 이 DLL은 해당 매크로로 정의된 기호가 내보내지는 것으로 봅니다.
#ifndef _SKHYNIX_OPT_LIB_
#define _SKHYNIX_OPT_LIB_

#ifdef SKHYNIX_OTP_LIB_EXPORTS
#define SKHYNIX_OTP_LIB_API __declspec(dllexport)
#else
#define SKHYNIX_OTP_LIB_API __declspec(dllimport)
#endif

typedef struct LSC_RATIO{
	int		R;
	int		Gr;
	int		Gb;
	int		B;
}LSC_RATIO, *PLSC_RATIO;

typedef struct WB{
	int		Roi_Width;
	int		Roi_Height;
	int		StrX;
	int		StrY;
	int		EndX;
	int		EndY;
	float	WB_Ratio_RGr;
	float	WB_Ratio_BGb;
	float	WB_Ratio_GbGr;
}WB, *PWB;

typedef struct _IMG_INFO{
	int		Width;
	int		Height;
	LPWORD	bRawBuf;
	LSC_RATIO Ratio;//LSC Ration Control
	WB		 WB;	//WB Inform
}IMG_INFO, *PIMG_INFO;

INT32 SKhynix_OTP_Calibration(PIMG_INFO pImg, LPWORD LscPara);

 

#endif