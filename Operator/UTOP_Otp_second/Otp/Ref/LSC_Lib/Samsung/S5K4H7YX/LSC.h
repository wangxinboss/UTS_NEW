#include <windows.h>
#if defined(LSCDLL_EXPORTS)
#define DLLFUNC extern "C" __declspec(dllexport)
#elif defined(__cplusplus)
#define DLLFUNC extern "C" __declspec(dllimport)
#else
#define DLLFUNC __declspec(dllimport)
#endif

/////////////////////////////////////Standard API////////////////////////////////////////////////

DLLFUNC const char * Initialize(
	_In_ const char* szMasterJson,		//Used
	_In_ const char* szBayerSetting,	//Used
	_In_ const char* szSynDataJson,		//Not used
	_In_ const char* szBinaryData		//Used
	);

DLLFUNC const char * Run (_In_ void* pwInputRaw);

DLLFUNC const char * GetOutCDEFormat (_In_ const char* szFolderPath);

DLLFUNC const char * GetOutSetFileFormat (_Out_ const char*& szOutString, _In_ int format);

DLLFUNC const char * GetOutNVMWrite (_Out_ const char*& szOutString, _In_ int nNthWrite, _In_ int format);





/////////////////////////////////////Tool Function////////////////////////////////////////////////
//This function is for LSC Tool
//////////////////////////////////////////////////////////////////////////////////////////////////
DLLFUNC const char * RunMaster (
	_In_ int nShadingPower,
	_In_ void* pwInputRawHorizon[], _In_ int nNumOfHorizon,		//Not used
	_In_ void* pwInputRawIncaA[], _In_ int nNumOfIncaA,			//Not used
	_In_ void* pwInputRawWW[], _In_ int nNumOfWW,				//Not used
	_In_ void* pwInputRawCW[], _In_ int nNumOfCW,				//Not used
	_In_ void* pwInputRawDL50[], _In_ int nNumOfDL50,			//Used Only 1 image
	_In_ void* pwInputRawDL65[], _In_ int nNumOfDL65,			//Not used
	_In_ void* pwInputRawDL75[], _In_ int nNumOfDL75,			//Not used
	_In_ void* pwInputRawOutdoor[], _In_ int nNumOfOutdoor,		//Not used
	_In_ const char* szBayerSetting,							//Used
	_Out_ const char* szMasterJson,								//Used
	_Out_ const char* szSynDataJson,							//Not used
	_Out_ const char* szBinaryData								//Used
	);


DLLFUNC const char * GetResult (
	_In_ void* pwInputRaw,
	_Out_ void* pwOutputRaw
	);


//Don't use
DLLFUNC const char * SetMaxGainThres (
	_In_ double dbMaxGainThres
	);

