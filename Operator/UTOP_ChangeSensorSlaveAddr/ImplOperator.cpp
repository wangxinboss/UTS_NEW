#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_ChangeSensorSlaveAddr.h"

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensorSlaveAddr"), m_param.nSensorSlaveAddr, 0, _T("SlaveAddr of sensor."));
		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		// 重新设定Sensor序列

		if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_SENSOR_SLAV_ADDR,
			&m_param.nSensorSlaveAddr, sizeof(int)))
		{
			uts.log.Error(_T("Device WriteValue DWVT_SENSOR_OUT_MODE [%s] Error."), m_param.nSensorSlaveAddr);
			*pnErrorCode = uts.errorcode.E_Fail;
			m_bResult = FALSE;
			goto end;
		}

		*pnErrorCode = uts.errorcode.E_Pass;
end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		return m_bResult;
	}

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
