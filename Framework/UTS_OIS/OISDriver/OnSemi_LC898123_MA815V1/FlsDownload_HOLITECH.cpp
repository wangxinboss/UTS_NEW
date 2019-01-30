//********************************************************************************
//
//		<< LC898123F40 Evaluation Soft >>
//	    Program Name	: FlsDownload_HOLITECH.c
//		History			: First edition
//********************************************************************************
//#define	__OISDWLHT__
//**************************
//	Include Header File
//**************************
#include "StdAfx.h"
#include	"Ois.h"

#include	"OisAPI.h"
#include	"FromCode_14_10.h"
#include	"OisDWLHT.h"


//****************************************************
//	CUSTOMER NECESSARY CREATING FUNCTION LIST
//****************************************************


//********************************************************************************
// Function Name 	: FlashDownloadF40_HOLITECH
// Retun Value		: NON
// Argment Value	: chiperase
// Explanation		: Flash Update for LC898123F40
// History			: First edition
//********************************************************************************
const DOWNLOAD_TBL DTbl_HOLITECH[] = {
	{0x1410, CcMagicCodeF40_14_10, sizeof(CcMagicCodeF40_14_10), CcFromCodeF40_14_10, sizeof(CcFromCodeF40_14_10) },
	{0xFFFF, 0,                0,                               0,               0                  }
};

UINT_8 FlashDownloadF40_HOLITECH( UINT_8 chiperase, UINT_8 ModuleVendor, UINT_8 ActVer )
{
	DOWNLOAD_TBL* ptr ;

	ptr = ( DOWNLOAD_TBL * )DTbl_HOLITECH ;
	do {
		if( ptr->Index == ( ((UINT_16)ModuleVendor<<8) + ActVer) ) {
			return FlashUpdateF40( chiperase, ptr );
		}
		ptr++ ;
	} while (ptr->Index != 0xFFFF ) ;

	return 0xF0 ;
}

