#include "StdAfx.h"
#include "DAC2DISOperator.h"
#include "UTOP_DAC2DIS.h"
#include "VCM.h"

#pragma comment(lib, "UTS_VCM.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    const byte SEND_COMMAND[6] = {0x24, 0x52, 0x45, 0x51, 0x0D, 0x0A};  // "$REQ\r\n"

    DAC2DISOperator::DAC2DISOperator(void)
        : m_com(FALSE)
    {
        OPERATOR_INIT;
    }

    DAC2DISOperator::~DAC2DISOperator(void)
    {
    }

    BOOL DAC2DISOperator::OnReadSpec()
    {
        int nValue = 0;
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("comParam.nbaud"), m_param.comParam.nbaud, 38400, _T("COM: baud"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("comParam.ndatabits"), m_param.comParam.ndatabits, 8, _T("COM: databits"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("comParam.nlenInput"), m_param.comParam.nlenInput, 128, _T("COM: lenInput"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("comParam.nlenOutput"), m_param.comParam.nlenOutput, 128, _T("COM: lenOutput"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("comParam.nparity"), m_param.comParam.nparity, NOPARITY, _T("COM: parity"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("comParam.nportNo"), m_param.comParam.nportNo, 1, _T("COM: portNo"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("comParam.nstopbits"), m_param.comParam.nstopbits, ONESTOPBIT, _T("COM: stopbits(0: 1stopbit, 1: 1.5stopbit, 2: 2stopbit)"));
        
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcmParam.nMinDac"), m_param.vcmParam.nMinDac, 0, _T("Min DAC."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcmParam.nMaxDac"), m_param.vcmParam.nMaxDac, 1023, _T("Max DAC."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcmParam.nStepDac"), m_param.vcmParam.nStepDac, 4, _T("1 step DAC number."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("vcmParam.nType"), m_param.vcmParam.nType, 0, DLLGetVCM_Discription());

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nMoveAfterTime"), m_param.nMoveAfterTime, 1000, _T("Wait time after vcm move."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSamplingInterval"), m_param.nSamplingInterval, 50, _T("Interval between sampling."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDummyCount"), m_param.nDummyCount, 2, _T("Dummy sampling count."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nAvgCount"), m_param.nAvgCount, 5, _T("Average sampling count."));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nMeasureCurrentEn"), m_param.nMeasureCurrentEn, 0, _T("0: Disable, 1: Enable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCurrentPIN"), m_param.nCurrentPIN, 3, _T("0:PIN1, 1:PIN2, 2:PIN3, 3:PIN4, 4:PIN35, 5:PIN36"));

        return TRUE;
    }

    BOOL DAC2DISOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //DLLSetVCM_Move(m_pDevice, m_param.vcmParam.nType, 0);
        //Sleep(500);
        //DLLSetVCM_Move(m_pDevice, m_param.vcmParam.nType, 500);
        //Sleep(500);
        //DLLSetVCM_Move(m_pDevice, m_param.vcmParam.nType, 0);
        //Sleep(500);
        //DLLSetVCM_Move(m_pDevice, m_param.vcmParam.nType, 500);
        //Sleep(500);
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        memset(m_result.arrDisForward, 0, sizeof(m_result.arrDisForward));
        memset(m_result.arrDisBackward, 0, sizeof(m_result.arrDisBackward));
        memset(m_result.arrCurrentForward, 0, sizeof(m_result.arrCurrentForward));
        memset(m_result.arrCurrentBackward, 0, sizeof(m_result.arrCurrentBackward));

        if (m_param.vcmParam.nMinDac < 0
            || m_param.vcmParam.nMaxDac > 1023
            || m_param.vcmParam.nMaxDac <= m_param.vcmParam.nMinDac
            || m_param.vcmParam.nStepDac <= 0
            || m_param.nAvgCount < 1
            || m_param.comParam.nbaud < 9600
            || m_param.comParam.nbaud > 115200
            || m_param.comParam.nportNo < 1
            || m_param.comParam.nportNo > 64)
        {
            uts.log.Error(_T("Param error."));
            *pnErrorCode = uts.errorcode.E_Fail;
            goto end;
        }

        // 打开串口
        BOOL bRet = m_com.OpenPort(
            &theApp, 
            m_param.comParam.nlenInput,
            m_param.comParam.nlenOutput,
            m_param.comParam.nportNo,
            m_param.comParam.nbaud,
            m_param.comParam.nparity,
            m_param.comParam.ndatabits,
            m_param.comParam.nstopbits);
        if (!bRet)
        {
            uts.log.Error(_T("Failed to open com port."));
            *pnErrorCode = uts.errorcode.E_Fail;
            goto end;
        }

        // 取得皮值
        DLLSetVCM_Move(m_pDevice, m_param.vcmParam.nType, 0);
        uts.log.Debug(_T("Move2Dac[%d]"), 0);
        Sleep(m_param.nMoveAfterTime);
        double dZeroValue = 0.0;
        bRet = GetAvgValue(m_param.nDummyCount * 2, m_param.nAvgCount * 2, dZeroValue);
        if (!bRet)
        {
            uts.log.Error(_T("Get Com value fail."));
            *pnErrorCode = uts.errorcode.E_Fail;
            goto end;
        }
        uts.log.Info(_T(">>> Get Zero value = %.1f <<<"), dZeroValue);

        int dac = 0;
        //-------------------------------------------------------------------------
        // forward
        uts.log.Info(_T("========== Forward =========="));
        for (dac = m_param.vcmParam.nMinDac;
            dac <= m_param.vcmParam.nMaxDac;
            dac += m_param.vcmParam.nStepDac)
        {
            if (*pbIsRunning == FALSE)
            {
                goto end;
            }
            DLLSetVCM_Move(m_pDevice, m_param.vcmParam.nType, dac);
            uts.log.Debug(_T("Move2Dac[%d]"), dac);
            Sleep(m_param.nMoveAfterTime);
            // 取得Z值
            double dValue = 0.0;
            bRet = GetAvgValue(m_param.nDummyCount, m_param.nAvgCount, dValue);
            if (!bRet)
            {
                uts.log.Error(_T("Get Com value fail."));
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }
            m_result.arrDisForward[dac] = dValue - dZeroValue;
            
            // 记录电流数据
            double dVcmCurrent = -1.0;
            if (m_param.nMeasureCurrentEn != 0)
            {
                if (!MeasureCurrentValue(dVcmCurrent))
                {
                    uts.log.Error(_T("Measure Current value fail."));
                    *pnErrorCode = uts.errorcode.E_Fail;
                    goto end;
                }
            }
            m_result.arrCurrentForward[dac] = dVcmCurrent;

            uts.log.Info(_T(">>> DAC[%d] Dis[%.1f] Current[%.1f] <<<"),
                dac, m_result.arrDisForward[dac], dVcmCurrent);
        }
        //-------------------------------------------------------------------------
        // backward
        dac -= m_param.vcmParam.nStepDac;
        uts.log.Info(_T("========== Backward =========="));
        for (dac = dac;
            dac >= m_param.vcmParam.nMinDac;
            dac -= m_param.vcmParam.nStepDac)
        {
            if (*pbIsRunning == FALSE)
            {
                goto end;
            }
            DLLSetVCM_Move(m_pDevice, m_param.vcmParam.nType, dac);
            uts.log.Debug(_T("Move2Dac[%d]"), dac);
            Sleep(m_param.nMoveAfterTime);
            // 取得Z值
            double dValue = 0.0;
            bRet = GetAvgValue(m_param.nDummyCount, m_param.nAvgCount, dValue);
            if (!bRet)
            {
                uts.log.Error(_T("Get Com value fail."));
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }
            m_result.arrDisBackward[dac] = dValue - dZeroValue;

            // 记录电流数据
            double dVcmCurrent = -1.0;
            if (m_param.nMeasureCurrentEn != 0)
            {
                if (!MeasureCurrentValue(dVcmCurrent))
                {
                    uts.log.Error(_T("Measure Current value fail."));
                    *pnErrorCode = uts.errorcode.E_Fail;
                    goto end;
                }
            }
            m_result.arrCurrentBackward[dac] = dVcmCurrent;

            uts.log.Info(_T(">>> DAC[%d] Dis[%.1f] Current[%.1f] <<<"),
                dac, m_result.arrDisBackward[dac], dVcmCurrent);
        }

end:
        if (m_com.GetComOpened())
        {
            m_com.ClosePort();
        }

        //------------------------------------------------------------------------------
        // 保存数据文件
        CString strDataFilename;
        strDataFilename.Format(_T("%s"), m_strOperatorName);
        bRet = SaveThroughData(strDataFilename);
        if (!bRet)
        {
            uts.log.Error(_T("SaveThroughData fail."));
            *pnErrorCode = uts.errorcode.E_Fail;
        }

        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        return m_bResult;
    }

    void DAC2DISOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
    }

    BOOL DAC2DISOperator::ReadZValue(double &dZValue)
    {
        BOOL bRet = TRUE;
        dZValue = 0.0;
        if (m_com.GetComOpened())
        {
            // 清空串口读入缓冲区
            m_com.PurgeInputBuffer();
            // 清空串口写缓冲区
            m_com.PurgeOutputBuffer();
            // 发送串口指令
            bRet = m_com.WriteSyncPort(SEND_COMMAND, ARRAY_SIZE(SEND_COMMAND));
            if (!bRet)
            {
                uts.log.Error(_T("WriteSyncPort fail."));
                goto end;
            }
            // 接收数据
            DWORD dwStartTime = GetTickCount();
            int nbufferLen = 0;
            char buffer[128] = {0};
            char bufferAll[1024] = {0};
            do
            {
                if (GetTickCount() - dwStartTime > 5000)
                {
                    uts.log.Error(_T("Timeout."));
                    bRet = FALSE;
                    goto end;
                }
                m_com.ReadSyncPort((BYTE *)buffer, ARRAY_SIZE(buffer));
                strcat(bufferAll, buffer);
                nbufferLen = strlen(buffer);
            } while (!(nbufferLen >= 2
                && buffer[nbufferLen - 2] == 0x0D
                && buffer[nbufferLen - 1] == 0x0A));
            // 解析数据
            bRet = ParseZValue(bufferAll, dZValue);
            if (!bRet)
            {
                uts.log.Error(_T("ND fail."));
                goto end;
            }
        }
        else
        {
            uts.log.Error(_T("Failed to open com port."));
            bRet = FALSE;
            goto end;
        }
end:
        return bRet;
    }

    BOOL DAC2DISOperator::ParseZValue(const char *pValueBuffer, double &dZValue)
    {
        int nBufferLen = strlen(pValueBuffer);
        int i = 0;
        for (i = nBufferLen; i > 0; i--)
        {
            if (pValueBuffer[i] == ',')
            {
                i++;
                break;
            }
        }
        int nZValueLen = nBufferLen - 2 - i;
        char szZValue[128] = {0};
        memcpy(szZValue, &pValueBuffer[i], nZValueLen);
        if (strcmp(szZValue, "ND") == 0)
        {
            return FALSE;
        }
        dZValue = atof(szZValue);
        return TRUE;
    }

    BOOL DAC2DISOperator::GetAvgValue(__in int nDummyCount, __in int nAvgCount, __out double &dAvgValue)
    {
        BOOL bRet = TRUE;
        dAvgValue = 0.0;
        double dValue = 0.0;
        CString strMsg;
        for (int i = 0; i < nDummyCount; i++)
        {
            bRet = ReadZValue(dValue);
            if (!bRet)
            {
                goto end;
            }
            strMsg.AppendFormat(_T("Dummy[%d] = %.1f "), i+1, dValue);
        }
        uts.log.Debug(strMsg);

        strMsg.Empty();
        for (int i = 0; i < nAvgCount; i++)
        {
            bRet = ReadZValue(dValue);
            if (!bRet)
            {
                goto end;
            }
            dAvgValue += dValue;
            strMsg.AppendFormat(_T("Value[%d] = %.1f "), i+1, dValue);
        }
        uts.log.Debug(strMsg);
        dAvgValue /= nAvgCount;
        uts.log.Debug(_T("Avg = %.1f"), dAvgValue);
end:
        return bRet;
    }

    BOOL DAC2DISOperator::SaveThroughData(LPCTSTR lpOperatorName)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        CString strFilePath = GetResultCSVFilename(lpOperatorName, st);

        FILE* fp = NULL;
        long fileLength ;
        errno_t etRet = _tfopen_s(&fp, strFilePath, _T("a+"));
        if (0 != etRet)
        {
            uts.log.Error(_T("Open file Fail. path = %s"), strFilePath);
            return FALSE;
        }
        fseek(fp, 0, SEEK_END);
        fileLength = ftell(fp);

        CString strTime;
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d:%03d"),
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        int _count = (m_param.vcmParam.nMaxDac - m_param.vcmParam.nMinDac) / m_param.vcmParam.nStepDac + 1;
        //------------------------------------------------------------------------------
        // 输出文件头
        _ftprintf_s(fp, _T("Time,SN,TestTime,DAC_Start,DAC_End,DAC_Div"));
        for (int i = 0; i < _count; i++)
        {
            _ftprintf_s(fp, _T(",DAC[%04d]"), m_param.vcmParam.nMinDac + i * m_param.vcmParam.nStepDac);
        }
        _ftprintf_s(fp, _T("\n"));

        //------------------------------------------------------------------------------
        // 输出文件内容(forward)
        _ftprintf_s(fp, _T("%s,%s,%.1lf,%d,%d,%d")
            , strTime
            , uts.info.strSN
            , m_TimeCounter.GetPassTime()
            , m_param.vcmParam.nMinDac
            , m_param.vcmParam.nMaxDac
            , m_param.vcmParam.nStepDac
            );
        for (int i = 0; i < _count; i++)
        {
            _ftprintf_s(fp, _T(",%.01lf"), m_result.arrDisForward[m_param.vcmParam.nMinDac + i * m_param.vcmParam.nStepDac]);
        }
        _ftprintf_s(fp, _T("\n"));

        //------------------------------------------------------------------------------
        // 输出文件内容(backward)
        _ftprintf_s(fp, _T(",,,,,"));
        for (int i = 0; i < _count; i++)
        {
            _ftprintf_s(fp, _T(",%.01lf"), m_result.arrDisBackward[m_param.vcmParam.nMinDac + i * m_param.vcmParam.nStepDac]);
        }
        _ftprintf_s(fp, _T("\n"));

        //-------------------------------------------------------------------------
        // 输出文件内容(current forward)
        if (m_param.nMeasureCurrentEn != 0)
        {
            _ftprintf_s(fp, _T(",,,,,"));
            for (int i = 0; i < _count; i++)
            {
                _ftprintf_s(fp, _T(",%.01lf"), m_result.arrCurrentForward[m_param.vcmParam.nMinDac + i * m_param.vcmParam.nStepDac]);
            }
            _ftprintf_s(fp, _T("\n"));
        }

        //-------------------------------------------------------------------------
        // 输出文件内容(current backward)
        if (m_param.nMeasureCurrentEn != 0)
        {
            _ftprintf_s(fp, _T(",,,,,"));
            for (int i = 0; i < _count; i++)
            {
                _ftprintf_s(fp, _T(",%.01lf"), m_result.arrCurrentBackward[m_param.vcmParam.nMinDac + i * m_param.vcmParam.nStepDac]);
            }
            _ftprintf_s(fp, _T("\n"));
        }

        fclose(fp);
        return TRUE;
    }

    BOOL DAC2DISOperator::MeasureCurrentValue(double &dVCMCurrent)
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
        // Measure Dynamic current
        // Input  Format: [int|PinIndexBaseFrom1][int|OffsetValue]
        // Output Format: [double|MeasuredCurrentValue]
        memset(arrBuffer, 0, sizeof(arrBuffer));
        nPinIndex = m_param.nCurrentPIN + 1;
        memcpy(arrBuffer, &nPinIndex, sizeof(int));
        memcpy(arrBuffer + sizeof(int), &nArrDynamicCurrentOffset[nPinIndex], sizeof(int));
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_CURRENT_DYNAMIC_MEASURE,
            arrBuffer,
            sizeof(arrBuffer)))
        {
            uts.log.Error(_T("Device ReadValue DRVT_CURRENT_DYNAMIC_MEASURE Error. Pin = %d"), nPinIndex);
            return FALSE;
        }
        memcpy(&dVCMCurrent, arrBuffer, sizeof(double));

        return TRUE;
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new DAC2DISOperator);
    }
    //------------------------------------------------------------------------------
}
