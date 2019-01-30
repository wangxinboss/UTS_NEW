#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_UVC_SetProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;
    }

    ImplOperator::~ImplOperator(void)
    {
    }

    BOOL ImplOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("Set property"), _T("Message shown when previewing"));

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_BrightnessFixEn"), m_param.UVC_BrightnessFixEn, 0, _T("Open & close brightness control flag"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_BrightnessAuto"), m_param.UVC_BrightnessAuto, 0, _T("brightness flag 0-Manual 1-auto"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_BrightnessValue"), m_param.UVC_BrightnessValue, 0, _T("brightness value"));

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_SharpnessFixEn"), m_param.UVC_SharpnessFixEn, 0, _T("Open & close sharpness control flag"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_SharpnessAuto"), m_param.UVC_SharpnessAuto, 0, _T("sharpness flag 0-Manual 1-auto"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_SharpnessValue"), m_param.UVC_SharpnessValue, 0, _T("sharpness value"));

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_WhiteBalanceFixEn"), m_param.UVC_WhiteBalanceFixEn, 0, _T("Open & close whitebalance control flag"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_WhiteBalanceAuto"), m_param.UVC_WhiteBalanceAuto, 0, _T("whitebalance flag 0-Manual 1-auto"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_WhiteBalanceValue"), m_param.UVC_WhiteBalanceValue, 0, _T("whitebalance value"));

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_FocusFixEn"), m_param.UVC_FocusFixEn, 0, _T("Open & close focus control flag"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_FocusAuto"), m_param.UVC_FocusAuto, 0, _T("focus flag 0-Manual 1-auto"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_FocusValue"), m_param.UVC_FocusValue, 0, _T("focus value"));

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_ExposureFixEn"), m_param.UVC_ExposureFixEn, 0, _T("Open & close exposure control flag"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_ExposureAuto"), m_param.UVC_ExposureAuto, 0, _T("exposure flag 0-Manual 1-auto"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("UVC_ExposureValue"), m_param.UVC_ExposureValue, 0, _T("exposure value"));

        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		CString strRegName = m_strOperatorName;
		long buffer[2] = {0};

		if(1 == m_param.UVC_BrightnessFixEn)
		{
			buffer[0] = m_param.UVC_BrightnessAuto;
			buffer[1] = m_param.UVC_BrightnessValue;

			if(TRUE != m_pDevice->WriteValue(DWVT_WRITE_SENSOR_PROPERTY_BRIGHTNESS,buffer,sizeof(buffer)))
				goto end;
		}

		if(1 == m_param.UVC_SharpnessFixEn)
		{
			buffer[0] = m_param.UVC_SharpnessAuto;
			buffer[1] = m_param.UVC_SharpnessValue;

			if(TRUE != m_pDevice->WriteValue(DWVT_WRITE_SENSOR_PROPERTY_SHARPNESS,buffer,sizeof(buffer)))
				goto end;
		}

		if(1 == m_param.UVC_WhiteBalanceFixEn)
		{
			buffer[0] = m_param.UVC_WhiteBalanceAuto;
			buffer[1] = m_param.UVC_WhiteBalanceValue;

			if(TRUE != m_pDevice->WriteValue(DWVT_WRITE_SENSOR_PROPERTY_WHITEBALANCE,buffer,sizeof(buffer)))
				goto end;
		}

		if(1 == m_param.UVC_FocusFixEn)
		{
			buffer[0] = m_param.UVC_FocusAuto;
			buffer[1] = m_param.UVC_FocusValue;

			if(TRUE != m_pDevice->WriteValue(DWVT_WRITE_SENSOR_PROPERTY_FOCUS,buffer,sizeof(buffer)))
				goto end;
		}

		if(1 == m_param.UVC_ExposureFixEn)
		{
			buffer[0] = m_param.UVC_ExposureAuto;
			buffer[1] = m_param.UVC_ExposureValue;

			if(TRUE != m_pDevice->WriteValue(DWVT_WRITE_SENSOR_PROPERTY_EXPOSURE,buffer,sizeof(buffer)))
				goto end;
		}

		Sleep(200);		//Sleep affter set property

		if(1 == m_param.UVC_BrightnessFixEn)
		{
			if(TRUE != m_pDevice->ReadValue(DRVT_READ_SENSOR_PROPERTY_BRIGHTNESS,buffer,sizeof(buffer)))
				goto end;
			if(1 == m_param.UVC_BrightnessAuto)
			{
				if(buffer[0] != m_param.UVC_BrightnessAuto)
					goto end;
			}
			else if((buffer[0] != m_param.UVC_BrightnessAuto) || (buffer[1] != m_param.UVC_BrightnessValue))
			{
				goto end;
			}	 
		}

		if(1 == m_param.UVC_SharpnessFixEn)
		{
			if(TRUE != m_pDevice->ReadValue(DRVT_READ_SENSOR_PROPERTY_SHARPNESS,buffer,sizeof(buffer)))
				goto end;
			if(1 == m_param.UVC_SharpnessAuto)
			{
				if(buffer[0] != m_param.UVC_SharpnessAuto)
					goto end;
			}
			else if((buffer[0] != m_param.UVC_SharpnessAuto) || (buffer[1] != m_param.UVC_SharpnessValue))
			{
				goto end;		 
			}
		}

		if(1 == m_param.UVC_WhiteBalanceFixEn)
		{
			if(TRUE != m_pDevice->ReadValue(DRVT_READ_SENSOR_PROPERTY_WHITEBALANCE,buffer,sizeof(buffer)))
				goto end;

			if(1 == m_param.UVC_WhiteBalanceAuto)
			{
				if(buffer[0] != m_param.UVC_WhiteBalanceAuto)
					goto end;
			}
			else if((buffer[0] != m_param.UVC_WhiteBalanceAuto) || (buffer[1] != m_param.UVC_WhiteBalanceValue))
			{
				goto end;		 
			} 
		}

		if(1 == m_param.UVC_FocusFixEn)
		{
			if(TRUE != m_pDevice->ReadValue(DRVT_READ_SENSOR_PROPERTY_FOCUS,buffer,sizeof(buffer)))
				goto end;

			if(1 == m_param.UVC_FocusAuto)
			{
				if(buffer[0] != m_param.UVC_FocusAuto)
					goto end;
			}
			else if((buffer[0] != m_param.UVC_FocusAuto) || (buffer[1] != m_param.UVC_FocusValue))
			{
				goto end;		 
			} 	 
		}

		if(1 == m_param.UVC_ExposureFixEn)
		{
			if(TRUE != m_pDevice->ReadValue(DRVT_READ_SENSOR_PROPERTY_EXPOSURE,buffer,sizeof(buffer)))
				goto end;

			if(1 == m_param.UVC_ExposureAuto)
			{
				if(buffer[0] != m_param.UVC_ExposureAuto)
					goto end;
			}
			else if((buffer[0] != m_param.UVC_ExposureAuto) || (buffer[1] != m_param.UVC_ExposureValue))
			{
				goto end;		 
			} 	 	 
		}

		return TRUE;
end:
		uts.log.Error(_T("Device WriteValue Property [%s] Error."),strRegName);
		return FALSE;
	}

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
