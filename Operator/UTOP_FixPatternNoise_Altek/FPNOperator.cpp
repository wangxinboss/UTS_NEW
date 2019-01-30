#include "StdAfx.h"
#include "FPNOperator.h"
#include "UTOP_FixPatternNoise_Altek.h"
//#include "Global_Memory_SFC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    FPNOperator::FPNOperator(void)
    {
        OPERATOR_INIT;
    }

    FPNOperator::~FPNOperator(void)
    {
    }

    BOOL FPNOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dThreshold"), m_param.dThreshold, 3.5, _T("Defect Threshold"));

        return TRUE;
    }

    BOOL FPNOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        m_dYvalue = 0.0;
        memset(&m_defectInfo, 0, sizeof(FPNInfo));

        //------------------------------------------------------------------------------
        if (m_param.nReCapture != 0)
        {
            CString strRegName = m_strOperatorName;
            if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
                strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
            {
                uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }

            if (!m_pDevice->Recapture(
                m_bufferObj,
                uts.info.nLTDD_DummyFrame,
                uts.info.nLTDD_AvgFrame))
            {
                uts.log.Error(_T("Recapture error. "));
                *pnErrorCode = uts.errorcode.E_NoImage;
                goto end;
            }
        }
        else
        {
            m_pDevice->GetCapturedBuffer(m_bufferObj);
        }

        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

        //------------------------------------------------------------------------------

        UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
        if (m_dYvalue < m_param.dLTMinY || m_dYvalue > m_param.dLTMaxY)
        {
            *pnErrorCode = uts.errorcode.E_Linumance;
            goto end;
        }

        //------------------------------------------------------------------------------
 
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
            FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_defectInfo.m_dRowDifMax,m_defectInfo.m_dColDifMax);
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        if (m_defectInfo.m_dRowDifMax > m_param.dThreshold
            || m_defectInfo.m_dColDifMax > m_param.dThreshold)
        {
            *pnErrorCode = uts.errorcode.E_FixPatternNoise;
        }
        else
        {
            *pnErrorCode = uts.errorcode.E_Pass;
        }
end:

        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        //------------------------------------------------------------------------------

        if (m_param.nReCapture != 0)
        {
            SaveImage();
        }

        //------------------------------------------------------------------------------

        //CString strLocationDataFilename;
        //strLocationDataFilename.Format(_T("%s-Location"), m_strOperatorName);
        //SaveLocationData(strLocationDataFilename);
        SaveData();

        return m_bResult;
    }

    void FPNOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_FixPatternNoise);
    }

    void FPNOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void FPNOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),Y_Avg,FPN_Result,")
            _T("RowMean,ColMean,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%.1f,%s,")
            _T("%.2f,%.2f,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
			, m_defectInfo.m_dRowDifMax
			, m_defectInfo.m_dColDifMax
            , strVersion, uts.info.strUserId);
    }

    void FPNOperator::SaveLocationData(LPCTSTR lpOperatorName)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        CString strTime;
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d:%03d"),
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        //-------------------------------------------------------------------------
        TestItemResult tir = {0};
        tir.sItemName = lpOperatorName;
        tir.sHeader = _T("Time, SN\n");
        tir.sData.Format(_T("%s, %s"), strTime, uts.info.strSN);
		//?
        tir.sData.Append(_T("\n"));

		//!!!---2018.08.01 Chi-Jen.Liao Remove SFC
		/*
        if (uts.info.nShopFlowEn != 0)
        {
            GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
            gmsfc->vecTestItemResult.push_back(tir);
        }
		*/
        //-------------------------------------------------------------------------
        // 输出Data file
        CString strFilePath = GetResultCSVFilename(lpOperatorName, st);

        FILE* fp = NULL;
        long fileLength;
        errno_t etRet = _tfopen_s(&fp, strFilePath, _T("a+"));
        if (0 != etRet)
        {
            uts.log.Error(_T("Open file Fail. path = %s"), strFilePath);
            return;
        }
        fseek(fp, 0, SEEK_END);
        fileLength = ftell(fp);

        //------------------------------------------------------------------------------
        // 输出文件
        if (fileLength < 3)
        {	
            _ftprintf_s(fp, tir.sHeader);
        }
        _ftprintf_s(fp, tir.sData);
        fclose(fp);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new FPNOperator);
    }
    //------------------------------------------------------------------------------
}
