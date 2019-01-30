#include "StdAfx.h"
#include "CurrentStOperator.h"
#include "UTOP_CurrentST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    CurrentStOperator::CurrentStOperator(void)
    {
        OPERATOR_INIT;
    }

    CurrentStOperator::~CurrentStOperator(void)
    {
    }

    BOOL CurrentStOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCurrent_DigPin"), m_param.nCurrent_DigPin, 3, _T("Digital Pin num, 1/2/3/4/5/6"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCurrent_AnaPin"), m_param.nCurrent_AnaPin, 1, _T("Analog Pin num, 1/2/3/4/5/6"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nST_DigLowThreshold"), m_param.nST_DigLowThreshold, 1, _T("Standby digital low threshold"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nST_DigHighThreshold"), m_param.nST_DigHighThreshold, 78, _T("Standby digital high threshold"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nST_AnaLowThreshold"), m_param.nST_AnaLowThreshold, 1, _T("Standby analog low threshold"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nST_AnaHighThreshold"), m_param.nST_AnaHighThreshold, 45, _T("Standby analog high threshold"));

        return TRUE;
    }

    BOOL CurrentStOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        CStringA strHeader, strKey;
        int nArrStandbyCurrentOffset[6] = {0};
        for (int i = 0; i < ARRAYSIZE(nArrStandbyCurrentOffset); i++)
        {
            strKey.Format("%d_StandbyCurrentOffset[%d]", uts.info.nDeviceIndex, i);
            nArrStandbyCurrentOffset[i] = DOUBLE2INT(uts.info.mapShared[strKey.GetBuffer()]);
        }

        //------------------------------------------------------------------------------
        // 切换Sensor序列
        CString strRegName = _T("STANDBY_SET");
        if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
            strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
        {
            uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
            *pnErrorCode = uts.errorcode.E_Fail;
            return FALSE;
        }

        unsigned char arrBuffer[16] = {0};
        int nPinIndex = 0;
        //------------------------------------------------------------------------------
        // Measure Standby Digital current
        // Input  Format: [int|PinIndexBaseFrom1][int|OffsetValue]
        // Output Format: [double|MeasuredCurrentValue]
        memset(arrBuffer, 0, sizeof(arrBuffer));
        nPinIndex = m_param.nCurrent_DigPin;
        memcpy(arrBuffer, &nPinIndex, sizeof(int));
        memcpy(arrBuffer + sizeof(int), &nArrStandbyCurrentOffset[nPinIndex-1], sizeof(int));  // fix bug #12: CurrentST与旧程序数据不一样
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_CURRENT_STANDBY_MEASURE,
            arrBuffer,
            sizeof(arrBuffer)))
        {
            uts.log.Error(_T("Device ReadValue DRVT_CURRENT_STANDBY_MEASURE Error. DigPin = %d"), m_param.nCurrent_DigPin);
            *pnErrorCode = uts.errorcode.E_Current;
            goto end;
        }
        memcpy(&m_dStDig, arrBuffer, sizeof(double));

        //------------------------------------------------------------------------------
        // Measure Standby Analog current
        // Input  Format: [int|PinIndexBaseFrom1][int|OffsetValue]
        // Output Format: [double|MeasuredCurrentValue]
        memset(arrBuffer, 0, sizeof(arrBuffer));
        nPinIndex = m_param.nCurrent_AnaPin;
        memcpy(arrBuffer, &nPinIndex, sizeof(int));
        memcpy(arrBuffer + sizeof(int), &nArrStandbyCurrentOffset[nPinIndex-1], sizeof(int));  // fix bug #12: CurrentST与旧程序数据不一样
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_CURRENT_STANDBY_MEASURE,
            arrBuffer,
            sizeof(arrBuffer)))
        {
            uts.log.Error(_T("Device ReadValue DRVT_CURRENT_STANDBY_MEASURE Error. AnaPin = %d"), m_param.nCurrent_AnaPin);
            *pnErrorCode = uts.errorcode.E_Current;
            goto end;
        }
        memcpy(&m_dStAna, arrBuffer, sizeof(double));

        //------------------------------------------------------------------------------
        // 判断规格
        if ((m_dStDig < m_param.nST_DigLowThreshold) || (m_dStDig > m_param.nST_DigHighThreshold) 
            || (m_dStAna < m_param.nST_AnaLowThreshold) || (m_dStAna > m_param.nST_AnaHighThreshold))
        {
            *pnErrorCode = uts.errorcode.E_Current;
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

    void CurrentStOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Current);
    }

    void CurrentStOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(_T("STANDBY_SET"));
    }

    void CurrentStOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,SensorID,TestTime(ms),Result,")
            _T("ST_Dig,ST_Ana,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%s,%.1f,%s,")
            _T("%.1f,%.1f,")
            _T("%s,%s\n")
            , lpTime
            , uts.info.strSN
            , uts.info.strSensorId
            , m_TimeCounter.GetPassTime()
            , strResult
            , m_dStDig, m_dStAna
            , strVersion, uts.info.strUserId);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new CurrentStOperator);
    }
    //------------------------------------------------------------------------------
}
