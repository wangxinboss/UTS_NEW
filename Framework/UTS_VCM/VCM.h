#pragma once
#include "BaseDevice.h"

#ifdef VCM_EXPORTS
#define VCM_API __declspec(dllexport)
#else
#define VCM_API __declspec(dllimport)
#endif

extern UTS::BaseDevice* g_pDevice;

namespace UTS
{
    EXTERN_C
    {
        VCM_API LPCTSTR DLLGetVCM_Discription(void);

        VCM_API void DLLSetVCM_Move(
            BaseDevice* pDevice,
            int nVCMDriverICType,
            int nVCM_CurrentDAC);
		VCM_API void DLLSetVCM_Initial(
			BaseDevice* pDevice,
			int nVCMDriverICType,
			const void *InitalData);
		VCM_API int DLLSetVCM_HallCal(
			BaseDevice* pDevice,
			int nVCMDriverICType,
			UINT32 *HallData);

		VCM_API int DLLSetVCM_GetZ1Z2(
			BaseDevice* pDevice,
			int nVCMDriverICType,
			UINT32 *Z1Z2Data);
    }
}
