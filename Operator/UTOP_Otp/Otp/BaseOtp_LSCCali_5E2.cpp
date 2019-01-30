#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "../../3rdparty/LSC_Lib/Samsung/S5K5E2/LSC.h"

#pragma comment(lib, "LSCDLL_5E2.lib")

int BaseOtp::doSamsungLSCCali_5E2(uint16_t *pRaw10,int width, int height, 
								  char *LSCSetting_OTP,int &LSCLen_OTP,
								  char *LSCSetting_SRAM,int &LSCLen_SRAM)
{
 	SetUserLensRI_5E2(51.8134715, 62.18151184, 66.75673173,72.584383,
 		             77.56883442, 82.19903152, 86.71519664, 91.63508621,
 		             96.21040812, 98.95732931, 100.00);

	
	SetParameter_5E2( 64, width, height, 6, 6, 6, 
		6, 0.3, 0.01, TRUE, FALSE );

	CString strWorkPath;
	strWorkPath = "D:\\";

	if( Run_5E2((BYTE*) pRaw10,(BYTE*) pRaw10, TRUE, strWorkPath) != TRUE )
	//if( Run_5E2((BYTE*) pRaw10,(BYTE*) pRaw10, FALSE, NULL) != TRUE )
	{
		return OTPCALI_ERROR_LSCCALI;
	}

	LSCLen_OTP = GetOTPSetfileSize_5E2();
	GetOTPSetfile_5E2( LSCSetting_OTP );
	
	LSCLen_SRAM = GetSRAMSetfileSize_5E2();
	GetSRAMSetfile_5E2( LSCSetting_SRAM );

	SaveOTPSetfile_5E2( "D:\\otp.txt" );
	SaveSRAMSetfile_5E2( "D:\\sram.txt" );
	
	return OTPCALI_ERROR_NO;
}
