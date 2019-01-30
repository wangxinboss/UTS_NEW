#include "StdAfx.h"
#include "RvcLibFun.h"


CRvcLibFun::CRvcLibFun(void)
{
	m_hLib = NULL;
}

CRvcLibFun::~CRvcLibFun(void)
{
	if (m_hLib != NULL) 
	{
		FreeLibrary(m_hLib);
	}
}

BOOL CRvcLibFun::LoadRvcLib()
{
	BOOL bResult = FALSE;

	FreeRvcLib();

	m_hLib = LoadLibrary(L"RvcLib.dll");
	if (m_hLib != NULL)
	{
		bResult = TRUE;
	}

	return bResult;
}

void CRvcLibFun::FreeRvcLib()
{
	if (m_hLib != NULL) 
	{
		FreeLibrary(m_hLib);
	}

	m_hLib = NULL;
	m_pfnRvcLib_Initialize = NULL;
	m_pfnRvcLib_UnInitialize = NULL;
	m_pfnReadSensorRegister8 = NULL;
	m_pfnWriteSensorRegister8 = NULL;
	m_pfnEEPROMWrite = NULL;
	m_pfnEEPROMRead = NULL;
	m_pfnSetLEDMode = NULL;
	m_pfnSetCameraStatus = NULL;
	m_pfnEEPROMWriteSN = NULL;
	m_pfnUVCGetManufacturer = NULL;
}   

BOOL CRvcLibFun::LoadRvcFun()
{   
	BOOL bResult = TRUE;

	m_pfnRvcLib_Initialize = (pfn_RvcLib_Initialize) GetProcAddress(m_hLib, ("RvcLib_Initialize"));
	m_pfnRvcLib_UnInitialize = (pfn_RvcLib_UnInitialize) GetProcAddress(m_hLib, ("RvcLib_UnInitialize"));
	m_pfnReadSensorRegister8 = (pfn_ReadSensorRegister8) GetProcAddress(m_hLib, ("ReadSensorRegister8"));
	m_pfnWriteSensorRegister8 = (pfn_WriteSensorRegister8) GetProcAddress(m_hLib, ("WriteSensorRegister8"));
	m_pfnEEPROMWrite = (pfn_EEPROMWrite) GetProcAddress(m_hLib, ("EEPROMWrite"));
	m_pfnEEPROMRead = (pfn_EEPROMRead) GetProcAddress(m_hLib, ("EEPROMRead"));
	m_pfnSetLEDMode = (pfn_SetLEDMode) GetProcAddress(m_hLib, ("SetLEDMode"));
	m_pfnSetCameraStatus = (pfn_SetCameraStatus) GetProcAddress(m_hLib, ("SetCameraStatus"));
	m_pfnEEPROMWriteSN = (pfn_EEPROMWriteSN) GetProcAddress(m_hLib, ("EEPROMWriteSN"));
	m_pfnUVCGetManufacturer = (pfn_UVCGetManufacturer) GetProcAddress(m_hLib, ("UVC_GetManufacturer"));

	return bResult;
}