#include "StdAfx.h"
#include "CurrentOpOperator.h"
#include "UTOP_CurrentOP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    CurrentOpOperator::CurrentOpOperator(void)
    {
        OPERATOR_INIT;
    }

    CurrentOpOperator::~CurrentOpOperator(void)
    {
    }

    BOOL CurrentOpOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCurrent_DigPin"), m_param.nCurrent_DigPin, 3, _T("Digital Pin num, 1/2/3/4/5/6"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCurrent_AnaPin"), m_param.nCurrent_AnaPin, 1, _T("Analog Pin num, 1/2/3/4/5/6"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nOP_DigLowThreshold"), m_param.nOP_DigLowThreshold, 1, _T("Dynamic digital low threshold"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nOP_DigHighThreshold"), m_param.nOP_DigHighThreshold, 78, _T("Dynamic digital high threshold"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nOP_AnaLowThreshold"), m_param.nOP_AnaLowThreshold, 1, _T("Dynamic analog low threshold"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nOP_AnaHighThreshold"), m_param.nOP_AnaHighThreshold, 45, _T("Dynamic analog high threshold"));

        return TRUE;
    }

    BOOL CurrentOpOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        CStringA strHeader, strKey;
        int nArrDynamicCurrentOffset[6] = {0};
        for (int i = 0; i < ARRAYSIZE(nArrDynamicCurrentOffset); i++)
        {
            strKey.Format("%d_DynamicCurrentOffset[%d]", uts.info.nDeviceIndex, i);
            nArrDynamicCurrentOffset[i] = DOUBLE2INT(uts.info.mapShared[strKey.GetBuffer()]);
        }

        unsigned char arrBuffer[16] = {0};
        int nPinIndex = 0;
        //------------------------------------------------------------------------------
        // Measure Dynamic Digital current
        // Input  Format: [int|PinIndexBaseFrom1][int|OffsetValue]
        // Output Format: [double|MeasuredCurrentValue]
        memset(arrBuffer, 0, sizeof(arrBuffer));
        nPinIndex = m_param.nCurrent_DigPin;
        memcpy(arrBuffer, &nPinIndex, sizeof(int));
        memcpy(arrBuffer + sizeof(int), &nArrDynamicCurrentOffset[nPinIndex-1], sizeof(int));  // fix bug #12: CurrentST与旧程序数据不一样
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_CURRENT_DYNAMIC_MEASURE,
            arrBuffer,
            sizeof(arrBuffer)))
        {
            uts.log.Error(_T("Device ReadValue DRVT_CURRENT_DYNAMIC_MEASURE Error. DigPin = %d"), m_param.nCurrent_DigPin);
            *pnErrorCode = uts.errorcode.E_Current;
            goto end;
        }
        memcpy(&m_dOpDig, arrBuffer, sizeof(double));

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
        memcpy(&m_dOpAna, arrBuffer, sizeof(double));

        //------------------------------------------------------------------------------
        // 判断规格
        if ((m_dOpDig < m_param.nOP_DigLowThreshold) || (m_dOpDig > m_param.nOP_DigHighThreshold) 
            || (m_dOpAna < m_param.nOP_AnaLowThreshold) || (m_dOpAna > m_param.nOP_AnaHighThreshold))
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

    void CurrentOpOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Current);
    }

    void CurrentOpOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,SensorID,TestTime(ms),Result,")
            _T("OP_Dig,OP_Ana,")
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
            , m_dOpDig, m_dOpAna
            , strVersion, uts.info.strUserId);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new CurrentOpOperator);
    }
    //------------------------------------------------------------------------------
}
