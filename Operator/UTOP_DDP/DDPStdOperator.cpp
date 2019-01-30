#include "StdAfx.h"
#include "DDPStdOperator.h"
#include "UTOP_DDP.h"
//#include "Global_Memory_SFC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    DDPStdOperator::DDPStdOperator(void)
    {
        OPERATOR_INIT;
    }

    DDPStdOperator::~DDPStdOperator(void)
    {
    }

    BOOL DDPStdOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDDMaxY"), m_param.dDDMaxY, 20.0, _T("Max Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dThreshold"), m_param.dThreshold, 0.35, _T("Defect Threshold"));
        int nValue = 0;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSizeROIX"), nValue, 31, _T("Defect ROI X"));
        m_param.sizeROI.cx = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSizeROIY"), nValue, 31, _T("Defect ROI Y"));
        m_param.sizeROI.cy = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSingleSpec"), m_param.nSingleSpec, 100, _T("Single defect pixel count, <= Spec will PASS"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDoubleSpec"), m_param.nDoubleSpec, 0, _T("Double defect pixel count, <= Spec will PASS"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nMultipleSpec"), m_param.nMultipleSpec, 0, _T("Multiple defect pixel count, <= Spec will PASS"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDefectPixel3ChannelEn"), m_param.nDefectPixel3ChannelEn, 0, _T("0: No / 1: Yes"));
        
        return TRUE;
    }

    BOOL DDPStdOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        m_dYvalue = 0.0;
        memset(&m_defectInfo, 0, sizeof(H4ISPDefectInfo));
        memset(m_DefPointTable, 0, sizeof(AA_TwoDPoint) * DEFECT_COUNT_MAX);

        //------------------------------------------------------------------------------
        // 抓图
        if (m_param.nReCapture != 0)
        {
            // 重新设定Sensor序列
            CString strRegName = m_strOperatorName;
            if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
                strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
            {
                uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }

            // 抓图
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
            // 使用上次的抓图
            m_pDevice->GetCapturedBuffer(m_bufferObj);
        }

        // 画图
        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

        //------------------------------------------------------------------------------
        // 判断画面平均亮度
        UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
        if (m_dYvalue > m_param.dDDMaxY)
        {
            *pnErrorCode = uts.errorcode.E_Linumance;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 测试
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_RAW8) != 0)
        {
            int nOutMode = 0;
            if (!GetSensorOutMode(nOutMode))
            {
                uts.log.Error(_T("GetSensorOutMode error."));
                *pnErrorCode = uts.errorcode.E_NoImage;
                goto end;
            }
            DefectPixels(
                m_bufferObj.pRaw8Buffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight,
                m_param.dThreshold,
                m_param.sizeROI.cx,
                m_param.sizeROI.cy,
                m_param.nDefectPixel3ChannelEn,
                m_defectInfo,
                m_DefPointTable,
                nOutMode);
        }
        else if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
            DefectPixelsRGB(
                m_bufferObj.pBmpBuffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight,
                m_param.dThreshold,
                m_param.sizeROI.cx,
                m_param.sizeROI.cy,
                m_defectInfo,
                m_DefPointTable);
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 判断规格
        if (m_defectInfo.TotalSingleDefectCount > m_param.nSingleSpec
            || m_defectInfo.TotalDoubleDefectCount > m_param.nDoubleSpec
            || m_defectInfo.TotalMultipleDefectCount > m_param.nMultipleSpec)
        {
            *pnErrorCode = uts.errorcode.E_DarkNG;
        }
        else
        {
            *pnErrorCode = uts.errorcode.E_Pass;
        }
end:
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        //------------------------------------------------------------------------------
        // 保存图片文件
        if (m_param.nReCapture != 0)
        {
            SaveImage();
        }

        //------------------------------------------------------------------------------
        // 保存数据文件
        CString strLocationDataFilename;
        strLocationDataFilename.Format(_T("%s-Location"), m_strOperatorName);
        SaveLocationData(strLocationDataFilename);
        SaveData();

        return m_bResult;
    }

    void DDPStdOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_DarkNG);
    }

    void DDPStdOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void DDPStdOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),Y_Avg,DD_Result,")
            _T("Single_Num,Double_Num,Multi_Num,ISPDef_Num,CDefect_Num,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%.1f,%s,")
            _T("%d,%d,%d,%d,%d,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
            , m_defectInfo.TotalSingleDefectCount
            , m_defectInfo.TotalDoubleDefectCount
            , m_defectInfo.TotalMultipleDefectCount
            , m_defectInfo.H4ISPDefectPixelCount
            , m_defectInfo.CDefectCount
            , strVersion, uts.info.strUserId);
    }

    void DDPStdOperator::SaveLocationData(LPCTSTR lpOperatorName)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        CString strTime;
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d:%03d"),
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        //-------------------------------------------------------------------------
        // 数据准备
        TestItemResult tir = {0};
        tir.sItemName = lpOperatorName;
        tir.sHeader = _T("Time, SN\n");
        tir.sData.Format(_T("%s, %s"), strTime, uts.info.strSN);
        for (int i = 0 ; i < DEFECT_COUNT_MAX; i++)
        {
            if (m_DefPointTable[i].value != 0)
            {
                tir.sData.AppendFormat(
                    _T(", x%d_y%d_v%.1lf")
                    , m_DefPointTable[i].x
                    , m_DefPointTable[i].y
                    , m_DefPointTable[i].value		
                    );
                if (i == DEFECT_COUNT_MAX - 1)
                {
                    tir.sData.Append(_T(", too manay"));    // over 1000 defect pixel
                    break;
                }
            }
            else
            {
                break;		// no more defect pixel
            }
        }
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
        return (new DDPStdOperator);
    }
    //------------------------------------------------------------------------------
}
