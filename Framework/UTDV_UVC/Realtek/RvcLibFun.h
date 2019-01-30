#pragma once

#ifndef REALTEKRvcLibFun
#define REALTEKRvcLibFun

#include "User.h"
class CRvcLibFun
{
public:
	CRvcLibFun(void);
	~CRvcLibFun(void);

public:
	BOOL LoadRvcLib();
	BOOL LoadRvcFun();
	void FreeRvcLib();

	HINSTANCE		m_hLib;

public:

	pfn_RvcLib_Initialize m_pfnRvcLib_Initialize;
	pfn_RvcLib_UnInitialize m_pfnRvcLib_UnInitialize;
	pfn_ReadSensorRegister8 m_pfnReadSensorRegister8;
	pfn_WriteSensorRegister8 m_pfnWriteSensorRegister8;
	pfn_EEPROMWrite m_pfnEEPROMWrite;
	pfn_EEPROMRead m_pfnEEPROMRead;
	pfn_SetLEDMode m_pfnSetLEDMode;
	pfn_SetCameraStatus m_pfnSetCameraStatus;
	pfn_EEPROMWriteSN m_pfnEEPROMWriteSN;
	pfn_UVCGetManufacturer m_pfnUVCGetManufacturer;

	pfn_GetAsicRegister m_pfnGetAsicRegister;
	pfn_SetAsicRegister m_pfnSetAsicRegister;
};

#endif