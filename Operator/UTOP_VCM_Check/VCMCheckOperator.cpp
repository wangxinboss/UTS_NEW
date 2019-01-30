#include "StdAfx.h"
#include "VCMCheckOperator.h"
#include "UTOP_VCM_Check.h"
#include "VCM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "UTS_VCM.lib")
namespace UTS
{
    VCMCheckOperator::VCMCheckOperator(void)
    {
        OPERATOR_INIT;
    }

    VCMCheckOperator::~VCMCheckOperator(void)
    {
    }

    BOOL VCMCheckOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nAFVDD_Pin"), m_param.nCurrent_AnaPin, 4, _T("AFVDD Pin num, 1/2/3/4/5/6"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nVCM_MaxDac"), m_param.nVCM_MaxDac, 1023, _T("VCM Max. DAC"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nVCM_DelayTime"), m_param.nVCM_DelayTime, 200, _T("VCM Delay Time"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dAF_Current_Level"), m_param.dAF_Current_Level, 30, _T("VCM Current Level"));

        return TRUE;
    }

    BOOL VCMCheckOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        CStringA strHeader, strKey;
        int nArrDynamicCurrentOffset[6] = {0};
		double dCurrentDelta=0;

		DLLSetVCM_Initial(uts.flow.m_pDevice, uts.info.nVCMType, NULL);
		uts.log.Debug(_T("nVCMType : %d "),uts.info.nVCMType);
		Sleep(300);
		
		DLLSetVCM_Move(uts.flow.m_pDevice, uts.info.nVCMType, 0);
		Sleep(m_param.nVCM_DelayTime);
        for (int i = 0; i < ARRAYSIZE(nArrDynamicCurrentOffset); i++)
        {
            strKey.Format("%d_DynamicCurrentOffset[%d]", uts.info.nDeviceIndex, i);
            nArrDynamicCurrentOffset[i] = DOUBLE2INT(uts.info.mapShared[strKey.GetBuffer()]);
        }

        unsigned char arrBuffer[16] = {0};
        int nPinIndex = 0;

        //------------------------------------------------------------------------------
        // Measure Dynamic Analog current
        // Input  Format: [int|PinIndexBaseFrom1][int|OffsetValue]
        // Output Format: [double|MeasuredCurrentValue]
        memset(arrBuffer, 0, sizeof(arrBuffer));
        nPinIndex = m_param.nCurrent_AnaPin;
        memcpy(arrBuffer, &nPinIndex, sizeof(int));
        memcpy(arrBuffer + sizeof(int), &nArrDynamicCurrentOffset[nPinIndex-1], sizeof(int));  // fix bug #12: CurrentST与旧程序数据不一样
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_CURRENT_DYNAMIC_MEASURE,
            arrBuffer,
            sizeof(arrBuffer)))
        {
            uts.log.Error(_T("Device ReadValue DRVT_CURRENT_DYNAMIC_MEASURE Error. AnaPin = %d"), m_param.nCurrent_AnaPin);
            *pnErrorCode = uts.errorcode.E_Current;
            goto end;
        }
        memcpy(&m_dAFCurrent_before, arrBuffer, sizeof(double));

		DLLSetVCM_Move(uts.flow.m_pDevice, uts.info.nVCMType, (m_param.nVCM_MaxDac)/2);
		Sleep(m_param.nVCM_DelayTime);
		memset(arrBuffer, 0, sizeof(arrBuffer));
		nPinIndex = m_param.nCurrent_AnaPin;
		memcpy(arrBuffer, &nPinIndex, sizeof(int));
		memcpy(arrBuffer + sizeof(int), &nArrDynamicCurrentOffset[nPinIndex-1], sizeof(int));  // fix bug #12: CurrentST与旧程序数据不一样
		if (!m_pDevice->ReadValue(
			eDeviceReadValueType::DRVT_CURRENT_DYNAMIC_MEASURE,
			arrBuffer,
			sizeof(arrBuffer)))
		{
			uts.log.Error(_T("Device ReadValue DRVT_CURRENT_DYNAMIC_MEASURE Error. AnaPin = %d"), m_param.nCurrent_AnaPin);
			*pnErrorCode = uts.errorcode.E_Current;
			goto end;
		}
		memcpy(&m_dAFCurrent_after, arrBuffer, sizeof(double));

		dCurrentDelta = abs(m_dAFCurrent_after-m_dAFCurrent_before);
        //------------------------------------------------------------------------------
        // 判断规格
        if (dCurrentDelta < m_param.dAF_Current_Level)
        {
            *pnErrorCode = uts.errorcode.E_VCM;
        }
        else
        {
            *pnErrorCode = uts.errorcode.E_Pass;
        }

end:
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        //------------------------------------------------------------------------------
        // 保存结果
        SaveData();

        return m_bResult;
    }

    void VCMCheckOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Current);
		vecReturnValue.push_back(uts.errorcode.E_VCM);
    }

    void VCMCheckOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

// 		strHeader = _T("Time,SN,SensorID,TestTime(ms),Result,")
// 			_T("AF_Current_Before,AF_Current_After,CurrentDelta")
// 			_T("Version,OP_SN\n");
		strHeader.Format(
			_T("Time,SN,SensorID,TestTime(ms),Result,")
			_T("AF_Current_Before,AF_Current_After,CurrentDelta,")
			_T("Version,OP_SN\n,")
			_T(",,,,,Current_Level,%.1f\n")
			,m_param.dAF_Current_Level
			);

        strData.Format(
            _T("%s,%s,%s,%.1f,%s,")
            _T("%.1f,%.1f,%.1f,")
            _T("%s,%s\n")
            , lpTime
            , uts.info.strSN
            , uts.info.strSensorId
            , m_TimeCounter.GetPassTime()
            , strResult
            , m_dAFCurrent_before
			, m_dAFCurrent_after
			, abs(m_dAFCurrent_after-m_dAFCurrent_before)
            , strVersion, uts.info.strUserId);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new VCMCheckOperator);
    }
    //------------------------------------------------------------------------------
}
