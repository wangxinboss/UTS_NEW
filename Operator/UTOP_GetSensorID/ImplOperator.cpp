#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_GetSensorID.h"
#include "SensorDriver.h"
#pragma comment(lib, "UTS_Sensor.lib")

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("Get SensorID"), _T("Message shown when wait Get SensorID"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strRegStrForCheck"), m_param.strRegStrForCheck, _T(".*"), _T("Regex string for checking SensorID"));

		return TRUE;
	}

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
#if 0   // for no device debug
        uts.info.strSensorId = _T("SENSORID_0001");
        uts.log.Warning(_T("Use dummy SensorID[%s]"), uts.info.strSensorId);
        *pnErrorCode = uts.errorcode.E_Pass;
#else
        SensorDriver *pSensor = GetSensorInstance(m_pDevice,uts.info.nSensorType );
        BOOL bRet = pSensor->GetSensorId(uts.info.strSensorId);
        if (bRet)
        {
			////
            uts.log.Info(_T("Get Sensor Id: [%s]"), uts.info.strSensorId);
            *pnErrorCode = uts.errorcode.E_Pass;
        }
        else
        {
            uts.log.Error(_T("GetSensorId Error."));
            *pnErrorCode = uts.errorcode.E_GetSensorID;
        }
#endif

		wcmatch mr;
		wregex rx(m_param.strRegStrForCheck);
		if (!regex_match((LPTSTR)(LPCTSTR)uts.info.strSensorId, mr, rx))  // fig bug #17: SN长度超长时不起管控作用，例如{8,8}，应该输入11位，但是输入11位以上都可以测试
		{
			*pnErrorCode = uts.errorcode.E_GetSensorID;
			return FALSE;
		}

        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        return m_bResult;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_GetSensorID);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
